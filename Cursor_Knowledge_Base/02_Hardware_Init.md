# 智云 F100 逆向工程：硬件初始化与外设证据（纠错版）

## 背景与前提

- 芯片：`APM32F103`（`Cortex-M3`，Thumb little-endian）
- 固件镜像：`dump/ZHIYUN-F100-full.bin`（Ghidra：`ARM:LE:32:Cortex`）
- 系统参数与启动链：`Cursor_Knowledge_Base/reverse_log.md`
- **纠错说明**：先前将 `SPI1` 与 **PL5500 数字控制** 绑定不符合硬件事实；本文档已删除该结论，仅保留 **寄存器级可复核证据**。

## 内存映射（摘要）

- Flash：`0x08000000` 起；SRAM：`0x20000000` 起（估算 `0x20000000 - 0x20004FFF`）
- 常用外设（Ghidra Segment）：`GPIOA 0x40010800`、`GPIOB 0x40010C00`、`RCM 0x40021000`、`TMR1 0x40012C00`、`SPI1 0x40013000`、`ADC1 0x40012400`、`DMA1 0x40020000`

---

## `main` 中与硬件相关的初始化调用（摘录）

- `Init_GPIO_InputMatrix` @ `0x08003A14`
- `Init_GPIO_StatusAndControlPins` @ `0x08003986`
- `Init_PWM_ControlContext` @ `0x080040BC`（经 `FUN_0800410E` 调用）
- `SPI1_StartupSequence` @ `0x08004184`
- `Init_SPI1` @ `0x080055A4`
- **`InitializeStartupFlagsAndPrep`** @ `0x08003CD2`：内部顺序为 **`Init_GPIO_PortBCD_StartupExtended`**（`0x080038C4`，含 **`GPIOC_Pin2Drive_BSRR_or_BRR`**）→ **`GPIOB_Pin9Drive_BSRR_or_BRR`**（`0x0800389C`）→ 置启动标志
- **`InitializeClock_PLLFlashSysTick`** @ `0x08002F9C`：早期 PLL 链内含 **`FMC_ConfigureWaitStatesAndSysClkCache`**（`0x08004618`）、**`RCM_ComputeSysClkHz`**、**`TickVar_Get`**（FMC **STS** 忙等）

---

## RCM 时钟门控

通过 **`RCM_SetOrClearAPB2ENR_Bits`** / **`RCM_SetOrClearAHBENR_Bits`** / **`RCM_SetOrClearAPB1ENR_Bits`**（`0x08004F96` / `0x08004F80` / `0x08004FAC`）对 `RCM` 寄存器 `OR`/`BIC`：

- **`RCM_SetOrClearAPB2ENR_Bits`**：`APB2ENR`（指针 `0x08004FC8 -> 0x40021018`）
- **`RCM_SetOrClearAHBENR_Bits`**：`AHBENR`（`0x08004FC4 -> 0x40021014`）
- **`RCM_SetOrClearAPB1ENR_Bits`**：`APB1ENR`（`0x08004FCC -> 0x4002101C`）

---

## GPIO：`GPIO_WriteCRLCRH_FromPinMaskDescriptor`（`0x08004FD0`）封装

对 `GPIOx` 写 `CRL/CRH`，并在特定模式编码下写 `BSRR`（`+0x10`）或 `BRR`（`+0x14`）。

### `Init_GPIO_InputMatrix` @ `0x08003A14`

- 使能：`IOPA/IOPB/IOPC/IOPD`
- 表驱动配置 `PA0-PA3`、`PB0-PB3`、`PC0-PC3`、`PD0-PD3` 为上拉输入（模式字含 `0x48`）

### `Init_GPIO_StatusAndControlPins` @ `0x08003986`

- `PA2`、`PA10`：推挽输出；`PD5`：推挽输出
- 初始电平：`PA10` 置位、`PA2` 复位、`PD5` 复位（具体位见反汇编）

### PB14（EN）/ PB15（ADIM）与 MP3398 使能（`04` §3.8.2，2026-04-04）

- **`MP3398_EN_SetHigh_BSRR_PB14`** @ `0x0800A11A`：`**GPIOB_BSRR**`（池字 **`DAT_0800A1EC`→`0x40010C10`**）写 **`0x4000`** → **PB14 置高**（维修手册 **EN** 网络）。指令为 **`mov.w r0,#0x4000`**，故全镜像 **`0x00004000` 四字扫描 0 命中** 与 **本路径不矛盾**。
- **调用链（高）**：**`thunk_MP3398_EN_SetHigh_BSRR_PB14`** ← 仅 **`PD_OnAttachDone_ResetEncoderAndUI`**（PD 附着完成后拉 EN）。
- **上电初始化拉低**：**`MP3398_EN_ADIM_InitLow_GPIOB_BRR`** @ `0x0800A0DA`：在 **`FUN_0800A340`** 配完 **`0xC000` 掩码** 后，对 **`GPIOB` 基址 `+0x14`（BRR）** 先写 **`0x4000`**（清 PB14）再 **`0x8000`**（清 PB15），与 §6 **EN / ADIM** 一致；**`get_function_callers`**：**`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**（大初始化序列内含 **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）等 TMR1 链）。

---

## SPI1：物理层与片选（已证实部分）

### 外设与引脚（`Init_SPI1` @ `0x080055A4`）

- 使能：`IOPA` + `SPI1`
- `GPIOA`：`PA4/PA5/PA6/PA7` 配置为 **复用推挽**（典型 `NSS/SCK/MISO/MOSI` 布局，以硬件原理图为准）
- `SPI1` 基址：`0x40013000`；`SPI1_CTRL1` 经 `FUN_08005BEC` 等配置后 **`SPE` 使能**

### 片选与辅助选择线

| 符号名（Ghidra） | 地址 | 行为摘要 |
|---|---|---|
| `GPIO_SPI_SelectLine_0x40` | `0x080039EE` | `param==0`：写 `GPIOB BSRR` `0x40` → **PB6 置高**；`param!=0`：写 `GPIOA BSRR` `0x40` → **PA6 置高** |
| `GPIO_SPI_CS_PB10_AssertLow` | `0x08003A00` | 写 `GPIOB BRR` `0x400` → **PB10 拉低**（常为 **CS 有效**，低有效） |

发送包装：

- `SPI1_SendConstBlockWithCS` @ `0x080041E6`：`GPIO_SPI_SelectLine_0x40(0)` → `GPIO_SPI_CS_PB10_AssertLow()` → `SPI1_TransferBytes` → 结束后 `*DAT_08003B50` 写 `GPIOB BSRR` `0x400` → **PB10 置高**（释放 CS）
- `SPI1_SendBlockWithCS_GPIOAlt` @ `0x08004208`：首步为 `GPIO_SPI_SelectLine_0x40(1)`（**PA6** 线），其余与上类似

`DAT_08003B50` 指向 `0x40010C10`（`GPIOB BSRR`）。

### `SPI1_TransferBytes` @ `0x08005664`

- 轮询 `SPI1_STS`（`DAT_080056E8 -> 0x40013008`）忙等待后搬运字节

### 对 `SPI1_TransferBytes` 的直接交叉引用（仅 2 处）

- `SPI1_SendConstBlockWithCS` @ `0x080041FC`
- `SPI1_SendBlockWithCS_GPIOAlt` @ `0x0800421E`

（所有 SPI 发送均经上述两包装；运行期大量事务由二者间接调用。）

### `SPI1_StartupSequence` @ `0x08004184`

1. `Init_SPI1()`
2. `Init_GPIO_StatusAndControlPins()`
3. `FUN_080041C8()`（含 `FUN_080056EC` 延时循环）
4. `SPI1_SendConstBlockWithCS(0x080041B0, 0x17)`：从 Flash 常量区发送 **23 字节**
5. `FUN_08004290(0)`；置位 `*DAT_080042e8 = 1`；调用 **`SPI1_PumpEightFramebufferSlices()`** 做首轮刷新

### 运行期 SPI 事务分型（`main` 与 **`SPI1_PumpEightFramebufferSlices`**，Ghidra + 指令扫描）

| 模板 | 长度 / 片选线 | 调用关系 | 证据 |
|------|----------------|----------|------|
| 启动常量块 | 23 B，`PB10`+`PB6` 选择线 | `SPI1_StartupSequence` → `SPI1_SendConstBlockWithCS(0x080041B0,0x17)` | `decompile_function` @ `0x08004184` |
| 通道前缀 | 3 B，`PB10` 线 | **`SPI1_Send3ByteSlicePrefixForChannel`** 在栈上组 3 字节 → `SPI1_SendConstBlockWithCS(sp,3)` | `disassemble_function` @ `0x0800422a`（`bl 0x080041e6`） |
| 块载荷 | 0x60（96）B，`PA6` 线 | **`SPI1_PumpEightFramebufferSlices`**：`for (i=0;i<8;i++) { SPI1_Send3ByteSlicePrefixForChannel(0,i); SPI1_SendBlockWithCS_GPIOAlt(DAT_080042ec+i*0x60, 0x60); }` | `decompile_function` @ `0x08004258` |
| 模板填充 | 逻辑缓冲 | `FUN_0800429e` 自 Flash 表（如 `0x08003030`）按索引写入 `DAT_080042ec` 各区（每区 `0x60` B 布局） | `decompile_function` @ `0x0800429e` |

- **结论**：运行期以 **8 路 ×（3 B 前缀 + 96 B 载荷）** 为主事务形态；**仅启动**使用 23 B 常量帧。  
- **可信度**：高（反编译与 `BL` 扫描一致）；器件语义仍 **待证实**（需 LA）。

### 主板 OLED 连接器连通性（硬件交叉证据，非寄存器结论）

万用表实测 **主板 16pin 座子 → MCU** 与上表引脚分配一致时，可降低「屏线插反/座子虚焊却误判为固件 SPI 初始化错误」的排查成本。详见 **[`Document/Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6.1**：pin8→`PB10`，pin9→`PB2`，pin10–12→`PA6/PA5/PA7`；pin7→`PB0`（与地网）；pin16 接地。**`PB2` 在屏侧的具体电气功能名**仍以原理图或固件 GPIO 写读为准，见 [`REVERSE_TASK_BOOK_FINDINGS.md`](REVERSE_TASK_BOOK_FINDINGS.md) 同日「主板 OLED 座子」条。

### 启动 burst 字节（**非 JEDEC Flash 读 ID**）

- 常量区：`0x080041B0`，长度 `0x17`（23）
- 原始十六进制（前 23 字节）：  
  `AE 00 10 40 81 FF A1 A8 3F AD 8B C8 D3 00 DA 12 D9 22 DB 2B A4 A6 AF 00`
- **首字节 `0xAE`**，未见 `0x9F`（JEDEC `READ ID`）等典型 SPI Flash 命令头；更像 **任意配置/数据流**。
- **待证实**：目标器件类别（需结合原理图与示波器/`SPI` 抓包）。

---

## ADC1：模拟通道与采样（电源/状态感知路径）

### GPIO 模拟输入：`Init_ADC_GPIO_AnalogChannels` @ `0x0800CAA8`

- 通过 `FUN_0800A340` 配置 `GPIOA`/`GPIOB` 上与 ADC 相关的引脚为模拟输入（`DAT_0800CBC4 -> 0x40010800` **GPIOA**，`DAT_0800CBC8 -> 0x40010C00` **GPIOB**）。
- **掩码与引脚（与 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) `GPIO` 位号一致）**：

| 调用次序 | 端口指针 | 掩码（`ushort`） | 典型引脚 | 与 ADC 通道索引（见下） |
|----------|----------|------------------|----------|-------------------------|
| 1 | GPIOA | `0x0010` | **PA4** | **IN4** |
| 2 | GPIOB | `0x0001` | **PB0** | **IN8** |
| 3 | GPIOB | `0x0002` | **PB1** | **IN9** |
| 4 | GPIOA | `0x0008` | **PA3** | **IN3** |

- **证据路径**：Ghidra `decompile_function` @ `0x0800CAA8`；`read_memory` @ `0x0800CBC4`  
- **可信度**：**高**（掩码 + 手册 **INx ↔ 引脚**）；模拟前端分压比仍 **中**（待原理图）。

### 通道配置 helper：`ADC1_SetChannelSampleTimeAndRegularRank1` @ `0x0800D50A`（原 `FUN_0800d50a`）

- **`ADC1_AverageSamples`** 在每次采样前调用 **`ADC1_SetChannelSampleTimeAndRegularRank1(ADC1, channel, rank=1, sample_time=7)`**。
- 反编译写入 **`param_1 + 0x10` / `+0x0C` / `+0x34`**：与 **`ADC_T`** 中 **`SMPTIM2` / `SMPTIM1` / `REGSEQ3`** 偏移一致（[`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h)：`SMPTIM2=@+0x10`，**`REGSEQ3=@+0x34`**）。**`sample_time=7`** 对应 **239.5 周期**量级（与 STM32/APM32 常用枚举一致）。
- **rank=1**：仅在 **`REGSEQ3.REGSEQC1`** 填入 **单通道** 序列，配合 **`ADC1_AverageSamples`** 的循环实现 **多次平均**。
- **证据路径**：Ghidra `decompile_function` @ `0x0800D50A`、`0x0800CB7A`；`rename_function_by_address`；**`PRE_COMMENT`** @ `0x0800D50A` / `0x0800CB7A`  
- **可信度**：**高**（MMIO 偏移与寄存器名闭合）。

### 采样封装：`ADC1_AverageSamples` @ `0x0800CB7A`

- 基址：`DAT_0800CBCC -> 0x40012400`（`ADC1`）
- 调用 **`ADC1_SetChannelSampleTimeAndRegularRank1`** 设置通道、**`ADC1_CR2_SetSwstartAdonMask`**（`0x0800D4F8`）置 **`ADC1->CTRL2`** 启动相关位、**`ADC1_RegBitsTestNonZero`**（`0x0800D5A4`，掩码 **`2`**）轮询 **EOC**、**`ADC1_DR_ReadU16`**（`0x0800D588`）读 **`ADC1->REGDATA`**（**`+0x4C`**，与 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) **`ADC_T.REGDATA`** **一致**）
- **Ghidra（2026-04-05）**：上列三助手已由 MCP **`rename_function_by_address`** 落盘；**`ADC1_AverageSamples`** **`PRE_COMMENT`** 摘要采样环；书签 **`ADC` @ `0x0800CB7A`**（**`ADC1_AverageSamples_inner_helpers`**）；**`save_program`**
- **调用方与通道实参（静态闭合）**：

| 函数 | 实参 `(channel, times)` | **手册 INx → 引脚**（APM32F103 LQFP48） |
|------|-------------------------|----------------------------------------|
| `PD_GPIO_StateMachine` | `(3, 4)` | **IN3 → PA3**（与维修笔记 **PA3 / DC 分压** 链相容） |
| `LightApply_ClassifyADC_ToControlByte` | `(4, 6)` | **IN4 → PA4**（维修笔记 **NTC**） |
| **`Input5Channel_ADC_IN8_BandClassify`**（原 `FUN_0800D68E`） | `(8, 4)` | **IN8 → PB0**（维修笔记 **PB0 / 47K 分压** 链相容）；**唯一 caller**：**`Input_5Channel_TableStateMachine`** |
| `BatteryGauge_ADC9_UpdateFromSysTick600`（`0x0800C2BA`） | `(9, 4)` | **IN9 → PB1**（19 脚）；板级 **VBAT_ADC** 测试点（[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6 第9条）；**唯一 caller**：**`PD_PeriodicDispatchFromSysTick`**（**>599 tick** 分支）；**`UI_Framebuffer_CopyRect`** 刷新 **电量条/UI**（见 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.5.1、§3.8.1）。**同函数**另调用 **`GPIOB_IDR_ReadBit12_PB12`**（`0x08009F7C`）与 **`GPIOB_IDR_ReadBit13_PB13`**（`0x08009F78`）：读 **`GPIOB->IDR`（`0x40010C08`）** 的 **bit12/bit13**，与维修笔记 **PB12/PB13 接 10k 网络**（§6 第11 条）**相容**；**仅** 被本函数 **静态引用** |

- **穷尽性**：Ghidra MCP **`get_function_callers` @ `ADC1_AverageSamples`（`0x0800CB7A`，limit=100）** → **恰好 4** 处，与上表一致（2026-04-04）。
- **证据路径**：Ghidra `decompile_function` @ 上表各 caller；`Init_ADC_GPIO_AnalogChannels` 掩码表（本节上表）；详见 [`03_Function_Modules.md`](03_Function_Modules.md) **§0.3.6**。  
- **可信度**：**高**（通道实参 + **INx→引脚** 数据手册 + GPIO 模拟初始化表 **三方一致**）。

**GPIOB `IDR` 字面量唯一性（PB12/PB13，2026-04-10）**：对 **`ZHIYUN-F100-full.bin`** 做 **32 位小端**扫描，**`0x40010C08`（`GPIOB->IDR`）** 字面量 **命中 1 次**（池 **`0x0800A1E0`**，与 **`GPIOB_IDR_ReadBit12_PB12`** / **`GPIOB_IDR_ReadBit13_PB13`** / 输入矩阵池 **共用** 一带）。**全镜像** 无 **单独** `0x00001000`/`0x00002000` **BSRR 掩码四字** 命中 — **PB12/PB13** 在固件侧以 **IDR 位读** 为主证据，**非** 输出推挽线（与 **10k 偏置网络** 输入读法 **相容**）。**Ghidra**：**`rename_function_by_address`** + **`BatteryGauge_*` `PLATE`**，`save_program`。

### 示例：灯光档位 ADC `LightApply_ClassifyADC_ToControlByte` @ `0x0800CFA8`

- **`ADC1_AverageSamples(4, 6)`**：**通道索引 4**、**6 次平均**（`ADC1_AverageSamples` 第二参为 **次数**）；**静态闭合**：**IN4 → PA4**，且 **`Init_ADC_GPIO_AnalogChannels`** 对 **PA4（掩码 `0x10`）** 置 **模拟输入**（与 [`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6 **NTC** 链 **高一致**）。**示波器** 仅用于 **模拟前端电压曲线**，非 **引脚名** 待证。
- Flash 阈值表 **`g_ADC_LightClassThresholdTable_u16`**（**`0x0800D1DA`**）+ 饱和返回 **`-1`**，与 [`03_Function_Modules.md`](03_Function_Modules.md) **§0.3.5** 一致

### 示例：分档/稳定判定 `Input5Channel_ADC_IN8_BandClassify` @ `0x0800D68E`

- `ADC1_AverageSamples(8, 4)`：对 **通道索引 8** 做 4 次平均；**静态闭合 IN8 → PB0**，且 **Init_ADC** 对 **PB0（掩码 `0x1`）** 置模拟输入（与维修笔记 **PB0** 链 **高一致**）。
- 与表 `DAT_0800D890` 比较，差值阈值 **`0x65`（101）** 量级用于稳定判定
- **与 `Input_5Channel_TableStateMachine`（`0x0800D74E`）关系**：返回值低部与 **`Input_SamplePA1PB4_As2bit()`**（**PA1+PB4**）**按位或** 后，作为 **5 路输入表** 的 **合成边沿源**（见 [`03_Function_Modules.md`](03_Function_Modules.md) §2.1）

### 示例：状态机与阈值 `PD_GPIO_StateMachine`（`FUN_0800C4E0` @ `0x0800C4E0`）

- **ADC 访问形态**：**无** `ADC1` 基址字面量/MMIO 直读，**仅**封装 **`ADC1_AverageSamples(3, 4)`**；**静态闭合 IN3 → PA3**，与 **Init_ADC** 对 **PA3（掩码 `0x8`）** 的模拟配置一致；与常量 **`0xD17`（3351）** 比较后写 `ctx+8` 并可能 **`PD_SetDoneFlagHalfword`**（见 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.2）。
- 与 **`ADC1_AverageSamples(8, 4)`**（**`Input5Channel_ADC_IN8_BandClassify`** 路径）等为 **并列** 业务分支，非同一调用点。

> **已证实**：固件存在 **多通道 ADC1 采样 + 阈值/状态机**，用于电压或负载相关判定；**通道索引 → MCU 引脚** 已由 **`ADC1_SetChannelSampleTimeAndRegularRank1` MMIO** + **`Init_ADC_GPIO_AnalogChannels` 掩码** + 数据手册 **INx** 表 **静态闭合**（见上表）。  
> **待证实**：各路 **模拟前端** 的分压比、运放、与 **PD 协议芯片** 的电气关系（需原理图/仪器）。

---

## I2C / HUSB238：固件内证据

- 对镜像做 **32 位小端字面量扫描**（全文件搜索字节序列 `00 54 00 40`、`00 58 00 40`）：**`0x40005400`（`I2C1`）、`0x40005800`（`I2C2`）出现次数均为 0**。
- **结论（当前证据）**：未见通过 **`I2C1/I2C2` 外设寄存器** 实现的通信初始化；**不能**据此排除 **bit-bang GPIO I2C** 或 **其他 MCU** 负责 `HUSB238`，但 **MCU 内置 I2C 控制器路径未建立**。
- **可信度**：高（字面量缺失）；bit-bang 仍 **中**（需波形/引脚模式）。

---

## Timer / PWM / DMA / Flash：`Init_PWM_ControlContext` @ `0x080040BC`

### `DAT_08004158` 控制块（SRAM）

- 字面量池 `0x08004158` → **`0x20004860`**
- `Init_PWM_ControlContext` 顺序写入 `puVar1[0..9]`（含 `0x400`、`0x40`、`0x1d`、`0x8000`、`0x20` 等），与 `FUN_08004a90` 根据 `DAT_08004ac4` 分档算出的周期量相关；**反编译中 `unaff_r7` 未闭合**，完整乘积关系 **待补**。
- **证据路径**：Ghidra `decompile_function` @ `0x080040BC`、`0x08004a90`；`read_memory` @ `0x08004158`  
- **可信度**：高（指针与表项）；中（与亮度占空比的闭合）

### `FUN_08004a6a` 链：Flash 接口（易与「PWM」混淆）

- `FUN_0800584c`：向 **`0x40022004`** 连续写入典型 **Flash 解锁键**（与 STM32/APM32 `FMC_KEYR` 用法一致）
- `FUN_080058d8(0x34)`：写 **`0x4002200C`**
- `FUN_0800585e`：读改写 **`0x40022010`**，**置位 `0x80`**
- **结论**：该链为 **FMC/Flash 控制**，**不是** TMR 寄存器；与 `Init_PWM_ControlContext` 名称并读时需区分。
- **可信度**：高（绝对地址）

### `FUN_08004092` → DMA 配置

- 满足条件时调用 `FUN_08005390` / `FUN_080053ae` → `FUN_080021ba` → `FUN_08001b2e` → `FUN_08002628`：在 **`param_1` 为 DMA 外设上下文** 的模型下写通道长度与模式位（`FUN_08002628` 内对 `param_1+0x404` / `+0x40c` 等写入与 STM32 **DMA 通道**寄存器布局一致）。
- **证据路径**：`decompile_function` @ `0x08004092`、`0x08002628`  
- **可信度**：高（DMA 形态）；中（与 `DAT_08004158` 时间参数在业务上的耦合需继续 xref）

### `TMR1` / `TMR4` 显式初始化（寄存器基址）

| 函数 | 指针池（示例） | 外设基址 | 说明 |
|------|------------------|----------|------|
| **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`） | `DAT_0800ded4` → `0x40012C00` | **TMR1** | 高级定时器参数、`FUN_0800aaf6` 等写 **CC/时基**；由 **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**（`0x0800ECEA`，`BL` @ `0x0800ed22`）在 **IRQ/系统初始化序列**中调用 |
| `FUN_0800de2e` | `DAT_0800dee0` → `0x40000800` | **TMR4** | `FUN_0800aaf6((int)pTVar1,3,2,2)` 等；由 **`TMR4_EncoderReset_ClearStateAndReinitQuadrature`**（`0x0800D38C`）调用（先清零 `DAT_0800d428` 上下文再调 **`FUN_0800a074`**） |
| **`Init_TMR3_PWM_GPIOB5`** | `DAT_0800dec8` → `0x40000400` | **TMR3** | `FUN_0800a340(DAT_0800decc, …)` 掩码 **`local_28 = 0x20`** → **GPIOB bit5 = PB5** 复用；`FUN_0800a6ac`/`FUN_0800a7ba`/`FUN_0800ab46` 等写 **CC/时基**；由 **`Init_GPIO_Then_TMR3_PB5_PWM`** @ `0x0800d006` 调用，**上游** **`Init_TMR3_PB5_AndTickSyncLoop`** → **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**（初始化序列） |

- **证据路径**：`decompile_function` @ `0x0800dc92`、`0x0800d006`、`0x0800cc7e`；`read_memory` @ `0x0800deb0`（字面量池含 **`0x40000400`**、**`0x40010C00`**）；Python 全镜像 **`0x40000400`** 字面量 **3 命中**；程序 `ZHIYUN-F100-full.bin`  
- **可信度**：高（与 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) TMR 基址一致；**PB5 ↔ 维修手册 MO_PWM** 见 [`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §5–§6）

#### `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`（`0x0800DD28`）引脚掩码（`FUN_0800a340` 形参，OI-004）

- 第一次 `FUN_0800a340(DAT_0800deac, …)`：`**0x300**` → **GPIOA** 上 **bit8、bit9** 置位 → 典型 **PA8、PA9** 复用推挽（与 **TMR1 CH1/CH2** 默认映像相容）。
- 第二次 `FUN_0800a340(…)`：`**0x8000**` → **bit15** → 典型 **PB15** 复用（依封装与重映射，可为 **TMR1** 互补通道或 remap 通道，需数据手册 **AF 表** 最终定名）。
- **可信度**：**高**（掩码字面量）；**引脚 ↔ 灯驱网络** 仍 **中**（需原理图对照维修笔记 §5 测试点名）。

#### `FUN_08004144` / `FUN_0800370c` 与 PWM 块（USB 交互）

- **`FUN_08004144`**：`iVar1 = *DAT_08004158 * (param_1 + DAT_08004158[5])`，向 **`iVar1 + 0x8000000`** 写 **半字序列**（`FUN_080058a6`），供 **USB 命令路径**（`FUN_0800370c` **case 2/7**）更新 **比较值/缓冲**。
- **可信度**：高（反编译）；与 **亮度闭环** 的业务语义 **中**（OI-004 仍待示波器）。

---

### 旋转编码器采样（`FUN_0800a16c` / `Input_SamplePA1PB4_As2bit` / **TMR4**）

#### 旋钮角增量（PB6 / PB7，**OI-006 已闭合**）

- **`Init_TMR4_EncoderQuadrature_PB6PB7`**（`0x0800DE2E`）：使能 **APB1 TMR4**，调用 **`TMR_ConfigureEncoderMode_SMCR_CCMR`**（`0x0800AAF6`，`**SMS=3`** 编码器模式）与 **`FUN_0800A89C`**（CC 输入映射），**`TMR4->CNT`** 清零；APM32F103 默认 **TMR4 TI1/TI2 ↔ PB6/PB7**。
- **`Encoder_DispatchDelta_FromTMR4`**（`0x0800D3D2`）：读 **`TMR4->CNT`（+0x24）**，与前次比较得 **`Δ`**，**`|Δ|≤0x65`** 视为死区；否则夹紧到 **`±0x7F`** 并通过 **函数指针** 调 UI。**唯一调用者**：**`PD_PowerAndProtocol_Block`**。
- **证据路径**：`decompile_function` @ `0x0800DE2E`、`0x0800D3D2`；`get_xrefs_to` **`0x40000824`**（`TMR4_CNT`）；Ghidra 书签 **`Encoder`** @ `0x0800DE2E` / `0x0800D3D2`。
- **可信度**：**高**（寄存器偏移与 STM32/APM32 通用定时器布局一致）。

#### 按键 / 其它 2 bit（PA1 / PB4，与旋钮 **并行**）

- **`Input_SamplePA1PB4_As2bit`**（`0x0800D670`）：返回 **`FUN_0800a16c(0)` | (`FUN_0800a16c(1)<<1)`** → **2 bit** 聚合（由 **`Input_5Channel_TableStateMachine`** 调用）。
- **`FUN_0800a16c`** 字面量池（`0x0800A1FC` 起）：**通道 0** → **`GPIOA` IDR + 掩码 `0x0002`**（**PA1**）；**通道 1** → **`GPIOB` IDR + 掩码 `0x0010`**（**PB4**）；可选极性由 `DAT_0800a21c[]` 异或。
- **用户万用表 + [`§6`](../Document/Zhiyun_F100_Repair_Notes.md)**：**右键 → PB4** 与 **`FUN_0800a16c(1)`** 一致。**PA1** **不是** 编码器 A（A 在 **PB6**）。
- **`Input_5Channel_TableStateMachine`**：将 **`Input_SamplePA1PB4_As2bit`** 与 **`Input5Channel_ADC_IN8_BandClassify`**（**ADC IN8** 三档比较）合成状态，驱动 **5 路** 表驱动状态机 — **不含** TMR4 计数，与 **旋钮** 为 **独立输入路径**。
- **可信度**：**高**。

#### 与 SPI 启动序列的引脚关系（**低–中**）

- **`GPIO_SPI_SelectLine_0x40`** 在 **`SPI1_SendConstBlockWithCS`** 中亦操纵 **PB6（`0x40`）** 作为 **片选辅助**；**TMR4 编码器** 复用 **PB6** 依赖 **先 SPI/GPIO 初始化、后 TMR4 AF** 的次序。建议 LA 确认上电阶段无冲突。

---

### `Init_GPIO_InputMatrix` 表项摘要（与编码器路径独立）

- 四轮 **`GPIO_WriteCRLCRH_FromPinMaskDescriptor`** 使用的端口指针依次为 **GPIOA、GPIOA、GPIOB、GPIOD**（`read_memory` @ `0x08003B6C` 起，步长 12 字节一组），掩码字包含 **PA1/PA2、PA3/PA4、PB13/PB3、PDx** 等组合；末次调用另配 **上拉输入** 模式字 **`0x48`**。
- **说明**：该表 **未**出现 **`0x00C0`（PB6+PB7）** 掩码；编码器 **走 TMR4/PB6/P7** 路径，**非** 本表 **`Input_SamplePA1PB4_As2bit`** 的 2 bit 键位。
- **可信度**：高（字面量池）。

---

### EXTI / `AFIO` 字面量

- 全镜像 **32 位小端** **`0x40010400`（EXTI）** 与 **`0x40010000`（AFIO）** 命中 **0**（本地 Python 扫描 `dump/ZHIYUN-F100-full.bin`）。
- **推论**：按键/编码器更可能 **轮询 IDR** 或 **TMR 编码器模式**，**非** EXTI 线典型用法（仍可能 **`movw/movt` 拆分**，故标 **中**）。

---

## Ghidra 命名（纠错后，地址 → 名称）

| 地址 | 名称 |
|---|---|
| `0x08002E74` | `main` |
| `0x08003986` | `Init_GPIO_StatusAndControlPins` |
| `0x08003A14` | `Init_GPIO_InputMatrix` |
| `0x080039EE` | `GPIO_SPI_SelectLine_0x40` |
| `0x08003A00` | `GPIO_SPI_CS_PB10_AssertLow` |
| `0x080041E6` | `SPI1_SendConstBlockWithCS` |
| `0x08004208` | `SPI1_SendBlockWithCS_GPIOAlt` |
| `0x08005664` | `SPI1_TransferBytes` |
| `0x080055A4` | `Init_SPI1` |
| `0x08004184` | `SPI1_StartupSequence` |
| `0x080040BC` | `Init_PWM_ControlContext` |
| `0x0800CAA8` | `Init_ADC_GPIO_AnalogChannels` |
| `0x0800CB7A` | `ADC1_AverageSamples` |
| `0x0800AD5C` | `PD_SetDoneFlagHalfword` |
| `0x0800DE2E` | `Init_TMR4_EncoderQuadrature_PB6PB7` |
| `0x0800dc92` | `Init_TMR3_PWM_GPIOB5` |
| `0x0800d006` | `Init_GPIO_Then_TMR3_PB5_PWM` |
| `0x0800AAF6` | `TMR_ConfigureEncoderMode_SMCR_CCMR` |
| `0x0800D3D2` | `Encoder_DispatchDelta_FromTMR4` |
| `0x0800C156` | `PD_AttachSequence_StateMachine` |
| `0x0800c684` | `ProtocolBytes_SetSlot0to2` |
| `0x0800c794` | `PD_GPIO_ReadPc13ToggleApplyPb3` |
| `0x0800c974` | `PD_OnAttachDone_ResetEncoderAndUI` |

---

## 证据边界汇总

| 项目 | 状态 |
|---|---|
| `SPI1` 物理初始化 + `PB10` 类 CS 时序 | **已证实**（`BSRR/BRR` + `SPI1` 寄存器） |
| 启动 23 字节 burst 的器件语义 | **待证实**（非典型 JEDEC） |
| `ADC1` 多通道采样与阈值状态机 | **已证实** |
| `TMR1`/`TMR3`/`TMR4` 基址字面量、`DMA` 通道形态、`DAT_08004158` 控制块 | **已证实**（寄存器级）；**TMR3→PB5（MO_PWM）** 见上表；与亮度/风扇映射 **部分闭合** |
| `HUSB238` / `I2C` 数字接口经 `I2C1/2` | **未证实**（无基址字面量；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.7 全镜像 **32 位**小端扫描 **I2C1/2 = 0 命中**） |
| **PB6/PB7** 旋钮 **`TMR4` 编码器模式** + **`Encoder_DispatchDelta_FromTMR4`** | **已证实**（见上 §旋转编码器） |
| `USART1`–`USART3` 基址 **`0x40013800`/`0x40004400`/`0x40004800`** 连续字面量 | **0 命中**（≠ 证明未用 UART；可能拆分立即数） | 见 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.7 |

---

## 下一步建议

- 对照官方原理图确认 `PA4-PA7`、`PB10`、`PB6`、`PA6` 的 **网络名**。
- 对 `0x080041B0` 发送序列做 **逻辑分析仪** 对比（与 §2.5 CS 时序同测）。
- 用 LA 或示波器确认 **SPI 启动阶段** 与 **TMR4 编码器** 对 **PB6** 的时分复用 **无电气冲突**。
