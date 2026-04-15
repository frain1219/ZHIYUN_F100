#!/usr/bin/env python3
"""
Renode Monitor：AddHook@PC(Pause) + emulation RunFor，读 PC / r0–r3（不经 GDB continue）。

修复：勿用 nc 短读 — Renode Monitor 带 Telnet IAC 前缀，需 socket 长读。

用法：
  python3 scripts/oi001_monitor_runfor_hook.py [hook_hex] [RunFor秒|start]
默认：0x080012E8 与 "15"（虚拟秒）。

第二参数为 **start** 时：改用 Monitor **`start`**（自由运行），在主机侧长读直至日志含
**Machine paused**（由 AddHook 内 Pause 触发）或超时，再读 PC — 用于命中「仅执行一次」的散载入口
（RunFor 长时间会把 PC 推到 PLL 主循环而错过钩子）。

环境变量：OI001_START_READ_TIMEOUT（默认 30）秒。

输出：stdout + 日志文件（优先 ${OI001_RUN_DIR}/monitor_runfor_hook.log；
      否则 logs/renode-oi001/<时间戳>/monitor_runfor_hook.log）
"""
from __future__ import annotations

import os
import re
import signal
import socket
import subprocess
import sys
import time
from datetime import datetime

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LOGS_ROOT = os.path.join(REPO, "logs", "renode-oi001")
RENODE = os.environ.get("RENODE_BIN", "/Applications/Renode.app/Contents/MacOS/renode")
RESC = os.path.join(REPO, "Renode/apm32_f100_gdb_first.resc")
MON_PORT = int(os.environ.get("RENODE_MONITOR_PORT", "49152"))
START_READ_TIMEOUT = float(os.environ.get("OI001_START_READ_TIMEOUT", "30"))


def strip_telnet_iac(data: bytes) -> bytes:
    out = bytearray()
    i = 0
    n = len(data)
    while i < n:
        if data[i] == 0xFF and i + 1 < n:
            cmd = data[i + 1]
            if cmd in (0xFB, 0xFC, 0xFD, 0xFE) and i + 2 < n:
                i += 3
                continue
            if cmd == 0xFF:
                out.append(0xFF)
                i += 2
                continue
        out.append(data[i])
        i += 1
    return bytes(out)


class MonitorSession:
    def __init__(self, host: str, port: int) -> None:
        self._s = socket.create_connection((host, port), timeout=60)
        self._s.settimeout(0.25)
        self._buf = b""

    def close(self) -> None:
        try:
            self._s.close()
        except OSError:
            pass

    def _drain_banner(self, total_timeout: float = 30.0) -> None:
        self._buf = b""
        t0 = time.time()
        while time.time() - t0 < total_timeout:
            try:
                chunk = self._s.recv(65536)
                if not chunk:
                    break
                self._buf += chunk
                if b"(ZHIYUN_F100)" in self._buf:
                    self._buf = b""
                    return
            except socket.timeout:
                continue
        raise RuntimeError("monitor banner timeout")

    def talk(
        self,
        line: str,
        idle: float = 2.0,
        max_wait: float = 600.0,
        min_elapsed: float = 0.0,
    ) -> str:
        """发送一行 Monitor 命令，累积响应直到 idle 秒无数据且已过 min_elapsed，或 max_wait。"""
        payload = line if line.endswith("\n") else line + "\n"
        self._s.sendall(payload.encode("utf-8", errors="replace"))
        t0 = time.time()
        last_data = t0
        self._buf = b""
        while time.time() - t0 < max_wait:
            try:
                chunk = self._s.recv(65536)
                if chunk:
                    self._buf += chunk
                    last_data = time.time()
                else:
                    break
            except socket.timeout:
                elapsed = time.time() - t0
                if (
                    elapsed >= min_elapsed
                    and time.time() - last_data >= idle
                    and b"(ZHIYUN_F100)" in self._buf
                ):
                    break
                continue
        text = strip_telnet_iac(self._buf).decode("utf-8", errors="replace")
        self._buf = b""
        return text

    def talk_until_substrings(
        self,
        line: str,
        *needles: str,
        timeout: float = 30.0,
    ) -> str:
        """发送命令后持续读，直到任一 needle 出现在文本中或超时。"""
        payload = line if line.endswith("\n") else line + "\n"
        self._s.sendall(payload.encode("utf-8", errors="replace"))
        self._buf = b""
        t0 = time.time()
        self._s.settimeout(0.25)
        needles_l = tuple(n.lower() for n in needles)
        while time.time() - t0 < timeout:
            try:
                chunk = self._s.recv(65536)
                if chunk:
                    self._buf += chunk
                else:
                    break
            except socket.timeout:
                pass
            text = strip_telnet_iac(self._buf).decode("utf-8", errors="replace").lower()
            if any(n in text for n in needles_l):
                break
        out = strip_telnet_iac(self._buf).decode("utf-8", errors="replace")
        self._buf = b""
        return out


def wait_monitor_port(host: str, port: int, seconds: float = 90.0) -> None:
    deadline = time.time() + seconds
    while time.time() < deadline:
        r = subprocess.run(
            ["nc", "-z", host, str(port)],
            capture_output=True,
            timeout=2,
        )
        if r.returncode == 0:
            return
        time.sleep(0.2)
    raise RuntimeError(f"monitor {host}:{port} not up in {seconds}s")


def main() -> int:
    hook_addr = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0x080012E8
    mode_arg = sys.argv[2] if len(sys.argv) > 2 else "15"
    use_start = mode_arg.strip().lower() == "start"
    runfor = "15" if len(sys.argv) < 3 else mode_arg

    run_dir = os.environ.get("OI001_RUN_DIR", "").strip()
    if not run_dir:
        ts = datetime.now().strftime("%Y%m%d_%H%M%S")
        run_dir = os.path.join(LOGS_ROOT, ts)
    os.makedirs(run_dir, exist_ok=True)
    log_path = os.path.join(run_dir, "monitor_runfor_hook.log")
    lines: list[str] = []

    def log(msg: str) -> None:
        print(msg)
        lines.append(msg)

    if not os.path.isfile(RESC):
        log(f"ERROR: missing {RESC}")
        return 1
    if not os.path.isfile(RENODE):
        log(f"ERROR: Renode not found: {RENODE} (set RENODE_BIN)")
        return 1

    proc = subprocess.Popen(
        [RENODE, "--disable-gui", "--plain", "-P", str(MON_PORT), RESC],
        cwd=REPO,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    try:
        wait_monitor_port("127.0.0.1", MON_PORT)
        m = MonitorSession("127.0.0.1", MON_PORT)
        m._drain_banner()

        log(m.talk('mach set "ZHIYUN_F100"', idle=1.5, max_wait=30))

        # Renode hooks.robot：Pause 用 machine.PauseAndRequestEmulationPause()；
        # OI001_HOOK_LOG=1 时打 log 验证钩子是否触发。
        pause_hook = "machine.PauseAndRequestEmulationPause()"
        if os.environ.get("OI001_HOOK_LOG", "").strip() in ("1", "yes", "true"):
            hook_body = (
                f"monitor.Parse('log \\\"OI001_HOOK_{hook_addr:#x}\\\"'); {pause_hook}"
            )
        else:
            hook_body = pause_hook
        # 与 Renode tests 一致优先 sysbus.cpu；Cortex-M3 实测散载地址钩仍可能不触发（见 04 §1.3.14）。
        hook_line = f'sysbus.cpu AddHook {hook_addr:#x} "{hook_body}"\n'
        log(m.talk(hook_line, idle=2.0, max_wait=60))

        if use_start:
            log("=== monitor start（等 AddHook 触发 Machine paused / 或 OI001_HOOK log）===")
            needles: list[str] = ["machine paused", "Machine paused"]
            if os.environ.get("OI001_HOOK_LOG", "").strip() in ("1", "yes", "true"):
                needles.append("OI001_HOOK")
            log(m.talk_until_substrings("start", *needles, timeout=START_READ_TIMEOUT))
        else:
            log(f"=== emulation RunFor \"{runfor}\" s (virtual) ===")
            rf = float(runfor)
            log(
                m.talk(
                    f'emulation RunFor "{runfor}"',
                    idle=2.0,
                    max_wait=rf + 300.0,
                    min_elapsed=rf + 1.0,
                )
            )

        log("=== pause（幂等；若已在 pause 则无妨）===")
        log(m.talk("pause", idle=2.0, max_wait=60))

        log("=== sysbus.cpu PC ===")
        log(m.talk("sysbus.cpu PC", idle=2.0, max_wait=30))

        log("=== GetRegister 0..15 ===")
        for i in range(16):
            log(m.talk(f"sysbus.cpu GetRegister {i}", idle=1.5, max_wait=20))

        # 若钩子命中，PC 应停在 hook 地址附近
        joined = "\n".join(lines)
        m_pc = re.search(r"0x[0-9a-fA-F]+", joined.split("=== sysbus.cpu PC ===")[-1][:800])
        if m_pc:
            log(f"=== PARSED_PC_TOKEN: {m_pc.group(0)} ===")
        if f"{hook_addr & ~1:#x}".lower().replace("0x", "0x") in joined.lower() or hex(hook_addr) in joined.lower():
            log("=== HOOK_ADDR_SEEN_IN_LOG (may be from AddHook line) ===")

        with open(log_path, "w", encoding="utf-8", errors="replace") as f:
            f.write(joined)
        log(f"=== wrote {log_path} ===")
        return 0
    finally:
        try:
            proc.send_signal(signal.SIGTERM)
        except Exception:
            pass
        try:
            proc.wait(timeout=8)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=3)


if __name__ == "__main__":
    sys.exit(main())
