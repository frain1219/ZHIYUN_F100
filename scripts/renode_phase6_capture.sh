#!/usr/bin/env bash
# =============================================================================
# Phase 6 固件 Renode 采样脚本（非 MCP）
#
# 目标：
# - 直接加载 zhiyun_f100_apm32 的 firmware.bin
# - 启动 Renode + GDB，采样与 Phase 6 相关的关键寄存器
# - 导出 SRAM 20KB 供后续比对
#
# 用法（在仓库根目录）：
#   ./scripts/renode_phase6_capture.sh
#
# 环境变量：
#   RENODE_BIN           默认 /Applications/Renode.app/Contents/MacOS/renode
#   PHASE6_FW_BIN        默认 ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.bin
#   PHASE6_FW_ELF        默认 ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf
#   RENODE_GDB_PORT      默认 3333
#   RENODE_MONITOR_PORT  默认 49162
#   PHASE6_RUN_SECONDS   默认 1（monitor start 后虚拟运行秒数）
#   PHASE6_STIM_USB      默认 1（通过 GDB 调用注入 USB 插入事件）
#   PHASE6_FORCE_PD      默认 1（若 USB 注入不可达，则强制置 usb_attached 观察 PD 输出支路）
#   PHASE6_CAPTURE_BREAK_ON_PD 默认 1（在 PD_GPIO_StateMachine 断点停机后采样，避免停在启动链）
#   PHASE6_CAPTURE_PD_IGNORE 默认 0（PD_GPIO_StateMachine 忽略前 N 次命中；>0 时用于停在第 N+1 次进入，
#   以便 scheduler_ticks 跨过 PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS(100)。注意：若首个 RunOnce 无法在 Renode
#   内完成，忽略计数可能永远达不到 — 需配合 PHASE6_RENODE_SPI_FALLBACK_INJECT 或调大 PHASE6_GDB_TIMEOUT_SECONDS）
#   PHASE6_POST_BREAK_RUN_SECONDS 默认 2（命中 PD 断点后继续虚拟运行再采样；与 SPI fallback 注入联用可使
#   main_step8 / main_iwdg_reload 在标准快照中非零）
#   PHASE6_RENODE_SPI_FALLBACK_INJECT 默认 1（命中 PD 断点后、继续运行前将 g_phase6_spi1_status_fallback_mode
#   置 1，与固件 Phase6_SPI1_Transmit 中 Renode STS 降级路径一致，避免首个 RunOnce 卡在 SPI 轮询。
#   实机对照或怀疑注入影响结论时设 0）
#   PHASE6_RENODE_SPI_STS_FORCE 默认 1（写 SPI1->STS @ 0x40013008 为 TXBE|RXBNE，与 renode_phase6_pd_breaktrace.sh
#   的 PHASE6_FORCE_SPI_STS_READY 一致；Renode 模型下可解除 TX/RX 位等待阻塞）
#   PHASE6_GDB_TIMEOUT_SECONDS 默认 90（GDB 会话硬超时，避免长时间卡住）
#
# 时基对照（vs OEM PD_PeriodicDispatchFromSysTick /20）：
#   GDB 日志含 g_phase6_scheduler_ticks 与 g_phase6_call_ticks 同窗及 SysTick 寄存器
#   （0xE000E010..18）；见 docs/phase6_mmio_mapping.md「OEM 时基对照」。
# =============================================================================
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RENODE_BIN="${RENODE_BIN:-/Applications/Renode.app/Contents/MacOS/renode}"
RENODE_GDB_PORT="${RENODE_GDB_PORT:-3333}"
RENODE_MONITOR_PORT="${RENODE_MONITOR_PORT:-49162}"
PHASE6_RUN_SECONDS="${PHASE6_RUN_SECONDS:-1}"
PHASE6_STIM_USB="${PHASE6_STIM_USB:-1}"
PHASE6_FORCE_PD="${PHASE6_FORCE_PD:-1}"
PHASE6_CAPTURE_BREAK_ON_PD="${PHASE6_CAPTURE_BREAK_ON_PD:-1}"
PHASE6_POST_BREAK_RUN_SECONDS="${PHASE6_POST_BREAK_RUN_SECONDS:-2}"
PHASE6_CAPTURE_PD_IGNORE="${PHASE6_CAPTURE_PD_IGNORE:-0}"
PHASE6_RENODE_SPI_FALLBACK_INJECT="${PHASE6_RENODE_SPI_FALLBACK_INJECT:-1}"
PHASE6_RENODE_SPI_STS_FORCE="${PHASE6_RENODE_SPI_STS_FORCE:-1}"
PHASE6_GDB_TIMEOUT_SECONDS="${PHASE6_GDB_TIMEOUT_SECONDS:-90}"
GDB="${GDB:-arm-none-eabi-gdb}"
PHASE6_FW_BIN="${PHASE6_FW_BIN:-${ROOT}/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.bin}"
PHASE6_FW_ELF="${PHASE6_FW_ELF:-${ROOT}/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.elf}"

LOGS_ROOT="${ROOT}/logs/renode-phase6"
TS="$(date +%Y%m%d_%H%M%S)"
RUN_DIR="${LOGS_ROOT}/${TS}"
mkdir -p "${RUN_DIR}"
printf '%s\n' "${RUN_DIR}" > "${LOGS_ROOT}/LATEST"

RENODE_LOG="${RUN_DIR}/renode_emulator.log"
GDB_LOG="${RUN_DIR}/gdb_phase6_capture.log"
SRAM_BIN="${RUN_DIR}/phase6_sram_20k.bin"
RESC_TMP="${RUN_DIR}/phase6_dynamic.resc"
NM_TXT="${RUN_DIR}/phase6_symbols_nm.txt"

if [[ ! -f "${PHASE6_FW_BIN}" ]]; then
  echo "缺少固件: ${PHASE6_FW_BIN}" >&2
  echo "请先在 zhiyun_f100_apm32 工程执行: pio run" >&2
  exit 1
fi
if [[ ! -f "${PHASE6_FW_ELF}" ]]; then
  echo "缺少符号文件: ${PHASE6_FW_ELF}" >&2
  echo "请先在 zhiyun_f100_apm32 工程执行: pio run" >&2
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
path = sys.argv[1]
with open(path, 'rb') as f:
    data = f.read(8)
if len(data) < 8:
    raise SystemExit("firmware.bin too small")
sp, pc = struct.unpack('<II', data)
print(hex(sp), hex(pc))
PY
}

read -r VEC_SP VEC_PC < <(read_vector_words)
arm-none-eabi-nm "${PHASE6_FW_ELF}" > "${NM_TXT}"

symbol_addr_hex() {
  local sym="$1"
  awk -v s="$sym" '$3==s {print "0x"$1; exit}' "${NM_TXT}"
}

SCHED_INPUT_ADDR="$(symbol_addr_hex g_phase6_scheduler_input)"
SCHED_DIRTY_ADDR="$(symbol_addr_hex g_phase6_scheduler_input_dirty)"
PENDING_INPUT_ADDR="$(symbol_addr_hex g_phase6_pending_input)"
HAS_PENDING_ADDR="$(symbol_addr_hex g_phase6_has_pending_input)"
PB3_STATE_ADDR="$(symbol_addr_hex g_phase6_pd_pb3_state)"
PB11_STATE_ADDR="$(symbol_addr_hex g_phase6_pd_pb11_state)"
USB_ATTACHED_ADDR="$(symbol_addr_hex g_phase6_usb_attached)"
USB_EVT_COUNT_ADDR="$(symbol_addr_hex g_phase6_usb_insert_event_count)"
PROTOCOL_EXPAND_ADDR="$(symbol_addr_hex g_phase6_protocol_lane_expand_count)"
PD_GPIO_COUNT_ADDR="$(symbol_addr_hex g_phase6_pd_gpio_count)"
SPI_STARTUP_TX_COUNT_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_tx_count)"
SPI_STARTUP_DONE_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_done)"
SPI_STARTUP_LAST_BYTE_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_last_byte)"
SPI_STARTUP_CS_LOW_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_cs_low_count)"
SPI_STARTUP_CS_HIGH_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_cs_high_count)"
SPI_STARTUP_WAIT_TIMEOUT_ADDR="$(symbol_addr_hex g_phase6_spi1_startup_wait_timeout_count)"
SPI_FLUSH_TRIGGER_ADDR="$(symbol_addr_hex g_phase6_spi_flush_trigger_count)"
SPI_FLUSH_SKIP_ADDR="$(symbol_addr_hex g_phase6_spi_flush_skip_count)"
MAIN_STEP8_ADDR="$(symbol_addr_hex g_phase6_main_boot_phase_counter8)"
MAIN_IWDG_RELOAD_ADDR="$(symbol_addr_hex g_phase6_main_iwdg_reload_count)"
STARTUP_MAGIC_ADDR="$(symbol_addr_hex g_phase6_boot_magic_word_shadow)"
STARTUP_MAGIC_MATCH_ADDR="$(symbol_addr_hex g_phase6_boot_magic_matched)"
STARTUP_IWDT_STARTED_ADDR="$(symbol_addr_hex g_phase6_iwdt_started)"
STARTUP_IWDT_RELOAD_ADDR="$(symbol_addr_hex g_phase6_iwdt_reload_shadow)"
CALL_TICKS_ADDR="$(symbol_addr_hex g_phase6_call_ticks)"
SCHED_TICKS_ADDR="$(symbol_addr_hex g_phase6_scheduler_ticks)"
SYSTICK_ENTRY_ADDR="$(symbol_addr_hex g_phase6_systick_entry_count)"
OLED_LAST_FLUSH_TICK_ADDR="$(symbol_addr_hex g_phase6_oled_last_flush_tick)"
UI_FRAME_DIRTY_ADDR="$(symbol_addr_hex g_phase6_ui_frame_dirty)"
MAIN_SPI_REQ_ADDR="$(symbol_addr_hex g_phase6_mainloop_spi_pump_request_count)"
MAIN_SPI_CONSUMED_ADDR="$(symbol_addr_hex g_phase6_mainloop_spi_pump_consumed_count)"
USB_GUARD_INIT_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_guard_init)"
USB_PREFLIGHT_READY_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_preflight_ready)"
USB_PREFLIGHT_FAIL_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_preflight_fail_count)"
USB_FLASH_ATTEMPT_ADDR="$(symbol_addr_hex g_phase6_usb_upgrade_flash_write_attempt_count)"
PD_GPIO_FUNC_ADDR="$(symbol_addr_hex PD_GPIO_StateMachine)"
SPI_FALLBACK_MODE_ADDR="$(symbol_addr_hex g_phase6_spi1_status_fallback_mode)"

if [[ -z "${PB3_STATE_ADDR}" || -z "${PB11_STATE_ADDR}" || -z "${USB_ATTACHED_ADDR}" || -z "${USB_EVT_COUNT_ADDR}" || -z "${PROTOCOL_EXPAND_ADDR}" || -z "${PD_GPIO_COUNT_ADDR}" ]]; then
  echo "无法定位 Phase6 关键状态符号（pd_pb3/pd_pb11/usb_*），请先确认 firmware.elf 未损坏。" >&2
  exit 1
fi
if [[ -z "${SPI_STARTUP_TX_COUNT_ADDR}" || -z "${SPI_STARTUP_DONE_ADDR}" || -z "${SPI_STARTUP_LAST_BYTE_ADDR}" || -z "${SPI_STARTUP_CS_LOW_ADDR}" || -z "${SPI_STARTUP_CS_HIGH_ADDR}" || -z "${SPI_STARTUP_WAIT_TIMEOUT_ADDR}" ]]; then
  echo "无法定位 SPI1 启动帧状态符号（spi1_startup_*），请先确认 firmware.elf 未损坏。" >&2
  exit 1
fi
if [[ -z "${SPI_FLUSH_TRIGGER_ADDR}" || -z "${SPI_FLUSH_SKIP_ADDR}" ]]; then
  echo "无法定位 SPI1 运行期帧泵状态符号（spi_flush_trigger/skip），请先确认 firmware.elf 未损坏。" >&2
  exit 1
fi
if [[ -z "${MAIN_STEP8_ADDR}" || -z "${MAIN_IWDG_RELOAD_ADDR}" ]]; then
  echo "无法定位 main 周期语义符号（step8/iwdg_reload），请先确认 firmware.elf 未损坏。" >&2
  exit 1
fi
if [[ -z "${STARTUP_MAGIC_ADDR}" || -z "${STARTUP_MAGIC_MATCH_ADDR}" || -z "${STARTUP_IWDT_STARTED_ADDR}" || -z "${STARTUP_IWDT_RELOAD_ADDR}" ]]; then
  echo "无法定位启动链补强符号（magic55aa/iwdt_*），请先确认 firmware.elf 未损坏。" >&2
  exit 1
fi
if [[ -z "${CALL_TICKS_ADDR}" || -z "${SCHED_TICKS_ADDR}" || -z "${SYSTICK_ENTRY_ADDR}" || -z "${OLED_LAST_FLUSH_TICK_ADDR}" || -z "${UI_FRAME_DIRTY_ADDR}" ]]; then
  echo "无法定位 SPI1 帧泵节拍辅助符号（scheduler_ticks/call_ticks/systick_entry_count/oled_last_flush_tick/ui_frame_dirty）。" >&2
  exit 1
fi
if [[ -z "${MAIN_SPI_REQ_ADDR}" || -z "${MAIN_SPI_CONSUMED_ADDR}" ]]; then
  echo "无法定位 main->SPI 帧泵请求符号（mainloop_spi_pump_*）。" >&2
  exit 1
fi
if [[ -z "${USB_GUARD_INIT_ADDR}" || -z "${USB_PREFLIGHT_READY_ADDR}" || -z "${USB_PREFLIGHT_FAIL_ADDR}" || -z "${USB_FLASH_ATTEMPT_ADDR}" ]]; then
  echo "无法定位 USB guard 风险符号（guard/preflight/flash_attempt）。" >&2
  exit 1
fi
if [[ "${PHASE6_CAPTURE_BREAK_ON_PD}" == "1" && -z "${PD_GPIO_FUNC_ADDR}" ]]; then
  echo "无法定位 PD_GPIO_StateMachine 符号地址，无法启用按 PD 断点采样。" >&2
  exit 1
fi
if [[ "${PHASE6_RENODE_SPI_FALLBACK_INJECT}" == "1" && -z "${SPI_FALLBACK_MODE_ADDR}" ]]; then
  echo "无法定位 g_phase6_spi1_status_fallback_mode，请关闭 PHASE6_RENODE_SPI_FALLBACK_INJECT=0。" >&2
  exit 1
fi

if [[ "${PHASE6_STIM_USB}" == "1" ]]; then
  if [[ -z "${PENDING_INPUT_ADDR}" || -z "${HAS_PENDING_ADDR}" ]]; then
    echo "无法定位 USB 注入所需符号（g_phase6_pending_input / has_pending）" >&2
    exit 1
  fi
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
  local host="$1" port="$2" name="$3" max="${4:-40}"
  local i=0
  while ! nc -z "${host}" "${port}" 2>/dev/null; do
    sleep 0.2
    i=$((i + 1))
    if (( i > max * 5 )); then
      echo "超时：${name} 未就绪 ${host}:${port}" >&2
      return 1
    fi
  done
}

if nc -z 127.0.0.1 "${RENODE_GDB_PORT}" 2>/dev/null; then
  echo "端口 ${RENODE_GDB_PORT} 已被占用，请先关闭已有 Renode/GDB stub。" >&2
  exit 1
fi

"${RENODE_BIN}" \
  --disable-gui \
  --plain \
  -P "${RENODE_MONITOR_PORT}" \
  --pid-file "${RUN_DIR}/renode.pid" \
  "${RESC_TMP}" >> "${RENODE_LOG}" 2>&1 &
RENODE_PID=$!

wait_port 127.0.0.1 "${RENODE_GDB_PORT}" "GDB server"

TMP_GDB="${RUN_DIR}/phase6_capture.gdb"
cat > "${TMP_GDB}" <<EOF
set pagination off
set confirm off
file ${PHASE6_FW_ELF}
target extended-remote localhost:${RENODE_GDB_PORT}
monitor start
shell sleep ${PHASE6_RUN_SECONDS}
interrupt
EOF

if [[ "${PHASE6_STIM_USB}" == "1" ]]; then
cat >> "${TMP_GDB}" <<EOF
echo === Stimulate USB insert via LightPd pending-event SRAM injection ===\n
set {unsigned char}(${PENDING_INPUT_ADDR} + 6) = 1
set {unsigned char}(${PENDING_INPUT_ADDR} + 7) = 1
set {unsigned char}(${PENDING_INPUT_ADDR} + 8) = 1
set {unsigned char}(${PENDING_INPUT_ADDR} + 9) = 1
set {unsigned char}(${PENDING_INPUT_ADDR} + 10) = 1
set {unsigned char}(${PENDING_INPUT_ADDR} + 11) = 1
set {unsigned char}${HAS_PENDING_ADDR} = 1
EOF
if [[ "${PHASE6_FORCE_PD}" == "1" ]]; then
cat >> "${TMP_GDB}" <<EOF
echo === Force PD attached branch (fallback) ===\n
set {unsigned int}${USB_ATTACHED_ADDR} = 1
set {unsigned int}${PROTOCOL_EXPAND_ADDR} = 1
EOF
fi
cat >> "${TMP_GDB}" <<EOF
monitor start
shell sleep ${PHASE6_RUN_SECONDS}
interrupt
EOF
fi

cat >> "${TMP_GDB}" <<EOF
if (${PHASE6_CAPTURE_BREAK_ON_PD} == 1)
  hbreak PD_GPIO_StateMachine
  set \$bp_pd_capture = \$bpnum
  if (${PHASE6_CAPTURE_PD_IGNORE} > 0)
    ignore \$bp_pd_capture ${PHASE6_CAPTURE_PD_IGNORE}
  end
  continue
  if (${PHASE6_RENODE_SPI_FALLBACK_INJECT} == 1)
    echo === Renode: g_phase6_spi1_status_fallback_mode=1 (SPI STS fast path, see phase6_startup_chain.c) ===\\n
    set {unsigned int}${SPI_FALLBACK_MODE_ADDR} = 1
  end
  if (${PHASE6_RENODE_SPI_STS_FORCE} == 1)
    echo === Renode: SPI1->STS TXBE|RXBNE (0x40013008) ===\\n
    set {unsigned short}0x40013008 = 0x0003
  end
  if (${PHASE6_POST_BREAK_RUN_SECONDS} > 0)
    delete \$bp_pd_capture
    monitor start
    shell sleep ${PHASE6_POST_BREAK_RUN_SECONDS}
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
x/1bx ${PB3_STATE_ADDR}
x/1bx ${PB11_STATE_ADDR}
x/1wx ${USB_ATTACHED_ADDR}
x/1wx ${USB_EVT_COUNT_ADDR}
x/1wx ${PD_GPIO_COUNT_ADDR}
x/1wx ${PROTOCOL_EXPAND_ADDR}
echo === TMR1 registers (PWM) ===\n
x/1wx 0x40012C00
x/1wx 0x40012C2C
x/1wx 0x40012C34
x/1wx 0x40012C3C
echo === TMR4_CNT (encoder source) ===\n
x/1wx 0x40000824
echo === SPI1 startup frame trace (23B) ===\n
x/1wx ${SPI_STARTUP_TX_COUNT_ADDR}
x/1wx ${SPI_STARTUP_DONE_ADDR}
x/1wx ${SPI_STARTUP_LAST_BYTE_ADDR}
x/1wx ${SPI_STARTUP_CS_LOW_ADDR}
x/1wx ${SPI_STARTUP_CS_HIGH_ADDR}
x/1wx ${SPI_STARTUP_WAIT_TIMEOUT_ADDR}
echo === SPI1 runtime framepump cadence ===\n
x/1wx ${SPI_FLUSH_TRIGGER_ADDR}
x/1wx ${SPI_FLUSH_SKIP_ADDR}
echo === SPI1 runtime cadence context (ticks/dirty) ===\n
echo === Scheduler vs PD vs SysTick (OEM: ~20 systick_entry per PD call; main loop is busy-wait) ===\n
x/1wx ${SCHED_TICKS_ADDR}
x/1wx ${CALL_TICKS_ADDR}
x/1wx ${SYSTICK_ENTRY_ADDR}
x/1wx ${OLED_LAST_FLUSH_TICK_ADDR}
x/1bx ${UI_FRAME_DIRTY_ADDR}
echo === SysTick (Cortex-M) — PD_PeriodicDispatchFromSysTick: /20 on IRQ entry count (see g_phase6_systick_entry_count) ===\n
x/1wx 0xE000E010
x/1wx 0xE000E014
x/1wx 0xE000E018
echo === main->SPI pump bridge counters ===\n
x/1wx ${MAIN_SPI_REQ_ADDR}
x/1wx ${MAIN_SPI_CONSUMED_ADDR}
echo === main loop semantic counters (Step8 / IWDG reload) ===\n
x/1wx ${MAIN_STEP8_ADDR}
x/1wx ${MAIN_IWDG_RELOAD_ADDR}
echo === startup chain magic/IWDT anchors ===\n
x/1wx ${STARTUP_MAGIC_ADDR}
x/1wx ${STARTUP_MAGIC_MATCH_ADDR}
x/1wx ${STARTUP_IWDT_STARTED_ADDR}
x/1wx ${STARTUP_IWDT_RELOAD_ADDR}
echo === USB upgrade guard risk counters ===\n
x/1wx ${USB_GUARD_INIT_ADDR}
x/1wx ${USB_PREFLIGHT_READY_ADDR}
x/1wx ${USB_PREFLIGHT_FAIL_ADDR}
x/1wx ${USB_FLASH_ATTEMPT_ADDR}
dump binary memory ${SRAM_BIN} 0x20000000 0x20005000
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

if [[ ! -f "${SRAM_BIN}" ]]; then
  echo "未生成 SRAM 转储: ${SRAM_BIN}" >&2
  exit 1
fi

SZ="$(wc -c < "${SRAM_BIN}" | tr -d ' ')"
if [[ "${SZ}" -ne 20480 ]]; then
  echo "SRAM 大小异常: ${SZ}（预期 20480）" >&2
  exit 1
fi

echo ""
echo "完成。"
echo "  run_dir: ${RUN_DIR}"
echo "  gdb_log: ${GDB_LOG}"
echo "  renode_log: ${RENODE_LOG}"
echo "  sram_bin: ${SRAM_BIN}"
