#include "phase6/phase6_scheduler.h"
#include "phase6/phase6_acceptance.h"
#ifndef PHASE6_HOST_TEST
#include "phase6/phase6_input_adapter.h"
#else
static void Phase6_InputAdapter_Init(void) {}
static unsigned char Phase6_InputAdapter_Poll(Phase6_InputEvent *out_event)
{
    (void)out_event;
    return 0U;
}
#endif
#include "phase6/phase6_light_pd_skeleton.h"
#include "phase6/phase6_startup_chain.h"
#include "phase6/phase6_usb_device.h"
#include "phase6/phase6_usb_guard.h"
#include "phase6/phase6_pd_systick_dispatch.h"
#ifndef PHASE6_HOST_TEST
#include "apm32f10x.h"
#endif

/*
 * Exported for Renode/GDB snapshots (scripts/renode_phase6_capture.sh).
 * OEM: PD_PowerAndProtocol_Block runs from SysTick path every 20 ticks
 * (PD_PeriodicDispatchFromSysTick, 04 §3.5.1). g_phase6_scheduler_ticks counts
 * main-loop RunOnce iterations; g_phase6_call_ticks counts PD block entries;
 * g_phase6_systick_entry_count counts SysTick IRQ entries (see phase6_pd_systick_dispatch.c).
 */
volatile unsigned int g_phase6_scheduler_ticks;
volatile unsigned char g_phase6_pd_irq_ready;
static volatile Phase6_InputEvent g_phase6_scheduler_input;
static volatile unsigned char g_phase6_scheduler_input_dirty;
static volatile unsigned char g_phase6_scheduler_startup_done;
static volatile unsigned int g_phase6_main_boot_phase_counter8;
static volatile unsigned int g_phase6_main_iwdg_reload_count;
static volatile unsigned int g_phase6_main_period_last_tick;

/*
 * OEM main (~0x08002E74) reloads IWDT on ~100 inner-loop iterations; Phase6 uses
 * one tick per RunOnce. Override via -DPHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=N for
 * Renode/G2 snapshots only when documented (nominal product build keeps 100).
 */
#ifndef PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS
#define PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS 100U
#endif
#define PHASE6_MAIN_SPI_PUMP_PERIOD_TICKS 32U

#ifdef PHASE6_HOST_TEST
/*
 * Host harness has no SysTick IRQ: simulate 20 OEM SysTick entries per RunOnce so
 * one PD block runs per iteration (matches prior acceptance harness cadence).
 */
static void Phase6_Scheduler_HostFeedSysTickForOnePd(void)
{
    unsigned int i;

    for (i = 0U; i < 20U; i++) {
        Phase6_PD_PeriodicDispatchFromSysTick();
    }
}
#endif

static void StepBootPhaseCounter8(void)
{
    g_phase6_main_boot_phase_counter8 =
        (g_phase6_main_boot_phase_counter8 + 1U) & 0x7U;
}

static void thunk_IWDG_KR_ReloadAAAA_Shadow(void)
{
    /*
     * Main-loop parity anchor:
     * OEM main periodically reloads IWDG KR (AAAA). Phase6 keeps shadow evidence
     * first and does not force new watchdog MMIO side effects in this cut.
     */
    g_phase6_main_iwdg_reload_count++;
#ifndef PHASE6_HOST_TEST
    IWDT->KEY = 0xAAAAU;
#endif
}

void Phase6_Scheduler_Init(void)
{
    g_phase6_scheduler_ticks = 0U;
    g_phase6_pd_irq_ready = 0U;
    g_phase6_scheduler_startup_done = 0U;
    g_phase6_scheduler_input.encoder_delta = 0;
    g_phase6_scheduler_input.adc_level = 0U;
    g_phase6_scheduler_input.has_adc_update = 0U;
    g_phase6_scheduler_input.requested_mode = PHASE6_UI_MODE_CCT;
    g_phase6_scheduler_input.has_mode_request = 0U;
    g_phase6_scheduler_input.usb_inserted = 0U;
    g_phase6_scheduler_input_dirty = 0U;
    g_phase6_main_boot_phase_counter8 = 0U;
    g_phase6_main_iwdg_reload_count = 0U;
    g_phase6_main_period_last_tick = 0U;
    Phase6_StartupChain_Reset();
    Phase6_Acceptance_Init();
}

void Phase6_Scheduler_RunStartupStage(void)
{
    if (g_phase6_scheduler_startup_done != 0U) {
        return;
    }

    Phase6_StartupChain_Run();
    Phase6_InputAdapter_Init();
    Phase6_UsbDevice_Init();
    Phase6_USB_UpgradePath_Guard_Init();
    g_phase6_scheduler_startup_done = 1U;
    g_phase6_pd_irq_ready = 1U;
}

void Phase6_Scheduler_RunBootstrapSequence(void)
{
    /*
     * Optional software bootstrap used by bench tests only.
     * Keep disabled in mainline runtime so hardware inputs remain authoritative.
     */
    Phase6_Scheduler_SelectMode(PHASE6_UI_MODE_CCT);
    Phase6_Scheduler_InjectAdcLevel(1200U);
    Phase6_Scheduler_InjectEncoderDelta(4);
    Phase6_Scheduler_RunOnce();

    Phase6_Scheduler_SelectMode(PHASE6_UI_MODE_HSI);
    Phase6_Scheduler_InjectEncoderDelta(7);
    Phase6_Scheduler_RunOnce();

    Phase6_Scheduler_NotifyUsbInserted();
    Phase6_Scheduler_RunOnce();
}

void Phase6_Scheduler_SelectMode(Phase6_UiMode mode)
{
    g_phase6_scheduler_input.requested_mode = mode;
    g_phase6_scheduler_input.has_mode_request = 1U;
    g_phase6_scheduler_input_dirty = 1U;
}

void Phase6_Scheduler_InjectEncoderDelta(int delta)
{
    g_phase6_scheduler_input.encoder_delta += delta;
    g_phase6_scheduler_input_dirty = 1U;
}

void Phase6_Scheduler_InjectAdcLevel(unsigned short adc_level)
{
    g_phase6_scheduler_input.adc_level = adc_level;
    g_phase6_scheduler_input.has_adc_update = 1U;
    g_phase6_scheduler_input_dirty = 1U;
}

void Phase6_Scheduler_NotifyUsbInserted(void)
{
    g_phase6_scheduler_input.usb_inserted = 1U;
    g_phase6_scheduler_input_dirty = 1U;
    Phase6_Acceptance_NotifyUsbInserted();
}

void Phase6_Scheduler_ReadAcceptanceReport(Phase6_AcceptanceReport *out_report)
{
    Phase6_Acceptance_ReadReport(out_report);
}

void Phase6_Scheduler_RunOnce(void)
{
    Phase6_LightPd_TraceSnapshot trace_snapshot;
    Phase6_InputEvent hardware_event;
    Phase6_InputEvent input_event;

    g_phase6_scheduler_ticks++;

    if (g_phase6_scheduler_startup_done == 0U) {
        Phase6_Scheduler_RunStartupStage();
    }

    if (Phase6_InputAdapter_Poll(&hardware_event) != 0U) {
        g_phase6_scheduler_input.encoder_delta += hardware_event.encoder_delta;
        if (hardware_event.has_adc_update != 0U) {
            g_phase6_scheduler_input.adc_level = hardware_event.adc_level;
            g_phase6_scheduler_input.has_adc_update = 1U;
        }
        if (hardware_event.has_mode_request != 0U) {
            g_phase6_scheduler_input.requested_mode = hardware_event.requested_mode;
            g_phase6_scheduler_input.has_mode_request = 1U;
        }
        if (hardware_event.usb_inserted != 0U) {
            g_phase6_scheduler_input.usb_inserted = 1U;
            Phase6_Acceptance_NotifyUsbInserted();
        }
        g_phase6_scheduler_input_dirty = 1U;
    }

    if (g_phase6_scheduler_input_dirty != 0U) {
        input_event = g_phase6_scheduler_input;
        Phase6_LightPd_SetInputEvent(&input_event);
        g_phase6_scheduler_input.encoder_delta = 0;
        g_phase6_scheduler_input.has_adc_update = 0U;
        g_phase6_scheduler_input.has_mode_request = 0U;
        g_phase6_scheduler_input.usb_inserted = 0U;
        g_phase6_scheduler_input_dirty = 0U;
    }

    /*
     * OEM: PD_PowerAndProtocol_Block is invoked from SysTick via
     * PD_PeriodicDispatchFromSysTick (every 20 SysTick IRQ entries, 04 §3.5.1).
     * USB upgrade guard runs in that path immediately before PD.
     * Host: simulate 20 SysTick entries per RunOnce (no hardware IRQ).
     */
#ifdef PHASE6_HOST_TEST
    Phase6_Scheduler_HostFeedSysTickForOnePd();
#endif

    /*
     * Reference: 05-MMIO-v1 — OEM main triggers StepBootPhaseCounter8 + IWDG reload
     * on ~100 tick cadence inside the long-running loop (main @ 0x08002E74 decompile).
     */
    if ((g_phase6_scheduler_ticks - g_phase6_main_period_last_tick) >=
        PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS) {
        StepBootPhaseCounter8();
        thunk_IWDG_KR_ReloadAAAA_Shadow();
        g_phase6_main_period_last_tick = g_phase6_scheduler_ticks;
    }
    if ((g_phase6_scheduler_ticks % PHASE6_MAIN_SPI_PUMP_PERIOD_TICKS) == 0U) {
        /*
         * OEM main contains a direct SPI1 pump anchor besides PD block tail flush.
         * Phase6 routes main-loop requests into UI_SPI flush gate to keep one
         * transaction backend while preserving dual-trigger semantics.
         */
        Phase6_MainLoop_RequestSpiPump();
    }
    Phase6_LightPd_ReadTraceSnapshot(&trace_snapshot);
    trace_snapshot.main_boot_phase_counter8 = g_phase6_main_boot_phase_counter8;
    trace_snapshot.main_iwdg_reload_count = g_phase6_main_iwdg_reload_count;
    Phase6_UsbDevice_MainLoop_DataPump((unsigned char)trace_snapshot.usb_attached);
    Phase6_Acceptance_OnLoopSample(&trace_snapshot);
}

