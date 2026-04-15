set pagination off
set confirm off
set breakpoint pending on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:4362
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



set {unsigned int}0x2000095c = 1
set {unsigned int}0x200008d8 = 1
if (0 > 0)
  set {unsigned int}0x20000a10 = 0
end
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
x/1wx 0x2000095c
x/1wx 0x200008d8
x/1wx 0x200008c4
x/1wx 0x200008bc
x/1bx 0x200008c9
x/1bx 0x200008c8
x/1wx 0x200005a8
x/1wx 0x200008e8
x/1wx 0x200008e0
echo === Main semantic counters (entry) ===\n
x/1wx 0x20000964
x/1wx 0x20000968
echo === main->SPI bridge counters (entry) ===\n
x/1wx 0x20000a08
x/1wx 0x20000a0c
echo === startup chain magic/IWDT (entry) ===\n
x/1wx 0x20000a30
x/1wx 0x20000a28
x/1wx 0x20000a20
x/1wx 0x20000a34
echo === USB guard risk counters (entry) ===\n
x/1wx 0x20000a4c
x/1wx 0x20000a54
x/1wx 0x20000a48
x/1wx 0x20000a50
echo === Flush cadence context (entry) ===\n
x/1wx 0x20000574
x/1wx 0x20000a10
x/1wx 0x200008b4
x/1bx 0x20000934
set $i = 0
while $i < 120
  stepi
  set $i = $i + 1
end
echo === Breaktrace snapshot (post-step 120) ===\n
info registers pc sp lr
echo === GPIO register view (post-step) ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view (post-step) ===\n
x/1wx 0x2000095c
x/1wx 0x200008d8
x/1wx 0x200008c4
x/1wx 0x200008bc
x/1bx 0x200008c9
x/1bx 0x200008c8
x/1wx 0x200005a8
x/1wx 0x200008e8
x/1wx 0x200008e0
echo === Main semantic counters (post-step) ===\n
x/1wx 0x20000964
x/1wx 0x20000968
echo === main->SPI bridge counters (post-step) ===\n
x/1wx 0x20000a08
x/1wx 0x20000a0c
echo === startup chain magic/IWDT (post-step) ===\n
x/1wx 0x20000a30
x/1wx 0x20000a28
x/1wx 0x20000a20
x/1wx 0x20000a34
echo === USB guard risk counters (post-step) ===\n
x/1wx 0x20000a4c
x/1wx 0x20000a54
x/1wx 0x20000a48
x/1wx 0x20000a50
echo === Flush cadence context (post-step) ===\n
x/1wx 0x20000574
x/1wx 0x20000a10
x/1wx 0x200008b4
x/1bx 0x20000934

detach
quit
