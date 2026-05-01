#!/bin/bash

echo "=========================================="
echo "F103 Skip .bss Clear Loop"
echo "=========================================="

st-util -p 4242 &
STUTIL_PID=$!
sleep 2

# First check the loop state
arm-none-eabi-gdb -batch -x skip_bss_clear.gdb

kill $STUTIL_PID 2>/dev/null

echo ""
echo "=========================================="
