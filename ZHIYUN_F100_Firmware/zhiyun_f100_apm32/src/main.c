/*!
 * @file    main.c
 * @brief   Phase 6 skeleton — APM32F103CBT6, Geehy StdPeriph (via PlatformIO).
 */

#include "apm32f10x.h"
#include "phase6/phase6_scheduler.h"

#ifndef PHASE6_BOOTSTRAP_SEQUENCE_ENABLE
#define PHASE6_BOOTSTRAP_SEQUENCE_ENABLE 0
#endif

int main(void)
{
    /* SystemInit() runs in startup before main */
    Phase6_Scheduler_Init();
    Phase6_Scheduler_RunStartupStage();
#if (PHASE6_BOOTSTRAP_SEQUENCE_ENABLE == 1)
    /*
     * Test-only path:
     * run a deterministic software sequence to populate acceptance counters.
     * Keep disabled by default to avoid hardcoding synthetic behavior.
     */
    Phase6_Scheduler_RunBootstrapSequence();
#endif

    while (1) {
        Phase6_Scheduler_RunOnce();
    }
}
