# 智云 F100 固件逆向工程任务书

> 文档定位：跨对话任务锚点（任务追踪与进度续传）  
> 项目：ZHIYUN F100 / APM32F103CBT6（Cortex-M3, 128KB Flash, 20KB SRAM）  
> 首版建立日期：2026-04-04  
> 当前维护状态：启用中  
> **终局交付标准（2026-04-06 修订）**：重构完成后须 **插上即用**、与 **原厂固件** 在文书与实机验证下 **一致**，并 **可代替原厂固件** 用于产品烧录；详见 **§1「总体目标」/「终局目标与工程路线」**。  
> **当前主轴（2026-04-06）**：**Phase 6 第一期**（可编译重构 + 行为验收 G1/G2 + 基线文档）已收口；后续以 **向终局目标收敛**（全量功能与实机等价、见 **§1「终局目标」**）、**第二期功能迭代**（SPI 全量/USB 升级等）或 **遇阻逆向** 为主；**静态逆向 / Ghidra / `03`–`05` 深化** 仍为 **遇重构阻碍时再启动**（见 **§1「Phase 6 验收口径与当前工作模式」**）。

---

## 1. 项目概述与目标

本任务书用于持续追踪智云 F100 棒灯固件逆向工程进展，覆盖从启动流程到外设、业务模块、协议路径再到完整重构的全过程。  
与资源索引文档分工如下：

- 资源定位与资料导航：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/PROJECT_INDEX.md`
- 任务状态与阶段进度：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md`
- 按日关键发现流水：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md`
- **STM32F407 侧车（引脚监听/激励预备）**：根目录 `CH549G下载器/`、`射日科技STM32F407资料/`；PIO 工程见 **§7「STM32F407 射日核心板 + WCH-Link」**

### 总体目标

- 建立可复核的启动链、内存模型与运行时框架
- 识别并验证关键外设（GPIO/SPI/ADC/TMR/I2C 等）真实用途
- 还原核心业务模块（电源、灯光、显示、人机交互）
- 逆向关键协议与控制路径（PD、SPI 目标器件通信）
- 形成可重复验证的模块化重构与证据闭环
- **终局交付（与下节「终局目标」一致）**：在重构工程完成后，固件须达到 **插上即用**、在 **用户可感知行为与说明书/文书规格** 上与 **原厂镜像 `ZHIYUN-F100-full.bin` 一致**，并 **可代替原厂固件** 作为产品烧录目标（含量产替换与售后维护）；**不**以「仅通过第一期软件门槛」为项目终点。

### 外部实测条件约束与阻塞项处理原则（2026-04-06）

**当前外部条件（明确）**：硬件侧**不具备**完整电测环境；现场**仅有一台万用表**，**没有示波器**，也**没有**逻辑分析仪等可对**时序/波形**作可靠观测的仪器。因此**无法**对 **PWM、高速数字边沿、占空比波形、SPI 总线时序** 等信号做**物理层**定量或定性测量（万用表仅能作通断、直流/慢变化电压等有限用途）。

**对任务推进的含义**：

- 凡**因缺少外部仪器测量**而原本会**阻塞**的结论（例如「等示波器再定 PWM/风扇/光学」），**不再等待**外部测量；**以反汇编 / Ghidra 反编译 / 寄存器与调用链证据为主**，必要时辅以 **Renode、本机 GDB、SRAM 转储** 等**软件侧**证据闭合。
- **不将**「获得示波器/LA 后的实机波形」列为**当前排期**或**门禁前提**；若文档中仍出现「待示波器」「待仪器」类表述，**语义上**一律理解为：**在外部不具备测量条件的前提下，以逆向与仿真路径结案或标注为逆向推导结论**，而非排期等待实机仪器。

### 终局目标与工程路线（可编译重构 → 还原度验证 → 二次开发）

**长期终局**（在静态分析与 Ghidra 证据充分的前提下）：

1. **可编译重构**：在 APM32 SDK + 链接脚本语义下，建立**手写/整理后的 C 工程**，能通过工具链生成 **与原始 `ZHIYUN-F100-full.bin` 在约定维度上可对齐** 的固件镜像（分段、关键符号、外设行为等目标由任务书后续细化；**不**声称与原厂源码逐行一致）。
2. **还原度验证**：对上述构建产物与原始固件做对照。**终局门禁**为：实机 **插上即用**（上电、交互、灯光、显示、供电/充电/PD 等**与原厂无差异**或差异在文书可接受范围内），**与原厂固件一致**（以说明书 + `03`/`04`/`05` + 实机回归清单为准），**可代替原厂固件** 刷入产品（含 USB 官方升级路径若产品需要，则需在解除 guard 并完成专验后纳入）。**二进制/段级/哈希一致** 为**工程上可追求的强约束**，用于缩小与原厂差异、便于回归；**未**达哈希一致**不**自动否定「行为一致」，但 **终局不以「仅行为级骨架通过」为结项**。
3. **阶段性里程碑（Phase 6 第一期等）**：当前以 **行为级 / 用户可感知路径可达**（G1/G2）+ 文档与 guard 策略为 **阶段性验收**；**第一期不宣称**「已可替代原厂量产」，**明确列为通向终局的中间站**。
4. **二次固件开发**：在**同一工程基线**上增加或修改功能（协议、灯效、UI、保护策略等），仍须保留可回归的构建与验证步骤；**终局达成后**的二次开发仍须满足可回归与产品安全边界。

**与 Ghidra 反编译的关系**：Ghidra 输出的是**反编译伪代码**，通常**不能**直接当可编译源码；可编译工程以 **SDK 范式重写 + 文档/调用图/Ghidra 命名** 为输入，伪代码仅作对照。

### Phase 6 验收口径与当前工作模式（2026-04-06）

**工作模式**

- **主轴**：**Phase 6** — 建立可编译工程、按模块重写为 C、用**可重复用例**验证**功能等价**。
- **逆向（Phase 3/4/5、Ghidra、`02`–`05` 增补）**：**非日常主线**；仅当重构实现或验收**遇到阻碍**（规格不足、调用关系不明、寄存器语义缺证据等）时，再**回到**静态分析 / MCP / **仿真与逆向**路径**补缺**（见上节 **「外部实测条件约束」** — **不**依赖示波器/LA），并将结论**回写**专项文档与 FINDINGS。

**评价**：在 `**05-MMIO-v1` 已冻结**、**OI 书面取舍** 已具备的前提下，**优先把已知规格落成可运行固件**，避免无目标的文档扩张；与「终局路线」一致。

**Phase 6 第一期 — 验收口径（功能等价）**

- **目标（阶段性）**：在**用户可感知行为**上与原厂固件 **高度等价**，包括但不限于：**开关机**、**色温调节**、**HSL/相关灯效路径**、**亮灯与调光**、**充电与供电场景**等（以说明书 + `[03](03_Function_Modules.md)` / `[04](04_Protocol_Reverse.md)` / `[05](05_Full_Reconstruction.md)` 为规格来源）。**本阶段**用于验证主链与文书对齐；**终局目标**（插上即用、与原厂一致、代替原厂固件）见 **§1「终局目标与工程路线」**，须通过后续迭代（含 SPI/OLED 全量、USB 栈与升级等）**闭环达成**。
- **USB 固件升级**：**第一期不纳入验收**；实现上可 **暂缓**（`**UsbCtl_RequestDispatch_StateMachine` 加密下发 / 写 Flash** 等链）。**不得**将本项与「插线充电」混为一谈。
- **插入 USB 与 PD 诱骗充电**：实机存在 **插 USB 时的 PD 诱骗充电** 行为 — **属高还原范围**，须在验收用例中覆盖（`**PD_*` / `PD_GPIO_StateMachine` / 附件与 ADC 阈值** 等，见 `**04` §3**）。与 **USB 数据面升级** 分开验收。
- **USB Device 栈**：若保留 **枚举 / 主循环泵** 仅为系统稳定或后续扩展，可在第一期 **最小化或桩实现**；以**不破坏** PD/灯/UI 主路径、**不误写 Flash** 为底线。

### Ghidra 符号与类型收敛（持续目标，由 Phase 6 需求牵引）

在工程 `**ZY_F100`** 中，对未命名或低置信度符号的**函数重命名、全局变量命名、结构体/字段定义与注释**（含 `**apply_data_type`**、`**PRE_COMMENT**`、书签），以**降低**源码化与联调成本。

**默认策略**：**不为穷尽而穷尽** — **优先闭合当前重构模块**在调用图与数据流上**缺口的符号**；其余可按 xref 热度**在遇阻时**再补。

**建议优先级（由高到低）**：

1. **启动与主循环**：`Reset` 链、`main`、`USB_MainLoop_DataPump`、`SPI1_*`、时钟/FMC/IWDT 等与上电确定性相关的符号。
2. **周期业务核**：`PD_PowerAndProtocol_Block`  callee 树、`EncoderGated_*`、`UI_OLED_*`、`Input_5Channel_*`。
3. **外设与 DMA**：`TMR*`、`ADC1_*`、`SPI1`、`DMA1`、`GPIO` 原子写读封装。
4. **数据面**：SRAM 上下文（如 `**LightApplyContext**`）、模式/描述符池、与 `[03](03_Function_Modules.md)` / `[05](05_Full_Reconstruction.md)` 已文档化的地址一致。

**口径**：不要求 100% 全镜像命名；**与行为路径相关的簇先闭合**，其余可按 xref 热度递增。

### 源码输出与职责划分（达「可启动构建」里程碑后执行）

当同时满足（或经任务书修订明确豁免）下列**可核查条件**时，在任务书 **§4** 与本节**更新日期**，宣布进入 **Phase 6 准备/执行**，并启动「伪代码/源码产物」输出流程：

- `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` 调用图与 **§5 MMIO↔业务** 至少 **冻结一版**（版本号或日期标注）；
- Phase 3/4 **开放项**（OI）已有**书面取舍**（接受 **§1** 下**外部仪器不可得**时的逆向结案 / 仅行为级重构等）；
- **Ghidra** 侧关键路径命名达到本节优先级 **①②** 的**约定完成度**（由维护者在任务书或 Ghidra 书签说明中勾选）。

**文书核查状态（2026-04-06）**：上述 **三条** 在任务书 **§4「Phase 6 入口条件进度」**、**§6「开放项书面取舍」**、**§4「Ghidra 优先级 ①② 书签对照」** 中 **已可勾选**；**「Phase 6 准备」** 与 **「建立可编译工程骨架 / 源码输出执行」**（`[05` §6](05_Full_Reconstruction.md)、**§3 Phase 6 清单**）**不同步** — **后者仍以仓库出现构建工程与构建说明为开工标志**。

**输出形态分工**：已抽离至 `[REVERSE_TASK_BOOK_TOOLS.md](REVERSE_TASK_BOOK_TOOLS.md)`，集中维护（含 Ghidra MCP / GUI Export / 可编译工程职责边界）。


---

## 2. 阶段状态看板


| 阶段      | 名称                | 当前状态                                                                                       | 里程碑完成度                                                                                                                                                       |
| ------- | ----------------- | ------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Phase 1 | 启动分析              | 已完成                                                                                        | 100%                                                                                                                                                         |
| Phase 2 | 外设识别              | **已完成**（静态）                                                                                | **100%**                                                                                                                                                     |
| Phase 3 | 功能模块分析            | **遇阻续作**（非主轴）；规格见 `[03](03_Function_Modules.md)`                                           | **80%**                                                                                                                                                      |
| Phase 4 | 协议逆向              | **遇阻续作**（非主轴）；规格见 `[04](04_Protocol_Reverse.md)`                                           | **96%**                                                                                                                                                      |
| Phase 5 | 完整重构              | **文书冻结 `05-MMIO-v1**`；**非阻碍不主动扩张**；阻碍时返岗更新 `**05` §6** 等                                   | **49%**                                                                                                                                                      |
| Phase 6 | 可编译重构、行为验收与二次开发预备 | **第一期收口**（G1/G2+文档+基线）；**第二期**（SPI 全量/USB 升级等）另立条目；**终局**见 **§1「终局目标」**（插上即用、与原厂一致、代替原厂固件） | **第一期里程碑已完成**（PIO 工程见 **§7** `[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/)`）；**终局交付未完成**，主轴须继续向 **原厂等价 + 可量产替换** 收敛 |


---

## 3. 分阶段任务清单（含状态）

状态枚举：`待开始` / `进行中` / `已完成`

### Phase 1：启动分析（向量表 -> Reset -> Runtime -> main）

- [已完成] 核对向量表前 16 项与 Cortex-M3 语义一致
- [已完成] 锁定 Reset 入口与 Thumb bit 关系
- [已完成] 建立启动主链锚点：`VectorTable -> Reset_Handler -> SystemInit? -> C Runtime? -> main?`
- [已完成] 精确定位 `.data` 拷贝循环地址与参数（`__scatterload`：`**scatterload_table_iterate**` @ `**0x08006B20**`，表 `[0x08006B6C,0x08006B8C)`；详见 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)`）
- [已完成] 精确定位 `.bss` 清零循环地址与参数（表项二：`0x54C` 字节 @ `0x20000000`；同上）
- [已完成] **散表外层迭代器** 汇编级闭合（`**r1`/`r4` 界、池字 `0x2C`/`0x48`、`blx` 实参 `r0=行+4**`）：`[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` **§1.3.15**（2026-04-05）；Ghidra `**CRT_ScatterloadThenMain**` 链 @ `**0x08006B8C`…`0x08006BBC**`
- [已完成] 记录 SVD/段可见性边界（段级可见，伪代码层多为常量引用）

### Phase 2：外设识别（GPIO/RCM/SPI/ADC/TMR/I2C）

- [已完成] RCM 时钟门控入口与 APB/AHB 使能路径证据化
- [已完成] GPIO 初始化封装行为归纳（CRL/CRH、BSRR/BRR）；**PB14/EN、PB15/ADIM** 经 `**MP3398_EN_***` 与 `04` §3.8.2 闭合（2026-04-04）
- [已完成] SPI1 初始化与 CS 时序证据建立（含 PB10 低有效链路）
- [已完成] ADC1 多通道采样与阈值状态机证据建立
- [已完成] TMR1 + TMR4 显式初始化链、`DAT_08004158` SRAM 控制块与 DMA 配置形态（亮度语义仍待业务闭合）；**TMR4 编码器模式（PB6/PB7）** 与 `**Encoder_DispatchDelta_FromTMR4**` 已证据化（见 `02` §旋转编码器）
- [已完成] I2C1/I2C2 字面量全镜像扫描（0 次命中）；bit-bang 假设保留
- [已完成] **静态口径收口**（2026-04-04）：**寄存器级 + 调用链** 证据已齐；**占空比→光学 / PB6 SPI 与编码器时分 / 风扇 PWM 语义** 等 **无外部示波器/LA 时不可实测** 的项 **不降低** 本阶段完成度，归入 **Phase 5** 与 **OI-004**，**以逆向/文档推导补齐，不等待仪器**

### Phase 3：功能模块分析（业务逻辑）

> **当前**：**遇阻续作**（非主轴）。Phase 6 重写 UI/灯/输入时若规格不足，再返岗本条。

- [已完成] 电源管理状态机（输入源/电池/负载）函数簇梳理（`**PD_PowerAndProtocol_Block**` 顺序 callee、§0 mermaid、`**EncoderGated_LightOutputApplyChain**`；见 `[03_Function_Modules.md](03_Function_Modules.md)` §0）
- [进行中] LED 驱动与亮度、色温、RGB 混光控制路径还原（**TMR1 `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15**` @ `**0x0800DD28**` + `**EncoderGated_LightOutputApplyChain**` + `**[03` §0.3.1](03_Function_Modules.md) 子链细化**；占空比→光学关系**在无示波器条件下以固件与手册逆推为主**，见 **§1「外部实测条件约束」**）
- [进行中] OLED 刷新链路与菜单状态机 **深化（逆推）**：`**UI_OLED_MainStateMachine**` / `**UI_ModeRender_Dispatch**` / 模式池与 **说明书** 对照（`[03_Function_Modules.md](03_Function_Modules.md)` §1.5 等），**不依赖**屏厂 PDF
- [进行中] 用户交互输入矩阵与事件分发逻辑还原（`**Input_5Channel_TableStateMachine**`、`**Input_SamplePA1PB4_As2bit**`/`**Input5Channel_ADC_IN8_BandClassify**` 合成路径已表；`**Input5Channel_DispatchRowEvent**` 已命名；见 `[03_Function_Modules.md](03_Function_Modules.md)` §2）
- [进行中] 关键全局结构体/上下文块命名收敛（`**g_LightApply_ADCContextState**` / `**g_LightApply_UIContextPtr**` / `**g_TMR_*WriteTargetPtr**` 等已落 Ghidra；`**0x20000180**`：`**LightApplyContext**`（52 B）已 `**apply_data_type**`，字段语义仍随业务深化）

### Phase 4：协议逆向（SPI/PD）

> **当前**：**遇阻续作**（非主轴）。Phase 6 验收卡 SPI/PD 时再返岗本条。

- [进行中] 启动 **23 字节 SPI burst** 与 **SSD1306 族命令语义逆推**（**§2.4**）：以 **固件常量 + Ghidra 调用链 + 开源参考序列** 逐条闭合，**不等待**屏厂 PDF；**外部无 LA** 时**不等待**物理层印证（见 **§1**）
- [已完成] SPI 事务模板分型（启动 23 B 常量帧 + 运行期 8×(3 B+96 B)；见 `[02_Hardware_Init.md](02_Hardware_Init.md)`、`[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §2.3）
- [进行中] HUSB238 相关数字控制路径定位（已记录：固件侧为 **GPIOC IDR / GPIOB BSRR-BRR**，与 I2C 外设字面量无直接证据；见 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §3）
- [已完成] PD 协商关键状态转换与阈值表（`0x65`/`0x15`/`0x7d1`/`0xD17`，`**PD_WriteGPIOB11_BSRR_BRR**`→PB11、`FUN_08009F90`→PB3；见 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §3）
- [已完成] 异常分支（欠压/过流/温控/降额）与协议联动 **静态**验证（`**[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §3.11**：`**PD_GPIO_StateMachine**` 分支与 `**ADC1_AverageSamples**` **4** 处 `**get_function_callers**` 穷尽；**PB14/EN** 不在该状态机内，与 **§3.8.2** 第二路径 **一致**；**运行时波形** 类结论归 **OI-004**，**在无示波器条件下以逆向/Renode 表述**，见 **§1**）

### Phase 5：完整重构（闭环与可复现）

> **当前**：`**05-MMIO-v1` 冻结段** 为 Phase 6 **主规格**；**非阻碍不主动扩张** §5 正文。遇阻可增 `**05` §6**、FINDINGS。

- [进行中] 建立模块级调用图（启动/电源/灯光/显示/协议）— 初版见 `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)`
- [进行中] 完成核心函数重命名与注释策略统一（续：`**HSI_FloatMath_ComputeRGBTriple**`、`**RGBTriple_PushPattern_TMR2Gated**`、`**EncoderGated_ADC_TMR1Compare_Apply**`；`**TMR1_***` 上 **PRE_COMMENT** 与 `**03` §0.3.2** 池解析）
- [进行中] 输出寄存器级与业务级双视图文档（`**[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` §5.1 + §5.3**；**业务级** 以 §5.1 为主，**寄存器字段** 以 §5.3 为主）
- [待开始] 建立最小可复现实验步骤（**仿真/GDB/SRAM**；**不依赖**示波器/LA/抓包 — 见 **§1「外部实测条件约束」**）
- [待开始] 汇总剩余未知项并给出风险评级与后续路线

### Phase 6：可编译重构、行为验收与二次开发预备（进行中 · 主任务）

> **入口**：见 **§1「源码输出与职责划分」**、**§1「Phase 6 验收口径与当前工作模式」** 与 **§4「Phase 6 入口条件进度」**（文书三条已齐）。  
> **验收**：**行为等价** 为主（§1）；**USB 固件升级** 第一期不测；**插 USB 时 PD 诱骗充电** 须纳入高还原用例。

- [已完成] **PlatformIO** 可编译骨架 — `[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/)`（Geehy **启动/链接/StdPeriph** 经 `**src/geehy_sdk/**` + `**scripts/link_geehy_sdk.sh**`；**ST-Link**；详见 **§7**）
- [已完成] 按模块将 `**PD_PowerAndProtocol_Block` / 灯光 / UI / 输入 / SPI·OLED** 等从规格 **重写为可编译 C**（与 Ghidra 命名对齐；遇阻再开逆向补缺）
- [已完成] **USB**：**Device 栈** 按第一期策略 **最小化或桩**；**固件升级 / AES 解密写 Flash** 链 **暂不实现**（见 §1）
- [已完成] **行为验收（第一期）**：**可重复用例清单**（`[phase6_behavior_acceptance_checklist.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)`）— **G1** 软件门槛、`PH6-BHV-001~005`；**G2** Renode+GDB（`logs/renode-phase6/LATEST`）；**万用表可及步骤** 已写入清单（**不**声称波形）；**G3** 实机波形/光学定量 **非**第一期门禁（见 **§1「外部实测条件约束」**）
- [已完成] **次要：二进制对照（第一轮）**：已完成原始 `ZHIYUN-F100-full.bin` vs 构建产物的 **向量表、映像长度、关键常量** 对照；**不作为第一期门禁**
- [已完成] **二次开发预备基线**：`[phase6_secondary_dev_baseline.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)` 已含构建命令、USB 升级边界、SPI/OLED 第二期说明；**版本标签**（如 `phase6-period1-baseline`）为可选，由维护者按需打标

#### Phase 6 第二期开启说明（2026-04-06）

> **状态**：已从「第一期收口」切换到「第二期开启」执行面。  
> **边界**：第二期仍受终局目标约束；`phase6_usb_guard` 未解除前，严禁引入写 Flash 路径。

**第二期开启目标（面向终局收敛）**

1. 收紧 `SPI1/OLED` 运行期帧泵行为（触发时机、分片节拍、可观测计数）到可与原厂对照的粒度。  
2. 持续补强 Renode + 本机 GDB 的动态证据链，减少“待证实”项。  
3. 在文书中同步“已证实/待证实/下一步验证法”，确保工程与规格双向闭环。  
4. 评估 USB 官方升级链第二期入口条件（仅任务化与专验准备），保持第一期 guard 风险边界不破坏。

**第二期当前任务（执行清单）**

- [进行中] `SPI1/OLED`：运行期帧泵从“固定强刷”收敛为“事件触发 + 周期保底”并固化证据字段。  
- [进行中] 动态取证：使用 `scripts/renode_phase6_capture.sh`、`scripts/renode_phase6_pd_breaktrace.sh` 形成可复现实验记录。  
- [进行中] 文书回写：更新 `phase6_mmio_mapping`、`phase6_behavior_acceptance_checklist`、FINDINGS，显式标注残余缺口。  
- [已完成（2026-04-06）] **USB 第二期专验前置条件** 已写入 [`phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md) **「第二期：USB 官方升级入口条件与风险」** 扩展条；**未**解除 `phase6_usb_guard`，**未**引入写 Flash 路径。**实现**官方升级数据面仍 **待开始**（须专验清单 + 任务书批准后再动代码）。

---

## 4. 当前进度标记（Current Focus）

### 全局里程碑

- 已完成：Phase 1；**Phase 2（静态）**；Phase 4 主链（SPI/PD/GPIO）与 **§3.11** 静态穷尽  
- **当前主轴（2026-04-06）**：**Phase 6 可编译重构 + 行为验收**（§1 验收口径）。**日常不主动扩张** Phase 3/4/5；**重构遇阻**时再逆向补缺并回写文档。  
- **背景策略（仍有效）**：**OI-002 / OI-005** 以 **固件逆推 + 说明书** 为主；**COG 丝印 / 厂方 PDF** 为补强；**LA/示波器** 在 **§1** 约束下**不作为可得补强**，**卡壳时**仍优先 **逆向深化**。  
- **Phase 6**：**第一期已收口**（G1/G2、次要二进制对照、基线文档与万用表步骤 — 见 **§3**、`[phase6_behavior_acceptance_checklist.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)` 回合 M）。**G3（实机波形/光学定量）**：**外部仅万用表、无示波器**，**不具备**波形/PWM 类实测条件 — **不排期等待**；**USB 固件升级** 第一期不测；第二期（SPI 全量/USB 升级）另立任务条目。  
- **Phase 5 文书**：`**05-MMIO-v1**` 冻结段为重构**主规格**；**§6** 与 FINDINGS 仍可在遇阻迭代中增补。
- **OI-001 / `.map`（2026-04-10）**：已在本库标注 — **无法**从 **原始工程构建** 或 **已保存的构建产物** 取得链接器 `**.map**`（见 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` **§1.0**）；**OI-001** 闭合 **不**再依赖路径 **①**，仅 **调试器 H-OI-001 / SRAM 转储 / 指令仿真**。
- **终局路线（§1 增补）**：长期目标为 **可编译重构 → 还原度验证（直至与原厂一致、可代替原厂刷机）→ 二次固件开发**。**Ghidra 命名/类型** 以 **§1** 为口径，**默认随 Phase 6 遇阻补缺**，非独立主线。**伪代码导出**（MCP/手工）与 **可编译工程**（SDK 重写）分工见 **§1** 表。**Phase 6** 为当前执行面（**§2**、**§3**）；**第一期收口 ≠ 终局交付**。
- **F407 侧车方向（2026-04-07）**：已明确本轮目标为 **无副板自动绕过 + OLED 物理链路抓取**；Renode 继续作为补充证据，不作为量产镜像唯一验证通路。接线先行、代码后行、文档与日志同步回填（见 `Document/APM32F103CB_to_STM32F407V_Wiring.md` 与 FINDINGS 同日条目）。

### 新对话接续提示（Handoff）

- **符号规范（TMR1 @ `0x0800DD28`）**：**一律**使用 `**Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15**`。旧 Ghidra 自动名 `**FUN_0800dd28**` 仅作 **考古**（见 `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` §5.3.1 `**BDT**` 行、`[REVERSE_TASK_BOOK_FINDINGS.md](REVERSE_TASK_BOOK_FINDINGS.md)` 按日流水），**新写段落勿再当主符号**。
- **Ghidra 工程**：`ZY_F100` / `ZHIYUN-F100-full.bin`；灯光链近期命名：`**LightApply_ClassifyADC_ToControlByte**`、`**EncoderGated_ADC_TMR1Compare_Apply**`、`**LightApply_UpdateRampScratchFromADC**`（`**0x0800DEE4**` **PRE_COMMENT**）、全局 `**g_LightApply_*` / `g_ADC_*` / `g_TMR_***`（均已 `save_program`）。**USB 链**：`**RCM_EnableAPB1_USBCLK_IfBaseIsUsbDevice**`…`**UsbDevice_ExecuteConnectEnableSequence**`（见 `**05` §1/§5.1**）。**HSI microlib**：`**Microlib_***` @ `**0x0800B390**` 起簇。`**main` IWDT 纠正**：原 `**thunk_FUN_08005184**` 实为 `**IWDT_BASE` `0x40003000**` 看门狗序列 — `**Iwdg_UnlockConfigurePrescaleReloadAndStart**`、`**thunk_Iwdg_***` / `**thunk_IWDG_KR_ReloadAAAA**`、`**InitializeFlashPrefetch_SecondStage**`；书签 `**MainInit` @ `0x08005184**`（见 `**05` §1** 补充段、**FINDINGS** 按日流水）。
- **文档锚点**：`[03_Function_Modules.md](03_Function_Modules.md)` **§0.3.3–§0.3.6**（指针池、`**pcVar1` 偏移表**、ADC 分档、`**ADC1_AverageSamples` 四调用者全表**）；`[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` **§1.0**（`**.map` 不可用**）、**§1.3.16**（**OI-001 一页式摘要**）、**OI-001 §1.3.4–§1.3.9**（含 **H-OI-001**、**§1.3.8**（动态调试与脚本路径的已知限制）、`**Scatterload_DecompressRegionOrMemcpy**` @ `**0x080012E8**` **xref** 复核、`**Scatterload_MemcpyBytes**` @ `**0x080016CC**`）；`[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` 调用图 + **§5.1 MMIO↔业务** + **§5.3 `apm32f103xb.h` 寄存器偏移对照** + **§6.4** Renode+GDB **脚本化最小可复现实验**。**PB1/IN9**：`[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` **§6 第9条**（**VBAT_ADC**）、**§5 第17条**。**OI-005**：`**g_UI_ModeRender_ActiveDescriptorPtr**` **DATA READ** 四处与 `**UI_OLED**` 池实参关系见 `**03` §1.5**；`**CCT_WriteHalfwordPair_ToDed8Ptr**` @ `**0x0800DE20**`；**五行 Init**：`**Input5Channel_StoreInitOpaqueArgAtContextPlus8**` @ `**0x0800D422**`（`**03` §1.5.2**）。
- **建议下一刀（Phase 6 主轴）**：① **第一期收口**已对齐：**G1/G2**、次要二进制对照、任务书与 `phase6_*` 门禁表述 — 见 `[phase6_behavior_acceptance_checklist.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)` 回合 M、`[phase6_mmio_mapping.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)`（`05` §2 顺序对照）。② **可选实机**：按清单「万用表可及步骤」记录 **PC13/PB3/PB11** 直流/通断（**不**声称波形）。③ **第二期候选**：**SPI1/OLED 全量**、**USB 固件升级** — 须单独任务书条目 + 解除 `phase6_usb_guard` 策略前不得写 Flash。**遇阻时**再启：④ **OLED / UI**（`**03` §1.5**、`**04` §2.4**）；⑤ **ADC 模拟前端**；⑥ **OI-005 余量**（**仅当** UI 验收阻塞）。
- **Renode + 本机 GDB 采集脚本（OI-001 动态证据）**：**不推荐**在 Cursor 中通过 **F100 GDB MCP**（`user-f100-gdb-mcp`）驱动仿真调试 — **该 MCP 易用性差**（会话与 Renode 真机 GDB Server 分离、`**continue` 后**交互与寄存器快照不可靠等，历史负向见 `[04` §1.3.8](04_Protocol_Reverse.md)、`[05` §6.4](05_Full_Reconstruction.md)）。**推荐替代**：**自行**使用仓库内 `**./scripts/*.sh**`、`***.gdb**`、`**Renode/*.resc**`，按需**编写、修改、重跑**，启动 **Renode** 仿真并连接 `**arm-none-eabi-gdb**`（`**target extended-remote localhost:3333**`，见 `[04` §1.3.10](04_Protocol_Reverse.md)），**迭代直到**日志与 `**renode_sram_20k.bin**` 等产物符合预期。**默认入口**：`**./scripts/renode_oi001_auto.sh**` / `**./scripts/renode_gdb_capture.sh**` + `**scripts/renode_oi001_capture.gdb**`（`**monitor start` → `interrupt**`、导出 SRAM）。**每次运行产物集中目录**：`**logs/renode-oi001/<时间戳>/**`（内含 `**gdb_capture.log**`、`**renode_emulator.log**`、`**renode_sram_20k.bin**`、`**oi001_capture_all.sh**` 时另有 `**monitor_runfor_hook.log**` 等）；`**logs/renode-oi001/LATEST**` 指向最近一次目录。**聚合**：`**./scripts/oi001_capture_all.sh**`。分析时读 **上述目录** 而非 `**scripts/out/**` 混放。**历史示例（2026-04-05）** 路径仍为 `**scripts/out/renode_gdb_capture_20260405_171110.log**` + 根目录 `**renode_sram_20k.bin**`，见 `**[04` §1.3.12](04_Protocol_Reverse.md)**。

### 本轮优先任务（建议每次对话至少完成 1 项）

**Phase 6 主轴（默认）**

1. `**PH6-BHV-005` 动态补证**：`renode_phase6_pd_breaktrace.sh` 已可复现 G2 证据（见 `logs/renode-phase6/LATEST`）；新场景按需追加快照并记入 `phase6_behavior_acceptance_checklist` / FINDINGS。
2. **次要二进制对照**：第一轮已完成（第一期非门禁）；第二期收紧时再跑对照脚本。
3. **门禁分级**：已写入任务书 §3 与 `phase6_*` 文档；**G3** 不列为第一期门禁（见 **§1「外部实测条件约束」**）。

**遇阻时逆向补缺（非默认排期）**

1. ~~补齐 `.data/.bss` 初始化循环~~（**2026-04-06 闭合**）；**OI-001 单一 Flash `src` 块** 仍开放 — 见 `[04` §1.3](04_Protocol_Reverse.md)。
2. `**0x080041B0` 23 B**：文书已闭合 — `[04` §2.4](04_Protocol_Reverse.md)。
3. **OI-005 余量**（**仅当** UI 行为验收阻塞）：`[03` §1.5.5.3](03_Function_Modules.md)、§1.5.3.2.1；**物理键→`param_2**`、`**tag` RAM**。

### Phase 6 入口条件进度（文书可勾选，2026-04-06）


| §1 条件                              | 状态            | 证据路径                                                                                                                                                                             |
| ---------------------------------- | ------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **05：§5 MMIO↔业务 + §1–§3 调用图** 冻结一版 | **已满足**       | `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` 文首 `**05-MMIO-v1**` @ **2026-04-06**                                                                                    |
| **Phase 3/4 OI 书面取舍**              | **已满足**       | 见 **§6** 表前 **「开放项书面取舍（Phase 6 预备）」**                                                                                                                                            |
| **Ghidra ①② 约定完成度**                | **已满足（书签抽样）** | 见下 **「Ghidra 优先级 ①② 书签对照」**；工程 `**ZY_F100**` / `**ZHIYUN-F100-full.bin**`，`list_bookmarks` **112** 条（**2026-04-06** MCP）；**2026-04-06（续）** 复核 **113** 条（**±1**，**FINDINGS** 记差异） |


### Ghidra 优先级 ①② 书签对照（§1 约定完成度，2026-04-06）

**口径**：与 **§1「Ghidra 符号与类型收敛」** 优先级 **①②** 对齐；下列为 **用户书签（Note）** 抽样，**非**要求穷尽全程序命名。


| 优先级   | 主题     | 抽样书签（地址 → 注释摘要）                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| ----- | ------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **①** | 启动与主循环 | `**Startup` @ `0x08006B20**`（scatterload 驱动）；`**Scatterload` @ `0x080012E8**`（`**Scatterload_DecompressRegionOrMemcpy**`）；`**MainInit` @ `0x08002E74**`（`**main**`）；`**MainInit` @ `0x080042F4` / `0x080047FC**`（**RCM PLL/APB**）；`**USB` @ `0x0800207C**`、`**0x0800405C**`、`**Phase5` @ `0x08004092**`（`**USB_MainLoop_DataPump**`）；`**Protocol` @ `0x08004258**`（`**SPI1_PumpEightFramebufferSlices**`）；`**OLED`/`Protocol` @ `0x080041B0**`（23 B） |
| **②** | 周期业务核  | `**PD` @ `0x0800CBFC**`（`**PD_PowerAndProtocol_Block**`）；`**PD` @ `0x0800C156**`（`**PD_AttachSequence_StateMachine**`）；`**Light`/`PD` @ `0x0800C8C0**`（`**EncoderGated_LightOutputApplyChain**`）；`**UI` @ `0x0800D89C**`（`**UI_OLED_MainStateMachine**`）；`**Input` @ `0x0800D6DA**`（`**Input5Channel_DispatchRowEvent**`）；`**Encoder` @ `0x0800D3D2**`（`**Encoder_DispatchDelta_FromTMR4**`）                                                         |


**可信度**：**高**（MCP `list_bookmarks` 与已 `save_program` 工程一致）；**未抽样到的次要符号** 仍可按 **§1 口径** 续收敛。

---

## 5. 关键发现记录区（持续追加）

> 规则：每条记录包含「日期 / 结论 / 证据路径 / 可信度」  
> **按日流水全文**已迁至 `**[REVERSE_TASK_BOOK_FINDINGS.md](REVERSE_TASK_BOOK_FINDINGS.md)**`（减轻主任务书体积，便于检索与分文件投喂 AI）。  
> **更新流程**：新发现请先追加至 `REVERSE_TASK_BOOK_FINDINGS.md` 对应日期小节（无则新建 `### YYYY-MM-DD`），再视需要同步任务书 **§2–§4**、**§6** 与专项文档（`02`/`03`/`04`/`05`）。

---

## 6. 待解决问题列表（Open Issues）

### 开放项书面取舍（Phase 6 预备，2026-04-06）

下列为 **进入 Phase 6 源码化/可编译重构** 时，对 **仍开放 OI** 的 **明确接受条件**（**不**视为阻塞 **行为级** 重写与 **还原度清单** 起草）。**外部实测**：**仅万用表、无示波器/LA**（见 **§1「外部实测条件约束」**）；**仪器/LA/抓屏** 在文书上视为 **不可得补强**，**不作为等待前提**。


| ID         | 书面取舍                                                                                                                                          |
| ---------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| **OI-001** | 接受 **无链接器 `.map**`（`**04` §1.0**）；**单一 Flash 压缩源块** 在 **H-OI-001 / SRAM 转储 / 汇编推导** 未完全闭合前，**Phase 6** 以 **已文档化散载语义 + 二进制对照** 为主，**不**等待 map。 |
| **OI-002** | 接受 **23 B + `04` §2.4 逆推表** 为 **SSD1306 族指纹** 主证据；**COG 丝印 / 厂方 PDF / LA** **非门禁**。                                                           |
| **OI-003** | 接受 **GPIO+PD 静态链** 与 **万用表蜂鸣档负向（HUSB238 I2C 非直连 MCU）** 为 **当前主结论**；副板走线 **可选** LA。                                                            |
| **OI-004** | 接受 **寄存器级 PWM 路径** 已闭合；**占空比→光学 / 风扇转速** 在 **无示波器、不能测 PWM 波形** 的前提下，**以反汇编/固件语义推导与标注**结案，**不等待**外部波形测量，**不降低** 静态与行为级完成度。                     |
| **OI-005** | 接受 **§1.5.5.3 静态编号对照**；`**UI_ModeEvent` 内 `r1**` 与 `**merged≡tag                                                                              |
| **OI-006** | 接受 **软件路径已闭合**；**PB6 SPI 与编码器时分** 以 **固件与调用链** 表述；**无 LA** 时不以波形/时分实测为阻塞（见 **§1**）。                                                           |



| ID     | 问题                                                                                                                                                                                                                                                                                                                                                                                      | 优先级 | 状态                                    | 依赖/阻塞                                                                                                                            |
| ------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --- | ------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| OI-001 | **§1.3.1–§1.3.9**：**§1.3.9**（2026-04-09）已 **汇编闭合** `**scatterload_rw_decompress_entry`→`Scatterload_DecompressRegionOrMemcpy`** 的 `**r0`/`r1`/栈第 5 参** 与 `**DAT_0800695c→0x200002C4**`；**§1.3.8** 仍记录 **本机 GDB/脚本** 路径的**负向**（含 `**continue` 后批处理/超时**）。**单一 Flash 压缩源 `[src,src+L)**` 仍 **无** 闭合                                                                                       | 高   | 进行中                                   | **无 `.map**`：已标注 **无法**从原始工程构建或已保存构建产物取得（`**04` §1.0**）；闭合仅 **SRAM 转储 / 板载调试器 / H-OI-001**（`**04` §1.3.6–§1.3.9**、`**05` §6.4**） |
| OI-002 | 23 B：**SSD1306 族命令指纹** — `**04` §2.4** 已为 **全 23 字节**逐条表；**实物**：FPC `**02832-MF1-B**` 等（`[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` **§6.1**）。**策略**：**逆推优先**；**COG/LA** 为补强                                                                                                                                                                               | 高   | **进行中（逆推，静态表已闭合）**                    | 可选 **LA** 物理层印证；PDF **非必须**                                                                                                      |
| OI-003 | **GPIO**（PC13 / PB3 / **PB11**）已证实；**I2C 字面量 0 命中**；**蜂鸣档未测得 HUSB238 SDA/SCL 直连 MCU** → **本 MCU 不经 I2C 控 HUSB238** 推断加强；**PD 诱骗 20V** 与 GPIO 状态机相容                                                                                                                                                                                                                                      | 高   | **部分闭合**                              | 副板/串联电阻/LA 复核；若 HUSB238 仅被动电源路径可结案                                                                                               |
| OI-004 | **TMR1/TMR3/TMR4** 同上。**PB14→EN**：硬件 **万用表** + 固件 `**MP3398_EN_SetHigh_BSRR_PB14**` / `**MP3398_EN_ADIM_InitLow_GPIOB_BRR**` 已 **静态闭合**。**占空比→光学/风扇**：**外部无示波器**，**不以波形测量为阻塞** — **仅逆向/Ghidra 与文档推导**推进；**关灯/保护** 是否另有 **BSRR/BRR** 路径 — **静态穷尽 + 逆向**，**不等待 LA**                                                                                                                       | 中   | **部分闭合**（PB14 主路径 **高**；波形类 **逆向结案**） | `[02](02_Hardware_Init.md)`、`[04` §3.8](04_Protocol_Reverse.md)、**§1 外部实测约束**                                                    |
| OI-005 | `**[03` §1.5](03_Function_Modules.md)–§1.5.5.3**：说明书 **§二/§三** ↔ `**+0x12**` ↔ **五选一池**；`**0x0800D668`→`0x20000184**`。`**UI_ModeRender_Dispatch` 入口 `r1**` = **模式半字**（**§1.5.3.3**）。`**merged ≡ UI_ModeEvent_*` 第二实参 `r1**` = `**tag` 按位或 `param_2**`（**§1.5.3.2.1**，**2026-04-06 续**）。**余量**：**物理键→`param_2**`；`**tag` @ `0x20000020+row×0x14**`；说明书逐键文案；**三套 `0x41` vtable** **静态互斥** | 中   | **进行中（键位 / `tag` 运行态）**               | SRAM 快照；Renode `**hbreak *0x0800D5C6**`；**不等待**抓屏/LA（**§1**）                                                                     |
| OI-006 | **硬件**：**A/B→PB6/PB7**，**右键→PB4**。**固件**：`**TMR4` 编码器模式** + `**Encoder_DispatchDelta_FromTMR4**` 读 `**TMR4->CNT**`；`**FUN_0800a16c**` 仅 **PA1+PB4** 与旋钮并行。**PB6** 兼 **SPI `GPIO_SPI_SelectLine_0x40**`：**外部无 LA** 时以 **固件路径与调用关系** 表述时分，**不等待** LA                                                                                                                                      | 中   | **已闭合（软件）**                           | `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §3.8.2、§2.5；`[02_Hardware_Init.md](02_Hardware_Init.md)` §编码器                 |


---

## 7. 文件输出索引（按阶段产出）

### Phase 1：启动分析

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/reverse_log.md`

### Phase 2：外设识别

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`

### Phase 3：功能模块分析

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/03_Function_Modules.md`

### Phase 4：协议逆向

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/04_Protocol_Reverse.md`

### Phase 5：完整重构（文书冻结 · 遇阻维护）

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/05_Full_Reconstruction.md`（轻量 `**main` / `PD_PowerAndProtocol_Block**` 调用图与 mermaid；**§5 MMIO↔业务双表**；`**05-MMIO-v1` @ 2026-04-06** 为 Phase 6 **主规格**；**§6** 非冻结）

### Phase 6：可编译重构与二次开发（进行中 · 主任务）

- **PlatformIO 工程根目录**：`[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/)`（向导工程名 `**zhiyun_f100_apm32**`；环境 `**apm32f103cb**`）
- **配置**：`[platformio.ini](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/platformio.ini)` — **无 ST 框架**（`framework` 空）；**板卡** `genericSTM32F103CB`（与 **APM32F103CBT6** 同档 128KB/20KB）；**烧录/调试** `upload_protocol` / `debug_tool` = **stlink**；**链接脚本** 指向仓库 **Geehy** `[apm32f103xb_flash.ld](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/gcc/apm32f103xb_flash.ld)`；**宏** `-DAPM32F103xB`、`-DUSE_STDPERIPH_DRIVER` 与 **SDK 头路径** 见同文件 `build_flags`
- **应用入口**：`[src/main.c](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/main.c)`（当前为 **最小骨架**）
- **SDK 源链入**：`[src/geehy_sdk/](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/geehy_sdk/)` — 对 `[SDK/APM32F10x_SDK_V2.0.0/](../SDK/APM32F10x_SDK_V2.0.0/)` 下 **StdPeriphDriver `*.c` + `system_apm32f10x.c` + `startup_apm32f103xb.S**` 的**相对路径符号链接**；`**.gitignore` 忽略该目录下链接文件**（仅保留说明 `[README.txt](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/geehy_sdk/README.txt)`）
- **克隆或清空后首步**：于工程根执行 `[scripts/link_geehy_sdk.sh](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/scripts/link_geehy_sdk.sh)`（`chmod +x` 后 `./scripts/link_geehy_sdk.sh`），再 `**pio run**` / `**pio run -t upload**`
- **说明**：PlatformIO **lib/** 在未解析到依赖时可能 **不编译** 库源，故采用 `**src/geehy_sdk/**` 链入 SDK；**VS Code** 装 PIO 扩展 **烧录/调试** 与 **Cursor** 打开**同一仓库**改源码可并行
- **第一期不测**：**USB 固件升级**（加密下发 / 写 Flash）；**须验**：**插 USB 时 PD 诱骗充电** 等行为等价（验收 **§1**；清单 **§3 Phase 6**）
- **MMIO 映射清单**：`[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)`（`05-MMIO-v1` ↔ 可编译代码符号对照）
- **二次开发预备基线**：`[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)`
- Ghidra 工程：`ZY_F100`（**遇阻时**补缺命名/类型，见 **§1**）

### STM32F407 射日核心板 + WCH-Link（侧车硬件 / 2026-04-07）

> **定位**：与量产目标 **APM32F103CBT6** 分离的 **STM32F407** 实验板，用于后续 **GPIO 并行监听 / 可控激励**（深度逆向与实机互证）；**不**替代 Phase 6 主工程验收路径。

- **厂商资料（仓库根目录）**
  - **射日科技 F407V 核心板**：`[射日科技STM32F407资料/](../射日科技STM32F407资料/)` — 含原理图 `4，原理图&封装库/STM32F407VET6_V1.2(1).pdf`（文档型号 **STM32F407VET6**，512KB Flash）、`2，例程代码/库函数版本/例1 LED灯`（**LED0 = PA0**，`system_stm32f4xx.c` 载明 **HSE 25 MHz** → **168 MHz**）。
  - **CH549 / WCH-Link 下载器**：`[CH549G下载器/](../CH549G下载器/)` — 驱动与原理图 PDF；**DAP 模式**下作 **CMSIS-DAP** 使用；固件异常（LED 闪烁、无法识别）处理见 `烧录固件工具/LED灯闪烁解决方法.txt`（WCHISP、**WCH-Link_APP_IAP_ARM.bin** 等）。
- **PlatformIO 工程路径**：`[ZHIYUN_F100_Firmware/stm32f407vgt6_probe/](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/)`（目录名与 **VGT6** 一致；**默认环境已对齐实物 STM32F407VGT6**）。
- **`platformio.ini` 环境**
  - **`sheritech_f407vg_cmsis_dap`**（**默认**）：`board = black_f407vg`（**STM32F407VGT6**，1MB）；`upload_protocol` / `debug_tool` = **cmsis-dap**（WCH-Link DAP）；**`board_upload.maximum_size = 1048576`** 纠正上游 `black_f407vg.json` 误标的 512KB。
  - **`sheritech_f407ve_cmsis_dap`**：**仅当**实物为 **VET6**（512KB）时选用（`black_f407ve`，`board_upload.maximum_size = 524288`）。
  - **`sheritech_f407vg_stlink`**：**ST-Link** 独立接线时改用（`stlink` / `stlink`，仍按 VGT6 容量）。
- **固件行为**：`src/main.c` — **HSE 25 MHz** PLL 至 **168 MHz**，**PA0** 500 ms 翻转（与射日「例 1 LED」一致，便于首烧验证）。
- **常用命令**（工程目录下）：`pio run`；`pio run -t upload`；`pio debug`（DAP 环境下与 **cmsis-dap** 配套）。

### 任务书附录（按日关键发现流水）

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md`（原 **§5** 全文；**新发现优先追加于此**）

### 跨阶段辅助证据

- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/PROJECT_INDEX.md`
- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Zhiyun_F100_Repair_Notes.md`
- `/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Markdown/IC_MCU_逻辑关系推测.md`

### 运行时采集脚本（Renode + 本机 GDB，OI-001 / H-OI-001）

> 工具与脚本清单已抽离至 `[REVERSE_TASK_BOOK_TOOLS.md](REVERSE_TASK_BOOK_TOOLS.md)`，集中维护（含脚本列表、产物目录、推荐运行方式与已知限制指引）。

---

## 8. 使用与更新规范

- 每次新对话开始先读取本任务书，再读取 `PROJECT_INDEX.md` 定位资源；**默认按 Phase 6 主轴**推进；涉及 **OI-001 动态验证** 时查阅 **§7** 与 `**logs/renode-oi001/**`（或 `**LATEST**`）已有采集
- **逆向补缺**：仅在 **Phase 6 遇阻**时启动；结论回写 `**03`–`05` / FINDINGS**，并更新 **§6 OI** 若适用
- 子任务以“单次对话可完成”为粒度，完成即更新状态
- 新发现先写入 `[REVERSE_TASK_BOOK_FINDINGS.md](REVERSE_TASK_BOOK_FINDINGS.md)`（见 **§5** 指引），再同步修改对应阶段任务状态
- 新产出文件必须登记到“文件输出索引”
- 若结论仅为推测，必须标注“待证实”并注明下一步验证动作；**若下一步验证依赖示波器/LA/PWM 实测而外部不具备条件**，则**不得**将任务挂起等待仪器 — 应改为 **逆向/仿真可执行** 的验证路径或 **明确标注为逆向推导结论**

---

## 9. 维护记录

- 2026-04-07：**现状摘要回填** — 新增“F103 量产逆向 + F407 侧车”现状归纳（进度/阻塞/Phase6 差异 + Ghidra 关键证据：`PD_GPIO_StateMachine`、`0x0800A1E4` 常量池、`SPI1_StartupSequence` 与 `0x080041B0` 23B）。并在接线规范中新增 OLED 抓取专用映射（先文档后代码）。
- 2026-04-07：**STM32F407 侧车** — 登记根目录 `CH549G下载器/`、`射日科技STM32F407资料/` 与 PIO 工程 `[ZHIYUN_F100_Firmware/stm32f407vgt6_probe/](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/)`（默认 **black_f407vg / VGT6** + **cmsis-dap**，HSE 25 MHz，**PA0** 心跳）；详见 **§7「STM32F407 射日核心板 + WCH-Link」**；FINDINGS 同日流水。
- 2026-04-06：**动态仿真口径** — 任务书删除 **F100 GDB MCP**（`user-f100-gdb-mcp`）的操作指引与推荐；**Renode + 本机脚本/GDB** 为默认路径（**§4 Handoff**、**§7**），自行编写/修改脚本并反复仿真直至产物符合预期。
- 2026-04-04：创建首版任务书，完成阶段框架、状态初始化、问题清单与输出索引。
- 2026-04-04：新增 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)`，更新 Phase 1/4 任务项、OI-001～003 与关键发现区。
- 2026-04-04：Ghidra MCP 会话收口 Phase 2/4：更新 `02`/`04`/任务书（TMR、SPI 分型、PD 阈值与 `**PD_WriteGPIOB11_BSRR_BRR**`、I2C 字面量扫描）。
- 2026-04-04：续推计划执行——离线反汇编闭合散表 RW 解压语义与 PD 调用链（§1.4 / §3.5）；修正 23 B SPI hex；记录 MCP 无实例时改用 bin 分析。
- 2026-04-04：Ghidra MCP 重连成功，书签写入 `ZY_F100` / `ZHIYUN-F100-full.bin` 并保存。
- 2026-04-04：续推计划执行——闭合 23 B SPI xref、`SysTick` 向量与 `PD_PeriodicDispatchFromSysTick` 命名、外设基址字面量表、引脚 PC13 vs PA3 说明；更新 `04`/`02`/任务书 §5 与 OI-003。
- 2026-04-04：用户修订 `[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` §6；同步 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §3.8 与任务书 §5/OI-003（PB3/PB11/PC13 网络、PB6/PB7 编码器与 HUSB238 假设关系）。
- 2026-04-04：用户万用表测量——**HUSB238 SDA/SCL 未直连 MCU 任意脚**；记录 **PD 诱骗 20V**；更新 `04` §3.1.1、任务书 §5、**OI-003** 为部分闭合。
- 2026-04-04：Ghidra MCP 续推——补充 `04` §3.9（`PD_PowerAndProtocol_Block` callee、USB 链）、`02`（TMR1 引脚掩码、编码器 PA1/PB4、EXTI 0 命中、`**PD_SetDoneFlagHalfword**` 命名）；更新 **OI-004/OI-006**、Phase 4 完成度；书签 `**USB**` @ `0x0800207C`、`PD` @ `0x0800CBFC`，`save_program`。
- 2026-04-04：用户 **再次扩展 `[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` §6**（OLED/PWM/NTC/编码器与按键5 等）；同步 `04` §3.8.1–3.8.2、任务书 **OI-004/OI-006** 与 §5 条目；**TMR1↔PA8/PA9/PB15** 与手册 **强一致**；**PB4** 按键 vs 固件 **PA1+PB4** 编码器扫描 **OI-006** 保留；Ghidra `**Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15**`（`0x0800DD28`）前置注释已关联手册 `**§6 W_PWM/C_PWM/ADIM**`，`save_program`。
- 2026-04-04：用户 **万用表确认** 编码器 **A/B→PB6/PB7**、**右键→PB4**；更新 `04`/`02`/任务书：**PB4 与 `FUN_0800a16c(1)` 闭合**；**PA1+PB4 非 PB6/PB7 正交**；**OI-006** 改为 **部分闭合**（PB6/PB7 软件路径仍查）；Ghidra 书签 `**Input**` @ `**0x0800A16C**`，`save_program`。
- 2026-04-04：用户明确 **§6 第19–20条**——**PB6/PB7** 为编码器 **A/B**，**C 相接地**；同步 `04`/`02`/任务书（EC11 类公端接法、仅 A/B 进 MCU）。
- 2026-04-04：续分析计划执行——`**TMR4` 编码器** + `**Encoder_DispatchDelta_FromTMR4**` 闭合 **OI-006**；`**04` §2.5** SPI CS 与 OLED 假设；`**04` §3.10** `FUN_0800C156` 初稿；Ghidra 重命名 `**Init_TMR4_EncoderQuadrature_PB6PB7**` / `**TMR_ConfigureEncoderMode_SMCR_CCMR**` / `**Encoder_DispatchDelta_FromTMR4**`，书签 `**Encoder**`，`save_program`。
- 2026-04-04：计划实施——**§3.10** 闭合 `**PD_AttachSequence_StateMachine**` 与 `**PD_GPIO_ReadPc13ToggleApplyPb3**` / `**PD_OnAttachDone_ResetEncoderAndUI**`；**§2.6** SPI–UI 静态解耦；**TMR3+PB5** 闭合 **MO_PWM** 固件路径；Ghidra 书签 `**PD**` @ `0x0800C156`、`**Timer**` @ `0x0800dc92`，`save_program`。
- 2026-04-04：续分析计划落地——`**04` §1.3.1** OI-001 收紧；**§2.4** SSD1306 对照；`**03_Function_Modules.md**`；`**Scatterload_FindRegionNode**` / `**UI_OLED_MainStateMachine**` 重命名；书签 `**Scatterload**` / `**UI**`；`save_program`。
- 2026-04-04：续推——`**UI_ModeRender_Dispatch**` / `**UI_ModeBitmap_CopyRows**` / `**UI_ModeActiveContext_Clear**` / `**UI_ModeRender_PostProcess**` 反编译与 `**03` §1.5**；字面量池 `**0x080081C8`/`0x080082CC**` 位图抽样；说明书 **CCT/HSI/FX** 与模式字 **2–7** **推测表**；Ghidra 书签 @ `**0x0800D600**`，`save_program`。
- 2026-04-04：**OLED 子项暂缓**——待 **屏型号规格书** 后再继续 **OI-005** 与 **23 B 与数据手册逐条核对**；任务书 Phase 3/4 相关条、`**03**` 头注、**OI-005** 状态已同步。
- 2026-04-04：续推计划落地——`**04` §1.3.2** OI-001（池指针、handler 槽、`FUN_0800154C` 语义）；`**03` §2** `PD_PowerAndProtocol_Block` 三子函数数据流；`**02**` ADC 与 **5 路输入** 关联；**PA1** 掩码闭合、**PB14** 静态未闭合；Ghidra 重命名 `**Input_5Channel_TableStateMachine**` / `**PD_HelperSeq_3000tick_ThenPc13Gpio**` / `**ProtocolLanes_BitExpandTick**`，书签 `**Scatterload` @ `0x080014B0**`，`save_program`。
- 2026-04-04：**计划实施**——`**03` §0** 电源/PD 总览与 **mermaid**；`**EncoderGated_LightOutputApplyChain**` 命名与 `**PD_PowerAndProtocol_Block**` 注释、书签 `**PD` @ `0x0800C8C0**`；**PB14** 全镜像 `**0x00004000**` 字面量 **0 命中**；更新 `**04` §3.8–3.9**、任务书 **Phase 2/3** 完成度与 §5。
- 2026-04-04：**用户万用表** — **MCU PB14 ↔ EN 测试点 直通**；更新 `**04` §3.8.2**、任务书 **§5**、**OI-004**；**Phase 2** **98%**；Ghidra 书签 `**GPIO` @ `0x0800A340**`（PB14/EN 与池字说明），`save_program`。
- 2026-04-04：**计划实施** — `**get_xrefs_to` @ `0x40010C10**` 闭合 **PB14 BSRR/BRR** 路径：`**MP3398_EN_SetHigh_BSRR_PB14**`、`**MP3398_EN_ADIM_InitLow_GPIOB_BRR**`；`**PD_OnAttachDone_ResetEncoderAndUI**` 更新；`**03`/`02`/`04**` 与 **OI-004** 同步；**Phase 2** **99%**；Ghidra `**GPIO` 书签 @ `0x0800A11A`/`0x0800A0DA**`，`save_program`；**§1.3.2** 增加 **OI-001 复核** 行（源块仍开放）。
- 2026-04-04：**续推计划实施** — `**04` §1.3.3**（`**FUN_080016CC**`、`**FUN_08000f3c**`、`**Scatterload_ParallelDecompressEntry**` 无静态 xref）；`**04` §3.11**（`**PD_GPIO_StateMachine**` 与 **ADC**/`GPIOB_BSRR` 关系）；任务书 **Phase 2=100%（静态）**、**Phase 4=96%**；Ghidra 重命名/注释/书签/`save_program`（见 §5 新条）。
- 2026-04-04：**OLED 型号发掘** — 新增 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` **§2.7**（**SSD1306 族 / 128×64 / SPI**；**模组料号** 仍依赖 **拆机**）；更新 **OI-002/OI-005**；任务书 §5；Ghidra 书签 `**OLED` @ `0x080041B0**`、`PRE_COMMENT` @ `**SPI1_StartupSequence**`，`save_program`。
- 2026-04-04：用户补充 **OLED 玻璃/FPC 喷码与尺寸、主板座 Pin13/16 等实测** — 写入 `[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` **§6.1**；同步 `[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §2.7、任务书 **OI-002** 与 §5。
- 2026-04-04：用户反馈 **未** 在屏体上观察到 **COG 芯片可读丝印** — `[Zhiyun_F100_Repair_Notes.md](../Document/Zhiyun_F100_Repair_Notes.md)` §6.1 增 **负向观测**；`[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` §2.7.3 更新；**OI-002** 仍以 **固件指纹 + FPC 料号 + 尺寸** 为主证据。
- 2026-04-04：**续推计划实施** — `[03_Function_Modules.md](03_Function_Modules.md)` **§0.3.1**（灯光子链）；`[04_Protocol_Reverse.md](04_Protocol_Reverse.md)` **§1.3.4**（OI-001 验证路径）；新增 `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)`；Ghidra 重命名 `**LightOutput_PendingHSI_Dispatch**` / `**CCT_Slew_TableSplit_TMR1Shadow**` / `**TMR1_CompareHalfword_Write**` / `**TMR1_PeriodHalfword_Write**`，书签 `**Light**`，`save_program`；任务书 **Phase 3=70%**、**Phase 5=15%**。
- 2026-04-04：**继续** — Ghidra 重命名 `**HSI_FloatMath_ComputeRGBTriple**`、`**RGBTriple_PushPattern_TMR2Gated**`、`**EncoderGated_ADC_TMR1Compare_Apply**`；`**TMR1_CompareHalfword_Write`/`TMR1_PeriodHalfword_Write**` **PRE_COMMENT**；`[03` §0.3.2](03_Function_Modules.md) `**0x0800DED0**` 池与 `**TMR_T**` 偏移对照；任务书 **Phase 3=72%**、**Phase 5=18%**，`save_program`。
- 2026-04-04：**继续-2** — Ghidra `**rename_global_variable**`：`**g_LightApply_ADCContextState**`、`**g_LightApply_UIContextPtr**`、`**g_TMR_CCR_WriteTargetPtr**`、`**g_TMR_PeriodWriteTargetPtr**`；`**LightApply_ReadContextState0/3**`；`[03` §0.3.3](03_Function_Modules.md)；任务书 **Phase 3=75%**，`save_program`。
- 2026-04-04：**继续-3** — `[03` §0.3.4](03_Function_Modules.md) `**pcVar1**` 偏移 / `**LightApply_UpdateRampScratchFromADC**`；`**g_ADC_Average_u32**`、`**g_SystemTickCounter_u32**`、`**g_LightModeAuxFlags_u16**`；`xxd` `**68 02 00 20**`；任务书 **Phase 3=78%**，`save_program`。
- 2026-04-04：**继续-4（会话收尾）** — `**LightApply_ClassifyADC_ToControlByte**`、`**g_ADC_LightClassThresholdTable_u16**`；`[03` §0.3.5](03_Function_Modules.md)；任务书 **§4 Handoff**、**Phase 3=80%**，`save_program`。
- 2026-04-04：**Ghidra 续分析计划** — `**ADC1_SetChannelSampleTimeAndRegularRank1**`（`0x0800D50A`）、**ADC IN3/4/8/9→引脚** 与 `**Init_ADC**` 表写入 `[02](02_Hardware_Init.md)`；**SRAM** 块 + `**LightApplyContext**` @ `**0x20000180**`；任务书 §5、§4 Handoff、`[03](03_Function_Modules.md)` §0.3.3–0.3.5 同步。
- 2026-04-04：**PD GPIO 原子函数追踪** — `**PD_ReadGPIOC13_IdrBit**` / `**PD_WriteGPIOB3_BSRR_BRR**`、`**PD_600tickAttachWindow_StateMachine**`、`**BatteryGauge_ADC9_UpdateFromSysTick600**`；`[04](04_Protocol_Reverse.md)` §3.1.2、§3.5.1、§3.8.1、§3.11；`[02](02_Hardware_Init.md)` caller 表；任务书 §3/§5/§9。
- 2026-04-04：**下一阶段分析计划实施** — `**get_function_callers` @ `ADC1_AverageSamples**` 穷尽 **4** 处；`**Input5Channel_ADC_IN8_BandClassify**` + `**ADC1_AverageSamples` PRE_COMMENT/书签**；`[03` §0.3.6](03_Function_Modules.md)、`[02` §ADC](02_Hardware_Init.md)、`[05` §5](05_Full_Reconstruction.md) MMIO↔业务双表、`[04` §1.3.4](04_Protocol_Reverse.md) **H-OI-001**；任务书 §5、Phase 5 **22%**，`save_program`。
- 2026-04-04：**任务书「下一刀」— §5.3 寄存器字段表** — `[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` **§5.3**（`TMR`/`ADC`/`GPIO`/`SPI`）；Ghidra `**Phase5**` 书签 @ `**0x0800DED0**`、`**0x0800D50A**`；Phase 5 **28%**；清单「双视图文档」改 **进行中**，`save_program`。
- 2026-04-04：**Phase 5 计划实施** — `**05` §5.3.5**（`**DMA_Channel_T**` / `**TMR2` CCR DMA**）、§5.3.1 `**BDT` 负向**；修正 `**main**` 对 `**0x08004092**` 归类为 **USB**；Ghidra 重命名 `**USB_MainLoop_DataPump**`、`**DMA1_Channel_ApplyCfgAndAddresses**`、`**Init_DMA1Ch1_TMR2CcrDMA**` 等 + 书签 + `save_program`；任务书 §5 与 Phase 5 **32%**。
- 2026-04-04：**Phase 5 续推** — `**main` 直接 callee** `**FUN_08002DF6`…`FUN_08003DE8**`、`**SetUsbDeviceContextPointer**`、`**CopyFontGlyphRunsToWorkBuffer**` 等 **批命名**；`**ResetDmaChannelAndEnableAHB1**`；`[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` **§1** 扩表；书签 `**MainInit**`；Phase 5 **36%**。
- 2026-04-04：**Ghidra 续分析计划落地** — `**RCM_*`/`GPIO_WriteCRLCRH_FromPinMaskDescriptor**`；`**SPI1_PumpEightFramebufferSlices**` / `**SPI1_Send3ByteSlicePrefixForChannel**`；`**UsbDevice_CompleteInitFromContext**` / `**UI_SPI1_PumpEightFramebufferSlices**` / `**UI_OLED_DisplayInitAndFlushFramebuffer**`；`**02` §RCM** AHB/APB 映射纠错；`**03`/`04`/`05**` 同步符号；任务书 **§5** 新条、**§4 Handoff**、Phase 5 **40%**；`save_program`。
- 2026-04-04：**Phase 5 启动链再命名** — `**Init_GPIO_PortBCD_StartupExtended**`、`**GPIOC_Pin2Drive_BSRR_or_BRR**`、`**GPIOB_Pin9Drive_BSRR_or_BRR**`、`**FMC_DelayFromStoredSysClkThenKeySequence**`；`**02**` `main` 摘录；任务书 **§5**、**§4 Handoff**、Phase 5 **42%**；书签 `**MainInit` @ `0x080038c4`/`0x080050dc**`，`save_program`。
- 2026-04-04：**FMC 等待状态主块** — `**FMC_ConfigureWaitStatesAndSysClkCache**`、`**RCM_ComputeSysClkHz**`、`**TickVar_Get**`；`**05` §1** `**InitializeClock_PLLFlashSysTick**` 表注；任务书 **§5**、**§4 Handoff**、Phase 5 **44%**；书签 `**MainInit` @ `0x08004618**`，`save_program`。
- 2026-04-04：**UI_OLED_MainStateMachine callee 批命名** — `**UI_Framebuffer_***`、`**UI_SPI_GpioIdlePattern**`、`**UI_PromptGlobals_***`、`**UI_DialogTimed_SequenceA/B**`、`**UI_ActiveContext_Byte1_Get**`、`**UI_Framebuffer_DrawAsciiString**`；`**03`/`04**` 表项同步；任务书 **§5**、**§4 Handoff**、Phase 5 **47%**；书签 `**UI` @ `0x0800d89c**`，`save_program`。
- 2026-04-04：**Phase 5（Ghidra MCP 续分析）** — `**FUN_080042f4`/`FUN_080047fc**` → `**RCM_SequencePLLHSEAndBusEnable_ApplyControlBlock**` / `**RCM_SequenceSysClkMuxAndAPBPrescale_ApplyControlBlock**`（`**InitializeClock_PLLFlashSysTick**` 唯二调用；`**RCM` `0x40021000`/`CFG**`、`**PMU` `0x40007000**`）；`**FUN_0800e9dc**` → `**UI_ModeRender_LightApplyDiagnosticsAscii**`（`**UI_ModeRender_Dispatch**` `**bx` vtable**；`**FUN_08009828**` 唯一 caller）。更新 `**05` §1**、`**03` §1.5**、任务书 **§4/§5**；书签 `**MainInit` @ `0x080042f4`/`0x080047fc**`、`**UI` @ `0x0800e9dc**`，`save_program`。
- 2026-04-04：**任务书优先项 3（PD GPIO 向下追踪）** — `**PD_WriteGPIOB11_BSRR_BRR**`、`**GPIOA_Pin0_Drive_BSRR_or_BRR**`、`**PD_ResetPc13ToggleContext_ClearGPIOA0**`、`**PD_GPIO_ToggleHook_NoOp**`、`**SysClkHz_ReadCachedFromSRAM**`：Ghidra **重命名/PRE_COMMENT/书签**（`**PD_GPIO` @ `0x0800A1C0`/`0x08009FA0**`）；同步 `**04` §3** 证据行、`**05` §5.3.3**、任务书 **§4/§5/§9**；`**save_program**`。
- 2026-04-04：**下一阶段 Ghidra 计划实施** — **USB** 全链符号化（见 **§5** 新条）；`**EncoderGated**` → **HSI/RGB** 子树 **microlib** 与 **DMA/TMR/NVIC** 助手重命名；`**LightApply_UpdateRampScratchFromADC**` 注释；`**05` §1** `UsbDevice_CompleteInitFromContext` 表行同步；**Phase 5** 进度 **47%→49%**（符号表与 §5.1 对齐）。
- 2026-04-04：**续分析计划实施（IWDT）** — `**thunk_FUN_08005184**` 实为 `**IWDT_BASE` `0x40003000**`：`**Iwdg_UnlockConfigurePrescaleReloadAndStart**`、`**IWDG_KR_***` 助手、`**thunk_Iwdg_***` / `**thunk_IWDG_KR_ReloadAAAA**`；`**FUN_08005100**` → `**InitializeFlashPrefetch_SecondStage**`；`**04` §1.3.4** 增补 **OI-001/OI-004 静态止点**；`**03` §0**、`**05` §1** 补充段；任务书 **§4 Handoff**、**§5**；`save_program`。
- 2026-04-04：**PB1→VBAT_ADC** — 维修笔记 **§6 第9条**、**§5 第17条**；`**04` §3.8.1**、`**02` §ADC**、`**05` §5.1**；Ghidra `**BatteryGauge_ADC9_UpdateFromSysTick600**` **PRE_COMMENT**；任务书 **§5**；`save_program`。
- 2026-04-04：**续分析计划（Ghidra MCP）** — `**Input5Channel_DispatchRowEvent**`、`**Input_SamplePA1PB4_As2bit**`、`**GPIOB_Pin9_BSRR_BRR_ProtocolLane0**`；`**Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence**`、`**Init_TMR3_PB5_AndTickSyncLoop**`；`**04` §1.3.5**（`**FUN_080012E8` xref** + **OI-001** 复核）；`**02`/`03`/`05**` 符号同步；任务书 **§4 Handoff** 增补 **PA0/PB12/PB13** 万用表建议；§5/§6 **OI-001** 行指向 **§1.3.5**；`save_program`。
- 2026-04-04：**逆向续作计划落地** — Ghidra `**FUN_0800d38c**` → `**TMR4_EncoderReset_ClearStateAndReinitQuadrature**`（`**PRE_COMMENT**`、书签 `**Encoder` @ `0x0800D38C**`）；`[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` 新 **§6**（**OI-001** 三选一验证表、**OI-004** 示波器建议、**OI-002/005** 暂缓、万用表可选项、**§6.3** TMR4 复位说明）；任务书 **§5** 新条；`[04` §3.10](04_Protocol_Reverse.md) Ghidra 重命名行、`02` TMR4 表已对齐；`save_program`。
- 2026-04-04：**任务策略更新 — OLED 逆推**：**不再等待**屏体规格书；**OI-002/OI-005** 改为 **固件逆推优先**（23 B / `**UI_ModeRender_***` / 说明书对照）；更新 **§3 Phase 4 清单**、**§4 Current Focus**、**§6 OI 表**、**Handoff**、`[03](03_Function_Modules.md)` 头注、`[05` §6.1](05_Full_Reconstruction.md)、`[04` §2.0](04_Protocol_Reverse.md)。
- 2026-04-05：**计划实施** — `[04` §2.4](04_Protocol_Reverse.md) **23 B 全表**；`[03` §1.5](03_Function_Modules.md) **Flash `0x0800DB5C` 池 + 说明书** 对照；任务书 **§5**/**§6 OI-002/OI-005**；Ghidra `**g_UI_ModeRender_ActiveDescriptorPtr**`、`**UI_ModeDescriptorPtrPool_Flash**`、书签 `**UI` @ `0x0800DB5C**`，`save_program`。
- 2026-04-06：**OI-005 续** — `[03` §1.5.1](03_Function_Modules.md)：`**PD_OnAttachDone_ResetEncoderAndUI**` 置 `**g_LightApply_UIContextPtr+0x12=1**`；`**get_xrefs_to` `0x2000001e**` 仅 **一** 处 **WRITE**；`**0x0800CA68**` 标签 `**flash_literal_g_light_apply_ui_ctx_ptr**`（与 `**0x0800D1C8**` 同字面量）；Ghidra `**PRE_COMMENT**` @ `**0x0800C974**`，`save_program`；任务书 **§4 Handoff**、**§6 OI-005**。
- 2026-04-07：**OI-005 / Input** — `[03` §1.5.2](03_Function_Modules.md)：**SRAM `0x20000020**` 五行表、`**Init_TMR3_PB5_AndTickSyncLoop**` 注册 `**Input5Channel_SetRowPrimaryCallback**`；池 `**0x0800CCF4`…`CD08**`；Ghidra `**FUN_0800d70a`→`Input5Channel_SetRowPrimaryCallback**`、`**Init_TMR3**` `**PRE_COMMENT**`、书签 `**Input` @ `0x0800CC90**`，`save_program`；任务书 **§5**、**§4 Handoff**。
- 2026-04-05：**Cursor 计划执行** — **OI-005**：Ghidra `**create_function**` `**ProtocolLane_Row3_Entry_NopThenOrMergeVtable**` / `**ProtocolLane_Row4_BranchToOrMergeVtable**` / `**ProtocolLane_Row1_BranchToOrMergeVtable**`，`**PRE_COMMENT**` + 书签 `**Protocol` @ `0x0800CBD4**`；`[03` §1.5.2](03_Function_Modules.md) 闭合表同步。**OI-001**：工作区 **无 `.map**`；`**get_xrefs_to` @ `0x080012E8**` 复核；`[04` §1.3.6](04_Protocol_Reverse.md) 增补 **map 负向** 与 **xref** 行；任务书 **§5**「**2026-04-05（计划执行闭环）**」；`**save_program**`。
- 2026-04-08：**续分析计划落地** — `**Scatterload_DecompressRegionOrMemcpy**`（原 `**FUN_080012E8**`）、`**HSI_CommitPendingRGB_ToShadow**`、`**CCT_Context_***`；`[04` §1.3.6](04_Protocol_Reverse.md) **OI-001 验证优先级**；`[05` §6.4](05_Full_Reconstruction.md) **最小可复现实验**；`[03` §1.5.1](03_Function_Modules.md) `**UI_ApplyEventCode` 尾链**；任务书 **§5**、**§6 OI-001** 行；`**save_program**`。
- 2026-04-10：`**.map` 项目约束** — 新增 `[04` §1.0](04_Protocol_Reverse.md)：标注 **无法**从 **原始工程构建** 或 **已保存的构建产物** 取得链接器 `**.map**`；同步 `**04` §1.3.4–§1.3.8** 表中 **①**、**OI-001** 依赖列（**§6**）、**§4** 里程碑与 Handoff、`[05` §6.1/§6.4](05_Full_Reconstruction.md) 风险与实验表述。
- 2026-04-10：**终局路线与 Phase 6** — **§1** 增补 **可编译重构 → 还原度验证 → 二次固件开发**；**Ghidra 符号/类型收敛**优先级与口径；**源码输出职责表**（MCP 伪代码 vs 用户 Export vs SDK 手写工程）；**§2** 增加 **Phase 6** 行；**§3** 增加 **Phase 6** 清单；**§7** 增加 Phase 6 索引；**§4** 里程碑一条。
- 2026-04-06：**§4 优先项 4（OI-005）** — `[03` §1.5.5.3](03_Function_Modules.md) 增 **说明书 §二 图示 + §三** ↔ `**+0x12**` ↔ **五选一池** 一页表；`**0x0800D668**` 与 **五选一池** 职责 **书面分离**；Ghidra MCP 复核 `**0x0800D668→0x20000184**` 与 **xref×4**；**§6 OI-005**、**FINDINGS** 同步。
- 2026-04-06：**Phase 6 入口文书** — `[05](05_Full_Reconstruction.md)` 冻结 `**05-MMIO-v1**`（**§1–§3**+**§5**）；**§1** 增三条 **文书核查状态**；**§4** 增 **Phase 6 入口条件进度** + **Ghidra ①② 书签对照**（`list_bookmarks`）；**§6** 增 **开放项书面取舍** 表；**§2** Phase 6 行注明 **准备 vs 工程未建**。
- 2026-04-05：**任务书「按书续作」** — **OI-005**：Ghidra `**UI_OLED_MainState_SubstateScratch**` / `**LightApply_UIContext**` + `**flash_literal_ptr_*` @ `0x0800DB48`/`58**` + SRAM `**apply_data_type**`；`**UI_OLED_MainStateMachine` PRE_COMMENT**；**§5** 新小节；`**03` §1.5.7** 与清单 **[x]**；`**save_program**`。
- 2026-04-05：**逆向续推计划（会话）** — Ghidra `**UI_DrawAsciiTripleColumns_14_23_32**`、`**Init_LightEngine_ClockTreeSequence_OrHalt**`、`**RCM_BlockingApplyDescriptor_***` / `**LightEngineSeq_ReadTickU32**` / `**RCM_LookupScaledDivider_FromCfgBits**`；`**scatterload_rw_decompress_entry**` **PRE_COMMENT**（OI-001）；Renode+GDB `**target remote localhost:3333**` 实验与 `**continue` 后寄存器读负向**（后由脚本化路径替代）；任务书 **§5** 新增 **2026-04-05（逆向续推计划）** 条；`**save_program**`。
- 2026-04-05：**Renode + 本机 GDB 脚本入库** — 新增 `[scripts/renode_gdb_capture.sh](../scripts/renode_gdb_capture.sh)`、`[scripts/renode_oi001_capture.gdb](../scripts/renode_oi001_capture.gdb)`；任务书 **§4 Handoff**、**§7** 增 **「运行时采集脚本」** 索引。**2026-04-05（续）**：采集产物改入 `**logs/renode-oi001/<时间戳>/**` + `**LATEST**`，**§8** 同步为查阅该目录。
- 2026-04-05：**USB 控制传输 + AES 表** — Ghidra 重命名 `**Memset32_ExpandBytePattern**`、`**GF256_InitExpLogTables_AesField**`、`**AES_BuildSboxFromGFInverse**`、`**AES_InitWorkspaceTables**`、`**AES_InitRoundConstantsFromFlashTemplate**`、`**AES_DecryptTransformBlock16_InPlace**`、`**UsbCtl_RequestDispatch_StateMachine**`；`**InitializeUsbDescriptorsAfterPwmInit**` / `**UsbCtl_RequestDispatch_StateMachine**` **PLATE**；书签 `**USB` @ `0x0800370C**`；`[05_Full_Reconstruction.md](05_Full_Reconstruction.md)` `**main` 链表行** 修正；任务书 **§5** 新发现条；`**save_program**`。
- 2026-04-06：**文档结构** — 原 **§5 关键发现记录区** 全文迁至 `[REVERSE_TASK_BOOK_FINDINGS.md](REVERSE_TASK_BOOK_FINDINGS.md)`；任务书 **§5** 改为索引段；**§7/§8** 同步指向新文件；`[PROJECT_INDEX.md](../PROJECT_INDEX.md)` **§5** 增录该附录。
- 2026-04-06：**Phase 6 文书审计回合** — `[04` §1.3.16](04_Protocol_Reverse.md) **OI-001 一页摘要**；`[04` §2.4.1](04_Protocol_Reverse.md) **23 B MCP 复核**；`[03` §1.5.3.3](03_Function_Modules.md) `**UI_ModeRender_Dispatch` 入口 `r1**`；Ghidra `**UI_ModeRender_Dispatch` PLATE** + `**save_program**`；**§4 Handoff**/**§6 OI-005**/**FINDINGS** 同步；`**list_bookmarks` 113** 与 §4 表 **112** 记 **±1**。
- 2026-04-06：**OI-005 续（按计划）** — `[03` §1.5.3.2.1](03_Function_Modules.md)：`**ProtocolLane_OrMergeR0R1**`（`**orrs` @ `0x0800CBD6**`）+ `**ProtocolLane_VtableDispatch_FromPtr20000184**`（`**mov r1,r0` @ `0x0800D5C0**`）→ `**UI_ModeEvent_*` 第二实参 `r1 ≡ merged**`；`**merged = tag|param_2**`（`**tag**`=`uxtb(ldrh(0x20000020+row×0x14))`**）。Ghidra `set_decompiler_comment` @ `0x0800CBD6`/`0x0800D5B4`，`**save_program`**；任务书 §4 优先项 2/4、**§6 OI-005** 行更新。
- 2026-04-06：**§4 优先任务 ①（续）** — `[04` §1.3.17](04_Protocol_Reverse.md) **Entry1**（`**0x20000000`/`0x54C`**）**handler `0x08006964`** 汇编级（`**r1`/`r2`/`r3` 环**）；**§1.1/§1.2/§1.3.16** 同步；Ghidra `**Scatterload_TableEntry1_DecompressToSRAM_20000000_54C`** + `**Scatterload` 书签 @ `0x08006964**`，`**save_program**`；**§4「本轮优先任务」①** 划闭；**FINDINGS** 追加。
- 2026-04-06：**工作模式与 Phase 6 验收** — 确立 **Phase 6 为主、遇阻再逆向**；增补 **§1「Phase 6 验收口径与当前工作模式」**（**行为等价** 主验收；**USB 固件升级** 第一期不测；**插 USB·PD 诱骗充电** 必验）；**§2** 看板、**§3 Phase 6** 清单、**§4** 里程碑与优先任务、**§7/§8** 同步；**§1** 终局 **还原度验证** 条注明第一期以行为为主、二进制次要；**Ghidra** 小节改为 **Phase 6 需求牵引**。
- 2026-04-06：**Phase 6 工程落库** — `[ZHIYUN_F100_Firmware/zhiyun_f100_apm32/](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/)` **PlatformIO**（**ST-Link V2**、**Geehy `apm32f103xb_flash.ld`**、`**src/geehy_sdk/**` 相对链至 `**SDK/APM32F10x_SDK_V2.0.0**`，`**[scripts/link_geehy_sdk.sh](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/scripts/link_geehy_sdk.sh)**` 再生）；**§7 Phase 6** 索引由「待建立」改为上表路径。
- 2026-04-06：**Phase 6 执行回合 B** — 完成 `main` 启动注入收敛（默认硬件输入、可选 bootstrap 宏）、`PD_PowerAndProtocol_Block` 阶段语义与 3000-tick helper 门控、CCT/HSI 计算可维护化、USB 升级 guard 读数接口与验收报表字段扩展；`pio run` 与 `phase6_ui_event_router_host_test` 均通过；本回合未触发 Ghidra/动态仿真补缺阻塞。详见 `[REVERSE_TASK_BOOK_FINDINGS.md](REVERSE_TASK_BOOK_FINDINGS.md)` 2026-04-06 新条。
- 2026-04-06：**Phase 6 收口计划（文书一致性）** — 修订 §3/§4：将“行为验收”明确为**软件门槛已通过、实机闭环待证实**；更新「下一项 / 建议下一刀 / 本轮优先任务」为 `**PH6-BHV-005` 动态补证 + 次要二进制对照**；同步 `phase6_behavior_acceptance_checklist` / `phase6_mmio_mapping` / `phase6_secondary_dev_baseline` 增加 **G1/G2/G3 门禁分级**。
- 2026-04-06：**外部实测条件** — 在 **§1** 增补 **「外部实测条件约束与阻塞项处理原则」**：明确 **仅万用表、无示波器**，**不能**测 PWM/波形；**因缺外部测量而阻塞的项** 改为 **仅反汇编/逆向（及 Renode/GDB）推进，不等待外部测量**；同步 **§3/§4/§6 OI-004/OI-006** 与 **§8** 表述。
- 2026-04-06：**Phase 6 收口计划（动态补证 + 回归断言）** — 增强 `[scripts/renode_phase6_pd_breaktrace.sh](../scripts/renode_phase6_pd_breaktrace.sh)`（`PHASE6_PD_STEP_INSN`、`PHASE6_TRACE_HELPER_BP`、entry/post-step 双快照）；在 `logs/renode-phase6/20260406_144126/` 观测到 `pd_gpio_count` 与 `PB3/PB11` 影子 **0→1**。`phase6_scheduler_acceptance_host_test` 增加负向断言（初始化不误判、USB 注入前 `BHV-005` 不通过）。
- 2026-04-06：**Phase 6 收口计划（次要二进制对照）** — 完成原厂 `dump/ZHIYUN-F100-full.bin` 与构建产物 `firmware.bin` 的第一轮对照：向量表入口、镜像长度、23B 启动 SPI 常量命中；结果已回写 Phase 6 文档，保持“第一期非门禁”口径。并复跑 `pio run` 与两项 host test，均通过。
- 2026-04-06：**Phase 6 计划执行（第一期收口）** — 复验 `pio run` 与两项 host test；复跑 `renode_phase6_pd_breaktrace.sh` 归档 `logs/renode-phase6/20260406_150719/`；`phase6_mmio_mapping.md` 增补 `05` §2 与 `PD_PowerAndProtocol_Block` 顺序对照；`phase6_behavior_acceptance_checklist.md` 增补万用表步骤与回合 M；`phase6_secondary_dev_baseline.md` 增补 USB 边界与 SPI/OLED 第二期说明；任务书 §2/§3/§4「优先任务」与 Phase 6 文档对齐。

