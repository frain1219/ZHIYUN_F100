set pagination off
set confirm off
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
monitor start
shell sleep 2
interrupt
echo === Stimulate USB insert via LightPd pending-event SRAM injection ===\n
set {unsigned char}(0x20000520 + 6) = 1
set {unsigned char}(0x20000520 + 7) = 1
set {unsigned char}(0x20000520 + 8) = 1
set {unsigned char}(0x20000520 + 9) = 1
set {unsigned char}(0x20000520 + 10) = 1
set {unsigned char}(0x20000520 + 11) = 1
set {unsigned char}0x200004e8 = 1
echo === Force PD attached branch (fallback) ===\n
set {unsigned int}0x20000560 = 1
set {unsigned int}0x2000052c = 1
monitor start
shell sleep 2
interrupt
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
x/1bx 0x2000051d
x/1bx 0x2000051c
x/1wx 0x20000560
x/1wx 0x20000564
x/1wx 0x20000518
x/1wx 0x2000052c
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
echo === SPI1 startup frame trace (23B) ===\n
x/1wx 0x200005a4
x/1wx 0x200005a8
x/1wx 0x200005b0
x/1wx 0x200005a0
x/1wx 0x200005ac
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_155711/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
