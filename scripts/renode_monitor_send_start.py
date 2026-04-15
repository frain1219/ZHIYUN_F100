#!/usr/bin/env python3
"""
向 Renode Monitor（-P 端口，telnet 模式）发送一行命令。
先做一次 recv 丢弃协商/欢迎片段，再发命令，减少 GDB 日志里的 IAC 乱码。
用法: python3 renode_monitor_send_start.py [port=49152] [line=start]
"""
import socket
import sys

def main() -> int:
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 49152
    line = sys.argv[2] if len(sys.argv) > 2 else "start"

    s = socket.create_connection(("127.0.0.1", port))
    s.settimeout(1.0)
    try:
        _ = s.recv(65536)
    except OSError:
        pass
    payload = line.encode("ascii", "replace") + b"\n"
    s.sendall(payload)
    s.settimeout(2.0)
    try:
        out = s.recv(8192)
        sys.stdout.buffer.write(out)
        sys.stdout.flush()
    except OSError:
        pass
    s.close()
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
