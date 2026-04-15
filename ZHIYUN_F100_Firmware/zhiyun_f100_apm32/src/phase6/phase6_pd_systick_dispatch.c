#include "phase6/phase6_pd_systick_dispatch.h"
#include "phase6/phase6_light_pd_skeleton.h"
#include "phase6/phase6_usb_guard.h"

#ifndef PHASE6_HOST_TEST
#include "apm32f10x.h"
#endif

extern volatile unsigned char g_phase6_pd_irq_ready;

volatile unsigned int g_phase6_systick_entry_count;

#define PHASE6_PD_SYSTICK_DIVIDER 20U

void Phase6_PD_PeriodicDispatchFromSysTick(void)
{
    g_phase6_systick_entry_count++;
    if (g_phase6_pd_irq_ready == 0U) {
        return;
    }
    if ((g_phase6_systick_entry_count % PHASE6_PD_SYSTICK_DIVIDER) != 0U) {
        return;
    }
    (void)Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded();
    PD_PowerAndProtocol_Block();
}

#ifndef PHASE6_HOST_TEST
void SysTick_Handler(void)
{
    Phase6_PD_PeriodicDispatchFromSysTick();
}
#endif
