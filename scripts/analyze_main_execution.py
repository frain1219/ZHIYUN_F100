#!/usr/bin/env python3
"""
分析F103 main函数执行流程
"""

print("\n" + "="*70)
print("F103 Main函数执行流程分析")
print("="*70)

print("\n【Main函数调用序列】")
print("="*70)

functions = [
    ("0x8003d1c", "函数1 - 未知"),
    ("0x8002f9c", "函数2 - 未知"),
    ("0x8003d84", "函数3 - 未知"),
    ("0x8003de8", "函数4 - 未知"),
    ("0x8003946", "函数5 - 未知"),
    ("0x800402c", "函数6 - 未知"),
    ("0x8004040", "函数7 - 未知"),
    ("0x8003b0a", "函数8 - 未知"),
    ("0x800405c", "函数9 - 未知"),
    ("0x8003aac", "函数10 - 未知"),
    ("0x8003a14", "函数11 - 未知"),
    ("0x80040bc", "函数12 - 未知"),
    ("0x800410e", "函数13 - 未知"),
    ("0x8004184", "SPI1_StartupSequence ← 关键函数!"),
]

for i, (addr, desc) in enumerate(functions, 1):
    print(f"{i:2d}. {addr}: {desc}")

print("\n【关键发现】")
print("="*70)

print("\n1. SPI1_StartupSequence调用位置")
print("   - 调用地址: 0x08002F2C (在main函数中)")
print("   - 函数地址: 0x08004184")
print("   - 调用指令: bl 0x8004184")

print("\n2. 当前执行位置分析")
print("   - 当前PC: 0x0800570C (延时函数)")
print("   - LR: 0x08004399 (在某个函数中)")
print("   - 栈上返回地址: 0x08002FC5 (在main函数中)")
print("   - 0x08002FC5 > 0x08002F2C (已过SPI1初始化位置)")

print("\n3. SPI1状态分析")
print("   - SPI1_CR1 = 0x00000000 (未初始化)")
print("   - SPI1_CR2 = 0x00000000 (未初始化)")
print("   - SPI1_SR = 0x00000000 (未初始化)")
print("   - RCC_APB2ENR bit 12 (SPI1EN) = 0 (时钟未使能)")

print("\n【矛盾分析】")
print("="*70)

print("\n矛盾点:")
print("1. F103已经执行过了SPI1_StartupSequence的调用位置")
print("2. 但是SPI1寄存器全为0，说明SPI1未初始化")

print("\n可能的原因:")
print("1. SPI1_StartupSequence函数执行失败")
print("2. SPI1初始化被条件跳过")
print("3. SPI1初始化在延时之后才会执行")
print("4. 需要特定的硬件条件才能初始化SPI1")

print("\n【SPI1_StartupSequence函数分析】")
print("="*70)

print("\n函数调用序列:")
print("1. bl 0x80055a4 - 未知函数")
print("2. bl 0x8003986 - 未知函数")
print("3. bl 0x80041c8 - 未知函数")
print("4. bl 0x80041e6 - 未知函数")
print("5. bl 0x8004290 - 未知函数")

print("\n【下一步建议】")
print("="*70)

print("\n1. 单步执行，观察SPI1初始化过程")
print("2. 检查SPI1_StartupSequence函数的条件判断")
print("3. 检查是否需要外部硬件信号")
print("4. 使用F407侧车实时监控SPI总线")

print("\n" + "="*70)
print("分析完成")
print("="*70)
