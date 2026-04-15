set pagination off
set confirm off
set breakpoint pending on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
if 1 == 1
  break PD_GPIO_StateMachine
else
  hbreak PD_GPIO_StateMachine
end
set $bp_pd = $bpnum
if 0 > 0
  ignore $bp_pd 0
end
set $bp_helper = 0
set $bp_flush = 0
set $bp_flush_post_trigger = 0
set $bp_flush_post_skip = 0

hbreak UI_SPI1_PumpEightFramebufferSlices
set $bp_flush = $bpnum

set {unsigned int}0x20000944 = 1
set {unsigned int}0x200008c0 = 1
monitor start
continue
echo === Breaktrace snapshot (entry) ===\n
info breakpoints
info registers pc sp lr
echo === GPIO register view ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view ===\n
x/1wx 0x20000944
x/1wx 0x200008c0
x/1wx 0x200008ac
x/1bx 0x200008b1
x/1bx 0x200008b0
x/1wx 0x20000590
x/1wx 0x200008d0
x/1wx 0x200008c8
echo === Main semantic counters (entry) ===\n
x/1wx 0x2000094c
x/1wx 0x20000950
echo === main->SPI bridge counters (entry) ===\n
x/1wx 0x200009d4
x/1wx 0x200009d8
echo === startup chain magic/IWDT (entry) ===\n
x/1wx 0x200009f0
x/1wx 0x200009e8
x/1wx 0x200009e0
x/1wx 0x200009f4
echo === USB guard risk counters (entry) ===\n
x/1wx 0x20000a0c
x/1wx 0x20000a14
x/1wx 0x20000a08
x/1wx 0x20000a10
echo === Flush cadence context (entry) ===\n
x/1wx 0x2000055c
x/1wx 0x2000089c
x/1bx 0x2000091c
set $i = 0
while $i < 48
  stepi
  set $i = $i + 1
end
echo === Breaktrace snapshot (post-step 48) ===\n
info registers pc sp lr
echo === GPIO register view (post-step) ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view (post-step) ===\n
x/1wx 0x20000944
x/1wx 0x200008c0
x/1wx 0x200008ac
x/1bx 0x200008b1
x/1bx 0x200008b0
x/1wx 0x20000590
x/1wx 0x200008d0
x/1wx 0x200008c8
echo === Main semantic counters (post-step) ===\n
x/1wx 0x2000094c
x/1wx 0x20000950
echo === main->SPI bridge counters (post-step) ===\n
x/1wx 0x200009d4
x/1wx 0x200009d8
echo === startup chain magic/IWDT (post-step) ===\n
x/1wx 0x200009f0
x/1wx 0x200009e8
x/1wx 0x200009e0
x/1wx 0x200009f4
echo === USB guard risk counters (post-step) ===\n
x/1wx 0x20000a0c
x/1wx 0x20000a14
x/1wx 0x20000a08
x/1wx 0x20000a10
echo === Flush cadence context (post-step) ===\n
x/1wx 0x2000055c
x/1wx 0x2000089c
x/1bx 0x2000091c
disable $bp_pd
if $bp_helper > 0
  disable $bp_helper
end

echo === Continue to UI flush breakpoint ===\n
continue

if 0 == 1
  set {unsigned short}0x40013008 = 0x0003
  echo === SPI1->STS forced TXBE|RXBNE ===\n
end
if $bp_flush_post_trigger > 0
  disable $bp_flush
  continue
end
set $flush_trigger_before = *((unsigned int*)0x200008d0)
set $flush_skip_before = *((unsigned int*)0x200008c8)
set $flush_rehit_left = 0
while ($flush_rehit_left > 0) && (*((unsigned int*)0x200008d0) == $flush_trigger_before) && (*((unsigned int*)0x200008c8) == $flush_skip_before)
  continue
  set $flush_rehit_left = $flush_rehit_left - 1
end
set $j = 0
while $j < 64
  stepi
  set $j = $j + 1
end
echo === Flush breakpoint snapshot (rehit+post-step 64) ===\n
info registers pc sp lr
x/1wx 0x20000590
x/1wx 0x200008d0
x/1wx 0x200008c8
detach
quit
