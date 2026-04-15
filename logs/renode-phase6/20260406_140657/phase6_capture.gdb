set pagination off
set confirm off
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
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
x/1bx 0x20000511
x/1bx 0x20000510
x/1wx 0x20000554
x/1wx 0x20000558
x/1wx 0x2000050c
x/1wx 0x20000520
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_140657/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
