# F407VGT6 固件修复计划

## 问题概述

### 问题 1：PF1 和 PG1 引脚不可用
STM32F407VGT6 LQFP100 封装中，PF1 和 PG1 未引出，导致：
- `bypass_fsm.c` 中 `g_pin_pb3` 查找 `PG1` 返回 NULL
- `InjectEngine_Set("PF1", ...)` 操作无法执行
- 接线文档中 PB11 和 PB3 信号无法监听/注入

### 问题 2：晶振配置错误
开发板实际使用 8MHz 晶振，但固件配置为 25MHz：
- `platformio.ini`: `HSE_VALUE=25000000`
- `stm32f4xx_hal_conf.h`: `HSE_VALUE 25000000U`
- `main.c`: `PLLM = 25`（PLL M 分频系数）

---

## 修复方案

### 任务 1：重新分配 PF1 和 PG1 到可用引脚

**引脚选择原则**：
- 选择 LQFP100 封装中可用的 GPIO
- 优先选择支持 EXTI 的引脚（便于边沿检测）
- 避免与现有引脚冲突

**新引脚分配**：
| F103 信号 | 原F407引脚 | 新F407引脚 | 理由 |
|-----------|------------|------------|------|
| PB11 (PD GPIO) | PF1 (不可用) | **PE1** | 支持 EXTI，与 PE0 相邻便于管理 |
| PB3 (VS_EN) | PG1 (不可用) | **PE2** | 支持 EXTI，与 PE0/PE1 相邻便于管理 |

**需修改的文件**：

1. **`src/probe_pins.c`**
   - 添加 `PE1` 引脚定义（用于 F103 PB11 监听/注入）
   - 添加 `PE2` 引脚定义（用于 F103 PB3 监听/注入）

2. **`src/bypass_fsm.c`**
   - 修改 `g_pin_pb3` 从 `"PG1"` 改为 `"PE2"`
   - 修改 `InjectEngine_Set("PF1", ...)` 为 `InjectEngine_Set("PE1", ...)`
   - 修改 `InjectEngine_Release("PF1")` 为 `InjectEngine_Release("PE1")`

3. **`src/main.c`**
   - 在 `Exti_Init()` 中添加 PE1 和 PE2 的 EXTI 初始化
   - 添加 `EXTI1_IRQHandler` 和 `EXTI2_IRQHandler` 相关逻辑
   - 在 `HAL_GPIO_EXTI_Callback` 中添加 PE1 和 PE2 的处理

4. **`Document/APM32F103CB_to_STM32F407V_Wiring.md`**
   - 更新接线表，将 PB11 映射到 PE1
   - 更新接线表，将 PB3 映射到 PE2
   - 移除"不可用"的警告说明

---

### 任务 2：修复晶振配置（25MHz → 8MHz）

**PLL 配置计算**：
- HSE = 8MHz
- PLLM = 8（HSE 分频，VCO 输入 = 1MHz，推荐范围 1-2MHz）
- PLLN = 336（VCO 输出 = 336MHz）
- PLLP = 2（SYSCLK = 168MHz）
- PLLQ = 7（USB 48MHz）

**需修改的文件**：

1. **`platformio.ini`**
   - 修改 `HSE_VALUE=25000000` 为 `HSE_VALUE=8000000`

2. **`include/stm32f4xx_hal_conf.h`**
   - 修改 `HSE_VALUE 25000000U` 为 `HSE_VALUE 8000000U`

3. **`src/main.c`**
   - 修改 `RCC_OscInitStruct.PLL.PLLM = 25` 为 `RCC_OscInitStruct.PLL.PLLM = 8`

---

## 实施步骤

### 步骤 1：修改晶振配置
1. 编辑 `platformio.ini`，将 HSE_VALUE 改为 8000000
2. 编辑 `include/stm32f4xx_hal_conf.h`，将 HSE_VALUE 改为 8000000U
3. 编辑 `src/main.c`，将 PLLM 改为 8

### 步骤 2：添加新引脚定义
1. 编辑 `src/probe_pins.c`，在 `g_probe_pins[]` 数组中添加 PE1 和 PE2

### 步骤 3：更新 bypass_fsm.c
1. 修改 `g_pin_pb3` 查找名称为 `"PE2"`
2. 修改所有 `"PF1"` 字符串为 `"PE1"`

### 步骤 4：更新 main.c EXTI 配置
1. 在 `Exti_Init()` 中添加 PE1 和 PE2 的中断初始化
2. 添加 EXTI1 和 EXTI2 的中断处理

### 步骤 5：更新接线文档
1. 编辑 `Document/APM32F103CB_to_STM32F407V_Wiring.md`
2. 更新引脚映射表

---

## 验证方法

1. 编译固件确认无错误
2. 烧录后检查 LED 闪烁频率是否正常（确认时钟配置正确）
3. 通过 USB CDC 发送 `LIST_PINS` 确认 PE1 和 PE2 已加入监听表
4. 通过 USB CDC 发送 `BYPASS_FSM STATUS` 确认功能可用
