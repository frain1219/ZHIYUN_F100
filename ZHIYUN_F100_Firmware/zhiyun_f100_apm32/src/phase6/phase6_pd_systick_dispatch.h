#ifndef PHASE6_PD_SYSTICK_DISPATCH_H
#define PHASE6_PD_SYSTICK_DISPATCH_H

/*
 * OEM anchor: FUN_0800CC3C / PD_PeriodicDispatchFromSysTick (04 §3.5.1):
 * SysTick IRQ increments a counter; every 20 entries (count % 0x14 == 0) calls
 * PD_PowerAndProtocol_Block.
 *
 * g_phase6_systick_entry_count mirrors the OEM tick counter for GDB/Renode evidence.
 */

void Phase6_PD_PeriodicDispatchFromSysTick(void);

extern volatile unsigned int g_phase6_systick_entry_count;

#endif /* PHASE6_PD_SYSTICK_DISPATCH_H */
