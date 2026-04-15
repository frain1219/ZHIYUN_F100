set pagination off
set confirm off
set breakpoint pending on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
hbreak PD_GPIO_StateMachine

set {unsigned int}0x20000560 = 1
set {unsigned int}0x2000052c = 1
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
x/1wx 0x20000560
x/1wx 0x2000052c
x/1wx 0x20000518
x/1bx 0x2000051d
x/1bx 0x2000051c
x/1wx 0x20000500
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
x/1wx 0x20000560
x/1wx 0x2000052c
x/1wx 0x20000518
x/1bx 0x2000051d
x/1bx 0x2000051c
x/1wx 0x20000500
detach
quit
