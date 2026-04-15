set pagination off
set confirm off
file /Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
target extended-remote localhost:4336
monitor start
shell sleep 4
interrupt
echo === Stimulate USB insert via LightPd pending-event SRAM injection ===\n
set {unsigned char}(0x2000086c + 6) = 1
set {unsigned char}(0x2000086c + 7) = 1
set {unsigned char}(0x2000086c + 8) = 1
set {unsigned char}(0x2000086c + 9) = 1
set {unsigned char}(0x2000086c + 10) = 1
set {unsigned char}(0x2000086c + 11) = 1
set {unsigned char}0x20000530 = 1
echo === Force PD attached branch (fallback) ===\n
set {unsigned int}0x200008f0 = 1
set {unsigned int}0x20000878 = 1
monitor start
shell sleep 4
interrupt
if 0 == 1
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
x/1bx 0x20000869
x/1bx 0x20000868
x/1wx 0x200008f0
x/1wx 0x200008f4
x/1wx 0x20000864
x/1wx 0x20000878
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
echo === SPI1 startup frame trace (23B) ===\n
x/1wx 0x20000970
x/1wx 0x20000974
x/1wx 0x2000097c
x/1wx 0x20000968
x/1wx 0x20000978
x/1wx 0x2000096c
echo === SPI1 runtime framepump cadence ===\n
x/1wx 0x20000888
x/1wx 0x20000880
echo === SPI1 runtime cadence context (ticks/dirty) ===\n
x/1wx 0x2000051c
x/1wx 0x20000854
x/1bx 0x200008c8
echo === main loop semantic counters (Step8 / IWDG reload) ===\n
x/1wx 0x200008f8
x/1wx 0x200008fc
dump binary memory /Users/milocheung/Documents/codeProject/ZHIYUN_F100/logs/renode-phase6/20260406_195122/phase6_sram_20k.bin 0x20000000 0x20005000
detach
quit
