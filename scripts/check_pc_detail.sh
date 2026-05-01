#!/bin/bash

echo "=========================================="
echo "F103 PC Location Detail Check"
echo "=========================================="

st-util -p 4242 &
STUTIL_PID=$!
sleep 2

arm-none-eabi-gdb -batch -x check_pc_detail.gdb

kill $STUTIL_PID 2>/dev/null

echo ""
echo "=========================================="
