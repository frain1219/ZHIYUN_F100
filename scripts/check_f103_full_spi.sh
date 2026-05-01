#!/bin/bash

echo "=========================================="
echo "F103 Full SPI Status Check"
echo "=========================================="

st-util -p 4242 &
STUTIL_PID=$!
sleep 2

arm-none-eabi-gdb -batch -x check_f103_full_spi.gdb

kill $STUTIL_PID 2>/dev/null

echo ""
echo "=========================================="
