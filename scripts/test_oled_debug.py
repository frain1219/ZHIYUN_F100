#!/usr/bin/env python3
import serial
import json
import time

port = "/dev/tty.usbmodem208D37654D331"
ser = serial.Serial(port, 115200, timeout=2.0)
time.sleep(0.5)
ser.reset_input_buffer()
ser.reset_output_buffer()

def send_cmd(cmd):
    ser.write((cmd + "\n").encode())
    ser.flush()
    time.sleep(0.2)
    line = ser.readline().decode('utf-8').strip()
    return line

print("PING:", send_cmd("PING"))
print("START:", send_cmd("OLED_SNIFF_START"))

print("\nCapturing for 3 seconds...")
time.sleep(3)

status = send_cmd("OLED_SNIFF_STATUS")
print("\nSTATUS:", status)

edges = send_cmd("OLED_SNIFF_DUMP edge 20")
print("\nEDGES:", edges[:300] if edges else "None")

print("\nSTOP:", send_cmd("OLED_SNIFF_STOP"))

ser.close()
