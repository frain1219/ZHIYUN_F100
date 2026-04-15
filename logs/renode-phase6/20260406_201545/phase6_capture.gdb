set pagination off
set confirm off
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:3333
monitor start
shell sleep 2
interrupt
echo === Stimulate USB insert via LightPd pending-event SRAM injection ===\n
set {unsigned char}(0x2000088c + 6) = 1
set {unsigned char}(0x2000088c + 7) = 1
set {unsigned char}(0x2000088c + 8) = 1
set {unsigned char}(0x2000088c + 9) = 1
set {unsigned char}(0x2000088c + 10) = 1
set {unsigned char}(0x2000088c + 11) = 1
set {unsigned char}0x20000550 = 1
echo === Force PD attached branch (fallback) ===\n
set {unsigned int}0x2000091c = 1
set {unsigned int}0x20000898 = 1
monitor start
shell sleep 2
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
x/1bx 0x20000889
x/1bx 0x20000888
x/1wx 0x2000091c
x/1wx 0x20000920
x/1wx 0x20000884
x/1wx 0x20000898
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
echo === SPI1 startup frame trace (23B) ===\n
x/1wx 0x200009a8
x/1wx 0x200009ac
x/1wx 0x200009b4
x/1wx 0x200009a0
x/1wx 0x200009b0
x/1wx 0x200009a4
echo === SPI1 runtime framepump cadence ===\n
x/1wx 0x200008a8
x/1wx 0x200008a0
echo === SPI1 runtime cadence context (ticks/dirty) ===\n
x/1wx 0x20000534
x/1wx 0x20000874
x/1bx 0x200008f4
echo === main loop semantic counters (Step8 / IWDG reload) ===\n
x/1wx 0x20000924
x/1wx 0x20000928
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_201545/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
