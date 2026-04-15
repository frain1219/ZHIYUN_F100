#!/usr/bin/env python3
"""开发用：验证 Renode Cortex-M AddHook 是否触发（本机实测 log/PC 均表明复位钩未在 expect 时刻命中）。"""
import os
import signal
import socket
import subprocess
import time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RENODE = os.environ.get("RENODE_BIN", "/Applications/Renode.app/Contents/MacOS/renode")
RESC = os.path.join(REPO, "Renode/apm32_f100_gdb_first.resc")
PORT = int(os.environ.get("TEST_MON_PORT", "49160"))

logf = open(os.path.join(REPO, "scripts/out/oi001_sysbus_hook_renode.stderr.log"), "w")
p = subprocess.Popen(
    [RENODE, "--disable-gui", "--plain", "-P", str(PORT), RESC],
    cwd=REPO,
    stdout=subprocess.DEVNULL,
    stderr=logf,
)
try:
    time.sleep(2.5)
    s = socket.create_connection(("127.0.0.1", PORT), timeout=30)
    s.settimeout(0.3)

    def drain() -> bytes:
        b = b""
        for _ in range(200):
            try:
                b += s.recv(16384)
            except socket.timeout:
                break
        return b

    drain()
    hook = (
        'sysbus.cpu AddHook 0x08006BE9 '
        '"monitor.Parse(\'log \\"FIRE_7392A\\"\'); machine.PauseAndRequestEmulationPause()"'
    )
    for cmd in ['mach set "ZHIYUN_F100"', hook, "start"]:
        s.sendall((cmd + "\n").encode())
        time.sleep(0.05)
    time.sleep(0.3)
    s.sendall(b"sysbus.cpu PC\n")
    time.sleep(1.0)
    out = drain().decode("utf-8", errors="replace")
    logf.flush()
    print("--- monitor tail ---")
    print(out[-2000:])
finally:
    p.send_signal(signal.SIGTERM)
    p.wait(timeout=8)
    logf.close()
    err_path = os.path.join(REPO, "scripts/out/oi001_sysbus_hook_renode.stderr.log")
    with open(err_path, "r", errors="replace") as f:
        err = f.read()
    print("--- renode stderr (log) FIRE_7392A? ---", "FIRE_7392A" in err)
    print(err[-2000:] if len(err) > 2000 else err)
