set pagination off
set confirm off
set breakpoint pending on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
hbreak PD_GPIO_StateMachine
set $bp_pd = $bpnum
set $bp_helper = 0
set $bp_flush = 0
set $bp_flush_post_trigger = 0
set $bp_flush_post_skip = 0

hbreak UI_SPI1_PumpEightFramebufferSlices
set $bp_flush = $bpnum
break *UI_SPI1_PumpEightFramebufferSlices+0x84
set $bp_flush_post_trigger = $bpnum
break *UI_SPI1_PumpEightFramebufferSlices+0xa8
set $bp_flush_post_skip = $bpnum
set {unsigned int}0x200008e0 = 1
set {unsigned int}0x20000868 = 1
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
x/1wx 0x200008e0
x/1wx 0x20000868
x/1wx 0x20000854
x/1bx 0x20000859
x/1bx 0x20000858
x/1wx 0x20000538
x/1wx 0x20000878
x/1wx 0x20000870
set $i = 0
while $i < 32
  stepi
  set $i = $i + 1
end
echo === Breaktrace snapshot (post-step 32) ===\n
info registers pc sp lr
echo === GPIO register view (post-step) ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view (post-step) ===\n
x/1wx 0x200008e0
x/1wx 0x20000868
x/1wx 0x20000854
x/1bx 0x20000859
x/1bx 0x20000858
x/1wx 0x20000538
x/1wx 0x20000878
x/1wx 0x20000870
disable $bp_pd
if $bp_helper > 0
  disable $bp_helper
end
set {unsigned char}0x200008b8 = 0
set {unsigned int}0x2000050c = 1
set {unsigned int}0x20000844 = 0
echo === Force flush branch: SKIP ===\n
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
set $flush_trigger_before = *((unsigned int*)0x20000878)
set $flush_skip_before = *((unsigned int*)0x20000870)
set $flush_rehit_left = 0
while ($flush_rehit_left > 0) && (*((unsigned int*)0x20000878) == $flush_trigger_before) && (*((unsigned int*)0x20000870) == $flush_skip_before)
  continue
  set $flush_rehit_left = $flush_rehit_left - 1
end
set $j = 0
while $j < 0
  stepi
  set $j = $j + 1
end
echo === Flush breakpoint snapshot (rehit+post-step 0) ===\n
info registers pc sp lr
x/1wx 0x20000538
x/1wx 0x20000878
x/1wx 0x20000870
detach
quit
