set pagination off
set confirm off
target extended-remote localhost:3333
monitor start
shell sleep 5
interrupt
echo === Phase6 Snapshot: PC/SP/LR ===\n
info registers pc sp lr
echo === GPIOC_IDR (PC13 source) ===\n
x/1wx 0x40011008
echo === GPIOB_BSRR / BRR (PB3/PB11 sink) ===\n
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_135430/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
