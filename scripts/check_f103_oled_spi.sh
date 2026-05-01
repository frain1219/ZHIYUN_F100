#!/bin/bash

echo "=========================================="
echo "F103 OLED SPI Status Check via STLINK"
echo "=========================================="
echo ""

echo "[1] Starting st-util GDB server..."
st-util -p 4242 &
STUTIL_PID=$!
sleep 2

echo "[2] Running GDB script..."
arm-none-eabi-gdb -batch -x check_f103_oled_spi.gdb

echo ""
echo "[3] Stopping st-util..."
kill $STUTIL_PID 2>/dev/null

echo ""
echo "=========================================="
echo "Check Complete"
echo "=========================================="
