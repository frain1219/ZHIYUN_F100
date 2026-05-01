#!/usr/bin/env python3
"""
分析F407捕获的边沿数据
"""

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
    buffer = ""
    start_time = time.time()
    while time.time() - start_time < 2.0:
        bytes_to_read = ser.in_waiting
        if bytes_to_read > 0:
            data = ser.read(bytes_to_read).decode('utf-8', errors='ignore')
            buffer += data
        while '\n' in buffer or '\r' in buffer:
            newline_pos = -1
            for i, ch in enumerate(buffer):
                if ch == '\n' or ch == '\r':
                    newline_pos = i
                    break
            if newline_pos >= 0:
                line = buffer[:newline_pos].strip()
                buffer = buffer[newline_pos + 1:]
                if line:
                    try:
                        return json.loads(line)
                    except json.JSONDecodeError:
                        continue
        time.sleep(0.01)
    return None

print("========================================")
print("F407 Edge Data Analysis")
print("========================================\n")

# 启动OLED Sniffer
print("[1] Starting OLED Sniffer...")
send_cmd("OLED_SNIFF_START")

# 监听5秒
print("[2] Listening for 5 seconds...")
time.sleep(5)

# 获取状态
status = send_cmd("OLED_SNIFF_STATUS")
if status:
    print(f"\n[3] Sniffer Status:")
    print(f"  Bus Type: {status.get('bus_type')}")
    print(f"  Edge Count: {status.get('edge_count')}")
    print(f"  Transaction Count: {status.get('txn_count')}")
    print(f"  Byte Count: {status.get('byte_count')}")

# 导出边沿数据
edges = send_cmd("OLED_SNIFF_DUMP edge 50")
if edges and edges.get('type') == 'oled_dump':
    items = edges.get('items', [])
    print(f"\n[4] Edge Analysis (first 50):")
    print(f"  Total edges captured: {len(items)}")
    
    # 分析边沿类型
    line_counts = {0: 0, 1: 0, 2: 0, 3: 0, 4: 0}
    line_names = {0: 'SCK', 1: 'MOSI', 2: 'CS', 3: 'RES', 4: 'DC'}
    
    for edge in items:
        line = edge[1]
        line_counts[line] = line_counts.get(line, 0) + 1
    
    print(f"\n  Edge count by line:")
    for line, count in sorted(line_counts.items()):
        print(f"    {line_names[line]} (line {line}): {count} edges")
    
    # 显示前10个边沿
    print(f"\n  First 10 edges:")
    print(f"  [timestamp, line, level]")
    for i, edge in enumerate(items[:10]):
        line_name = line_names.get(edge[1], f"line{edge[1]}")
        print(f"    {i+1}. [{edge[0]}, {line_name}, {edge[2]}]")

# 停止
send_cmd("OLED_SNIFF_STOP")

ser.close()

print("\n========================================")
print("Analysis Complete")
print("========================================")
