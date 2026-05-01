#!/usr/bin/env python3
"""
按键触发 + OLED监听测试脚本
通过F407注入按键信号来触发F103的OLED初始化
"""

import serial
import json
import time
import sys

class ButtonTriggerOLEDSniffer:
    def __init__(self, port):
        self.port = port
        self.ser = None
        
    def connect(self):
        """连接到F407"""
        try:
            self.ser = serial.Serial(self.port, 115200, timeout=2.0)
            time.sleep(0.5)
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            return True
        except Exception as e:
            print(f"❌ 连接失败: {e}")
            return False
    
    def disconnect(self):
        """断开连接"""
        if self.ser and self.ser.is_open:
            self.ser.close()
    
    def send_cmd(self, cmd, debug=False):
        """发送命令并接收响应"""
        if not self.ser or not self.ser.is_open:
            return None
        
        try:
            self.ser.reset_input_buffer()
            time.sleep(0.05)
            
            if debug:
                print(f"  [TX] {cmd}")
            
            self.ser.write((cmd + "\n").encode('utf-8'))
            self.ser.flush()
            time.sleep(0.2)
            
            buffer = ""
            start_time = time.time()
            
            while time.time() - start_time < 2.0:
                bytes_to_read = self.ser.in_waiting
                if bytes_to_read > 0:
                    data = self.ser.read(bytes_to_read).decode('utf-8', errors='ignore')
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
                                if debug:
                                    print(f"  [RX] {line[:100]}...")
                                return response
                            except json.JSONDecodeError:
                                continue
                
                time.sleep(0.01)
            
            return None
            
        except Exception as e:
            print(f"❌ 通信错误: {e}")
            return None
    
    def test_connection(self):
        """测试连接"""
        print("\n🔍 测试F407连接...")
        response = self.send_cmd("PING")
        
        if response and response.get("type") == "pong":
            print("✅ F407连接正常")
            return True
        else:
            print("❌ F407连接失败")
            return False
    
    def start_oled_sniffer(self):
        """启动OLED Sniffer"""
        print("\n🔍 启动OLED Sniffer...")
        response = self.send_cmd("OLED_SNIFF_START")
        
        if response and response.get("type") == "ok":
            print("✅ OLED Sniffer已启动")
            return True
        else:
            print("❌ 启动OLED Sniffer失败")
            return False
    
    def stop_oled_sniffer(self):
        """停止OLED Sniffer"""
        response = self.send_cmd("OLED_SNIFF_STOP")
        return response and response.get("type") == "ok"
    
    def get_sniffer_status(self):
        """获取Sniffer状态"""
        return self.send_cmd("OLED_SNIFF_STATUS")
    
    def inject_button_press(self, pin, duration_ms=100):
        """注入按键信号"""
        print(f"\n🔘 注入按键信号: {pin} (持续{duration_ms}ms)...")
        
        # 拉低(模拟按下)
        response = self.send_cmd(f"GPIO_SET {pin} 0 {duration_ms}")
        
        if response and response.get("type") == "ok":
            print(f"✅ 按键信号注入成功")
            return True
        else:
            print(f"❌ 按键信号注入失败")
            return False
    
    def inject_button_sequence(self, pin, press_count=1, duration_ms=100, interval_ms=200):
        """注入按键序列"""
        print(f"\n🔘 注入按键序列: {pin} x{press_count}次")
        
        for i in range(press_count):
            print(f"  第{i+1}次按下...")
            
            # 拉低(按下)
            self.send_cmd(f"GPIO_SET {pin} 0")
            time.sleep(duration_ms / 1000.0)
            
            # 释放
            self.send_cmd(f"GPIO_RELEASE {pin}")
            
            if i < press_count - 1:
                time.sleep(interval_ms / 1000.0)
        
        print("✅ 按键序列注入完成")
        return True
    
    def run_test(self):
        """运行完整测试"""
        print(f"\n{'='*80}")
        print("按键触发 + OLED监听测试")
        print(f"{'='*80}")
        print(f"测试时间: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"串口设备: {self.port}")
        
        if not self.test_connection():
            return False
        
        # 启动OLED Sniffer
        if not self.start_oled_sniffer():
            return False
        
        print("\n⏳ 监听OLED通信...")
        print("提示: 将注入按键信号来触发OLED初始化")
        
        # 等待1秒,观察初始状态
        time.sleep(1)
        
        # 获取初始状态
        status_before = self.get_sniffer_status()
        if status_before:
            print(f"\n📊 按键触发前的状态:")
            print(f"  总线类型: {status_before.get('bus_type', 'unknown')}")
            print(f"  边沿计数: {status_before.get('edge_count', 0)}")
            print(f"  事务计数: {status_before.get('txn_count', 0)}")
            print(f"  字节计数: {status_before.get('byte_count', 0)}")
        
        # 注入按键信号 - 尝试PA1 (PE6)
        print("\n" + "="*60)
        print("第一阶段: 尝试PA1按键 (PE6)")
        print("="*60)
        self.inject_button_sequence("PE6", press_count=2, duration_ms=100, interval_ms=200)
        
        # 等待2秒,观察OLED响应
        print("\n⏳ 等待OLED响应...")
        time.sleep(2)
        
        # 检查状态
        status_after1 = self.get_sniffer_status()
        if status_after1:
            print(f"\n📊 PA1按键触发后的状态:")
            print(f"  总线类型: {status_after1.get('bus_type', 'unknown')}")
            print(f"  边沿计数: {status_after1.get('edge_count', 0)}")
            print(f"  事务计数: {status_after1.get('txn_count', 0)}")
            print(f"  字节计数: {status_after1.get('byte_count', 0)}")
        
        # 注入按键信号 - 尝试PB4 (PC0)
        print("\n" + "="*60)
        print("第二阶段: 尝试PB4按键 (PC0)")
        print("="*60)
        self.inject_button_sequence("PC0", press_count=2, duration_ms=100, interval_ms=200)
        
        # 等待2秒,观察OLED响应
        print("\n⏳ 等待OLED响应...")
        time.sleep(2)
        
        # 检查状态
        status_after2 = self.get_sniffer_status()
        if status_after2:
            print(f"\n📊 PB4按键触发后的状态:")
            print(f"  总线类型: {status_after2.get('bus_type', 'unknown')}")
            print(f"  边沿计数: {status_after2.get('edge_count', 0)}")
            print(f"  事务计数: {status_after2.get('txn_count', 0)}")
            print(f"  字节计数: {status_after2.get('byte_count', 0)}")
        
        # 停止OLED Sniffer
        self.stop_oled_sniffer()
        
        # 分析结果
        print("\n" + "="*80)
        print("测试结果分析")
        print("="*80)
        
        if status_before and status_after2:
            edge_diff = status_after2.get('edge_count', 0) - status_before.get('edge_count', 0)
            txn_diff = status_after2.get('txn_count', 0) - status_before.get('txn_count', 0)
            byte_diff = status_after2.get('byte_count', 0) - status_before.get('byte_count', 0)
            
            print(f"边沿变化: +{edge_diff}")
            print(f"事务变化: +{txn_diff}")
            print(f"字节变化: +{byte_diff}")
            
            if byte_diff > 0:
                print("\n✅ 成功捕获到OLED数据!")
                print("   SPI1已初始化,OLED通信正常")
                return True
            elif edge_diff > 0 or txn_diff > 0:
                print("\n⚠️ 检测到SPI活动,但未捕获到完整字节")
                print("   可能需要更长的监听时间或不同的触发方式")
                return False
            else:
                print("\n❌ 未检测到任何SPI活动")
                print("   按键触发可能未能启动OLED初始化")
                return False
        
        return False


def main():
    import serial.tools.list_ports
    
    # 自动检测端口
    print("🔍 自动检测F407设备...")
    ports = serial.tools.list_ports.comports()
    
    port = None
    device_map = {}
    
    # 构建设备映射 (cu -> tty)
    for p in ports:
        device_map[p.device] = p
        # 如果是cu设备,也添加对应的tty设备
        if '/dev/cu.' in p.device:
            tty_device = p.device.replace('/dev/cu.', '/dev/tty.')
            device_map[tty_device] = p
    
    # 优先匹配F407 Probe Bridge (使用tty设备)
    for dev, p in device_map.items():
        desc_lower = p.description.lower()
        if 'f407' in desc_lower and 'probe' in desc_lower:
            # 优先使用tty设备
            if '/dev/tty.' in dev:
                print(f"✅ 找到F407 Probe设备: {dev}")
                port = dev
                break
            elif port is None:
                port = dev
    
    if not port:
        for dev, p in device_map.items():
            desc_lower = p.description.lower()
            if any(keyword in desc_lower for keyword in ['f407', 'probe', 'bridge', 'stm32']):
                if '/dev/tty.' in dev:
                    print(f"✅ 找到设备: {dev}")
                    port = dev
                    break
                elif port is None:
                    port = dev
    
    if not port:
        print("❌ 未找到F407设备")
        return 1
    
    tester = ButtonTriggerOLEDSniffer(port)
    
    try:
        if not tester.connect():
            return 1
        
        success = tester.run_test()
        
        return 0 if success else 1
        
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
