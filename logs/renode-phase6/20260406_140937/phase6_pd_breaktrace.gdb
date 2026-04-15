set pagination off
set confirm off
set breakpoint pending on
set mi-async on
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
hbreak PD_GPIO_StateMachine
hbreak PD_HelperSeq_3000tick_ThenPc13Gpio
set {unsigned int}0x20000554 = 1
set {unsigned int}0x20000520 = 1
monitor start
continue&
shell sleep 4
interrupt
echo === Breaktrace snapshot ===\n
info breakpoints
info registers pc sp lr
echo === GPIO register view ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view ===\n
x/1wx 0x20000554
x/1wx 0x20000520
x/1wx 0x2000050c
x/1bx 0x20000511
x/1bx 0x20000510
x/1wx 0x200004f4
detach
quit
