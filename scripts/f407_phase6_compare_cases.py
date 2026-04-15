#!/usr/bin/env python3
import json
import sys
import time
from pathlib import Path

try:
    import serial  # type: ignore
except Exception:
    print("Missing pyserial. Install with: python3 -m pip install pyserial", file=sys.stderr)
    sys.exit(2)


def write_cmd(ser, cmd: str) -> None:
    ser.write((cmd + "\r\n").encode("ascii"))
    ser.flush()


def read_lines(ser, timeout_s: float):
    end = time.time() + timeout_s
    lines = []
    while time.time() < end:
        raw = ser.readline()
        if not raw:
            continue
        text = raw.decode("utf-8", errors="replace").strip()
        if text:
            lines.append(text)
    return lines


def run_case(ser, run_dir: Path, name: str, commands, read_timeout: float = 0.25):
    out_file = run_dir / f"{name}.log"
    all_lines = [f"# case={name}"]
    has_err = False
    for cmd in commands:
        all_lines.append(f">>> {cmd}")
        write_cmd(ser, cmd)
        lines = read_lines(ser, read_timeout)
        if lines:
            all_lines.extend(lines)
            for ln in lines:
                if '"type":"err"' in ln:
                    has_err = True
    out_file.write_text("\n".join(all_lines) + "\n", encoding="utf-8")
    return out_file, has_err


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <serial_port> [baud] [run_dir]", file=sys.stderr)
        return 1

    port = sys.argv[1]
    baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    run_dir = Path(sys.argv[3]) if len(sys.argv) > 3 else Path("logs/f407-phase6") / time.strftime("%Y%m%d_%H%M%S")
    run_dir.mkdir(parents=True, exist_ok=True)

    cases = []
    failed = []

    with serial.Serial(port=port, baudrate=baud, timeout=0.08) as ser:
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.2)

        c, err = run_case(
            ser,
            run_dir,
            "caseA_software_check",
            [
                "PING",
                "GET_CAPS",
                "LIST_PINS",
                "SELF_CHECK",
                "RESET_STATS",
                "SCENE_LIST",
                "SCENE_RUN KEY_RIGHT_TAP",
                "SCENE_STOP",
                "GET_STATS",
            ],
        )
        cases.append(str(c))
        if err:
            failed.append("caseA_software_check")

        c, err = run_case(
            ser,
            run_dir,
            "caseB1_bypass_fsm",
            [
                "RESET_STATS",
                "BYPASS_FSM STATUS",
                "BYPASS_FSM ON",
                "BYPASS_FSM STATUS",
                "SUBSCRIBE ON",
                "SCENE_RUN PD_ATTACH_BLIP",
                "GET_STATUS",
                "GET_STATS",
                "SUBSCRIBE OFF",
                "BYPASS_FSM OFF",
            ],
            read_timeout=0.5,
        )
        cases.append(str(c))
        if err:
            failed.append("caseB1_bypass_fsm")

        c, err = run_case(
            ser,
            run_dir,
            "caseB2_oled_bus_type",
            [
                "RESET_STATS",
                "OLED_SNIFF_STATUS",
                "OLED_SNIFF_START",
                "SCENE_RUN KEY_RIGHT_TAP",
                "OLED_SNIFF_STATUS",
                "GET_STATS",
                "OLED_SNIFF_STOP",
            ],
            read_timeout=0.5,
        )
        cases.append(str(c))
        if err:
            failed.append("caseB2_oled_bus_type")

        c, err = run_case(
            ser,
            run_dir,
            "caseB3_oled_capture",
            [
                "RESET_STATS",
                "OLED_SNIFF_START",
                "SCENE_RUN KEY_RIGHT_TAP",
                "OLED_SNIFF_DUMP edge 128",
                "OLED_SNIFF_DUMP txn 32",
                "OLED_SNIFF_DUMP bytes 256",
                "GET_STATS",
                "OLED_SNIFF_STOP",
            ],
            read_timeout=0.6,
        )
        cases.append(str(c))
        if err:
            failed.append("caseB3_oled_capture")

    summary = {
        "serial_port": port,
        "baud": baud,
        "run_dir": str(run_dir),
        "cases": cases,
        "failed_cases": failed,
        "pass": len(failed) == 0,
        "notes": "BYPASS/OLED cases require wiring and active F103 target for meaningful counters.",
    }
    (run_dir / "summary.json").write_text(json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False))
    return 0 if len(failed) == 0 else 3


if __name__ == "__main__":
    raise SystemExit(main())
#!/usr/bin/env python3
import json
import sys
import time
from pathlib import Path

try:
    import serial  # type: ignore
except Exception:
    print("Missing pyserial. Install with: python3 -m pip install pyserial", file=sys.stderr)
    sys.exit(2)


def write_cmd(ser, cmd):
    ser.write((cmd + "\r\n").encode("ascii"))
    ser.flush()


def read_lines(ser, timeout_s):
    end = time.time() + timeout_s
    lines = []
    while time.time() < end:
        raw = ser.readline()
        if not raw:
            continue
        text = raw.decode("utf-8", errors="replace").strip()
        if text:
            lines.append(text)
    return lines


def run_case(ser, run_dir, name, commands, read_timeout=0.25):
    out_file = run_dir / f"{name}.log"
    all_lines = [f"# case={name}"]
    for cmd in commands:
        all_lines.append(f">>> {cmd}")
        write_cmd(ser, cmd)
        lines = read_lines(ser, read_timeout)
        if lines:
            all_lines.extend(lines)
    out_file.write_text("\n".join(all_lines) + "\n", encoding="utf-8")
    return out_file


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <serial_port> [baud] [run_dir]", file=sys.stderr)
        return 1

    port = sys.argv[1]
    baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    run_dir = Path(sys.argv[3]) if len(sys.argv) > 3 else Path("logs/f407-phase6") / time.strftime("%Y%m%d_%H%M%S")
    run_dir.mkdir(parents=True, exist_ok=True)

    with serial.Serial(port=port, baudrate=baud, timeout=0.08) as ser:
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.2)

        case1 = run_case(
            ser,
            run_dir,
            "case1_i2c",
            [
                "RESET_STATS",
                "I2C_SLAVE_EMU CFG 0x3C",
                "I2C_SLAVE_EMU REG_WRITE 0x10 0x5A",
                "I2C_SLAVE_EMU XFER r 0x10",
                "I2C_SLAVE_EMU XFER w 0x11 0xA5",
                "I2C_SLAVE_EMU REG_READ 0x11",
                "I2C_SLAVE_EMU STATUS",
                "GET_STATS",
            ],
        )
        case2 = run_case(
            ser,
            run_dir,
            "case2_spi",
            [
                "RESET_STATS",
                "SPI_SLAVE_EMU CFG PE5 0 0x11,0x22,0x33,0x44",
                "SPI_SLAVE_EMU XFER 0xAA,0xBB,0xCC,0xDD",
                "SPI_SLAVE_EMU XFER 0x01,0x02,0x03",
                "SPI_SLAVE_EMU STATUS",
                "GET_STATS",
            ],
        )
        case3 = run_case(
            ser,
            run_dir,
            "case3_phase6_scene",
            [
                "RESET_STATS",
                "SUBSCRIBE ON",
                "I2C_SLAVE_EMU CFG 0x3C",
                "SPI_SLAVE_EMU CFG PE5 0 0x90,0x91",
                "SCENE_RUN KEY_RIGHT_TAP",
                "I2C_SLAVE_EMU XFER r 0x10",
                "SPI_SLAVE_EMU XFER 0x55,0x66",
                "GET_STATUS",
                "GET_STATS",
                "SUBSCRIBE OFF",
            ],
            read_timeout=0.5,
        )

    summary = {
        "serial_port": port,
        "baud": baud,
        "run_dir": str(run_dir),
        "cases": [str(case1), str(case2), str(case3)],
    }
    (run_dir / "summary.json").write_text(json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(json.dumps(summary, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
