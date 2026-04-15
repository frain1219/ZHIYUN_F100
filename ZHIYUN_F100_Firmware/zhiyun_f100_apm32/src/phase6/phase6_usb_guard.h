#ifndef PHASE6_USB_GUARD_H
#define PHASE6_USB_GUARD_H

/*
 * Phase 6 first pass:
 * USB firmware upgrade path is explicitly excluded from acceptance.
 * Keep this flag at 0 unless a dedicated implementation/verification task is opened.
 */
#define PHASE6_USB_FW_UPGRADE_ENABLED (0)

typedef struct {
    unsigned int guard_initialized;
    unsigned int blocked_count;
    unsigned int enabled_flag;
    unsigned int preflight_ready;
    unsigned int preflight_fail_count;
    unsigned int flash_write_attempt_count;
} Phase6_UsbUpgradeGuardTrace;

void Phase6_USB_UpgradePath_Guard_Init(void);
int Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded(void);
unsigned int Phase6_USB_UpgradePath_ReadBlockedCount(void);
unsigned int Phase6_USB_UpgradePath_ReadEnableFlag(void);
void Phase6_USB_UpgradePath_SetPreflightReady(unsigned char ready);
void Phase6_USB_UpgradePath_RecordFlashWriteAttempt(void);
void Phase6_USB_UpgradePath_ReadTrace(Phase6_UsbUpgradeGuardTrace *out_trace);

#endif /* PHASE6_USB_GUARD_H */

