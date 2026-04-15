#!/usr/bin/env bash
# =============================================================================
# 通过本机 arm-none-eabi-gdb（非 MCP）连接 Renode :3333，采集 OI-001 散载/解压断点证据。
#
# 一键自动化（推荐 macOS）：
#   cd /path/to/ZHIYUN_F100 && ./scripts/renode_oi001_auto.sh
# 聚合（GDB 转储 + Monitor 快照 + 校验 20KB）：./scripts/oi001_capture_all.sh
#   （自动启动 Renode；-P Monitor 仍可用于手动调试；start 须为 GDB「monitor start」见 renode_oi001_capture.gdb）
#
# 手动用法（OI-001 散载断点 — 须先 **不自动 start**）：
#   1) 终端 A：Renode 中
#        include @/path/to/ZHIYUN_F100/Renode/apm32_f100_gdb_first.resc
#      （勿用默认 apm32_f100.resc，否则散载在你连 GDB 前已跑完。）
#   2) 终端 B：
#        cd /path/to/ZHIYUN_F100
#        ./scripts/renode_gdb_capture.sh
#
# 环境变量：
#   GDB              默认 arm-none-eabi-gdb
#   RENODE_GDB_HOST  默认 localhost
#   RENODE_GDB_PORT  默认 3333
#   GDB_TIMEOUT      默认 180（秒）；`renode_oi001_auto.sh` 会设为 300
#   LOG_FILE         默认 ${OI001_RUN_DIR}/gdb_capture.log（见下）
#   OI001_RUN_DIR    单次采集目录；未设置时自动建 logs/renode-oi001/<时间戳>/
#   OI001_MODE       默认 sample：monitor start → interrupt（§7 默认，易完成）
#                    设为 hbreak：hbreak *0x080012e8 → monitor start → continue
#                    在 Scatterload_DecompressRegionOrMemcpy 入口停住并打印 r0–r3（H-OI-001）
#                    （实测 macOS Renode 1.x + arm-none-eabi-gdb：hbreak 路径常 120s 内无停机，
#                     以 exit 124 超时告终；可靠采集请用默认 sample。hbreak 脚本已加 set mi-async off 作尝试。）
#
# 输出（均在 OI001_RUN_DIR 下）：
#   - gdb_capture.log（或 LOG_FILE 覆盖）
#   - renode_sram_20k.bin：SRAM 0x20000000 起 20KB
#
# 说明：若断点永不命中，请在 Renode GUI 对机器 **Reset**，或重启 Renode 后立刻再运行本脚本。
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
GDB="${GDB:-arm-none-eabi-gdb}"
RENODE_GDB_HOST="${RENODE_GDB_HOST:-localhost}"
RENODE_GDB_PORT="${RENODE_GDB_PORT:-3333}"
RENODE_MONITOR_PORT="${RENODE_MONITOR_PORT:-49152}"
GDB_TIMEOUT="${GDB_TIMEOUT:-180}"
OI001_MODE="${OI001_MODE:-sample}"

LOGS_ROOT="${REPO_ROOT}/logs/renode-oi001"
if [[ -z "${OI001_RUN_DIR:-}" ]]; then
  TS="$(date +%Y%m%d_%H%M%S)"
  OI001_RUN_DIR="${LOGS_ROOT}/${TS}"
fi
mkdir -p "${OI001_RUN_DIR}"
export OI001_RUN_DIR
printf '%s\n' "${OI001_RUN_DIR}" > "${LOGS_ROOT}/LATEST"
LOG_FILE="${LOG_FILE:-${OI001_RUN_DIR}/gdb_capture.log}"
SRAM_DUMP="${OI001_RUN_DIR}/renode_sram_20k.bin"

if ! command -v "${GDB}" >/dev/null 2>&1; then
  echo "未找到 ${GDB}。请安装 GNU Arm Embedded Toolchain，或设置 GDB=/path/to/gdb" >&2
  exit 1
fi

TMP_GDB="$(mktemp /tmp/renode_oi001_XXXXXX.gdb)"
TMP_CAPTURE="$(mktemp /tmp/renode_oi001_cap_XXXXXX.gdb)"
cleanup() { rm -f "${TMP_GDB}" "${TMP_CAPTURE}"; }
trap cleanup EXIT

if [[ "${OI001_MODE}" == "hbreak" ]]; then
  CAPTURE_GDB="${SCRIPT_DIR}/renode_oi001_hbreak_capture.gdb"
else
  CAPTURE_GDB="${SCRIPT_DIR}/renode_oi001_capture.gdb"
fi
if [[ ! -f "${CAPTURE_GDB}" ]]; then
  echo "缺少: ${CAPTURE_GDB}" >&2
  exit 1
fi

sed -e "s/__RENODE_MONITOR_PORT__/${RENODE_MONITOR_PORT}/g" \
    -e "s|__REPO_ROOT__|${REPO_ROOT}|g" \
    -e "s|__OI001_SRAM_DUMP_PATH__|${SRAM_DUMP}|g" \
  "${CAPTURE_GDB}" > "${TMP_CAPTURE}"

cat > "${TMP_GDB}" <<EOF
# auto-generated wrapper
echo >>> target extended-remote ${RENODE_GDB_HOST}:${RENODE_GDB_PORT}\n
target extended-remote ${RENODE_GDB_HOST}:${RENODE_GDB_PORT}
echo >>> sourcing ${TMP_CAPTURE} (Monitor port ${RENODE_MONITOR_PORT})\n
source ${TMP_CAPTURE}
EOF

echo "采集目录 OI001_RUN_DIR=${OI001_RUN_DIR}"
echo "日志: ${LOG_FILE}"
echo "SRAM: ${SRAM_DUMP}"
echo "GDB : ${GDB}"
echo "远程: ${RENODE_GDB_HOST}:${RENODE_GDB_PORT}"
echo "OI001_MODE: ${OI001_MODE}（sample=interrupt 采样；hbreak=0x080012E8 入口停机）"
echo ""
if [[ "${OI001_MODE}" == "hbreak" ]]; then
  echo "提示：hbreak 模式将 continue 直至命中 0x080012E8；若超时见 GDB_TIMEOUT 与 04 §1.3.13。"
else
  echo "提示：默认模式为 monitor start → interrupt（见 renode_oi001_capture.gdb 头注释）。"
fi
echo ""

cd "${REPO_ROOT}"

run_gdb() {
  # 减少 tee 日志里的 ANSI / MI 转义（file 被误判为 binary）
  GDB_COLORS=never "${GDB}" -n -q -x "${TMP_GDB}" "$@"
}

# macOS 通常无 GNU timeout；用 python3 子进程超时作后备
run_gdb_timeout() {
  if command -v gtimeout >/dev/null 2>&1; then
    gtimeout "${GDB_TIMEOUT}" run_gdb "$@"
  elif command -v timeout >/dev/null 2>&1; then
    timeout "${GDB_TIMEOUT}" run_gdb "$@"
  elif command -v python3 >/dev/null 2>&1; then
    export _RENODE_TMP_GDB="${TMP_GDB}"
    export _RENODE_GDB_BIN="${GDB}"
    GDB_COLORS=never python3 -c '
import os, subprocess, sys
secs = int(os.environ.get("GDB_TIMEOUT", "180"))
cmd = [os.environ["_RENODE_GDB_BIN"], "-n", "-q", "-x", os.environ["_RENODE_TMP_GDB"]]
try:
    p = subprocess.run(cmd, timeout=secs)
    sys.exit(p.returncode)
except subprocess.TimeoutExpired:
    sys.stderr.write("GDB 超时 %ds（已终止子进程）\n" % secs)
    sys.exit(124)
'
  else
    run_gdb "$@"
  fi
}

if command -v gtimeout >/dev/null 2>&1 || command -v timeout >/dev/null 2>&1 || command -v python3 >/dev/null 2>&1; then
  run_gdb_timeout 2>&1 | tee "${LOG_FILE}"
else
  run_gdb 2>&1 | tee "${LOG_FILE}"
fi

echo ""
echo "完成。本次产物目录: ${OI001_RUN_DIR}"
echo "  - ${LOG_FILE}"
echo "  - ${SRAM_DUMP}"
