#!/usr/bin/env bash
# =============================================================================
# Phase 6 / PD 断点追踪（Renode + 本机 GDB，非 MCP）
#
# 目标：
# - 在 Phase 6 firmware.elf 上对 PD 关键函数下断点
# - 观察是否命中 `PD_GPIO_StateMachine`
# - 记录命中后/中断后的关键 SRAM 与 GPIO 寄存器状态
#
# 用法：
#   ./scripts/renode_phase6_pd_breaktrace.sh
#
# 环境变量：
#   RENODE_BIN           默认 /Applications/Renode.app/Contents/MacOS/renode
#   PHASE6_FW_BIN        默认 .pio/.../firmware.bin
#   PHASE6_FW_ELF        默认 .pio/.../firmware.elf
#   RENODE_GDB_PORT      默认 3333
#   RENODE_MONITOR_PORT  默认 49163
#   TRACE_RUN_SECONDS    默认 3
#   PHASE6_PD_STEP_INSN  默认 48（命中 PD 断点后的单步条数）
#   PHASE6_TRACE_HELPER_BP 默认 0（=1 时额外下断 PD_HelperSeq_3000tick_ThenPc13Gpio）
#   PHASE6_PD_BREAK_IGNORE 默认 0（忽略前 N 次 PD_GPIO_StateMachine 断点命中）
#   PHASE6_PD_SOFT_BP 默认 0（=1 时对 PD_GPIO_StateMachine 使用软件断点 break）
#   PHASE6_TRACE_FLUSH_BP  默认 1（=1 时追加断点 UI_SPI1_PumpEightFramebufferSlices）
#   PHASE6_TRACE_FLUSH_COUNTER_BP 默认 0（可选：在 flush 计数写回点下断；Renode 下可能耗时）
#   PHASE6_FLUSH_STEP_INSN 默认 64（命中 UI flush 断点后的单步条数）
#   PHASE6_FLUSH_REHIT_MAX 默认 0（可选：UI flush 断点重复命中次数上限，用于等待计数跃迁）
#   PHASE6_FORCE_FLUSH_SKIP 默认 0（=1 时强制 UI flush 走 skip 分支）
#   PHASE6_FORCE_FLUSH_TRIGGER 默认 0（=1 时强制 UI flush 走 trigger 分支）
#   PHASE6_FORCE_SPI_STS_READY 默认 0（=1 时将 SPI1->STS 置为 TXBE|RXBNE 以加速触发分支）
#   PHASE6_FORCE_SYSTICK_ENTRY 默认 0（>0 时在 continue 前强制写 g_phase6_systick_entry_count，
#   用于压测 PD_HelperSeq_3000tick_ThenPc13Gpio 的窗口边界）
#   PHASE6_GDB_TIMEOUT_SECONDS 默认 120（GDB 会话硬超时，避免断点等待卡住）
# =============================================================================
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RENODE_BIN="${RENODE_BIN:-/Applications/Renode.app/Contents/MacOS/renode}"
RENODE_GDB_PORT="${RENODE_GDB_PORT:-3333}"
RENODE_MONITOR_PORT="${RENODE_MONITOR_PORT:-49163}"
TRACE_RUN_SECONDS="${TRACE_RUN_SECONDS:-3}"
PHASE6_PD_STEP_INSN="${PHASE6_PD_STEP_INSN:-48}"
PHASE6_TRACE_HELPER_BP="${PHASE6_TRACE_HELPER_BP:-0}"
PHASE6_PD_BREAK_IGNORE="${PHASE6_PD_BREAK_IGNORE:-0}"
PHASE6_PD_SOFT_BP="${PHASE6_PD_SOFT_BP:-1}"
PHASE6_TRACE_FLUSH_BP="${PHASE6_TRACE_FLUSH_BP:-1}"
PHASE6_TRACE_FLUSH_COUNTER_BP="${PHASE6_TRACE_FLUSH_COUNTER_BP:-0}"
PHASE6_FLUSH_STEP_INSN="${PHASE6_FLUSH_STEP_INSN:-64}"
PHASE6_FLUSH_REHIT_MAX="${PHASE6_FLUSH_REHIT_MAX:-0}"
PHASE6_FORCE_FLUSH_SKIP="${PHASE6_FORCE_FLUSH_SKIP:-0}"
PHASE6_FORCE_FLUSH_TRIGGER="${PHASE6_FORCE_FLUSH_TRIGGER:-0}"
PHASE6_FORCE_SPI_STS_READY="${PHASE6_FORCE_SPI_STS_READY:-0}"
PHASE6_FORCE_SYSTICK_ENTRY="${PHASE6_FORCE_SYSTICK_ENTRY:-0}"
PHASE6_GDB_TIMEOUT_SECONDS="${PHASE6_GDB_TIMEOUT_SECONDS:-120}"
GDB="${GDB:-arm-none-eabi-gdb}"
PHASE6_FW_BIN="${PHASE6_FW_BIN:-${ROOT}/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.bin}"
PHASE6_FW_ELF="${PHASE6_FW_ELF:-${ROOT}/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf}"

LOGS_ROOT="${ROOT}/logs/renode-phase6"
TS="$(date +%Y%m%d_%H%M%S)"
RUN_DIR="${LOGS_ROOT}/${TS}"
mkdir -p "${RUN_DIR}"
printf '%s\n' "${RUN_DIR}" > "${LOGS_ROOT}/LATEST"

RENODE_LOG="${RUN_DIR}/renode_emulator.log"
GDB_LOG="${RUN_DIR}/gdb_phase6_pd_breaktrace.log"
RESC_TMP="${RUN_DIR}/phase6_dynamic.resc"
NM_TXT="${RUN_DIR}/phase6_symbols_nm.txt"

if [[ ! -f "${PHASE6_FW_BIN}" || ! -f "${PHASE6_FW_ELF}" ]]; then
  echo "缺少 Phase 6 构建产物，请先在 zhiyun_f100_apm32 执行 pio run。" >&2
  exit 1
fi
if [[ ! -x "${RENODE_BIN}" ]]; then
  echo "未找到 Renode 可执行文件: ${RENODE_BIN}" >&2
  exit 1
fi
if ! command -v "${GDB}" >/dev/null 2>&1; then
  echo "未找到 ${GDB}" >&2
  exit 1
fi

read_vector_words() {
  python3 - "$PHASE6_FW_BIN" <<'PY'
import struct, sys
with open(sys.argv[1], "rb") as f:
    data = f.read(8)
sp, pc = struct.unpack("<II", data)
print(hex(sp), hex(pc))
PY
}
read -r VEC_SP VEC_PC < <(read_vector_words)
arm-none-eabi-nm "${PHASE6_FW_ELF}" > "${NM_TXT}"

symbol_addr_hex() {
  local sym="$1"
  awk -v s="$sym" '$3==s {print "0x"$1; exit}' "${NM_TXT}"
}

USB_ATTACHED_ADDR="$(symbol_addr_hex g_phase6_usb_attached)"
PROTO_COUNT_ADDR="$(symbol_addr_hex g_phase6_protocol_lane_expand_count)"
PD_GPIO_COUNT_ADDR="$(symbol_addr_hex g_phase6_pd_gpio_count)"
PD_ATTACH_WINDOW_ADDR="$(symbol_addr_hex g_phase6_pd_attach_window_count)"
PB3_STATE_ADDR="$(symbol_addr_hex g_phase6_pd_pb3_state)"
PB11_STATE_ADDR="$(symbol_addr_hex g_phase6_pd_pb11_state)"
LAST_STAGE_ADDR="$(symbol_addr_hex g_phase6_last_stage)"
SPI_FLUSH_TRIGGER_ADDR="$(symbol_addr_hex g_phase6_spi_flush_trigger_count)"
SPI_FLUSH_SKIP_ADDR="$(symbol_addr_hex g_phase6_spi_flush_skip_count)"
CALL_TICKS_ADDR="$(symbol_addr_hex g_phase6_call_ticks)"
SYSTICK_ENTRY_ADDR="$(symbol_addr_hex g_phase6_systick_entry_count)"
OLED_LAST_FLUSH_TICK_ADDR="$(symbol_addr_hex g_phase6_oled_last_flush_tick)"
UI_FRAME_DIRTY_ADDR="$(symbol_addr_hex g_phase6_ui_frame_dirty)"
MAIN_STEP8_ADDR="$(symbol_addr_hex g_phase6_main_boot_phase_counter8)"
MAIN_IWDG_RELOAD_ADDR="$(symbol_addr_hex g_phase6_main_iwdg_reload_count)"
MAIN_SPI_REQ_ADDR="$(symbol_addr_hex g_phase6_mainloop_spi_pump_request_count)"
MAIN_SPI_CONSUMED_ADDR="$(symbol_addr_hex g_phase6_mainloop_spi_pump_consumed_count)"
STARTUP_MAGIC_ADDR="$(symbol_addr_hex g_phase6_boot_magic_word_shadow)"
STARTUP_MAGIC_MATCH_ADDR="$(symbol_addr_hex g_phase6_boot_magic_matched)"
STARTUP_IWDT_STARTED_ADDR="$(symbol_addr_hex g_phase6_iwdt_started)"
STARTUP_IWDT_RELOAD_ADDR="$(symbol_addr_hex g_phase6_iwdt_reload_shadow)"
USB_GUARD_INIT_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_guard_init)"
USB_PREFLIGHT_READY_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_preflight_ready)"
USB_PREFLIGHT_FAIL_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_preflight_fail_count)"
USB_FLASH_ATTEMPT_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_flash_write_attempt_count)"
if [[ -z "${USB_ATTACHED_ADDR}" || -z "${PROTO_COUNT_ADDR}" || -z "${PD_GPIO_COUNT_ADDR}" || -z "${PD_ATTACH_WINDOW_ADDR}" || -z "${PB3_STATE_ADDR}" || -z "${PB11_STATE_ADDR}" || -z "${LAST_STAGE_ADDR}" || -z "${SPI_FLUSH_TRIGGER_ADDR}" || -z "${SPI_FLUSH_SKIP_ADDR}" ]]; then
  echo "缺少关键符号地址，无法执行断点追踪。" >&2
  exit 1
fi
if [[ -z "${CALL_TICKS_ADDR}" || -z "${SYSTICK_ENTRY_ADDR}" || -z "${OLED_LAST_FLUSH_TICK_ADDR}" || -z "${UI_FRAME_DIRTY_ADDR}" ]]; then
  echo "缺少 flush 控制符号地址（call_ticks/systick_entry_count/oled_last_flush_tick/ui_frame_dirty）。" >&2
  exit 1
fi
if [[ -z "${MAIN_STEP8_ADDR}" || -z "${MAIN_IWDG_RELOAD_ADDR}" ]]; then
  echo "缺少 main 周期语义符号地址（step8/iwdg_reload）。" >&2
  exit 1
fi
if [[ -z "${MAIN_SPI_REQ_ADDR}" || -z "${MAIN_SPI_CONSUMED_ADDR}" ]]; then
  echo "缺少 main->SPI 帧泵桥接符号地址（mainloop_spi_pump_*）。" >&2
  exit 1
fi
if [[ -z "${STARTUP_MAGIC_ADDR}" || -z "${STARTUP_MAGIC_MATCH_ADDR}" || -z "${STARTUP_IWDT_STARTED_ADDR}" || -z "${STARTUP_IWDT_RELOAD_ADDR}" ]]; then
  echo "缺少启动链补强符号地址（magic55aa/iwdt_*）。" >&2
  exit 1
fi
if [[ -z "${USB_GUARD_INIT_ADDR}" || -z "${USB_PREFLIGHT_READY_ADDR}" || -z "${USB_PREFLIGHT_FAIL_ADDR}" || -z "${USB_FLASH_ATTEMPT_ADDR}" ]]; then
  echo "缺少 USB guard 风险符号地址（guard/preflight/flash_attempt）。" >&2
  exit 1
fi

cat > "${RESC_TMP}" <<EOF
using sysbus
mach create "ZHIYUN_F100_PHASE6"
machine LoadPlatformDescription @platforms/cpus/stm32f103.repl
sysbus LoadBinary @${PHASE6_FW_BIN} 0x08000000
cpu SP ${VEC_SP}
cpu PC ${VEC_PC}
machine StartGdbServer ${RENODE_GDB_PORT}
logLevel 3
EOF

RENODE_PID=""
cleanup() {
  if [[ -n "${RENODE_PID}" ]] && kill -0 "${RENODE_PID}" 2>/dev/null; then
    kill "${RENODE_PID}" 2>/dev/null || true
    wait "${RENODE_PID}" 2>/dev/null || true
  fi
}
trap cleanup EXIT INT TERM

wait_port() {
  local host="$1" port="$2" max="${3:-40}"
  local i=0
  while ! nc -z "${host}" "${port}" 2>/dev/null; do
    sleep 0.2
    i=$((i + 1))
    if (( i > max * 5 )); then
      echo "端口未就绪: ${host}:${port}" >&2
      return 1
    fi
  done
}

if nc -z 127.0.0.1 "${RENODE_GDB_PORT}" 2>/dev/null; then
  echo "端口 ${RENODE_GDB_PORT} 已占用，请先关闭已有 Renode/GDB。" >&2
  exit 1
fi

"${RENODE_BIN}" \
  --disable-gui \
  --plain \
  -P "${RENODE_MONITOR_PORT}" \
  --pid-file "${RUN_DIR}/renode.pid" \
  "${RESC_TMP}" >> "${RENODE_LOG}" 2>&1 &
RENODE_PID=$!
wait_port 127.0.0.1 "${RENODE_GDB_PORT}"

TMP_GDB="${RUN_DIR}/phase6_pd_breaktrace.gdb"
HELPER_BP_CMD=""
FLUSH_BP_CMD=""
FLUSH_COUNTER_BP_CMD=""
FLUSH_BP_TRACE_BLOCK=""
FLUSH_FORCE_PREP_BLOCK=""
if [[ "${PHASE6_TRACE_HELPER_BP}" == "1" ]]; then
  HELPER_BP_CMD=$'hbreak PD_HelperSeq_3000tick_ThenPc13Gpio\nset $bp_helper = $bpnum'
fi

if [[ "${PHASE6_FORCE_FLUSH_SKIP}" == "1" && "${PHASE6_FORCE_FLUSH_TRIGGER}" == "1" ]]; then
  echo "PHASE6_FORCE_FLUSH_SKIP 与 PHASE6_FORCE_FLUSH_TRIGGER 不能同时为 1。" >&2
  exit 1
fi
if [[ "${PHASE6_FORCE_FLUSH_SKIP}" == "1" ]]; then
  FLUSH_FORCE_PREP_BLOCK=$(cat <<EOF
set {unsigned char}${UI_FRAME_DIRTY_ADDR} = 0
set {unsigned int}${CALL_TICKS_ADDR} = 1
set {unsigned int}${OLED_LAST_FLUSH_TICK_ADDR} = 0
echo === Force flush branch: SKIP ===\n
EOF
)
fi
if [[ "${PHASE6_FORCE_FLUSH_TRIGGER}" == "1" ]]; then
  FLUSH_FORCE_PREP_BLOCK=$(cat <<EOF
set {unsigned char}${UI_FRAME_DIRTY_ADDR} = 1
echo === Force flush branch: TRIGGER ===\n
EOF
)
fi

if [[ "${PHASE6_TRACE_FLUSH_BP}" == "1" ]]; then
  FLUSH_BP_CMD=$'hbreak UI_SPI1_PumpEightFramebufferSlices\nset $bp_flush = $bpnum'
  if [[ "${PHASE6_TRACE_FLUSH_COUNTER_BP}" == "1" ]]; then
    FLUSH_COUNTER_BP_CMD=$'break *UI_SPI1_PumpEightFramebufferSlices+0x84\nset $bp_flush_post_trigger = $bpnum\nbreak *UI_SPI1_PumpEightFramebufferSlices+0xa8\nset $bp_flush_post_skip = $bpnum'
  fi
  FLUSH_BP_TRACE_BLOCK=$(cat <<EOF
disable \$bp_pd
if \$bp_helper > 0
  disable \$bp_helper
end
${FLUSH_FORCE_PREP_BLOCK}
echo === Continue to UI flush breakpoint ===\n
continue
${FLUSH_FORCE_PREP_BLOCK}
if ${PHASE6_FORCE_SPI_STS_READY} == 1
  set {unsigned short}0x40013008 = 0x0003
  echo === SPI1->STS forced TXBE|RXBNE ===\n
end
if \$bp_flush_post_trigger > 0
  disable \$bp_flush
  continue
end
set \$flush_trigger_before = *((unsigned int*)${SPI_FLUSH_TRIGGER_ADDR})
set \$flush_skip_before = *((unsigned int*)${SPI_FLUSH_SKIP_ADDR})
set \$flush_rehit_left = ${PHASE6_FLUSH_REHIT_MAX}
while (\$flush_rehit_left > 0) && (*((unsigned int*)${SPI_FLUSH_TRIGGER_ADDR}) == \$flush_trigger_before) && (*((unsigned int*)${SPI_FLUSH_SKIP_ADDR}) == \$flush_skip_before)
  continue
  set \$flush_rehit_left = \$flush_rehit_left - 1
end
set \$j = 0
while \$j < ${PHASE6_FLUSH_STEP_INSN}
  stepi
  set \$j = \$j + 1
end
echo === Flush breakpoint snapshot (rehit+post-step ${PHASE6_FLUSH_STEP_INSN}) ===\n
info registers pc sp lr
x/1wx ${LAST_STAGE_ADDR}
x/1wx ${SPI_FLUSH_TRIGGER_ADDR}
x/1wx ${SPI_FLUSH_SKIP_ADDR}
EOF
)
fi

cat > "${TMP_GDB}" <<EOF
set pagination off
set confirm off
set breakpoint pending on
file ${PHASE6_FW_ELF}
target extended-remote localhost:${RENODE_GDB_PORT}
if ${PHASE6_PD_SOFT_BP} == 1
  break PD_GPIO_StateMachine
else
  hbreak PD_GPIO_StateMachine
end
set \$bp_pd = \$bpnum
if ${PHASE6_PD_BREAK_IGNORE} > 0
  ignore \$bp_pd ${PHASE6_PD_BREAK_IGNORE}
end
set \$bp_helper = 0
set \$bp_flush = 0
set \$bp_flush_post_trigger = 0
set \$bp_flush_post_skip = 0
${HELPER_BP_CMD}
${FLUSH_BP_CMD}
${FLUSH_COUNTER_BP_CMD}
set {unsigned int}${USB_ATTACHED_ADDR} = 1
set {unsigned int}${PROTO_COUNT_ADDR} = 1
if (${PHASE6_FORCE_SYSTICK_ENTRY} > 0)
  set {unsigned int}${SYSTICK_ENTRY_ADDR} = ${PHASE6_FORCE_SYSTICK_ENTRY}
end
monitor start
continue
echo === Breaktrace snapshot (entry) ===\n
info breakpoints
info registers pc sp lr
echo === GPIO register view ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view ===\n
x/1wx ${USB_ATTACHED_ADDR}
x/1wx ${PROTO_COUNT_ADDR}
x/1wx ${PD_GPIO_COUNT_ADDR}
x/1wx ${PD_ATTACH_WINDOW_ADDR}
x/1bx ${PB3_STATE_ADDR}
x/1bx ${PB11_STATE_ADDR}
x/1wx ${LAST_STAGE_ADDR}
x/1wx ${SPI_FLUSH_TRIGGER_ADDR}
x/1wx ${SPI_FLUSH_SKIP_ADDR}
echo === Main semantic counters (entry) ===\n
x/1wx ${MAIN_STEP8_ADDR}
x/1wx ${MAIN_IWDG_RELOAD_ADDR}
echo === main->SPI bridge counters (entry) ===\n
x/1wx ${MAIN_SPI_REQ_ADDR}
x/1wx ${MAIN_SPI_CONSUMED_ADDR}
echo === startup chain magic/IWDT (entry) ===\n
x/1wx ${STARTUP_MAGIC_ADDR}
x/1wx ${STARTUP_MAGIC_MATCH_ADDR}
x/1wx ${STARTUP_IWDT_STARTED_ADDR}
x/1wx ${STARTUP_IWDT_RELOAD_ADDR}
echo === USB guard risk counters (entry) ===\n
x/1wx ${USB_GUARD_INIT_ADDR}
x/1wx ${USB_PREFLIGHT_READY_ADDR}
x/1wx ${USB_PREFLIGHT_FAIL_ADDR}
x/1wx ${USB_FLASH_ATTEMPT_ADDR}
echo === Flush cadence context (entry) ===\n
x/1wx ${CALL_TICKS_ADDR}
x/1wx ${SYSTICK_ENTRY_ADDR}
x/1wx ${OLED_LAST_FLUSH_TICK_ADDR}
x/1bx ${UI_FRAME_DIRTY_ADDR}
set \$i = 0
while \$i < ${PHASE6_PD_STEP_INSN}
  stepi
  set \$i = \$i + 1
end
echo === Breaktrace snapshot (post-step ${PHASE6_PD_STEP_INSN}) ===\n
info registers pc sp lr
echo === GPIO register view (post-step) ===\n
x/1wx 0x40011008
x/1wx 0x40010C0C
x/1wx 0x40010C10
x/1wx 0x40010C14
echo === PD shadow/counter view (post-step) ===\n
x/1wx ${USB_ATTACHED_ADDR}
x/1wx ${PROTO_COUNT_ADDR}
x/1wx ${PD_GPIO_COUNT_ADDR}
x/1wx ${PD_ATTACH_WINDOW_ADDR}
x/1bx ${PB3_STATE_ADDR}
x/1bx ${PB11_STATE_ADDR}
x/1wx ${LAST_STAGE_ADDR}
x/1wx ${SPI_FLUSH_TRIGGER_ADDR}
x/1wx ${SPI_FLUSH_SKIP_ADDR}
echo === Main semantic counters (post-step) ===\n
x/1wx ${MAIN_STEP8_ADDR}
x/1wx ${MAIN_IWDG_RELOAD_ADDR}
echo === main->SPI bridge counters (post-step) ===\n
x/1wx ${MAIN_SPI_REQ_ADDR}
x/1wx ${MAIN_SPI_CONSUMED_ADDR}
echo === startup chain magic/IWDT (post-step) ===\n
x/1wx ${STARTUP_MAGIC_ADDR}
x/1wx ${STARTUP_MAGIC_MATCH_ADDR}
x/1wx ${STARTUP_IWDT_STARTED_ADDR}
x/1wx ${STARTUP_IWDT_RELOAD_ADDR}
echo === USB guard risk counters (post-step) ===\n
x/1wx ${USB_GUARD_INIT_ADDR}
x/1wx ${USB_PREFLIGHT_READY_ADDR}
x/1wx ${USB_PREFLIGHT_FAIL_ADDR}
x/1wx ${USB_FLASH_ATTEMPT_ADDR}
echo === Flush cadence context (post-step) ===\n
x/1wx ${CALL_TICKS_ADDR}
x/1wx ${SYSTICK_ENTRY_ADDR}
x/1wx ${OLED_LAST_FLUSH_TICK_ADDR}
x/1bx ${UI_FRAME_DIRTY_ADDR}
${FLUSH_BP_TRACE_BLOCK}
detach
quit
EOF

python3 - "${PHASE6_GDB_TIMEOUT_SECONDS}" "${GDB}" "${TMP_GDB}" <<'PY' 2>&1 | tee "${GDB_LOG}"
import os
import subprocess
import sys

timeout_s = int(sys.argv[1])
gdb_bin = sys.argv[2]
gdb_script = sys.argv[3]

env = os.environ.copy()
env["GDB_COLORS"] = "never"
cmd = [gdb_bin, "-n", "-q", "-x", gdb_script]

try:
    result = subprocess.run(cmd, env=env, timeout=timeout_s, check=False)
except subprocess.TimeoutExpired:
    print(f"GDB 会话超时（>{timeout_s}s），已主动终止以避免卡住。", file=sys.stderr)
    sys.exit(124)

sys.exit(result.returncode)
PY

echo ""
echo "完成。"
echo "  run_dir: ${RUN_DIR}"
echo "  gdb_log: ${GDB_LOG}"
echo "  renode_log: ${RENODE_LOG}"
