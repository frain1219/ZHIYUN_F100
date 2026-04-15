set pagination off
set confirm off
set breakpoint pending on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
hbreak PD_GPIO_StateMachine
set $bp_pd = $bpnum
if 1 > 0
  ignore $bp_pd 1
end
set $bp_helper = 0
set $bp_flush = 0
set $bp_flush_post_trigger = 0
set $bp_flush_post_skip = 0



set {unsigned int}0x200008f0 = 1
set {unsigned int}0x20000878 = 1
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
x/1wx 0x200008f0
x/1wx 0x20000878
x/1wx 0x20000864
x/1bx 0x20000869
x/1bx 0x20000868
x/1wx 0x20000548
x/1wx 0x20000888
x/1wx 0x20000880
echo === Main semantic counters (entry) ===\n
x/1wx 0x200008f8
x/1wx 0x200008fc
set $i = 0
while $i < 8
  stepi
  set $i = $i + 1
end
echo === Breaktrace snapshot (post-step 8) ===\n
info registers pc sp lr
echo === GPIO register view (post-step) ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view (post-step) ===\n
x/1wx 0x200008f0
x/1wx 0x20000878
x/1wx 0x20000864
x/1bx 0x20000869
x/1bx 0x20000868
x/1wx 0x20000548
x/1wx 0x20000888
x/1wx 0x20000880
echo === Main semantic counters (post-step) ===\n
x/1wx 0x200008f8
x/1wx 0x200008fc

detach
quit
