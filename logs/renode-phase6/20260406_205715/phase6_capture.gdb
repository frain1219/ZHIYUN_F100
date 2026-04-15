set pagination off
set confirm off
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
monitor start
shell sleep 1
interrupt
echo === Stimulate USB insert via LightPd pending-event SRAM injection ===\n
set {unsigned char}(0x200008b4 + 6) = 1
set {unsigned char}(0x200008b4 + 7) = 1
set {unsigned char}(0x200008b4 + 8) = 1
set {unsigned char}(0x200008b4 + 9) = 1
set {unsigned char}(0x200008b4 + 10) = 1
set {unsigned char}(0x200008b4 + 11) = 1
set {unsigned char}0x20000578 = 1
echo === Force PD attached branch (fallback) ===\n
set {unsigned int}0x20000944 = 1
set {unsigned int}0x200008c0 = 1
monitor start
shell sleep 1
interrupt
if 1 == 1
  hbreak PD_GPIO_StateMachine
  set $bp_pd_capture = $bpnum
  continue
  if 0 > 0
    disable $bp_pd_capture
    stepi
    monitor start
    shell sleep 0
    interrupt
  end
end
echo === Phase6 Snapshot: PC/SP/LR ===\n
info registers pc sp lr
echo === GPIOC_IDR (PC13 source) ===\n
x/1wx 0x40011008
echo === GPIOB_ODATA (PB3/PB11 observable state) ===\n
x/1wx 0x40010C0C
echo === GPIOB_BSRR / BRR shadow check ===\n
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === Phase6 PD state shadows (SRAM) ===\n
x/1bx 0x200008b1
x/1bx 0x200008b0
x/1wx 0x20000944
x/1wx 0x20000948
x/1wx 0x200008ac
x/1wx 0x200008c0
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
echo === SPI1 startup frame trace (23B) ===\n
x/1wx 0x200009ec
x/1wx 0x200009f8
x/1wx 0x20000a00
x/1wx 0x200009dc
x/1wx 0x200009fc
x/1wx 0x200009e4
echo === SPI1 runtime framepump cadence ===\n
x/1wx 0x200008d0
x/1wx 0x200008c8
echo === SPI1 runtime cadence context (ticks/dirty) ===\n
echo === Scheduler vs PD-call tick parity (expect equal: one RunOnce == one PD block) ===\n
x/1wx 0x200009d8
x/1wx 0x2000055c
x/1wx 0x2000089c
x/1bx 0x2000091c
echo === SysTick (Cortex-M) — OEM PD_PeriodicDispatchFromSysTick is /20 vs this counter; Phase6 main is busy-loop ===\n
x/1wx 0xE000E010
x/1wx 0xE000E014
x/1wx 0xE000E018
echo === main->SPI pump bridge counters ===\n
x/1wx 0x200009d0
x/1wx 0x200009d4
echo === main loop semantic counters (Step8 / IWDG reload) ===\n
x/1wx 0x2000094c
x/1wx 0x20000950
echo === startup chain magic/IWDT anchors ===\n
x/1wx 0x200009f0
x/1wx 0x200009e8
x/1wx 0x200009e0
x/1wx 0x200009f4
echo === USB upgrade guard risk counters ===\n
x/1wx 0x20000a0c
x/1wx 0x20000a14
x/1wx 0x20000a08
x/1wx 0x20000a10
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_205715/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
