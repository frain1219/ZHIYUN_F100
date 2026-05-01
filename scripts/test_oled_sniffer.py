#!/usr/bin/env python3
"""
OLED Sniffer验证脚本
使用F407侧车平台验证OLED显示部分的逆向分析结果

使用方法:
    python test_oled_sniffer.py [--port /dev/tty.usbmodemxxx] [--duration 10]

依赖:
    pip install pyserial
"""

import argparse
import json
import serial
import serial.tools.list_ports
import sys
import time
from typing import Dict, List, Optional, Tuple


class OLEDSnifferTester:
    """OLED Sniffer测试器"""
    
    EXPECTED_INIT_SEQUENCE = [
        0xAE, 0x00, 0x10, 0x40, 0x81, 0xFF, 0xA1, 0xA8, 0x3F,
        0xAD, 0x8B, 0xC8, 0xD3, 0x00, 0xDA, 0x12, 0xD9, 0x22,
        0xDB, 0x2B, 0xA4, 0xA6, 0xAF
    ]
    
    def __init__(self, port: str, baudrate: int = 115200, timeout: float = 5.0):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial: Optional[serial.Serial] = None
        
    def connect(self) -> bool:
        """连接到F407"""
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout
            )
            time.sleep(0.5)
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
            self.serial.reset_input_buffer()
            time.sleep(0.05)
            
            if debug:
                print(f"  [DEBUG] 发送命令: {cmd}")
            self.serial.write((cmd + "\n").encode('utf-8'))
            self.serial.flush()
            
            time.sleep(0.2)
            
            response_lines = []
            buffer = ""
            start_time = time.time()
            
            while time.time() - start_time < self.timeout:
                bytes_to_read = self.serial.in_waiting
                if bytes_to_read > 0:
                    data = self.serial.read(bytes_to_read).decode('utf-8', errors='ignore')
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
                                response = json.loads(line)
                                response_lines.append(response)
                                break
                            except json.JSONDecodeError:
                                continue
                
                if response_lines:
                    break
                    
                time.sleep(0.01)
            
            return response_lines[0] if response_lines else None
            
        except Exception as e:
            print(f"❌ 通信错误: {e}")
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
    
    def start_oled_sniffer(self) -> bool:
        """启动OLED sniffer"""
        print("\n🔍 启动OLED Sniffer...")
        response = self.send_command("OLED_SNIFF_START")
        
        if response and response.get("type") == "ok":
            print("✅ OLED Sniffer已启动")
            return True
        else:
            print("❌ 启动OLED Sniffer失败")
            return False
    
    def stop_oled_sniffer(self) -> bool:
        """停止OLED sniffer"""
        print("\n🔍 停止OLED Sniffer...")
        response = self.send_command("OLED_SNIFF_STOP")
        
        if response and response.get("type") == "ok":
            print("✅ OLED Sniffer已停止")
            return True
        else:
            print("❌ 停止OLED Sniffer失败")
            return False
    
    def get_sniffer_status(self) -> Optional[Dict]:
        """获取sniffer状态"""
        response = self.send_command("OLED_SNIFF_STATUS")
        
        if response and response.get("type") == "oled_status":
            return response
        else:
            return None
    
    def dump_bytes(self, limit: int = 128) -> Optional[List[int]]:
        """导出捕获的字节数据"""
        response = self.send_command(f"OLED_SNIFF_DUMP bytes {limit}")
        
        if response and response.get("type") == "oled_dump":
            items = response.get("items", [])
            return items
        else:
            return None
    
    def dump_transactions(self, limit: int = 32) -> Optional[List[Dict]]:
        """导出捕获的事务数据"""
        response = self.send_command(f"OLED_SNIFF_DUMP txn {limit}")
        
        if response and response.get("type") == "oled_dump":
            items = response.get("items", [])
            return [{"start": t[0], "end": t[1], "len": t[2]} for t in items]
        else:
            return None
    
    def verify_init_sequence(self, captured_bytes: List[int]) -> Tuple[bool, str]:
        """验证初始化序列"""
        print("\n🔍 验证OLED初始化序列...")
        
        if len(captured_bytes) < len(self.EXPECTED_INIT_SEQUENCE):
            return False, f"捕获数据不足: 期望至少{len(self.EXPECTED_INIT_SEQUENCE)}字节, 实际{len(captured_bytes)}字节"
        
        for i, expected_byte in enumerate(self.EXPECTED_INIT_SEQUENCE):
            if i >= len(captured_bytes):
                return False, f"捕获数据不完整: 期望{len(self.EXPECTED_INIT_SEQUENCE)}字节, 实际{i}字节"
            
            if captured_bytes[i] != expected_byte:
                return False, f"第{i}字节不匹配: 期望0x{expected_byte:02X}, 实际0x{captured_bytes[i]:02X}"
        
        return True, f"初始化序列完全匹配 ({len(self.EXPECTED_INIT_SEQUENCE)}字节)"
    
    def verify_framebuffer_format(self, transactions: List[Dict]) -> Tuple[bool, str]:
        """验证帧缓冲格式"""
        print("\n🔍 验证OLED帧缓冲格式...")
        
        if not transactions:
            return False, "未捕获到任何事务"
        
        expected_txn_count = 8
        expected_bytes_per_txn = 99
        
        if len(transactions) < expected_txn_count:
            return False, f"事务数量不足: 期望至少{expected_txn_count}个, 实际{len(transactions)}个"
        
        for i, txn in enumerate(transactions[:expected_txn_count]):
            if txn["len"] != expected_bytes_per_txn:
                return False, f"第{i}个事务长度不匹配: 期望{expected_bytes_per_txn}字节, 实际{txn['len']}字节"
        
        return True, f"帧缓冲格式匹配 (8个事务, 每个{expected_bytes_per_txn}字节)"
    
    def run_verification(self, duration: float = 10.0) -> Dict:
        """运行完整验证"""
        print(f"\n{'='*80}")
        print("OLED Sniffer验证测试")
        print(f"{'='*80}")
        print(f"测试时间: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"串口设备: {self.port}")
        print(f"监听时长: {duration}秒")
        
        results = {
            "connection": False,
            "sniffer_start": False,
            "capture": False,
            "init_sequence": False,
            "framebuffer": False,
            "init_message": "",
            "framebuffer_message": "",
            "captured_bytes": [],
            "transactions": []
        }
        
        if not self.test_connection():
            return results
        
        results["connection"] = True
        
        if not self.start_oled_sniffer():
            return results
        
        results["sniffer_start"] = True
        
        print(f"\n⏳ 监听OLED通信 {duration}秒...")
        print("提示: 如果F103正在运行,应该能捕获到OLED初始化序列和帧缓冲数据")
        
        time.sleep(duration)
        
        status = self.get_sniffer_status()
        if status:
            print(f"\n📊 Sniffer状态:")
            print(f"  总线类型: {status.get('bus_type', 'unknown')}")
            print(f"  边沿计数: {status.get('edge_count', 0)}")
            print(f"  事务计数: {status.get('txn_count', 0)}")
            print(f"  字节计数: {status.get('byte_count', 0)}")
            print(f"  丢弃计数: {status.get('drop_count', 0)}")
            
            if status.get('byte_count', 0) > 0:
                results["capture"] = True
        
        self.stop_oled_sniffer()
        
        if results["capture"]:
            print("\n📦 导出捕获数据...")
            
            captured_bytes = self.dump_bytes(limit=256)
            if captured_bytes:
                results["captured_bytes"] = captured_bytes[:len(self.EXPECTED_INIT_SEQUENCE)]
                print(f"  导出 {len(captured_bytes)} 字节")
                print(f"  前23字节: {' '.join([f'{b:02X}' for b in captured_bytes[:23]])}")
            
            transactions = self.dump_transactions(limit=16)
            if transactions:
                results["transactions"] = transactions
                print(f"  导出 {len(transactions)} 个事务")
                for i, txn in enumerate(transactions[:8]):
                    print(f"    事务{i}: {txn['len']}字节")
            
            if captured_bytes and len(captured_bytes) >= len(self.EXPECTED_INIT_SEQUENCE):
                init_match, init_msg = self.verify_init_sequence(captured_bytes)
                results["init_sequence"] = init_match
                results["init_message"] = init_msg
                print(f"\n{'✅' if init_match else '❌'} {init_msg}")
            
            if transactions:
                fb_match, fb_msg = self.verify_framebuffer_format(transactions)
                results["framebuffer"] = fb_match
                results["framebuffer_message"] = fb_msg
                print(f"\n{'✅' if fb_match else '❌'} {fb_msg}")
        
        return results
    
    def generate_report(self, results: Dict) -> str:
        """生成测试报告"""
        report = []
        report.append("\n" + "="*80)
        report.append("OLED Sniffer验证测试报告")
        report.append("="*80)
        report.append(f"测试时间: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report.append(f"串口设备: {self.port}")
        report.append("")
        
        report.append("📊 测试结果:")
        report.append(f"  连接测试: {'✅ 通过' if results['connection'] else '❌ 失败'}")
        report.append(f"  Sniffer启动: {'✅ 通过' if results['sniffer_start'] else '❌ 失败'}")
        report.append(f"  数据捕获: {'✅ 通过' if results['capture'] else '❌ 失败'}")
        report.append(f"  初始化序列验证: {'✅ 通过' if results['init_sequence'] else '❌ 失败'}")
        report.append(f"  帧缓冲格式验证: {'✅ 通过' if results['framebuffer'] else '❌ 失败'}")
        report.append("")
        
        if results["init_message"]:
            report.append(f"初始化序列: {results['init_message']}")
        
        if results["framebuffer_message"]:
            report.append(f"帧缓冲格式: {results['framebuffer_message']}")
        
        if results["captured_bytes"]:
            report.append("")
            report.append("捕获的初始化序列 (前23字节):")
            report.append(f"  {' '.join([f'{b:02X}' for b in results['captured_bytes']])}")
            report.append("")
            report.append("期望的初始化序列:")
            report.append(f"  {' '.join([f'{b:02X}' for b in self.EXPECTED_INIT_SEQUENCE])}")
        
        report.append("")
        report.append("="*80)
        
        overall_pass = all([
            results['connection'],
            results['sniffer_start'],
            results['capture'],
            results['init_sequence'],
            results['framebuffer']
        ])
        
        if overall_pass:
            report.append("✅ OLED显示部分硬件层逆向验证通过!")
            report.append("   所有验证项均匹配,确认逆向分析结果正确。")
        else:
            report.append("❌ OLED显示部分硬件层逆向验证未完全通过")
            report.append("   请检查失败项并重新测试。")
        
        report.append("="*80)
        
        return "\n".join(report)


def auto_detect_port() -> Optional[str]:
    """自动检测F407的USB CDC端口"""
    print("🔍 自动检测F407设备...")
    
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        desc_lower = port.description.lower()
        if 'f407' in desc_lower and 'probe' in desc_lower:
            print(f"✅ 找到F407 Probe设备: {port.device} - {port.description}")
            return port.device
    
    for port in ports:
        desc_lower = port.description.lower()
        if any(keyword in desc_lower for keyword in ['f407', 'probe', 'bridge', 'stm32', 'cdc']):
            print(f"✅ 找到设备: {port.device} - {port.description}")
            return port.device
        
        if port.vid == 0x0483:
            print(f"✅ 找到STM32设备: {port.device} - {port.description}")
            return port.device
    
    print("❌ 未找到F407设备")
    return None


def main():
    parser = argparse.ArgumentParser(
        description="OLED Sniffer验证脚本",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '--port', '-p',
        type=str,
        default=None,
        help='串口设备路径'
    )
    
    parser.add_argument(
        '--duration', '-d',
        type=float,
        default=10.0,
        help='监听持续时间（秒）'
    )
    
    parser.add_argument(
        '--output', '-o',
        type=str,
        default=None,
        help='测试报告输出文件路径'
    )
    
    args = parser.parse_args()
    
    port = args.port
    if not port:
        port = auto_detect_port()
        if not port:
            print("\n❌ 未找到F407设备，请使用 --port 参数手动指定")
            return 1
    
    tester = OLEDSnifferTester(port=port)
    
    try:
        if not tester.connect():
            return 1
        
        results = tester.run_verification(duration=args.duration)
        
        report = tester.generate_report(results)
        print(report)
        
        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(report)
            print(f"\n📄 测试报告已保存到: {args.output}")
        
        overall_pass = all([
            results['connection'],
            results['sniffer_start'],
            results['capture'],
            results['init_sequence'],
            results['framebuffer']
        ])
        
        return 0 if overall_pass else 1
        
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
