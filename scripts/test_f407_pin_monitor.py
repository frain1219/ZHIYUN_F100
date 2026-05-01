#!/usr/bin/env python3
"""
F407引脚监听自动化测试脚本
测试APM32F103CB到STM32F407V的接线是否正常（3.1-3.6模块）

使用方法:
    python test_f407_pin_monitor.py [--port /dev/tty.usbmodemxxx] [--duration 10]

依赖:
    pip install pyserial
"""

import argparse
import json
import serial
import serial.tools.list_ports
import sys
import time
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple


@dataclass
class PinStatus:
    """引脚状态"""
    name: str
    value: int
    mode: int
    analog: bool
    note: str


@dataclass
class TestResult:
    """测试结果"""
    module: str
    pin_name: str
    f103_signal: str
    status: str  # "PASS", "FAIL", "SKIP"
    message: str
    value: Optional[int] = None
    event_count: int = 0


class F407ProbeTester:
    """F407探针测试器"""
    
    # 测试引脚映射（基于接线文档3.1-3.6）
    TEST_PINS = {
        "3.1_系统状态与控制链路": [
            {"f407_pin": "PE0", "f103_signal": "PC13", "note": "PD状态机关键输入"},
            {"f407_pin": "PE5", "f103_signal": "PA0", "note": "供电路径相关控制"},
            {"f407_pin": "PE1", "f103_signal": "PB11", "note": "PD GPIO控制"},
            {"f407_pin": "PE2", "f103_signal": "PB3", "note": "VS_EN相关"},
        ],
        "3.2_按键输入模块": [
            {"f407_pin": "PE6", "f103_signal": "PA1", "note": "按键采样路径"},
            {"f407_pin": "PC0", "f103_signal": "PB4", "note": "右键输入"},
        ],
        "3.3_OLED抓取模块": [
            {"f407_pin": "PE8", "f103_signal": "PA5", "note": "OLED SPI SCK候选"},
            {"f407_pin": "PE9", "f103_signal": "PA7", "note": "OLED SPI MOSI候选"},
            {"f407_pin": "PE10", "f103_signal": "PB10", "note": "OLED CS低有效候选"},
            {"f407_pin": "PE11", "f103_signal": "PB2", "note": "OLED RES候选"},
            {"f407_pin": "PE12", "f103_signal": "PA6", "note": "OLED D/C候选"},
        ],
        "3.4_编码器与PWM模块": [
            {"f407_pin": "PC1", "f103_signal": "PB5", "note": "MO_PWM(下)"},
            {"f407_pin": "PC2", "f103_signal": "PB6", "note": "编码器A相"},
            {"f407_pin": "PC3", "f103_signal": "PB7", "note": "编码器B相"},
            {"f407_pin": "PE7", "f103_signal": "PA2", "note": "RGB PWM相关输出"},
        ],
        "3.5_模拟采样模块": [
            {"f407_pin": "PA0", "f103_signal": "PA3", "note": "ADC输入"},
            {"f407_pin": "PA1", "f103_signal": "PA4", "note": "ADC输入"},
            {"f407_pin": "PA2", "f103_signal": "PB0", "note": "ADC输入"},
            {"f407_pin": "PA3", "f103_signal": "PB1", "note": "ADC输入(VBAT)"},
        ],
        "3.6_副板控制模块": [
            {"f407_pin": "PD2", "f103_signal": "PB12", "note": "AUX"},
            {"f407_pin": "PD3", "f103_signal": "PB13", "note": "AUX/STAT"},
            {"f407_pin": "PD4", "f103_signal": "PB14", "note": "EN测试点"},
            {"f407_pin": "PD5", "f103_signal": "PB15", "note": "ADIM测试点"},
            {"f407_pin": "PD6", "f103_signal": "PA8", "note": "W_PWM"},
            {"f407_pin": "PD7", "f103_signal": "PA9", "note": "C_PWM"},
        ],
    }
    
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 2.0):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial: Optional[serial.Serial] = None
        self.results: List[TestResult] = []
        self.pin_list: List[Dict] = []
        self.event_counts: Dict[str, int] = {}
        
    def connect(self) -> bool:
        """连接到F407"""
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout
            )
            time.sleep(0.5)  # 等待串口稳定
            self.serial.reset_input_buffer()
            self.serial.reset_output_buffer()
            return True
        except Exception as e:
            print(f"❌ 连接失败: {e}")
            return False
    
    def disconnect(self):
        """断开连接"""
        if self.serial and self.serial.is_open:
            self.serial.close()
    
    def send_command(self, cmd: str, debug: bool = False) -> Optional[Dict]:
        """发送命令并接收响应"""
        if not self.serial or not self.serial.is_open:
            return None
        
        try:
            # 彻底清空输入缓冲区
            self.serial.reset_input_buffer()
            time.sleep(0.05)  # 短暂等待，确保清空完成
            
            # 如果缓冲区又有数据，说明设备还在发送，再次清空
            if self.serial.in_waiting > 0:
                self.serial.reset_input_buffer()
                time.sleep(0.05)
            
            # 发送命令
            if debug:
                print(f"  [DEBUG] 发送命令: {cmd}")
            self.serial.write((cmd + "\n").encode('utf-8'))
            self.serial.flush()
            
            # 等待数据开始到达
            time.sleep(0.2)
            
            # 读取响应 - 使用更长的超时和更大的缓冲区
            response_lines = []
            raw_lines = []
            start_time = time.time()
            buffer = ""
            last_receive_time = time.time()
            
            while time.time() - start_time < self.timeout:
                # 读取所有可用数据
                bytes_to_read = self.serial.in_waiting
                if bytes_to_read > 0:
                    data = self.serial.read(bytes_to_read).decode('utf-8', errors='ignore')
                    buffer += data
                    last_receive_time = time.time()
                    if debug:
                        print(f"  [DEBUG] 已接收 {len(buffer)} 字节...")
                
                # 检查是否收到完整的JSON行（以换行符分隔）
                while '\n' in buffer or '\r' in buffer:
                    # 找到第一个换行符
                    newline_pos = -1
                    for i, ch in enumerate(buffer):
                        if ch == '\n' or ch == '\r':
                            newline_pos = i
                            break
                    
                    if newline_pos >= 0:
                        line = buffer[:newline_pos].strip()
                        buffer = buffer[newline_pos + 1:]
                        
                        if line:
                            raw_lines.append(line)
                            if debug:
                                print(f"  [DEBUG] 收到完整行 ({len(line)} 字节): {line[:80]}...")
                            try:
                                response = json.loads(line)
                                response_lines.append(response)
                                # 对于某些命令，只读取第一个响应
                                if cmd in ["PING", "LIST_PINS", "GET_CAPS", "SELF_CHECK", "GET_STATUS"]:
                                    break
                            except json.JSONDecodeError as e:
                                if debug:
                                    print(f"  [DEBUG] JSON解析失败: {e}")
                                    print(f"  [DEBUG] 完整数据: {line}")
                
                # 如果已经收到响应，退出
                if response_lines:
                    break
                
                # 如果超过200ms没有新数据，认为传输结束
                if time.time() - last_receive_time > 0.2 and buffer:
                    if debug:
                        print(f"  [DEBUG] 传输超时，缓冲区剩余: {len(buffer)} 字节")
                    break
                    
                time.sleep(0.01)
            
            if debug and not response_lines:
                print(f"  [DEBUG] 未收到有效响应，原始数据行数: {len(raw_lines)}")
                if buffer:
                    print(f"  [DEBUG] 缓冲区剩余数据: {buffer}")
            
            return response_lines[0] if response_lines else None
            
        except Exception as e:
            print(f"❌ 通信错误: {e}")
            import traceback
            traceback.print_exc()
            return None
    
    def test_connection(self) -> bool:
        """测试基本连接"""
        print("\n🔍 测试基本连接...")
        response = self.send_command("PING")
        
        if response and response.get("type") == "pong":
            print("✅ 连接正常")
            return True
        else:
            print("❌ 连接失败")
            return False
    
    def get_pin_list(self) -> bool:
        """获取引脚列表"""
        print("\n📋 获取引脚列表...")
        response = self.send_command("LIST_PINS", debug=True)  # 启用调试
        
        if response and response.get("type") == "pins":
            self.pin_list = response.get("list", [])
            print(f"✅ 获取到 {len(self.pin_list)} 个引脚")
            return True
        else:
            print("❌ 获取引脚列表失败")
            if response:
                print(f"  响应类型: {response.get('type', 'unknown')}")
            return False
    
    def get_current_status(self) -> Optional[Dict]:
        """获取当前状态快照"""
        print("  [DEBUG] 发送命令: GET_STATUS")
        response = self.send_command("GET_STATUS", debug=True)
        
        if response:
            print(f"  [DEBUG] 响应类型: {response.get('type')}")
            if response.get("type") == "snapshot":
                pins = response.get("pins", {})
                print(f"  [DEBUG] 获取到 {len(pins)} 个引脚状态")
                return pins
            else:
                print(f"  [DEBUG] 响应内容: {response}")
        else:
            print("  [DEBUG] 未收到响应")
        
        return None
    
    def test_pin_monitoring(self, duration: float = 5.0) -> bool:
        """测试引脚监听功能"""
        print(f"\n📊 开始监听测试（持续 {duration} 秒）...")
        print("提示：请在F103侧触发一些信号变化（如按键、OLED刷新等）")
        
        # 开启订阅
        response = self.send_command("SUBSCRIBE ON")
        if not response or response.get("type") != "ok":
            print("❌ 开启订阅失败")
            return False
        
        print("✅ 订阅已开启，开始监听事件...")
        
        # 监听事件
        start_time = time.time()
        event_count = 0
        pin_events: Dict[str, int] = {}
        
        while time.time() - start_time < duration:
            if self.serial.in_waiting > 0:
                line = self.serial.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    try:
                        response = json.loads(line)
                        
                        # 处理事件
                        if response.get("type") == "event":
                            event_count += 1
                            pin_name = response.get("pin", "UNK")
                            pin_events[pin_name] = pin_events.get(pin_name, 0) + 1
                            
                            # 实时显示事件
                            value = response.get("v", 0)
                            src = "EXTI" if response.get("src") == 1 else "POLL"
                            print(f"  📌 {pin_name}: {value} ({src})")
                        
                        # 处理快照
                        elif response.get("type") == "snapshot":
                            pins = response.get("pins", {})
                            # 可以在这里记录快照数据
                            
                    except json.JSONDecodeError:
                        continue
            
            time.sleep(0.01)
        
        # 关闭订阅
        self.send_command("SUBSCRIBE OFF")
        
        # 彻底清空缓冲区，确保后续命令不受干扰
        print("  ⏳ 清空缓冲区...")
        time.sleep(0.5)  # 等待订阅完全关闭和剩余事件传输
        
        # 主动读取并丢弃所有剩余数据
        if self.serial and self.serial.is_open:
            discarded_count = 0
            while self.serial.in_waiting > 0:
                discarded_count += self.serial.in_waiting
                self.serial.read(self.serial.in_waiting)
                time.sleep(0.05)  # 短暂等待，看是否还有新数据到达
            if discarded_count > 0:
                print(f"  🗑️ 已丢弃 {discarded_count} 字节残留数据")
        
        print(f"\n📈 监听统计:")
        print(f"  总事件数: {event_count}")
        print(f"  涉及引脚数: {len(pin_events)}")
        
        if pin_events:
            print("\n  各引脚事件数:")
            for pin, count in sorted(pin_events.items()):
                print(f"    {pin}: {count} 次")
        
        self.event_counts = pin_events
        return event_count > 0
    
    def verify_pins(self) -> List[TestResult]:
        """验证测试引脚"""
        print("\n🔬 验证测试引脚...")
        
        results = []
        
        # 获取当前状态
        current_status = self.get_current_status()
        if not current_status:
            print("❌ 无法获取当前状态")
            return results
        
        # 按模块验证
        for module, pins in self.TEST_PINS.items():
            print(f"\n📦 测试模块: {module}")
            
            for pin_info in pins:
                f407_pin = pin_info["f407_pin"]
                f103_signal = pin_info["f103_signal"]
                note = pin_info["note"]
                
                # 检查引脚是否在列表中
                pin_found = False
                pin_mode = -1
                pin_analog = False
                
                for p in self.pin_list:
                    if p["name"] == f407_pin:
                        pin_found = True
                        pin_mode = p["mode"]
                        pin_analog = p["analog"] == 1
                        break
                
                if not pin_found:
                    result = TestResult(
                        module=module,
                        pin_name=f407_pin,
                        f103_signal=f103_signal,
                        status="FAIL",
                        message=f"引脚 {f407_pin} 不在F407引脚列表中",
                        event_count=0
                    )
                    results.append(result)
                    print(f"  ❌ {f407_pin} ({f103_signal}): 引脚未定义")
                    continue
                
                # 获取当前值
                current_value = current_status.get(f407_pin, None)
                event_count = self.event_counts.get(f407_pin, 0)
                
                # 判断状态
                if current_value is not None:
                    status = "PASS"
                    message = f"正常监听 (当前值: {current_value}, 事件数: {event_count})"
                    
                    # 如果有事件，说明监听有效
                    if event_count > 0:
                        message += " ✨检测到活动"
                else:
                    status = "FAIL"
                    message = f"无法读取引脚状态"
                
                result = TestResult(
                    module=module,
                    pin_name=f407_pin,
                    f103_signal=f103_signal,
                    status=status,
                    message=message,
                    value=current_value,
                    event_count=event_count
                )
                results.append(result)
                
                # 显示结果
                icon = "✅" if status == "PASS" else "❌"
                print(f"  {icon} {f407_pin} ({f103_signal}): {message}")
        
        self.results = results
        return results
    
    def generate_report(self) -> str:
        """生成测试报告"""
        report = []
        report.append("=" * 80)
        report.append("F407引脚监听测试报告")
        report.append("=" * 80)
        report.append(f"测试时间: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report.append(f"串口设备: {self.port}")
        report.append("")
        
        # 统计
        total = len(self.results)
        passed = sum(1 for r in self.results if r.status == "PASS")
        failed = sum(1 for r in self.results if r.status == "FAIL")
        skipped = sum(1 for r in self.results if r.status == "SKIP")
        
        report.append("📊 测试统计:")
        report.append(f"  总计: {total}")
        report.append(f"  通过: {passed}")
        report.append(f"  失败: {failed}")
        report.append(f"  跳过: {skipped}")
        report.append("")
        
        # 按模块分组
        modules = {}
        for result in self.results:
            if result.module not in modules:
                modules[result.module] = []
            modules[result.module].append(result)
        
        # 详细结果
        report.append("📋 详细结果:")
        report.append("")
        
        for module, results in modules.items():
            report.append(f"【{module}】")
            for r in results:
                icon = "✅" if r.status == "PASS" else "❌" if r.status == "FAIL" else "⏭️"
                report.append(f"  {icon} {r.pin_name} (F103: {r.f103_signal})")
                report.append(f"     状态: {r.status}")
                report.append(f"     说明: {r.message}")
                if r.value is not None:
                    report.append(f"     当前值: {r.value}")
                report.append(f"     事件数: {r.event_count}")
            report.append("")
        
        report.append("=" * 80)
        
        # 建议
        report.append("💡 建议:")
        if failed > 0:
            report.append("  1. 检查失败引脚的接线是否正确")
            report.append("  2. 确认F103侧对应信号是否正常工作")
            report.append("  3. 检查100欧电阻是否正确串联")
        else:
            report.append("  ✅ 所有引脚监听正常，可以进行下一步实验")
        
        report.append("")
        report.append("=" * 80)
        
        return "\n".join(report)


def auto_detect_port() -> Optional[str]:
    """自动检测F407的USB CDC端口"""
    print("🔍 自动检测F407设备...")
    
    ports = serial.tools.list_ports.comports()
    
    # 优先匹配F407 Probe Bridge
    for port in ports:
        desc_lower = port.description.lower()
        # 优先匹配F407 Probe Bridge（最精确）
        if 'f407' in desc_lower and 'probe' in desc_lower:
            print(f"✅ 找到F407 Probe设备: {port.device} - {port.description}")
            return port.device
    
    # 其次匹配其他STM32或CDC设备
    for port in ports:
        desc_lower = port.description.lower()
        # 匹配包含f407、probe、bridge、stm32等关键词的设备
        if any(keyword in desc_lower for keyword in ['f407', 'probe', 'bridge', 'stm32', 'cdc']):
            print(f"✅ 找到设备: {port.device} - {port.description}")
            return port.device
        
        # 检查VID/PID（STM32的常见VID）
        if port.vid == 0x0483:  # STMicroelectronics
            print(f"✅ 找到STM32设备: {port.device} - {port.description}")
            return port.device
    
    print("❌ 未找到F407设备")
    print("可用端口:")
    for port in ports:
        print(f"  {port.device} - {port.description}")
    
    return None


def main():
    parser = argparse.ArgumentParser(
        description="F407引脚监听自动化测试脚本",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  # 自动检测端口，监听10秒
  python test_f407_pin_monitor.py --duration 10
  
  # 指定端口，监听5秒
  python test_f407_pin_monitor.py --port /dev/tty.usbmodem123456 --duration 5
  
  # 列出所有可用端口
  python test_f407_pin_monitor.py --list-ports
        """
    )
    
    parser.add_argument(
        '--port', '-p',
        type=str,
        default=None,
        help='串口设备路径（如 /dev/tty.usbmodem123456 或 COM3）'
    )
    
    parser.add_argument(
        '--baudrate', '-b',
        type=int,
        default=115200,
        help='波特率（默认: 115200）'
    )
    
    parser.add_argument(
        '--duration', '-d',
        type=float,
        default=5.0,
        help='监听持续时间（秒，默认: 5.0）'
    )
    
    parser.add_argument(
        '--list-ports', '-l',
        action='store_true',
        help='列出所有可用串口'
    )
    
    parser.add_argument(
        '--output', '-o',
        type=str,
        default=None,
        help='测试报告输出文件路径'
    )
    
    args = parser.parse_args()
    
    # 列出端口
    if args.list_ports:
        print("\n可用串口设备:")
        print("-" * 60)
        ports = serial.tools.list_ports.comports()
        if ports:
            for port in ports:
                print(f"设备: {port.device}")
                print(f"  描述: {port.description}")
                print(f"  VID:PID: {hex(port.vid)}:{hex(port.pid)}")
                print()
        else:
            print("未找到任何串口设备")
        return 0
    
    # 自动检测或使用指定端口
    port = args.port
    if not port:
        port = auto_detect_port()
        if not port:
            print("\n❌ 未找到F407设备，请使用 --port 参数手动指定")
            print("使用 --list-ports 查看所有可用端口")
            return 1
    
    # 创建测试器
    tester = F407ProbeTester(port=port, baudrate=args.baudrate)
    
    try:
        # 连接
        print(f"\n🔌 连接到 {port}...")
        if not tester.connect():
            return 1
        
        # 测试连接
        if not tester.test_connection():
            return 1
        
        # 获取引脚列表
        if not tester.get_pin_list():
            return 1
        
        # 测试监听
        tester.test_pin_monitoring(duration=args.duration)
        
        # 验证引脚
        tester.verify_pins()
        
        # 生成报告
        report = tester.generate_report()
        print("\n" + report)
        
        # 保存报告
        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(report)
            print(f"\n📄 测试报告已保存到: {args.output}")
        
        # 返回状态码
        failed = sum(1 for r in tester.results if r.status == "FAIL")
        return 1 if failed > 0 else 0
        
    except KeyboardInterrupt:
        print("\n\n⚠️ 测试被用户中断")
        return 1
    
    except Exception as e:
        print(f"\n❌ 测试异常: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    finally:
        tester.disconnect()


if __name__ == "__main__":
    sys.exit(main())
