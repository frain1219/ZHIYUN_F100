#ifndef PHASE6_SCHEDULER_H
#define PHASE6_SCHEDULER_H

#include "phase6/phase6_acceptance.h"
#include "phase6/phase6_light_pd_skeleton.h"

/*
 * Top-level scheduler entry for Phase 6 first-pass reconstruction.
 */

void Phase6_Scheduler_Init(void);
void Phase6_Scheduler_RunStartupStage(void);
void Phase6_Scheduler_RunOnce(void);
void Phase6_Scheduler_RunBootstrapSequence(void);
void Phase6_Scheduler_SelectMode(Phase6_UiMode mode);
void Phase6_Scheduler_InjectEncoderDelta(int delta);
void Phase6_Scheduler_InjectAdcLevel(unsigned short adc_level);
void Phase6_Scheduler_NotifyUsbInserted(void);
void Phase6_Scheduler_ReadAcceptanceReport(Phase6_AcceptanceReport *out_report);

#endif /* PHASE6_SCHEDULER_H */

