#include "phase6/phase6_usb_guard.h"

#if (PHASE6_USB_FW_UPGRADE_ENABLED != 0) && (PHASE6_USB_FW_UPGRADE_ENABLED != 1)
#error "PHASE6_USB_FW_UPGRADE_ENABLED must be 0 or 1"
#endif

volatile unsigned int g_phase6_usb_upgrade_guard_init;
volatile unsigned int g_phase6_usb_upgrade_block_count;
volatile unsigned int g_phase6_usb_upgrade_preflight_ready;
volatile unsigned int g_phase6_usb_upgrade_preflight_fail_count;
volatile unsigned int g_phase6_usb_upgrade_flash_write_attempt_count;

void Phase6_USB_UpgradePath_Guard_Init(void)
{
    g_phase6_usb_upgrade_guard_init = 1U;
}

int Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded(void)
{
#if (PHASE6_USB_FW_UPGRADE_ENABLED == 1)
    /*
     * Placeholder only.
     * Anchor: task-book Phase 6 acceptance excludes
     * UsbCtl_RequestDispatch_StateMachine encryption/download/flash-write chain.
     * TODO(待证实, G3+): only enable after dedicated implementation and
     * Flash-write safety verification are completed.
     */
    if (g_phase6_usb_upgrade_preflight_ready == 0U) {
        g_phase6_usb_upgrade_preflight_fail_count++;
        return 0;
    }
    return 1;
#else
    if (g_phase6_usb_upgrade_guard_init == 0U) {
        Phase6_USB_UpgradePath_Guard_Init();
    }
    g_phase6_usb_upgrade_block_count++;
    return 0;
#endif
}

unsigned int Phase6_USB_UpgradePath_ReadBlockedCount(void)
{
    return g_phase6_usb_upgrade_block_count;
}

unsigned int Phase6_USB_UpgradePath_ReadEnableFlag(void)
{
    return (unsigned int)PHASE6_USB_FW_UPGRADE_ENABLED;
}

void Phase6_USB_UpgradePath_SetPreflightReady(unsigned char ready)
{
    g_phase6_usb_upgrade_preflight_ready = (ready != 0U) ? 1U : 0U;
}

void Phase6_USB_UpgradePath_RecordFlashWriteAttempt(void)
{
    g_phase6_usb_upgrade_flash_write_attempt_count++;
#if (PHASE6_USB_FW_UPGRADE_ENABLED == 0)
    g_phase6_usb_upgrade_preflight_fail_count++;
#endif
}

void Phase6_USB_UpgradePath_ReadTrace(Phase6_UsbUpgradeGuardTrace *out_trace)
{
    if (out_trace == 0) {
        return;
    }

    out_trace->guard_initialized = g_phase6_usb_upgrade_guard_init;
    out_trace->blocked_count = g_phase6_usb_upgrade_block_count;
    out_trace->enabled_flag = (unsigned int)PHASE6_USB_FW_UPGRADE_ENABLED;
    out_trace->preflight_ready = g_phase6_usb_upgrade_preflight_ready;
    out_trace->preflight_fail_count = g_phase6_usb_upgrade_preflight_fail_count;
    out_trace->flash_write_attempt_count = g_phase6_usb_upgrade_flash_write_attempt_count;
}

