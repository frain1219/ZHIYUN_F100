#!/usr/bin/env python3
"""启动 Renode、Monitor 注册 AddHook(Pause)、跑 GDB batch（带超时）。"""
import os
import signal
import subprocess
import sys
import time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RENODE = "/Applications/Renode.app/Contents/MacOS/renode"
RESC = os.path.join(REPO, "Renode/apm32_f100_gdb_first.resc")


def main() -> int:
    hook_pc = int(sys.argv[1], 0) if len(sys.argv) > 1 else 0x080045A2
    gdb_timeout = float(sys.argv[2]) if len(sys.argv) > 2 else 12.0

    ren = subprocess.Popen(
        [RENODE, "--disable-gui", "--plain", "-P", "49152", RESC],
        cwd=REPO,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    try:
        for _ in range(100):
            try:
                s = subprocess.run(
                    ["nc", "-z", "127.0.0.1", "49152"], capture_output=True, timeout=0.5
                )
                if s.returncode == 0:
                    break
            except subprocess.TimeoutExpired:
                pass
            time.sleep(0.15)
        else:
            print("Renode monitor not up", file=sys.stderr)
            return 2

        hook_py = (
            "from Antmicro.Renode.Core import EmulationManager\\n"
            "EmulationManager.Instance.CurrentEmulation.Pause()"
        )
        mon_in = (
            'mach set "ZHIYUN_F100"\n'
            f'cpu AddHook {hook_pc:#x} "{hook_py}"\n'
        )
        subprocess.run(
            ["nc", "-w", "4", "127.0.0.1", "49152"],
            input=mon_in.encode("utf-8", errors="replace"),
            capture_output=True,
            timeout=6,
        )

        gdb_cmd = [
            os.environ.get("GDB", "arm-none-eabi-gdb"),
            "-n",
            "-q",
            "-batch",
            "-ex",
            "set architecture armv3m",
            "-ex",
            "target extended-remote localhost:3333",
            "-ex",
            "set $sp=0x20001620",
            "-ex",
            "set $pc=0x08006be9",
            "-ex",
            "monitor start",
            "-ex",
            "continue",
            "-ex",
            'printf "PC=%#x r0=%#x r1=%#x r2=%#x r3=%#x\\n", $pc, $r0, $r1, $r2, $r3',
            "-ex",
            "x/4wx 0x200002c4",
            "-ex",
            "quit",
        ]
        try:
            p = subprocess.run(
                gdb_cmd,
                cwd=REPO,
                capture_output=True,
                text=True,
                timeout=gdb_timeout,
            )
            sys.stdout.write(p.stdout)
            sys.stderr.write(p.stderr)
            return p.returncode
        except subprocess.TimeoutExpired as e:
            print(f"\n[GDB TIMEOUT {gdb_timeout}s]\n", file=sys.stderr)
            sys.stdout.write(e.stdout or "")
            return 124
    finally:
        try:
            ren.send_signal(signal.SIGKILL)
        except Exception:
            pass
        ren.wait(timeout=3)


if __name__ == "__main__":
    sys.exit(main())
