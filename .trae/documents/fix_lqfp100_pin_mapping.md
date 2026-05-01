# 修复STM32F407VGT6 (LQFP100) 未引出引脚使用问题

## 问题分析

STM32F407VGT6采用LQFP100封装，**GPIOF和GPIOG的全部引脚均未引出**，但当前固件代码和文档中仍在使用这些引脚，导致硬件无法正常工作。

### 受影响的文件和位置

#### 1. 文档问题
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/APM32F103CB_to_STM32F407V_Wiring.md`
- **位置**: 第130-133行（模拟采样模块ADC）
- **问题**: 使用了PF3, PF4, PF5, PF6（均未引出）

#### 2. 固件代码问题

##### 2.1 probe_pins.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_pins.c`
- **位置**: 第27-30行
- **问题**: 定义了PF3, PF4, PF5, PF6引脚

##### 2.2 probe_adc.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_adc.c`
- **位置**: 第19-22行
- **问题**: ADC配置使用了PF3, PF4, PF5, PF6
- **额外问题**: 使用ADC3，但ADC3在LQFP100封装上可用通道极少

##### 2.3 inject_engine.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/inject_engine.c`
- **位置**: 第297行、第300行
- **问题**: 
  - 第297行使用PF1（未引出）
  - 第300行使用PG2（已重映射为PC0，但代码未更新）

##### 2.4 bridge_protocol.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c`
- **位置**: 第188行
- **问题**: 自检消息中提到PG2, PG5, PG6, PG7（已重映射，但消息未更新）

##### 2.5 bypass_fsm.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bypass_fsm.c`
- **位置**: 第164行
- **问题**: 使用PF1（未引出）

##### 2.6 main.c
- **文件**: `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/main.c`
- **位置**: 第250行
- **问题**: EXTI回调中将GPIO_PIN_5/6/7映射到GPIOG，应映射到GPIOC

## 修复方案

### ADC引脚重映射方案

由于ADC3在LQFP100封装上可用通道极少（仅PC0-PC5），需要切换到ADC1/2并使用PA0-PA3引脚：

| F103 信号 | 原F407引脚 | 新F407引脚 | ADC控制器 | ADC通道 | 说明 |
|----------|----------|----------|---------|--------|------|
| PA3 (ADC) | PF3 ❌ | PA0 | ADC1/2 | Channel 0 | 需确认不与PE5冲突 |
| PA4 (ADC) | PF4 ❌ | PA1 | ADC1/2 | Channel 1 | 需确认不与PE6冲突 |
| PB0 (ADC) | PF5 ❌ | PA2 | ADC1/2 | Channel 2 | 需确认不与PE7冲突 |
| PB1 (ADC) | PF6 ❌ | PA3 | ADC1/2 | Channel 3 | 新增ADC引脚 |

**注意**: PA0-PA3在probe_pins表中已被使用（PE5/PE6/PE7对应F103的PA0/PA1/PA2），但它们是**不同的物理引脚**：
- F407的PA0-PA3是物理引脚，用于ADC采样
- F407的PE5/PE6/PE7是物理引脚，用于数字监听
- 两者不冲突

### 其他引脚修复

| 原引脚 | 新引脚 | 功能 | 文件 |
|--------|--------|------|------|
| PF1 | PB8 | BYPASS_FSM注入 | bypass_fsm.c, inject_engine.c |
| PG2 | PC0 | 按键注入（已重映射） | inject_engine.c |

## 实施步骤

### 步骤1: 更新文档
- [ ] 修改 `APM32F103CB_to_STM32F407V_Wiring.md` 第130-133行
- [ ] 将PF3/PF4/PF5/PF6替换为PA0/PA1/PA2/PA3
- [ ] 添加说明：ADC功能从ADC3迁移到ADC1

### 步骤2: 修改probe_pins.c
- [ ] 删除第27-30行的PF3-PF6定义
- [ ] 添加PA0-PA3的ADC引脚定义
- [ ] 移除GPIOF时钟使能（第40行），如果不再使用GPIOF

### 步骤3: 修改probe_adc.c
- [ ] 将ADC3改为ADC1
- [ ] 更新第19-22行的引脚名称和通道配置
- [ ] 更新ADC通道映射：
  - PA0 -> ADC_CHANNEL_0
  - PA1 -> ADC_CHANNEL_1
  - PA2 -> ADC_CHANNEL_2
  - PA3 -> ADC_CHANNEL_3
- [ ] 修改初始化函数，使用ADC1而非ADC3

### 步骤4: 修改inject_engine.c
- [ ] 第297行：将PF1改为PB8（或禁用该场景）
- [ ] 第300行：将PG2改为PC0

### 步骤5: 修改bridge_protocol.c
- [ ] 第188行：更新自检消息中的exti_watch列表
- [ ] 将"PG2","PG5","PG6","PG7"改为"PC0","PC1","PC2","PC3"

### 步骤6: 修改bypass_fsm.c
- [ ] 第164行：将PF1改为PB8（或移除该释放操作）

### 步骤7: 修改main.c
- [ ] 第250行：将GPIOG改为GPIOC
- [ ] 确保EXTI回调正确映射PC1/PC2/PC3

### 步骤8: 验证和测试
- [ ] 编译固件，确认无错误
- [ ] 检查所有引脚定义的一致性
- [ ] 更新相关文档（如有）

## 风险评估

### 低风险
- 文档更新：仅影响接线说明
- 引脚名称更新：已验证新引脚可用

### 中等风险
- ADC控制器从ADC3切换到ADC1：需要验证ADC1初始化和配置
- BYPASS_FSM功能：PF1/PB8替换可能影响功能，需测试

### 需要确认的问题
1. **PA0-PA3是否与现有功能冲突**？
   - 当前PE5/PE6/PE7监听F103的PA0/PA1/PA2
   - F407的PA0-PA3用于ADC采样F103的PA3/PA4/PB0/PB1
   - 物理上不冲突，但需确认原理图

2. **PB8是否可用于BYPASS_FSM**？
   - 需确认PB8在LQFP100封装上是否引出（已引出）
   - 需确认PB8未被其他功能占用

3. **ADC1是否与其他功能冲突**？
   - ADC1和ADC2在STM32F407上共享某些资源
   - 需确认无其他代码使用ADC1

## 预期结果

修复完成后：
1. 所有引脚均使用LQFP100封装上实际引出的GPIO
2. ADC功能正常工作（使用ADC1和PA0-PA3）
3. EXTI监听功能正常（使用PC0-PC3）
4. 注入场景功能正常（使用PC0和PB8）
5. 文档与代码完全一致
