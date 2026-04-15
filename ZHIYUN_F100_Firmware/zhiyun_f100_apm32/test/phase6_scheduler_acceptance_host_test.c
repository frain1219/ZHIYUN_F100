#include <assert.h>
#include "phase6/phase6_scheduler.h"

int main(void)
{
    Phase6_AcceptanceReport report;
    unsigned int i;

    Phase6_Scheduler_Init();
    Phase6_Scheduler_ReadAcceptanceReport(&report);
    assert(report.bhv001_power_loop == PHASE6_ACCEPT_NOT_TESTED);
    assert(report.bhv002_cct_path == PHASE6_ACCEPT_NOT_TESTED);
    assert(report.bhv003_hsi_path == PHASE6_ACCEPT_NOT_TESTED);
    assert(report.bhv004_dimming_path == PHASE6_ACCEPT_NOT_TESTED);
    assert(report.bhv005_usb_pd_attach_path == PHASE6_ACCEPT_NOT_TESTED);
    assert(report.usb_insert_event_count == 0U);
    assert(report.startup_chain_completed == 0U);

    /* CCT path */
    Phase6_Scheduler_SelectMode(PHASE6_UI_MODE_CCT);
    Phase6_Scheduler_InjectAdcLevel(1400U);
    Phase6_Scheduler_InjectEncoderDelta(4);
    Phase6_Scheduler_RunOnce();

    /* HSI path */
    Phase6_Scheduler_SelectMode(PHASE6_UI_MODE_HSI);
    Phase6_Scheduler_InjectAdcLevel(3200U);
    Phase6_Scheduler_InjectEncoderDelta(7);
    Phase6_Scheduler_RunOnce();
    for (i = 0U; i < 120U; ++i) {
        Phase6_Scheduler_RunOnce();
    }
    Phase6_Scheduler_ReadAcceptanceReport(&report);
    assert(report.bhv001_power_loop == PHASE6_ACCEPT_PASS);
    assert(report.bhv002_cct_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv003_hsi_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv004_dimming_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv005_usb_pd_attach_path != PHASE6_ACCEPT_PASS);
    assert(report.usb_insert_event_count == 0U);

    /* USB attach path */
    Phase6_Scheduler_NotifyUsbInserted();
    Phase6_Scheduler_RunOnce();
    Phase6_Scheduler_ReadAcceptanceReport(&report);
    assert(report.bhv005_usb_pd_attach_path != PHASE6_ACCEPT_PASS);
    for (i = 0U; i < 3100U; ++i) {
        Phase6_Scheduler_RunOnce();
    }

    Phase6_Scheduler_ReadAcceptanceReport(&report);

    assert(report.bhv001_power_loop == PHASE6_ACCEPT_PASS);
    assert(report.bhv002_cct_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv003_hsi_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv004_dimming_path == PHASE6_ACCEPT_PASS);
    assert(report.bhv005_usb_pd_attach_path == PHASE6_ACCEPT_PASS);
    assert(report.spi_flush_slices >= 8U);
    assert(report.spi_flush_bytes >= (8U * (3U + 96U)));
    assert(report.spi_flush_trigger_count > 0U);
    assert(report.spi_flush_skip_count > 0U);
    assert(report.ui_framebuffer_checksum != 0U);
    assert(report.main_iwdg_reload_count > 0U);
    assert(report.main_boot_phase_counter8 < 8U);
    assert(report.mainloop_spi_pump_request_count > 0U);
    assert(report.mainloop_spi_pump_consumed_count > 0U);
    assert(report.tmr2_dma_push_count > 0U);
    assert(report.tmr2_dma_last_words >= 16U);
    assert(report.tmr3_pwm_apply_count > 0U);
    assert(report.tmr3_cc2_shadow <= 999U);
    assert(report.dma1_ch1_transfer_count > 0U);
    assert(report.usb_upgrade_enabled == 0U);
    assert(report.usb_upgrade_policy_guarded != 0U);
    assert(report.usb_upgrade_block_count > 0U);
    assert(report.pd_attach_window_count > 0U);
    assert(report.protocol_lane_expand_count > 0U);
    assert(report.usb_device_initialized != 0U);
    assert(report.usb_device_context_ptr_shadow != 0U);
    assert(report.usb_device_ep0_max_packet_shadow >= 8U);
    assert(report.usb_device_descriptors_ready != 0U);
    assert(report.usb_device_sof_ticks > 0U);
    assert(report.usb_ep0_setup_count > 0U);
    assert(report.usb_ep0_in_count > 0U);
    assert(report.startup_chain_completed != 0U);
    assert(report.startup_step_count >= 6U);
    assert(report.startup_last_step_id != 0U);
    assert(report.startup_magic55aa_matched == 0U);
    assert(report.startup_iwdt_started != 0U);
    assert(report.startup_iwdt_reload_shadow > 0U);
    assert(report.startup_clock_hsi_ready != 0U);
    assert(report.startup_clock_pll_ready != 0U);
    assert(report.startup_clock_pll_selected != 0U);
    assert(report.startup_fmc_latency_shadow != 0U);
    assert(report.startup_sysclk_hz_shadow >= 1000000U);
    assert(report.startup_systick_reload_shadow > 0U);
    assert(report.usb_upgrade_guard_initialized != 0U);
    assert(report.usb_upgrade_preflight_ready == 0U);
    assert(report.usb_upgrade_preflight_fail_count == 0U);
    assert(report.usb_upgrade_flash_write_attempt_count == 0U);

    return 0;
}
