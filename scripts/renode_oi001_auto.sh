#!/usr/bin/env bash
# =============================================================================
# 一键：启动 /Applications 下的 Renode（CLI）→ 加载 apm32_f100_gdb_first.resc →
# 运行 renode_gdb_capture.sh（hbreak → **GDB 内** shell 发 start → continue）
#
# 用法：
#   cd /path/to/ZHIYUN_F100
#   ./scripts/renode_oi001_auto.sh
#
# 环境变量：
#   RENODE_BIN          默认 /Applications/Renode.app/Contents/MacOS/renode
#   RENODE_MONITOR_PORT Monitor telnet 端口（默认 49152，须与 capture 内 nc 一致）
#   GDB_TIMEOUT         采集 GDB 超时秒数（默认 300）
#   OI001_MODE          传给 renode_gdb_capture.sh：默认 sample；设为 hbreak 时在 0x080012E8 停机
#   RENODE_ATTACH=1     不启动 Renode；使用已有 :3333（须手动 Monitor start，除非改 capture）
#   RENODE_LOG          Renode stdout/stderr（默认 ${OI001_RUN_DIR}/renode_emulator.log）
#  每次运行创建 logs/renode-oi001/<时间戳>/，并写入 logs/renode-oi001/LATEST 供下游脚本读取。
#
# 前置：arm-none-eabi-gdb；自动模式要求 :3333 未被占用（或 RENODE_ATTACH=1）。
# =============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

RENODE_BIN="${RENODE_BIN:-/Applications/Renode.app/Contents/MacOS/renode}"
RENODE_MONITOR_PORT="${RENODE_MONITOR_PORT:-49152}"
GDB_TIMEOUT="${GDB_TIMEOUT:-300}"
RENODE_GDB_PORT="${RENODE_GDB_PORT:-3333}"

LOGS_ROOT="${REPO_ROOT}/logs/renode-oi001"
TS="$(date +%Y%m%d_%H%M%S)"
RUN_DIR="${LOGS_ROOT}/${TS}"
mkdir -p "${RUN_DIR}"
export OI001_RUN_DIR="${RUN_DIR}"
printf '%s\n' "${RUN_DIR}" > "${LOGS_ROOT}/LATEST"

RENODE_LOG="${RENODE_LOG:-${RUN_DIR}/renode_emulator.log}"
RESC="${REPO_ROOT}/Renode/apm32_f100_gdb_first.resc"
CAPTURE="${SCRIPT_DIR}/renode_gdb_capture.sh"

RENODE_PID=""
export GDB_TIMEOUT
export RENODE_MONITOR_PORT
export OI001_MODE="${OI001_MODE:-sample}"

if [[ ! -x "${RENODE_BIN}" ]]; then
  echo "未找到可执行的 Renode: ${RENODE_BIN}" >&2
  echo "请安装 Renode 或设置 RENODE_BIN=/path/to/renode" >&2
  exit 1
fi

if [[ ! -f "${RESC}" ]]; then
  echo "缺少脚本: ${RESC}" >&2
  exit 1
fi

wait_port() {
  local host="$1" port="$2" label="$3" max="${4:-120}"
  local i=0
  echo "等待 ${label} ${host}:${port} ..."
  while ! nc -z "${host}" "${port}" 2>/dev/null; do
    sleep 0.2
    i=$((i + 1))
    if (( i > max * 5 )); then
      echo "超时：${label} 未在 ${max}s 内就绪" >&2
      return 1
    fi
  done
  echo "  ${label} 已就绪"
}

cleanup_renode() {
  if [[ "${RENODE_KEEP:-0}" == "1" ]]; then
    return 0
  fi
  if [[ -n "${RENODE_PID:-}" ]] && kill -0 "${RENODE_PID}" 2>/dev/null; then
    echo "结束 Renode (pid ${RENODE_PID}) ..."
    kill "${RENODE_PID}" 2>/dev/null || true
    wait "${RENODE_PID}" 2>/dev/null || true
  fi
}
trap cleanup_renode EXIT INT TERM

if [[ "${RENODE_ATTACH:-0}" != "1" ]]; then
  if nc -z 127.0.0.1 "${RENODE_GDB_PORT}" 2>/dev/null; then
    echo "端口 ${RENODE_GDB_PORT} 已被占用。请先关闭已有 Renode，或:" >&2
    echo "  RENODE_ATTACH=1 ./scripts/renode_oi001_auto.sh" >&2
    exit 1
  fi

  echo "启动 Renode: ${RENODE_BIN}"
  echo "  脚本: ${RESC}"
  echo "  Monitor telnet: 127.0.0.1:${RENODE_MONITOR_PORT}  |  GDB: ${RENODE_GDB_PORT}"
  echo "  日志: ${RENODE_LOG}"
  echo "  （采集：OI001_MODE=${OI001_MODE}，见 renode_gdb_capture.sh）"

  "${RENODE_BIN}" \
    --disable-gui \
    --plain \
    -P "${RENODE_MONITOR_PORT}" \
    --pid-file "${RUN_DIR}/renode.pid" \
    "${RESC}" >>"${RENODE_LOG}" 2>&1 &
  RENODE_PID=$!

  wait_port 127.0.0.1 "${RENODE_GDB_PORT}" "GDB server" 90
  wait_port 127.0.0.1 "${RENODE_MONITOR_PORT}" "Monitor (telnet)" 30
else
  echo "RENODE_ATTACH=1：使用已有 Renode（GDB :${RENODE_GDB_PORT}）"
  wait_port 127.0.0.1 "${RENODE_GDB_PORT}" "GDB server" 15
  echo "若采集卡住，请在 Renode Monitor 手动: start（需与 RENODE_MONITOR_PORT=${RENODE_MONITOR_PORT} 一致）"
fi

cd "${REPO_ROOT}"
"${CAPTURE}"

echo ""
echo "本次采集目录: ${RUN_DIR}"
echo "Renode 运行日志: ${RENODE_LOG}"
