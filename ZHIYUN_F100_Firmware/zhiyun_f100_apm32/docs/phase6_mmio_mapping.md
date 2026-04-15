# Phase 6 MMIO 映射清单（`05-MMIO-v1` 对齐）

## 文档目的

本清单用于将 Phase 6 可编译工程中的符号与 `05-MMIO-v1` 冻结规格进行一一映射。  
范围仅覆盖第一期行为验收所需链路：开关机主循环、CCT/HSI/调光、插 USB 的 PD 诱骗充电路径。  
未闭合项统一标注为“待证实”；在 **`REVERSE_TASK_BOOK.md` §1「外部实测条件约束」** 下，**补证不依赖示波器/LA**，以 **逆向/Renode/GDB** 可执行路径为准。

## 规格锚点

- `Cursor_Knowledge_Base/05_Full_Reconstruction.md`（文首 `05-MMIO-v1`，重点 §5.1/§5.3）
- `Cursor_Knowledge_Base/04_Protocol_Reverse.md`（PD GPIO 与阈值链，重点 §3.1/§3.2/§3.10/§3.11）
- `Cursor_Knowledge_Base/03_Function_Modules.md`（灯光/输入/UI 路径）
- `Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md`（Phase 6 验收口径）

## `05` §2 顺序对照（`PD_PowerAndProtocol_Block`）

[`05_Full_Reconstruction.md`](../../../Cursor_Knowledge_Base/05_Full_Reconstruction.md) §2 列出的 **直接 callee 顺序** 与 [`phase6_light_pd_skeleton.c`](../src/phase6/phase6_light_pd_skeleton.c) 中 `PD_PowerAndProtocol_Block` **一致**（自上而下）：

1. `PD_AttachSequence_StateMachine`
2. `Input_5Channel_TableStateMachine`
3. `PD_GPIO_StateMachine`
4. `PD_HelperSeq_3000tick_ThenPc13Gpio`
5. `ProtocolLanes_BitExpandTick`
6. `Encoder_DispatchDelta_FromTMR4`
7. `EncoderGated_LightOutputApplyChain`（内部再调 `CCT_Slew_TableSplit_TMR1Shadow` / `LightOutput_PendingHSI_Dispatch` 等，见 `03` §0.3）
8. `UI_OLED_MainStateMachine`
9. `UI_ModeRender_PostProcess`
10. `UI_SPI1_PumpEightFramebufferSlices`

**与原厂启动链的差异（文书可接受）**：`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` 在 OEM 中位于更广的 `main` 初始化链（`05` §1）；Phase 6 骨架在 **首次进入** `PD_PowerAndProtocol_Block` 时门控调用一次，用于第一期可编译路径与 `TMR1` 影子对齐，**不**改变上述 10 步顺序，**不**引入 `05` 未列出的额外 MMIO 语义。

## 映射表（代码符号 ↔ MMIO/业务语义）

| 代码符号（Phase 6） | 对齐原厂符号/地址 | MMIO 锚点 | 第一阶段语义 |
|---|---|---|---|
| `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` | `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` (`0x0800DD28`) | `GPIOA_CRL/CRH`、`GPIOB_CRH`、`TMR1` (`0x40012C00`) | 初始化 PA8/PA9/PB15 与 `TMR1` PWM 通道（待证实：与原厂预装载细节是否完全一致） |
| `TMR1_PeriodHalfword_Write` | `TMR1_PeriodHalfword_Write` | `TMR1->AUTORLD` (`+0x2C`) | 写入 CCT 周期影子并同步到 `AUTORLD` |
| `TMR1_CompareHalfword_Write` | `TMR1_CompareHalfword_Write` | `TMR1->CC1/CC3` (`+0x34/+0x3C`) | 写入 warm/cold 比较值影子并同步到 PWM 通道 |
| `CCT_Slew_TableSplit_TMR1Shadow` | `CCT_Slew_TableSplit_TMR1Shadow` | `ADC1` + `TMR1` | 根据输入更新 CCT 占空（待证实：光学映射系数） |
| `Encoder_DispatchDelta_FromTMR4` | `Encoder_DispatchDelta_FromTMR4` | `TMR4->CNT` (`0x40000824`) | 由 `input_adapter` 采样的编码器增量驱动灯光路径 |
| `PD_ReadGPIOC13_IdrBit` | `PD_ReadGPIOC13_IdrBit` (`0x08009F84`) | `GPIOC->IDR` bit13 (`0x40011008`) | 读取 USB/PD 附着提示 |
| `PD_WriteGPIOB3_BSRR_BRR` | `PD_WriteGPIOB3_BSRR_BRR` | `GPIOB->BSRR/BRR` bit3 (`0x40010C10/14`) | PD 状态输出线 VS_EN 门控 |
| `PD_WriteGPIOB11_BSRR_BRR` | `PD_WriteGPIOB11_BSRR_BRR` | `GPIOB->BSRR/BRR` bit11 (`0x40010C10/14`) | PD 状态输出线 DC_IN 门控 |
| `PD_GPIO_StateMachine` | `PD_GPIO_StateMachine` (`0x0800C4E0`) | `PC13/PB3/PB11` + ADC 门控 | USB 插入后推进 PD 输出子状态 |
| `PD_HelperSeq_3000tick_ThenPc13Gpio` | `PD_HelperSeq_3000tick_ThenPc13Gpio` | `SysTick` 入口计数 + `PC13` | 3000 tick 节拍辅助（已从“PD 调用计数”收敛为“`SysTick` 入口计数”；待证实：与原厂 600-tick 辅助窗并行关系） |
| `EncoderGated_LightOutputApplyChain` | `EncoderGated_LightOutputApplyChain` (`0x0800C8C0`) | `TMR1` / HSI 影子 | CCT 与 HSI 分支门控（`03` §0.3.1） |
| `LightOutput_PendingHSI_Dispatch` | `LightOutput_PendingHSI_Dispatch` | 运算 + `RGBTriple_PushPattern_TMR2Gated`（`TMR2 + DMA1 Ch1 + TMR3` 影子） | HSI 路径可达性与 DMA/TMR 协同 |
| `UI_OLED_MainStateMachine` | `UI_OLED_MainStateMachine` (`0x0800D89C`) | SRAM/UI 事件 | 第一期：事件路由消费，非全量 OLED 位图 |
| `UI_ModeRender_PostProcess` | `UI_ModeRender_PostProcess` | — | 渲染后处理计数 |
| `UI_SPI1_PumpEightFramebufferSlices` | `UI_SPI1_PumpEightFramebufferSlices` | SPI1（第一期占位） | 阶段推进至 `PHASE6_STAGE_UI_SPI_FLUSH` |
| `SPI1_StartupSequence` | `SPI1_StartupSequence` (`0x08004184`) | `SPI1->DATA/STS` + `GPIOB->BC/BSC` bit10 | 启动阶段真实发射 23B 常量帧（`AE ... AF`），并拉低/拉高 OLED CS（待证实：SPI 时序细节与 OEM 全位级一致性） |
| `InitializeClock_PLLFlashSysTick_Phase6Anchor` | `InitializeClock_PLLFlashSysTick` (`05` §1) | `RCM->CTRL/CFG`、`FMC->CTRL1`、`SysTick` | 启动阶段顺序对齐与有界寄存器序列 |
| `InitializeGPIOB_ClockAndPinMode_Phase6Anchor` | `InitializeGPIOB_ClockAndPinMode` (`05` §1) | `RCM->APB2CLKEN` (`AFIO/IOPB`) | 启动时 GPIOB 时钟路径显式化 |
| `EnableAHBPeriphClock_DMA1_Phase6Anchor` | `EnableAHBPeriphClock_DMA1` (`05` §1) | `RCM->AHBCLKEN` (`DMA1EN`) | DMA1 时钟锚点对齐 |
| `EnableAHBPeriphClock_SRAM_Phase6Anchor` | `EnableAHBPeriphClock_SRAM` (`05` §1) | `RCM->AHBCLKEN`（`SRAMEN`） | AHB SRAM 时钟门控锚点 |
| `SetUsbDeviceContextPointer_Phase6Anchor` | `SetUsbDeviceContextPointer` (`05` §1) | SRAM 上下文影子（第一期） | USB 上下文准备锚点，不触发升级写 Flash |
| `InitializeUsbControlEndpointDefaults_Phase6Anchor` | `InitializeUsbControlEndpointDefaults` (`05` §1) | EP0 默认参数影子 | USB 初始化顺序锚点 |
| `InitializeUsbDescriptorsAfterPwmInit_Phase6Anchor` | `InitializeUsbDescriptorsAfterPwmInit` (`05` §1) | 描述符初始化影子 | 保持“PWM 后 USB 初始化”阶段语义 |
| `StepBootPhaseCounter8`（Phase6 影子） | `StepBootPhaseCounter8`（`05` §1） | 无新增 MMIO（仅 `main` 周期影子计数） | 对齐原厂 `main` 中的 8 相位启动计数语义（第一期以可观测影子替代） |
| `thunk_IWDG_KR_ReloadAAAA`（Phase6 影子） | `thunk_IWDG_KR_ReloadAAAA`（`05` §1） | 无新增 MMIO（仅看门狗 reload 影子计数） | 对齐原厂主循环喂狗节拍语义，不在本回合引入新的 IWDG 写寄存器副作用 |
| `Phase6_PD_PeriodicDispatchFromSysTick` / `SysTick_Handler` | `PD_PeriodicDispatchFromSysTick`（`FUN_0800CC3C`，`04` §3.5.1） | `SysTick`（`0xE000E010` 起） | 每 **20** 次 SysTick 入口调一次 `PD_PowerAndProtocol_Block`；`g_phase6_systick_entry_count` 为证据计数 |

> 2026-04-06（回合 AA）Ghidra MCP 补证：  
> `main @ 0x08002e74` 反编译显示 `StepBootPhaseCounter8` + `thunk_IWDG_KR_ReloadAAAA` 位于主循环内，且受 `if (99 < tick_delta)` 门控；Phase6 已将该语义收敛为 `PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=100` 的周期影子计数。

## SPI1/OLED 差距矩阵（回合 S）

> 目标：只收敛“运行期帧泵触发时机/节拍”，不新增冻结规格外 MMIO 行为。  
> 判定口径：能用 Renode + 本机 `arm-none-eabi-gdb` 复现并留痕。

| 项目 | 原厂锚点（03/04/05） | 回合 R 前状态 | 回合 S 处理 | 证据位点 | 状态 |
|---|---|---|---|---|---|
| 运行期帧泵触发时机 | `05` §1/§2 中 `UI_SPI1_PumpEightFramebufferSlices` 处于周期主链末端 | 每轮无条件刷新 `8×(3B+96B)`，与“事件驱动+周期保底”节拍未区分 | 改为“脏帧触发 + 16 tick 保底刷新”，维持每次触发仍是全量 8 片 | `g_phase6_spi_flush_trigger_count` / `g_phase6_spi_flush_skip_count` | 已收敛（行为级） |
| UI 渲染与帧泵耦合 | `03` UI 事件路由 + `05` 周期链 | 无事件也持续重算并立即全量推送 | `UI_ModeRender_PostProcess` 增加节拍判定：仅在脏帧/首帧/保底节拍时重建并触发刷新 | `g_phase6_ui_event_dispatch_count`、`ui_framebuffer_checksum` | 已收敛（行为级） |
| SPI 事务完整性 | `04` §2.4、`05` §5.1（SPI1） | 已有真实 `Phase6_SPI1_Transmit`，但缺“触发/跳过”可观测计数 | 保留事务底座不变，只补运行期触发统计 | `g_phase6_spi1_startup_*` + 新增 flush cadence 计数 | 已收敛 |
| **G2：flush 触发/跳过计数（SRAM 跃迁）** | `UI_SPI1_PumpEightFramebufferSlices` 路径 | 缺 Renode 下可复现的计数读回 | `scripts/renode_phase6_pd_breaktrace.sh`：`PHASE6_TRACE_FLUSH_BP=1`，可选 `PHASE6_FORCE_FLUSH_TRIGGER=1` 或 `PHASE6_FORCE_FLUSH_SKIP=1`（与 TRIGGER 互斥） | [`REVERSE_TASK_BOOK_FINDINGS.md`](../../../Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md) **2026-04-06 回合 AD**（`g_phase6_spi_flush_trigger_count`/`g_phase6_spi_flush_skip_count`）；复验 `logs/renode-phase6/20260406_204902/`（本计划执行） | **已闭合（G2 定量）** |
| 刷新节拍与 OEM `SysTick` / `PD_PeriodicDispatchFromSysTick` 调度关系 | `04` §3.5：OEM 为 **SysTick IRQ 内每 20 计数** 调 `PD_PowerAndProtocol_Block` | 曾用 **每 `RunOnce` 一次 PD**（与 OEM IRQ 分频不等价） | 实机：`SysTick_Handler` → `Phase6_PD_PeriodicDispatchFromSysTick`（`/20` 后 `PD_PowerAndProtocol_Block`）；`RunOnce` 仅主循环输入/USB/喂狗/SPI 请求；**trace** 含 `systick_entry_count` / `pd_call_ticks`；主机验收：`PHASE6_HOST_TEST` 下每 `RunOnce` **模拟 20 次** SysTick 入口以保持 **1 PD/迭代** | `g_phase6_systick_entry_count`；[`phase6_pd_systick_dispatch.c`](../src/phase6/phase6_pd_systick_dispatch.c) | **已收敛（结构级）**；**与 OEM 600-tick 辅助窗** 仍 **待证实** |

**复验说明（终局收敛计划）**：`scripts/renode_phase6_pd_breaktrace.sh` 在 `PHASE6_FORCE_FLUSH_TRIGGER=1` 下快照读到 `g_phase6_spi_flush_trigger_count` 非零（`logs/renode-phase6/20260406_204902/gdb_phase6_pd_breaktrace.log`）；与 FINDINGS 中「强制 skip/双分支」回合 AD 交叉引用，**闭合 G2 定量**行；**不**声称与原厂二进制 tick 周期逐周期一致。

**OEM 时基对照（终局收敛 — PD/SysTick 结构对齐）**：

- **Phase6（实机）**：`g_phase6_scheduler_ticks` = 主循环 `RunOnce` 次数；`g_phase6_systick_entry_count` = `SysTick_Handler` 入口次数（与 OEM `PD_PeriodicDispatchFromSysTick` 计数同语义）；`g_phase6_call_ticks` = `PD_PowerAndProtocol_Block` 进入次数。稳态期望：**`g_phase6_systick_entry_count ≈ 20 × g_phase6_call_ticks`**（在 `g_phase6_pd_irq_ready` 已置位且未丢中断的前提下）。**`g_phase6_scheduler_ticks` 与 `g_phase6_call_ticks` 不再要求 1:1**（主循环可远快于 PD 周期）。
- **Phase6（主机验收）**：`PHASE6_HOST_TEST` 下每 `RunOnce` 调用 **20 次** `Phase6_PD_PeriodicDispatchFromSysTick()`，故 **`scheduler_ticks` 与 `call_ticks` 仍保持与旧 harness 一致的 1:1 关系**，不破坏 `PH6-BHV-*` 软件门槛。
- **证据**：`./scripts/renode_phase6_capture.sh` / `renode_phase6_pd_breaktrace.sh` 的 GDB 段输出 **`g_phase6_systick_entry_count`** 与 **`g_phase6_call_ticks`** 同窗；**历史** `logs/.../gdb_phase6_capture.log` 中 `scheduler_ticks == call_ticks` 的样本仅适用于 **改版前** 固件。

**SysTick LOAD ↔ `tick_delta`/Step8/IWDG 可复核关系表**（终局收敛续）：

| 项 | OEM（文书 / 反编译） | Phase6（源码） | GDB / Renode 复核 |
| --- | --- | --- | --- |
| **SysTick RELOAD（1ms @ 72MHz）** | 原厂 `InitializeClock` 链中间接配置（CMSIS 惯例：`LOAD = f_cpu/1000 − 1`） | [`phase6_startup_chain.c`](../src/phase6/phase6_startup_chain.c) `InitializeClock_PLLFlashSysTick_Phase6Anchor`：`SysTick->LOAD = (SystemCoreClock / 1000U) - 1U`；SDK 默认 [`system_apm32f10x.c`](../../../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c) `SystemCoreClock = 72MHz` | 读 `0xE000E014`：**`0x0001193F`** = 71999 = 72000000/1000 − 1 |
| **PD 调度（SysTick 路径）** | `PD_PeriodicDispatchFromSysTick`：**每 20 次** SysTick 入口调一次 `PD_PowerAndProtocol_Block`（`04` §3.5.1） | [`phase6_pd_systick_dispatch.c`](../src/phase6/phase6_pd_systick_dispatch.c)：`g_phase6_systick_entry_count % 20 == 0` → guard + `PD_PowerAndProtocol_Block`；启动完成前 `g_phase6_pd_irq_ready==0` 时仅计数、不调 PD | **结构对齐**：**20:1（SysTick 入口 : PD）**（与 OEM 分频一致；**600-tick 附着窗** 链 **未**在此文件实装 — **待证实**） |
| **`main` Step8 / IWDG 门控** | `main`：`tick_delta > 99` → `StepBootPhaseCounter8` + `IWDG` reload（Ghidra `main @ 0x08002E74`） | `PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS` = **100**，以 `g_phase6_scheduler_ticks` **差分**门控（[`phase6_scheduler.c`](../src/phase6/phase6_scheduler.c)） | **阈值 100** 与 OEM **同一数字**；单位为 **主循环迭代**，**不**自动等价于「100 个 SysTick 周期」 |

**结论**：**LOAD=0x1193F** 与 **72MHz / 1ms** 公式 **可复核**；**PD 分频 /20** 在源码层与 **`04` §3.5.1** **结构对齐**；**主循环与 SysTick 的 wall-clock 比例**（取决于 `RunOnce` 执行时间）仍可作为 **Renode 动态补证** 项；**100** 门槛为 **行为级数字对齐**。

## 当前实现边界（第一期）

- **已纳入**：主循环链路、CCT/HSI 路径可达、编码器与 ADC 事件接入、插 USB 后 PD GPIO 子路径可达。
- **未纳入**：USB 固件升级加密下发/写 Flash（由 `phase6_usb_guard` 显式阻断）。
- **待证实**：
  - CCT/HSI 到实际光学输出的定量关系；
  - `TMR1` 预装载/输出使能细节与原厂逐位一致性；
  - 3000 tick 辅助节拍与实机时基严格对应关系。

## PD / SysTick 时基 — 证实与待证实矩阵（回合 AN）

与 [`04_Protocol_Reverse.md`](../../../Cursor_Knowledge_Base/04_Protocol_Reverse.md) §3.5.1（`PD_PeriodicDispatchFromSysTick`）及 FINDINGS **回合 AJ–AM** 对齐；**不**新增 MMIO 语义，仅收敛文书口径。

| 主题 | 状态 | 证据 / 说明 |
|------|------|-------------|
| **每 20 次 `SysTick` 入口调用一次 `PD_PowerAndProtocol_Block`** | **已证实（结构级）** | [`phase6_pd_systick_dispatch.c`](../src/phase6/phase6_pd_systick_dispatch.c)；`g_phase6_systick_entry_count` 与 `g_phase6_call_ticks` **20:1** 期望 |
| **`PD_HelperSeq_3000tick_ThenPc13Gpio` 基于 `g_phase6_systick_entry_count` 的 3000 tick 窗** | **已证实（动态）** | 代码回合 AK；Renode **自然窗口**命中见 FINDINGS **回合 AM**（`PHASE6_PD_BREAK_IGNORE` 长会话 + `pd_attach_window_count: 0→1`）；边界注入见 **AJ** |
| **OEM `PD_600tickAttachWindow_StateMachine`（`>599` tick 分支）与 `BatteryGauge_ADC9_UpdateFromSysTick600`** | **待证实** | OEM 在 **独立 ISR 路径**内（`04` §3.5.1）；Phase6 **尚未**将同名状态机并入 `SysTick_Handler` — 与 **`PD_HelperSeq_3000tick`** 的**并行/优先级**须继续 **Renode+GDB 同窗**采样，**禁止**无证据合并两窗 |
| **600-tick 窗与 3000-tick helper 的 wall-clock 重叠关系** | **待证实** | 与上项相关；结论回写 `04`/FINDINGS，触及 ISR 合并须 **任务书 + 寄存器证据** |

## SPI1 / OLED 终局闭合矩阵（第二期执行面，回合 AN）

锚点：[`05_Full_Reconstruction.md`](../../../Cursor_Knowledge_Base/05_Full_Reconstruction.md) §1/§2 **SPI1**、[`04_Protocol_Reverse.md`](../../../Cursor_Knowledge_Base/04_Protocol_Reverse.md) §2.3/§2.4。

| 项目 | 当前工程状态 | 相对原厂差距 / 下一步 |
|------|-------------|----------------------|
| **启动 23 B `SPI1_StartupSequence`** | 已落地；G2 可计数 `tx_count=23`、`last_byte=0xAF` | 真实硬件上 `STS`/fallback 分支占比（待证实） |
| **运行期 `8×(3B+96B)` 全量帧泵** | 已实现；**脏帧 + 16 tick 保底** + `main`/PD 双请求；`trigger`/`skip` 计数可 G2 强制分支 | 与 OEM **逐字节帧内容**、`*DAT_0800980c` 类门控一致须对照 Ghidra + `04` §2.4 |
| **UI 描述符池 / `ui_framebuffer_checksum`** | 已接入路由与后处理 | `03` §1.5 全菜单与位图一致为 **终局**项，非本期单轮闭环 |
| **USB 写 Flash** | **`phase6_usb_guard` 阻断** | 见 [`phase6_secondary_dev_baseline.md`](phase6_secondary_dev_baseline.md) USB 专验章节 |

## 证据分级说明

- **G1（软件门槛）**：`phase6_acceptance` 轨迹计数 + host test 断言通过，证明路径可达与策略生效。
- **G2（动态补证）**：Renode/GDB 对关键分支和寄存器快照留痕，补强 `PH6-BHV-005` 行为链。
- **G3（与 §1 对齐）**：**无示波器/LA** 时，**不**将「波形/光学定量实机闭环」列为门禁；本文“待证实”条目在文书上归入 **逆向推导 + 行为级/寄存器影子**，**可选**将来仪器补强，**非**当前排期。

## 建议验证步骤

1. `scripts/link_geehy_sdk.sh` 后执行 `pio run`，确认工程可编译。
2. 通过主机侧验收测试验证 `PH6-BHV-001~005` 软件门槛。
3. 实机或 Renode+GDB 记录 `PC13/PB3/PB11` 与 `TMR1` 关键寄存器快照。
4. 若步骤 3 与规格偏离，再触发 Ghidra/F100-GDB 反查并回写 `03`~`05` 或 FINDINGS。

## 最近复验（2026-04-06，执行回合 N）

- 构建链：`./scripts/link_geehy_sdk.sh && pio run`，结果 `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
- 主机测试：`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`；本映射表与 `PD_PowerAndProtocol_Block` 十步顺序未改代码语义，**仍对齐 `05-MMIO-v1`**。

## 最近复验（2026-04-06，执行回合 H）

- 构建链：`chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run`，结果 `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
- 主机侧验收：`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 均退出码 `0`。
- 结论：当前映射表与代码实现未出现冲突；涉及光学定量与 PD 时序的条目仍标注“待证实”，**补证路径**按 §1 **不以示波器**为前提。

## 最近复验（2026-04-06，执行回合 I）

- 收敛项：
  - `phase6_light_pd_skeleton.c`、`phase6_input_adapter.c` 的 MMIO 访问从裸地址改为 Geehy SDK 结构体字段（`RCM->APB*CLKEN`、`GPIOx->IDATA/BSC/BC`、`TMR1->AUTORLD/CC1/CC3/CEG`、`ADC1->REGDATA`）。
  - 阶段判据从数值常量改为 `PHASE6_STAGE_UI_SPI_FLUSH` 枚举，避免验收逻辑与实现脱节。
- 构建与测试：
  - `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 7.0%`、`Flash 3.5%`）；
  - 两项 host test 均退出码 `0`。
- 口径说明：
  - 本回合仅做范式收敛，不新增 PD 行为假设；
  - `05-MMIO-v1` 中“待证实”条目保持不变；**补强**以 **逆向/Renode/GDB/寄存器快照** 为主（§1），**不等待**示波器。

## 最近复验（2026-04-06，执行回合 J/K）

- 回合 J（Renode 动态补证）：
  - `renode_phase6_pd_breaktrace.sh` 增加断点后单步快照；
  - 在 `logs/renode-phase6/20260406_144126/` 观测到 `pd_gpio_count: 0 -> 1`、`PB3/PB11: 0 -> 1`；
  - 证明 `PD_GPIO_StateMachine` 入口后状态推进在仿真中可复现（G2）。
- 回合 K（次要二进制对照）：
  - 原厂 `dump/ZHIYUN-F100-full.bin` 与构建产物 `firmware.bin` 已完成向量表/长度/关键常量对照；
  - 对照结论用于第二期收紧，不作为第一期门禁。

## 最近复验（2026-04-06，执行回合 L）

- 结构化重构（不改行为）：
  - `phase6_light_pd_skeleton.c` 新增 `Phase6_PD_SetOutputs`，统一 PB3/PB11 写入路径；
  - `phase6_light_pd_skeleton.c` 新增 `Phase6_Input_ApplyPendingEvent`，统一输入事件吸收与 UI 路由触发；
  - 以上改动不改变 `PD_PowerAndProtocol_Block` 调用序，仍与 `05-MMIO-v1` 对齐。
- 动态补证复验：
  - 执行 `TRACE_RUN_SECONDS=4 PHASE6_PD_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`；
  - 在 `logs/renode-phase6/20260406_144847/` 复现 `pd_gpio_count`、`PB3/PB11` 从 0 到 1 的推进证据。
- 口径维持：
  - USB 升级路径继续由 guard 隔离，不纳入第一期；
  - 本文“待证实”条目在 §1 约束下随 **G3（逆向/行为级表述）** 迭代，**不**等同于「示波器到货后的实机闭环阶段」。

## 最近复验（2026-04-06，Phase 6 计划执行 · 规格审计）

- **审计**：对照 `05` §2 与 `PD_PowerAndProtocol_Block`（`phase6_light_pd_skeleton.c`）— **10 步直接 callee 顺序一致**；`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` 仅作首次进入门控，与 `05` §1 原厂 `main` 初始化位置不同但无额外 MMIO 语义扩展。
- **构建**：`./scripts/link_geehy_sdk.sh && pio run` **SUCCESS**（`RAM 7.0%`、`Flash 3.5%`）。
- **主机测试**：`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- **本文**：增补 §2 顺序对照表与 `EncoderGated` / UI 三符号映射行。

## 最近复验（2026-04-06，执行回合 O）

- **构建**：`./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
- **G1**：`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- **G2**：`TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh` → 产物 `logs/renode-phase6/20260406_152754/`；`PD_GPIO_StateMachine` 断点命中后 `pd_gpio_count` 与 `PB3/PB11` 影子由 0→1 推进与 **`05-MMIO-v1`** / 本文映射一致；**未**引入新 MMIO 语义。
- **遇阻逆向**：本轮无需 Ghidra 补缺。

## 最近复验（2026-04-06，执行回合 T：UI flush 断点补证）

- **脚本改进**：`scripts/renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_TRACE_FLUSH_BP`（默认 1）与 `PHASE6_FLUSH_STEP_INSN`，可在 PD 断点后继续命中 `UI_SPI1_PumpEightFramebufferSlices`。
- **实测目录**：`logs/renode-phase6/20260406_172503/`。
- **观测**：
  - `PD_GPIO_StateMachine` 阶段后 `pd_gpio_count`、`PB3/PB11` 影子从 0 到 1；
  - 成功命中 `UI_SPI1_PumpEightFramebufferSlices`，并单步进入 `Phase6_SPI1_Transmit`；
  - `spi_flush_trigger/skip` 在当前采样窗口仍为 0（**待证实**：需扩大 flush 后段单步窗口）。
- **结论**：已把“帧泵节拍不可命中”收敛到“函数级可命中但计数跃迁点待抓取”，与 `05-MMIO-v1` 映射关系不冲突，未引入新 MMIO 语义。

## 最近复验（2026-04-06，执行回合 U：帧泵计数写回地址锚定）

- **静态锚定**：通过 `arm-none-eabi-objdump` 确认 `UI_SPI1_PumpEightFramebufferSlices` 内：
  - `g_phase6_spi_flush_trigger_count` 的“分支入口计数”位于 `+0x24` 附近（回合 W 收敛后）；
  - `g_phase6_spi_flush_skip_count` 对应写回点在 `+0xA8` 附近。
- **脚本对齐**：`renode_phase6_pd_breaktrace.sh` 增加可选计数写回断点开关 `PHASE6_TRACE_FLUSH_COUNTER_BP`（默认关闭，防止 Renode 长等待）。
- **动态结论**：
  - `PD` 与 `UI flush` 的函数级命中继续稳定；
  - 计数跃迁仍待下一轮抓取，但“写回位置不确定”这一缺口已关闭。

## 最近复验（2026-04-06，执行回合 V：flush 入口分支控制）

- **新增取证能力**：`renode_phase6_pd_breaktrace.sh` 可在 `UI_SPI1_PumpEightFramebufferSlices` 入口断点直接改寄存器：
  - `PHASE6_FORCE_FLUSH_SKIP=1`：强制 skip 判定路径；
  - `PHASE6_FORCE_FLUSH_TRIGGER=1`：强制 trigger 判定路径。
- **意义**：
  - 将“仅能被动等待分支选择”收敛为“可控分支注入”，降低了帧泵节拍补证的随机性；
  - 不改 `05-MMIO-v1` 对齐的业务链，仅增强动态证据采集策略。
- **状态**：
  - 入口注入已验证可执行；
  - 计数写回点（`+0x84/+0xA8`）首次命中仍待进一步会话稳定化补证。

## 最近复验（2026-04-06，执行回合 W：节拍计数跃迁闭合）

- **脚本链收敛**：
  - `renode_phase6_pd_breaktrace.sh` 在命中 `UI_SPI1_PumpEightFramebufferSlices` 后再次应用分支强制注入，避免入口前注入被后续路径覆盖；
  - 可选计数断点改为软件断点，规避硬件断点槽位限制导致的长等待。
- **代码可观测增强（语义不变）**：
  - `UI_SPI1_PumpEightFramebufferSlices` 将 `g_phase6_spi_flush_trigger_count++` 前移至 trigger 分支入口，使计数在进入 SPI 发送循环前即可被采样；
  - flush 事务、MMIO 写入序与 `05-MMIO-v1` 对齐关系不变。
- **动态证据**：
  - `logs/renode-phase6/20260406_181002/`：`skip_count=1`、`trigger_count=0`（强制 skip）；
  - `logs/renode-phase6/20260406_181024/`：`trigger_count=1`、`skip_count=0`（强制 trigger）。
- **判定**：
  - 运行期帧泵节拍的两类分支计数均已得到非零快照；
  - 与原厂等价差距从“只到函数命中”收敛为“可区分触发/跳过节拍并复现”。

## 最近复验（2026-04-06，执行回合 P：启动链收敛）

- **代码收敛**：
  - 新增 `phase6_startup_chain.c/.h`，将 `05` §1 的启动链关键符号以 Phase 6 锚点函数落地，并由 `Phase6_Scheduler_RunStartupStage()` 串接。
  - `main.c` 在 `Phase6_Scheduler_Init()` 后显式执行 `RunStartupStage()`，再进入周期调度；`RunOnce()` 保留启动阶段兜底调用。
- **G1**：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.2%`、`Flash 3.7%`）。
  - 两项 host test 均退出码 `0`，且新增启动链断言通过。
- **G2**：
  - `TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh` → `logs/renode-phase6/20260406_154347/`；
  - `post-step` 观测：`pd_gpio_count=1`、`PB3/PB11=1`、`last_stage=4`，维持 `PH6-BHV-005` 补证口径。
- **风险边界**：
  - USB 升级 guard 仍启用（`PHASE6_USB_FW_UPGRADE_ENABLED=0`）；
  - 启动链仅做“顺序/命名锚点”收敛，未引入冻结规格外 MMIO 语义。

## 最近复验（2026-04-06，执行回合 Q：SPI1 启动帧单点收敛）

- **代码收敛**：
  - `phase6_startup_chain.c` 新增 `SPI1_StartupSequence`，在 `RunStartupStage()` 内真实发送 23B 启动帧常量（`AE...AF`）；
  - 启动 trace 新增 `PHASE6_STARTUP_STEP_SPI1_STARTUP_FRAME`，便于验收链路定位；
  - 本回合仅收敛启动帧，不扩展运行期 `UI_SPI1_PumpEightFramebufferSlices` 全量帧泵。
- **G2 补证**（非 MCP）：
  - `scripts/renode_phase6_capture.sh` 新增 `g_phase6_spi1_startup_*` 符号采样；
  - `logs/renode-phase6/20260406_155924/` 实测：`tx_count=23`、`done=1`、`last_byte=0xAF`、`cs_low_count=1`、`cs_high_count=1`、`wait_timeout_count=23`（Renode SPI 状态位限制下的防卡死计数）。
- **边界维持**：
  - `phase6_usb_guard` 策略不变（`PHASE6_USB_FW_UPGRADE_ENABLED=0`）；
  - 未修改 `05-MMIO-v1` 冻结段，仅在 Phase 6 工程实现侧补齐文书已存在的 `SPI1_StartupSequence` 锚点。

## 最近复验（2026-04-06，执行回合 R：运行期SPI全量帧泵与USB数据面）

- **增补映射（代码符号 ↔ 冻结规格）**：
  - `Phase6_SPI1_Transmit` / `Phase6_SPI1_SetCs`（`phase6_startup_chain.c`）→ `SPI1->DATA/STS` + `GPIOB bit10`，覆盖启动帧与运行期切片共用事务底座。
  - `UI_SPI1_PumpEightFramebufferSlices`（`phase6_light_pd_skeleton.c`）→ 运行期 `8×(3B+96B)` 前缀+负载事务，保持 `PD_PowerAndProtocol_Block` 第 10 步顺序不变。
  - `Phase6_UI_RenderFramebuffer` + `Phase6_UiEventRouter_GetDescriptor`（`phase6_ui_event_router.c`）→ `03` §1.5 的模式/事件渲染语义收敛到可追踪描述符池与帧缓冲。
  - `Phase6_UsbDevice_MainLoop_DataPump`（`phase6_usb_device.c`）→ USB Device 数据面稳定计数锚点（SOF/EP0），与升级写Flash链解耦。
- **构建与测试**：
  - `pio run`：`SUCCESS`（`RAM 11.8%`、`Flash 5.1%`）；
  - `phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test`：退出码 `0`。
- **边界声明**：
  - USB 升级 guard 保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`；
  - 本回合新增 USB 仅为 Device 数据面泵，不包含写Flash。

## 最近复验（2026-04-06，执行回合 AC：SPI1 启动就绪语义收敛）

- **代码收敛**：
  - `phase6_startup_chain.c` 的 `Phase6_SPI1_Transmit` 从“每字节固定轮询并独立超时计数”改为“状态位驱动优先 + 首次超时后降级直发（fallback）”；
  - 目标是避免 Renode SPI `STS` 不完整时由 `TXBE` 轮询造成 `wait_timeout_count` 按字节线性放大，同时保留真实硬件上的事件优先分支。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.1%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`：退出码 `0`。
- **G2 补证（非 MCP）**：
  - `PHASE6_RUN_SECONDS=1 ... ./scripts/renode_phase6_capture.sh`；
  - 产物：`logs/renode-phase6/20260406_191647/`；
  - 快照：`g_phase6_spi1_startup_tx_count=23`、`g_phase6_spi1_startup_done=1`、`g_phase6_spi1_startup_last_byte=0xAF`、`g_phase6_spi1_startup_wait_timeout_count=1`（由此前按字节主导下降为非主导）。
- **等价距离收敛说明**：
  - 在不更改 `05-MMIO-v1` 冻结寄存器边界的前提下，将“仿真环境导致的每字节超时噪声”收敛为“一次性降级信号”，提高了 SPI1 启动链对 OEM 语义对照的可读性与稳定性。
- **仍缺项 / 待证实**：
  - fallback 为兼容 Renode 的保守策略，真实板级 `STS` 行为下是否始终停留在事件优先分支仍需后续实机/更高保真仿真补证（待证实）。
- **边界声明（USB）**：
  - `phase6_usb_guard` 保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`；
  - 本回合未触发、未实现任何 USB 写 Flash 路径。

## 最近复验（2026-04-06，执行回合 AD：证据链稳定化最小切口）

- **脚本稳定性收敛（不改业务语义）**：
  - `scripts/renode_phase6_capture.sh`、`scripts/renode_phase6_pd_breaktrace.sh` 增加 `PHASE6_GDB_TIMEOUT_SECONDS`，用于会话硬超时回收，避免断点等待卡住；
  - `renode_phase6_pd_breaktrace.sh` 默认切换 `PHASE6_PD_SOFT_BP=1`，降低硬件断点槽位与会话状态干扰；
  - 两脚本统一输出 `g_phase6_call_ticks`、`g_phase6_oled_last_flush_tick`、`g_phase6_ui_frame_dirty`，使 main/PD/SPI 同窗快照可读。
- **G2 复验（非 MCP）**：
  - capture：`logs/renode-phase6/20260406_195008/`，稳定命中 `PD_GPIO_StateMachine`，并读到 `spi1_startup_tx_count=23`、`startup_done=1`、`last_byte=0xAF`；
  - breaktrace(trigger)：`logs/renode-phase6/20260406_195020/`，`pd_gpio_count=1`、`PB3/PB11=1`，且 `spi_flush_trigger_count=1`；
  - breaktrace(skip)：`logs/renode-phase6/20260406_195035/`，`pd_gpio_count=1`、`PB3/PB11=1`，且 `spi_flush_skip_count=1`。
- **待证实（本轮残余）**：
  - 在本轮采样窗口内，`main_boot_phase_counter8` 与 `main_iwdg_reload_count` 仍多次为 `0`；
  - `PHASE6_PD_BREAK_IGNORE>0` 虽已有硬超时回收，但“后周期稳定命中且非零推进”仍待下一轮针对性参数矩阵继续收敛。
- **边界声明（USB）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；
  - 本轮仅做观测与取证链稳定化，未放通任何写 Flash 入口。

## 最近复验（2026-04-06，执行回合 AE：TMR3/DMA 与 USB Device 上下文收敛）

- **灯光链收敛（终局差距缩短）**：
  - `RGBTriple_PushPattern_TMR2Gated` 从“仅写 `TMR2->CC3` 首字”扩展为：
    - 初始化 `Init_TMR3_PWM_FanPB5`（`PB5/TMR3->CC2`）；
    - 初始化 `Init_DMA1Ch1_TMR2CcrDMA`（`DMA1_Channel1->CHPADDR/CHMADDR/CHNDATA/CHCFG`）；
    - 每次 HSI 推送同步回写 `tmr3_cc2_shadow`、`dma1_ch1_transfer_count`。
  - 对齐锚点：`05` §5.1/§5.3.5 的 `TMR3` 与 `DMA1 Ch1 + TMR2` 语义，未新增冻结规格外 MMIO。
- **USB Device 收敛（guard 内）**：
  - `phase6_startup_chain` 的 `SetUsbDeviceContextPointer`、`InitializeUsbControlEndpointDefaults`、`InitializeUsbDescriptorsAfterPwmInit` 现同步到 `phase6_usb_device`；
  - `Phase6_UsbDevice_CompleteInitFromContext` 在启动阶段完成数据面就绪（`context_ptr_shadow`、`ep0_max_packet_shadow`、`descriptors_ready`），但仍不触发升级写 Flash。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.3%`、`Flash 5.8%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`：退出码 `0`。
- **G2 动态补证（非 MCP）**：
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_201541/`
    - 关键快照：`spi_flush_trigger_count=1`（flush trigger 分支命中）、`pd_gpio_count` 与 `PB3/PB11` 由 0 到 1。
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh`
    - 产物：`logs/renode-phase6/20260406_201545/`
    - 关键快照：`startup_tx_count=23`、`startup_last_byte=0xAF`、`TMR1_CTRL1=0x81`、`TMR1_ARR=0x4B0`。
- **USB 风险边界（显式）**：
  - `phase6_usb_guard` 继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`；
  - 本轮仅增强 USB Device 数据面上下文，不放通、不实现任何写 Flash 路径。

## 最近复验（2026-04-06，执行回合 AF：A/B/C/D 四阶段一次性收敛）

- **阶段 A（启动链 + main 周期）**：
  - `phase6_startup_chain` 新增 `CheckMagicWord55AA` 与 `Iwdg_UnlockConfigurePrescaleReloadAndStart` 锚点；
  - `IWDT` 按 `KR=0x5555 -> PSC=6 -> RLR=0x30D -> KR=0xAAAA -> KR=0xCCCC` 顺序写入（host 测试路径保留影子）；
  - `Phase6_StartupTrace` 新增 `magic55aa_*`、`iwdt_*` 字段并进入验收报表。
- **阶段 B（SPI1 main/PD 双路径）**：
  - 新增 `Phase6_MainLoop_RequestSpiPump`，由 `phase6_scheduler` 周期请求；
  - `UI_SPI1_PumpEightFramebufferSlices` 在原 `dirty/force` 触发外接收 main 请求，形成 `main` 与 `PD` 双触发语义收敛；
  - 新增 `mainloop_spi_pump_request_count/consumed_count` 证据字段。
- **阶段 C（业务核可验收字段补齐）**：
  - 验收层新增并回填 `startup_magic55aa_value`、`startup_iwdt_started`、`mainloop_spi_pump_*`；
  - host test 断言同步收紧，确保启动链与双路径帧泵在软件门槛中持续生效。
- **阶段 D（USB guard 专验准备，不解除 guard）**：
  - `phase6_usb_guard` 新增 `preflight_ready`、`preflight_fail_count`、`flash_write_attempt_count`；
  - 在 `PHASE6_USB_FW_UPGRADE_ENABLED=0` 下保持“零写 Flash”并可审计风险计数。
- **G2 动态证据（非 MCP）**：
  - capture：`logs/renode-phase6/20260406_203926/`；
  - breaktrace：`logs/renode-phase6/20260406_203941/`；
  - 快照显示：`g_phase6_iwdt_started=1`、`g_phase6_iwdt_reload_shadow=1`、`g_phase6_spi_flush_trigger_count=1`、guard 风险计数全 0（未触发写 Flash 尝试）。

## 最近复验（2026-04-06，执行回合 AG：G2 main 周期与 capture 脚本）

- **脚本**（[`scripts/renode_phase6_capture.sh`](../../../scripts/renode_phase6_capture.sh)）：
  - 修正 GDB **`if`**：使用 **`if (expr)`** 避免 **`>`** 被解析为重定向；
  - 默认 **`PHASE6_POST_BREAK_RUN_SECONDS=2`**、**`PHASE6_CAPTURE_PD_IGNORE=0`**；
  - 新增 **`PHASE6_RENODE_SPI_FALLBACK_INJECT`** / **`PHASE6_RENODE_SPI_STS_FORCE`**（默认 1），在 Renode 下辅助完成首个 `RunOnce` 内 SPI 帧泵（与 breaktrace 的 STS 强制同思路）；
  - 后周期块使用 **`delete $bpnum`** 再 **`monitor start`**。
- **固件**：
  - [`phase6_scheduler.c`](../src/phase6/phase6_scheduler.c)：`PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS` 支持 **`-D` 覆盖**（默认 **100**，与 Ghidra `tick_delta>99` 对齐）；
  - [`env:apm32f103cb_renode_g2`](../platformio.ini)：**`-DPHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=1`**，专用于 **Renode/G2** 同屏（**非**量产默认语义）；
  - [`phase6_light_pd_skeleton.c`](../src/phase6/phase6_light_pd_skeleton.c)：`Phase6_LightPd_ReadTraceSnapshot` 的 **`scheduler_ticks`** 改为 **`g_phase6_scheduler_ticks`**。
- **G2 判读**：
  - **`100` 构建**：首次 `PD_GPIO` 断点快照中 **`main_boot_phase`/`main_iwdg_reload` 可为 0**（`scheduler_ticks` 未达门限）；
  - **`renode_g2` 构建** + 设置 **`PHASE6_FW_BIN`/`PHASE6_FW_ELF`** 指向该产物：同一快照位置可读到 **非零** `main_*`（专验用）。
- **USB**：`phase6_usb_guard` 未解除；无写 Flash。

## 最近复验（2026-04-06，执行回合 AH：启动链时钟收敛与 SysTick 影子）

- **启动链收敛（终局差距缩短）**：
  - [`phase6_startup_chain.c`](../src/phase6/phase6_startup_chain.c) 在 `InitializeClock_PLLFlashSysTick_Phase6Anchor` 中加入 `SystemCoreClockUpdate()`，并把 `sysclk_hz_shadow`、`systick_reload_shadow` 写入 startup trace；
  - `SysTick->LOAD` 由 `SystemCoreClock/1000-1` 计算并在合法范围（24-bit）内使能，避免“仅标记时钟步骤”的弱证据状态；
  - 该改动不新增 `05-MMIO-v1` 冻结范围外寄存器语义，仅把既有时钟步骤落成可核验数据。
- **验收联动**：
  - [`phase6_acceptance.c`](../src/phase6/phase6_acceptance.c) 新增 `startup_clock_*` / `startup_sysclk_hz_shadow` / `startup_systick_reload_shadow` 回填；
  - `PH6-BHV-001` 增加启动链时钟就绪门槛（`clock_hsi_ready/pll_ready/pll_selected/systick_reload_shadow`）。
- **G1 回归**：
  - `pio run`：`2 succeeded`（`RAM 13.1%`、`Flash 6.4%`）；
  - host 验收：`phase6_scheduler_acceptance_host_test` 退出码 `0`（新增启动时钟相关断言通过）。
- **待证实**：
  - 当前收敛的是“时钟步骤可核验性”，并非“与 OEM 每个时钟寄存器位级完全一致”；位级一致性仍按 `05-MMIO-v1` + Renode/GDB 持续补证。
- **USB 边界声明（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；
  - 本回合未实现、未放通、未调用任何写 Flash 路径。

## 最近复验（2026-04-06，执行回合 AI：PD Helper 时基收敛）

- **代码收敛**：
  - `PD_HelperSeq_3000tick_ThenPc13Gpio` 的 3000 tick 窗口改为基于 `g_phase6_systick_entry_count`（`SysTick` 入口计数），不再使用 `g_phase6_call_ticks`（PD 调用次数）计时。
- **等价距离缩短**：
  - 该改动消除了“PD 每 20 次 SysTick 才调度一次”导致的 helper 窗口放大偏差，使 tick 口径与 `PD_PeriodicDispatchFromSysTick` 更一致。
- **仍缺项（待证实）**：
  - `PD_HelperSeq_3000tick_ThenPc13Gpio` 与 `PD_600tickAttachWindow_StateMachine` 的并行关系仍需继续用 Renode+GDB 对照补证。
- **USB 风险边界**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；本回合未触及任何写 Flash 路径。
