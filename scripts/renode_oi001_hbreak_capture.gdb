# =============================================================================
# Renode + arm-none-eabi-gdb：在 Scatterload_DecompressRegionOrMemcpy（0x080012E8）
# 处硬件断点停机，读取 r0–r3（H-OI-001）
#
# 由 renode_gdb_capture.sh 在 OI001_MODE=hbreak 时注入（前置 target extended-remote）。
#
# 前置：Renode 已用 apm32_f100_gdb_first.resc 加载且尚未 start（与 sample 模式相同）。
#
# 与 renode_oi001_capture.gdb（interrupt 采样）的区别：本脚本使用
#   hbreak *0x080012e8 → monitor start → continue
# 以在入口指令处停下；若 continue 长时间不返回，见 Renode/GDB 版本说明或改用交互步骤（04 §1.3.13）。
#
# 可选：交互 GDB 中在 continue 前执行 set mi-async off（部分环境可改善停机同步）。
# =============================================================================

set pagination off
set confirm off
set verbose off
set remotetimeout 120
set architecture armv3m
set mi-async off

echo \n>>> [hbreak 模式] 对齐复位向量（与 apm32_f100_core.resc 一致）...\n
set $sp = 0x20001620
set $pc = 0x08006be9

echo \n>>> Flash 代码区请使用 hbreak（勿用 break）...\n
hbreak *0x080012e8

echo \n>>> Renode: monitor start\n
monitor start

echo \n>>> continue — 等待命中 0x080012E8（外层 shell 可设 GDB_TIMEOUT 超时）...\n
continue

echo \n==== H-OI-001：命中 Scatterload_DecompressRegionOrMemcpy @ 0x080012E8 ====\n
printf "PC=%#x LR=%#x SP=%#x\n", $pc, $lr, $sp
echo \n--- 实参（与反编译 param_1.. 对照；ctx=r0=*0x200002C4 族）---\n
printf "r0=%#x r1=%#x r2=%#x r3=%#x\n", $r0, $r1, $r2, $r3
info registers
echo \n--- SRAM 控制块 @ 0x200002C4 ---\n
x/4wx 0x200002c4
echo \n--- 当前 PC 处指令 ---\n
x/8i $pc

echo \n>>> 导出 SRAM 20KB（路径由 renode_gdb_capture.sh 注入）\n
dump binary memory __OI001_SRAM_DUMP_PATH__ 0x20000000 0x20005000

quit
