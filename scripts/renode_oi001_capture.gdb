# =============================================================================
# Renode + arm-none-eabi-gdb：采集 OI-001 / 散载相关运行时快照
#
# 由 renode_gdb_capture.sh 注入 target extended-remote（见脚本内临时 wrapper）
#
# 前置：Renode 已用 **apm32_f100_gdb_first.resc** 加载（未 start）。
#
# 重要（Renode 1.16.x + arm-none-eabi-gdb 实测）：
#   **`hbreak` / `break` 后 `continue` 或 `advance *addr` 往往永不返回**（远端不回报停止）。
#   因此本脚本采用 **`monitor start` → 立即 `interrupt`** 采样寄存器 + SRAM 转储；
#   停点 **不保证** 命中 0x0800692A / 0x080012E8，但与任务书 §7「动态证据」一致：**可重复日志 + SRAM 镜像**。
#   若需精确断点，请使用板载调试器或 Renode 版本升级后再试 hbreak 路径。
# =============================================================================

set pagination off
set confirm off
set verbose off
set remotetimeout 120
set architecture armv3m

echo \n>>> 将 PC/SP 置为与 apm32_f100_core.resc 一致（Thumb 复位向量）...\n
set $sp = 0x20001620
set $pc = 0x08006be9

echo \n>>> Renode: monitor start（经 GDB 转发）...\n
monitor start

echo \n>>> interrupt（采样当前执行点；勿使用 continue/advance，见文件头说明）...\n
interrupt

echo \n==== OI-001 快照：寄存器 / 栈 / 散载相关字 ====\n
printf "PC=%#x LR=%#x SP=%#x\n", $pc, $lr, $sp
info registers
echo \n--- stack 64 bytes ---\n
x/16wx $sp
echo \n--- Flash 池字 @ 0x0800695C（DAT_0800695C → 控制块指针）---\n
x/1wx 0x0800695c
echo \n--- SRAM @ 0x200002C4（控制块区域，见 04 §1.3）---\n
x/4wx 0x200002c4
echo \n--- r0–r3（便于与 Scatterload_DecompressRegionOrMemcpy 实参对照）---\n
printf "r0=%#x r1=%#x r2=%#x r3=%#x\n", $r0, $r1, $r2, $r3
echo \n--- 指令窗：0x0800692A（scatterload_rw_decompress_entry）---\n
x/8i 0x0800692a
echo \n--- 指令窗：0x080012E8（Scatterload_DecompressRegionOrMemcpy）---\n
x/8i 0x080012e8

echo \n>>> 导出 SRAM 20KB（路径由 renode_gdb_capture.sh 注入）\n
dump binary memory __OI001_SRAM_DUMP_PATH__ 0x20000000 0x20005000

quit
