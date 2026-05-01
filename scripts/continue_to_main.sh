#!/bin/bash

echo "=========================================="
echo "F103 Continue to main Function"
echo "=========================================="

st-util -p 4242 &
STUTIL_PID=$!
sleep 2

arm-none-eabi-gdb -batch <<'EOF'
set pagination off
set confirm off
target extended-remote localhost:4242

echo \n[1] Current PC:\n
info registers pc

echo \n[2] Set breakpoint at main (0x08002E74):\n
hbreak *0x08002E74

echo \n[3] Set r2=0 to skip .bss clear loop:\n
set $r2 = 0

echo \n[4] Continue execution...\n
continue

echo \n[5] Check if we hit main:\n
info registers pc

echo \n[6] Check SPI1 status:\n
x/1xw 0x40013000

echo \n[7] Check RCC clocks:\n
x/1xw 0x40021018

quit
EOF

kill $STUTIL_PID 2>/dev/null

echo ""
echo "=========================================="
