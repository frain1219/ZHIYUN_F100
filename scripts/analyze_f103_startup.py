#!/usr/bin/env python3
"""
F103启动检测机制分析脚本
检查复位后的状态和启动条件
"""

import subprocess
import time
import re

class F103StartupAnalyzer:
    def __init__(self):
        self.gdb_path = "/opt/homebrew/bin/arm-none-eabi-gdb"
        self.elf_path = "/Users/milocheung/Documents/codeProject/ZHIYUN_F100/extracted_firmware/F103_OLED_MCU/F103_OLED_MCU.elf"
        
    def run_gdb_script(self, script_path):
        """运行GDB脚本"""
        cmd = [
            self.gdb_path,
            "-ex", "target extended-remote localhost:3333",
            "-ex", f"source {script_path}",
            "-ex", "quit",
            self.elf_path
        ]
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=10
            )
            return result.stdout
        except subprocess.TimeoutExpired:
            return "Error: GDB timeout"
        except Exception as e:
            return f"Error: {e}"
    
    def check_reset_status(self):
        """检查复位状态"""
        print("\n" + "="*60)
        print("F103复位状态检查")
        print("="*60)
        
        output = self.run_gdb_script("/Users/milocheung/Documents/codeProject/ZHIYUN_F100/scripts/check_f103_reset_status.gdb")
        print(output)
        
        return output
    
    def analyze_boot_conditions(self):
        """分析启动条件"""
        print("\n" + "="*60)
        print("F103启动条件分析")
        print("="*60)
        
        print("\n1. BOOT引脚配置:")
        print("   - BOOT0: 决定启动模式")
        print("     * 0: 从Flash启动 (0x08000000)")
        print("     * 1: 从系统存储器启动")
        print("   - BOOT1: 辅助启动模式选择")
        print("     * 与BOOT0配合决定启动模式")
        
        print("\n2. 启动模式:")
        print("   - BOOT0=0, BOOT1=x: 主Flash启动")
        print("   - BOOT0=1, BOOT1=0: 系统存储器启动")
        print("   - BOOT0=1, BOOT1=1: 内置SRAM启动")
        
        print("\n3. 复位序列:")
        print("   1. 读取0x08000000处的初始SP值")
        print("   2. 读取0x08000004处的复位向量(PC)")
        print("   3. 跳转到复位向量执行")
        
    def check_power_sequence(self):
        """检查电源序列"""
        print("\n" + "="*60)
        print("电源和时钟检查")
        print("="*60)
        
        print("\n需要检查的硬件条件:")
        print("1. VDD电源: 2.0V - 3.6V")
        print("2. VBATT电池电源: RTC备份")
        print("3. 复位信号: NRST引脚状态")
        print("4. 时钟源: HSE/HSI/LSI/LSE")
        
    def analyze_startup_code(self):
        """分析启动代码"""
        print("\n" + "="*60)
        print("启动代码分析")
        print("="*60)
        
        print("\n关键启动文件:")
        print("1. startup_stm32f103xb.s - 启动汇编代码")
        print("2. system_stm32f1xx.c - 系统初始化")
        print("3. main.c - 主函数")
        
        print("\n启动流程:")
        print("Reset_Handler ->")
        print("  SystemInit() ->")
        print("  __main() ->")
        print("  $Sub$$main() [如果有] ->")
        print("  main()")
        
    def check_for_boot_conditions_in_code(self):
        """检查代码中的启动条件"""
        print("\n" + "="*60)
        print("代码中的启动条件检测")
        print("="*60)
        
        print("\n可能存在的启动条件检测:")
        print("1. 电源检测: PWR_CSR->SBF (待机标志)")
        print("2. 复位标志: RCC_CSR->SFTRSTF/PORRSTF/PINRSTF")
        print("3. 看门狗复位: RCC_CSR->WDGRSTF")
        print("4. 软件复位: RCC_CSR->SFTRSTF")
        print("5. 低功耗模式: PWR_CSR->WUF (唤醒标志)")
        
    def run_analysis(self):
        """运行完整分析"""
        print("\n" + "="*60)
        print("F103启动检测机制分析")
        print("="*60)
        
        # 1. 检查复位状态
        self.check_reset_status()
        
        # 2. 分析启动条件
        self.analyze_boot_conditions()
        
        # 3. 检查电源序列
        self.check_power_sequence()
        
        # 4. 分析启动代码
        self.analyze_startup_code()
        
        # 5. 检查代码中的启动条件
        self.check_for_boot_conditions_in_code()
        
        print("\n" + "="*60)
        print("分析完成")
        print("="*60)

if __name__ == "__main__":
    analyzer = F103StartupAnalyzer()
    analyzer.run_analysis()
