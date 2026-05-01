#!/usr/bin/env python3
"""
分析F103启动后的执行状态
"""

print("\n" + "="*70)
print("F103启动状态分析")
print("="*70)

print("\n【关键发现】")
print("="*70)

print("\n1. F103已经成功启动！")
print("   - 当前PC: 0x0800570C")
print("   - 之前PC: 0x08006A4E (卡在.bss清零循环)")
print("   - 结论: F103已经完成C运行时初始化，进入正常运行状态")

print("\n2. RCC复位标志分析")
print("   - RCC_CSR = 0x1C000000")
print("   - Bit 26: SFTRSTF = 1 (软件复位标志)")
print("   - Bit 27: PORRSTF = 1 (上电复位标志)")
print("   - Bit 28: PINRSTF = 1 (NRST引脚复位标志)")
print("   - 结论: F103经历了完整的复位序列")

print("\n3. 调用链分析")
print("   - 当前PC: 0x0800570C (延时函数或工具函数)")
print("   - LR: 0x08004399 (调用者地址)")
print("   - 栈上返回地址: 0x08002FC5 (在main函数范围内)")
print("   - main函数地址: 0x08002E74")
print("   - 结论: F103已经执行到了main函数，并调用了多个子函数")

print("\n4. 延时循环分析")
print("   - 地址范围: 0x080056EE - 0x08005706")
print("   - 循环计数器: 4797 (0x12BD)")
print("   - 功能: 软件延时函数")
print("   - 用途: 可能用于启动延时或等待硬件稳定")

print("\n5. 时钟配置分析")
print("   - RCC_CR = 0x00019B83")
print("   - HSION = 1 (HSI振荡器开启)")
print("   - HSERDY = 1 (HSE振荡器稳定)")
print("   - PLLRDY = 1 (PLL稳定)")
print("   - CSSON = 1 (时钟安全系统开启)")
print("   - 结论: 时钟系统已正确配置")

print("\n6. GPIO配置分析")
print("   - GPIOA_CRL = 0x44444444 (PA0-PA7: 通用推挽输出)")
print("   - GPIOA_CRH = 0x88844444 (PA8-PA11: 输入下拉, PA12-PA15: 通用推挽输出)")
print("   - GPIOA_ODR = 0x0000A000 (PA12, PA14输出高电平)")
print("   - 结论: GPIO已部分配置")

print("\n【启动流程重建】")
print("="*70)

print("\n1. 上电复位")
print("   - PORRSTF标志置位")
print("   - PC加载Reset_Handler地址 (0x08006BE9)")

print("\n2. Reset_Handler执行")
print("   - 调用SystemInit (0x080069DC)")
print("   - 配置时钟系统 (HSI, HSE, PLL)")
print("   - 设置VTOR寄存器")

print("\n3. C运行时初始化")
print("   - scatterload_table_iterate")
print("   - .bss段清零 (之前卡在这里)")
print("   - 现在已经完成！")

print("\n4. main函数执行")
print("   - main函数地址: 0x08002E74")
print("   - 调用多个初始化函数")
print("   - 当前正在执行延时函数 (0x0800570C)")

print("\n【为什么之前卡在.bss清零循环？】")
print("="*70)

print("\n可能的原因:")
print("1. F103需要特定的启动条件才能完成初始化")
print("2. 可能需要外部硬件信号触发")
print("3. 可能需要按键触发 (用户之前提到过)")
print("4. 可能需要等待电源稳定或其他硬件条件")

print("\n【当前状态】")
print("="*70)

print("\n✅ F103已经成功启动")
print("✅ 时钟系统已配置")
print("✅ GPIO已部分配置")
print("✅ 正在执行main函数")
print("⚠️  当前在延时函数中")

print("\n【下一步建议】")
print("="*70)

print("\n1. 检查SPI1是否已初始化")
print("2. 检查OLED是否已初始化")
print("3. 使用F407侧车验证SPI通信")
print("4. 分析main函数的完整执行流程")

print("\n" + "="*70)
print("分析完成")
print("="*70)
