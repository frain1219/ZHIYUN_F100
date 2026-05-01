#!/usr/bin/env python3
"""
跳过.bss清零循环,继续到main函数
"""

import subprocess
import time
import re

print("========================================")
print("F103 Skip .bss Clear and Continue to main")
print("========================================\n")

# 启动st-util
print("[1] Starting st-util...")
stutil_proc = subprocess.Popen(
    ['st-util', '-p', '4242'],
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True
)

time.sleep(2)

# GDB命令
gdb_commands = """
set pagination off
set confirm off
target extended-remote localhost:4242

echo \\n[2] Current state:\\n
info registers pc r1 r2 r3

echo \\n[3] Setting r2=0 to exit loop...\\n
set $r2 = 0

echo \\n[4] Continue execution...\\n
continue

echo \\n[5] Waiting 3 seconds...\\n

echo \\n[6] Check new PC:\\n
info registers pc

echo \\n[7] Check SPI1 status:\\n
x/1xw 0x40013000

quit
"""

print("[2] Running GDB commands...")
result = subprocess.run(
    ['arm-none-eabi-gdb', '-batch', '-x', '/dev/stdin'],
    input=gdb_commands,
    capture_output=True,
    text=True,
    timeout=10
)

print(result.stdout)

# 停止st-util
print("\n[3] Stopping st-util...")
stutil_proc.terminate()
stutil_proc.wait()

print("\n========================================")
print("Complete")
print("========================================")
