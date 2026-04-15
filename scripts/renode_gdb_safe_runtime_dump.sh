#!/usr/bin/env bash
# 安全运行时快照：仅 extended-remote + interrupt + x/ + dump binary memory
# 不使用 monitor python Reset / RequestReset（可能使 PC=0，需重新 include apm32_f100.resc）。
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${ROOT}/scripts/out"
mkdir -p "${OUT}"
TS="$(date +%Y%m%d_%H%M%S)"
LOG="${OUT}/renode_safe_dump_${TS}.log"
BIN="${OUT}/renode_sram_20k_safe_${TS}.bin"
GDB="${GDB:-arm-none-eabi-gdb}"
exec "${GDB}" -q -batch \
  -ex "set confirm off" -ex "set pagination off" \
  -ex "target extended-remote localhost:${RENODE_GDB_PORT:-3333}" \
  -ex "interrupt" \
  -ex "echo === PC SP LR ===\n" \
  -ex "info registers pc sp lr" \
  -ex "echo === 0x2000000C UI pool ===\n" -ex "x/4wx 0x2000000c" \
  -ex "echo === LightApplyContext 0x20000180 ===\n" -ex "x/16wx 0x20000180" \
  -ex "echo === OI-001 0x200002C4 ===\n" -ex "x/4wx 0x200002c4" \
  -ex "dump binary memory ${BIN} 0x20000000 0x20005000" \
  -ex "detach" -ex "quit" 2>&1 | tee "${LOG}"
echo "日志: ${LOG}"
echo "SRAM: ${BIN}"
