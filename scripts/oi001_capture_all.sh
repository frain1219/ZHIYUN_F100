#!/usr/bin/env bash
# =============================================================================
# OI-001 / 运行时采集：一键执行已验证路径，并校验产物。
#
# 所有产物写入同一目录：logs/renode-oi001/<时间戳>/
#   - renode_emulator.log
#   - renode.pid
#   - gdb_capture.log
#   - renode_sram_20k.bin
#   - monitor_runfor_hook.log（第二步）
#
# 用法：在仓库根目录
#   ./scripts/oi001_capture_all.sh
#
# 环境变量与单步脚本相同（RENODE_BIN、GDB_TIMEOUT、RENODE_MONITOR_PORT 等）。
# =============================================================================
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LATEST="${ROOT}/logs/renode-oi001/LATEST"

echo "=== [1/2] renode_oi001_auto.sh (GDB + SRAM 20KB) ==="
GDB_TIMEOUT="${GDB_TIMEOUT:-240}" "${ROOT}/scripts/renode_oi001_auto.sh"

if [[ ! -f "${LATEST}" ]]; then
  echo "ERROR: 未找到 ${LATEST}（renode_oi001_auto 应写入本次目录）" >&2
  exit 1
fi
RUN_DIR="$(tr -d '\r\n' < "${LATEST}")"
if [[ ! -d "${RUN_DIR}" ]]; then
  echo "ERROR: 采集目录不存在: ${RUN_DIR}" >&2
  exit 1
fi

BIN="${RUN_DIR}/renode_sram_20k.bin"
if [[ ! -f "${BIN}" ]]; then
  echo "ERROR: 未生成 ${BIN}" >&2
  exit 1
fi
SZ=$(wc -c < "${BIN}" | tr -d ' ')
if [[ "${SZ}" -ne 20480 ]]; then
  echo "ERROR: ${BIN} 大小应为 20480 字节，实际 ${SZ}" >&2
  exit 1
fi

echo ""
echo "=== SRAM 控制块区域 xxd（0x200002C4 相对文件偏移 0x2C4）==="
xxd -s 0x2c4 -l 64 "${BIN}" || true

echo ""
echo "=== [2/2] oi001_monitor_runfor_hook.py（RunFor 1s 快照）==="
export OI001_RUN_DIR="${RUN_DIR}"
python3 "${ROOT}/scripts/oi001_monitor_runfor_hook.py" 0x080012E8 1

echo ""
echo "完成。本次采集目录:"
echo "  ${RUN_DIR}"
echo "内含:"
set +o pipefail
ls -la "${RUN_DIR}"
set -o pipefail
