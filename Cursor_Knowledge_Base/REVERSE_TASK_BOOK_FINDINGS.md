# 智云 F100 逆向 — 关键发现记录（任务书附录）

> **主任务书**（阶段看板、当前进度、Handoff、OI、文件索引）：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md)  
> **定位**：由原任务书 **§5** 拆出，仅承载按日追加的「结论 / 证据路径 / 可信度」流水；**状态看板与开放问题仍以任务书 §2–§4、§6 为准**。

---

> 规则：每条记录包含「日期 / 结论 / 证据路径 / 可信度」

### 2026-04-07

- 结论：完成 **F407 侧车固件第一轮收敛**：修复 EXTI 事件归属歧义（由仅 `gpio_pin` 匹配改为 `port+pin` 精确匹配），降低同编号不同端口时的误记风险。  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/include/probe_engine.h`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_engine.c`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/main.c`  
  可信度：高

- 结论：新增 `SELF_CHECK` 协议命令，形成“未接线阶段”的软件层自检入口（引脚角色计数 + EXTI 监视列表 + 仅软件检查声明）；能力矩阵已同步。  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/probe_capability_matrix.md`  
  可信度：高

- 结论：完成 F407 相关文档清理与重写，去除乱码/重复段，并将阶段状态、对比用例、接线前后清单固化为可追溯文档。  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Firmware_Reverse_Guide.md`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_f407_compare_cases.md`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Reverse_Convergence_Status.md`  
  可信度：高

- 结论：本轮固化了 Ghidra MCP 快照（`ZHIYUN-F100-full.bin`，函数 `535`、书签 `114`）与 Renode 阻塞链路（scatterload/OI-001、`continue` 交互不稳、`hbreak` 超时、模型差异、SPI 状态位仿真缺口）作为后续验收前基线。  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Reverse_Convergence_Status.md`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md`  
  可信度：高

- 结论：修正 **F407 探针固件**与**接线规范**中 OLED 相关 IO 的冲突：将 `PE12` 的来源语义从 **`F103 PB6`** 调整为 **`F103 PA6`**（标注 *candidate/待 LA 验证*），避免与 **`PB6` 编码器 A 相**（`PG6`）产生“一脚两义”误导；接线规范同步将 OLED 可选增强线由 `PB6 -> PE12` 改为 `PA6 -> PE12`。  
  证据路径：量产固件 SPI 选线函数 `GPIO_SPI_SelectLine_0x40` @ `0x080039EE`（PB6/PA6 分支）、`SPI1_SendConstBlockWithCS` @ `0x080041E6`、`SPI1_SendBlockWithCS_GPIOAlt` @ `0x08004208`（Ghidra MCP 反编译）；文件变更：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_pins.c`、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/APM32F103CB_to_STM32F407V_Wiring.md`；背景引用：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`（`PA6` 运行期 SPI 选择线证据）、`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Zhiyun_F100_Repair_Notes.md`（屏座连通含 `PA6`）。  
  可信度：高（固件路径 + 文档/代码一致性）；中（`PA6` 是否为严格意义 `D/C` 仍需 LA 定名）

### 2026-04-04

- 结论：启动主链已锁定到 `main` 候选入口，向量表与 Cortex-M3 结构一致  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/reverse_log.md`  
  可信度：高

- 结论：SPI1 初始化、CS 拉低拉高时序、发送封装调用关系已具备寄存器级证据  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`  
  可信度：高

- 结论：ADC1 多通道采样 + 阈值判定状态机路径已证实  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`  
  可信度：高

- 结论：未发现 I2C1/I2C2 基址字面量路径，暂不能排除 GPIO bit-bang 或外部控制  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`  
  可信度：中

- 结论：`.data/.bss` 初始化由 **`scatterload_table_iterate`** @ **`0x08006B20`**（`__scatterload` 表驱动）驱动；表区间 **`[0x08006B6C,0x08006B8C)`**；第二表项对应 **ZI 清零 `0x54C` 字节 @ `0x20000000`**；第一项 RW 与链接器三元组需 map 进一步闭合；**外层循环寄存器/池字** 见 **`04` §1.3.15**  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/04_Protocol_Reverse.md`；Ghidra 书签 `Startup` @ `0x08006B20`/`0x08006B6C`  
  可信度：高（区间与 ZI）；中（RW 源址精确值）

- 结论：启动 **23** 字节 SPI 常量（`0x080041B0`）已落盘 hex；依据项目规格书 **排除** HUSB238（I2C）、MP3398E（PWM）等为主要目标；真实从设备需原理图/LA  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/04_Protocol_Reverse.md` §2；[`husb238规格书.md`](../Document/Markdown/husb238规格书.md)  
  可信度：高（字节与排除）；低–中（器件型号）

- 结论：`FUN_08009F84` 读 **GPIOC IDR**（**PC13**）；`FUN_08009F90` 写 **GPIOB BSRR/BRR**（**PB3**）；`FUN_0800C4E0` 为以 `short` 状态字驱动的电源/PD 外围状态机，与 I2C 访问 HUSB238 **无直接同一证据链**  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/04_Protocol_Reverse.md` §3；Ghidra 书签 `PD_GPIO` @ `0x0800C4E0`  
  可信度：高（寄存器）；中（与 HUSB238 引脚对应需原理图）

- 结论：**`PD_WriteGPIOB11_BSRR_BRR`**（原 `FUN_0800A1C0`）亦经 **GPIOB BSRR/BRR** 驱动 **`0x800` → PB11**（与 `FUN_08009F90` 的 `0x8`/PB3 **不同线**）；`FUN_0800C4E0` 阈值 **`0x65`/`0x15`/`0x7d1`/`0xD17`** 已与反编译对齐  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/04_Protocol_Reverse.md` §3；Ghidra `decompile_function` @ `0x0800C4E0`、`0x0800A1C0`  
  可信度：高

- 结论：运行期 SPI 为 **`FUN_08004258`**：**8 通道 ×（栈上 3 B 前缀 + `0x60` B 载荷）**，`PA6` 为第二片选线；**仅启动**走 `0x080041B0` 共 23 B  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md`；`decompile_function` @ `0x08004258`、`0x08004184`；程序 `ZHIYUN-F100-full.bin`  
  可信度：高

- 结论：**`TMR1`**（**`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`** @ **`0x0800DD28`**，由 **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** @ **`0x0800ECEA`** 调用）、**`TMR4`**（`FUN_0800de2e`）与 **`DAT_08004158`→`0x20004860`** 控制块、`FUN_08004092`→DMA 设置链已建立寄存器级证据；`Init_PWM` 前置 **`FUN_08004a6a`** 实为 **Flash FMC 解锁/控制**（`0x400220xx`），非 TMR 寄存器  
  证据路径：`/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Cursor_Knowledge_Base/02_Hardware_Init.md` §Timer；`decompile_function` @ `0x080040BC`、**`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）、`0x0800de2e`  
  可信度：高（外设址）；中（亮度调制与 `main` 闭环）

- 结论：全镜像 **32 位小端**搜索 **`I2C1`/`I2C2` 基址字面量** 命中 **0** 次（与先前结论一致）  
  证据路径：本地脚本扫描 `dump/ZHIYUN-F100-full.bin`；[`02_Hardware_Init.md`](02_Hardware_Init.md) §I2C  
  可信度：高

- 结论：散表 **Entry0** handler 解析为 **`0x0800692A`–`0x0800695A`** 模板；表参 **`0x08006B70`** 给出 **SRAM `0x20001620`**、**长度 `0x32DC`**；**`FUN_080012E8`/`FUN_080013C8`** 与 **`0x08006964`** 解压环共同完成 RW 映像恢复，**非**单次 `memcpy`；**压缩流 Flash 单块源址**仍无 map 闭合  
  证据路径：离线 Capstone 反汇编 [`dump/ZHIYUN-F100-full.bin`](../dump/ZHIYUN-F100-full.bin)；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §1.3  
  可信度：高（目的/解压语义）；中（Flash 源块）

- 结论：**`FUN_0800C4E0`** 仅被 **`bl` @ `0x0800CC0C`** 调用（包装器 **`FUN_0800CBFC`**）；**`b.w @ 0x0800F0C6` → `FUN_0800CC3C`**，且 **`0x0800F0B0`** stub 登记于 Flash **`~0x0800803C`** 向量表区 → **中断驱动周期路径** 与 PD 状态机耦合（非仅 `main` 顺序调用）  
  证据路径：`bl`/`b.w` 扫描同一 bin；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.5  
  可信度：高（地址与指令）；中（IRQ 序号命名）

- 结论：启动 SPI 23 字节 **完整 hex** 以固件为准 **无尾 `00`**；**`0x08004184`** 用 **`addw r0,pc,#0x18`** 精确指向 **`0x080041B0`**  
  证据路径：`xxd` / Capstone @ `0x08004184`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.1  
  可信度：高

- 结论：本轮 **Ghidra MCP `list_instances` 为空**（无本地桥进程），分析改用 **离线 bin + Capstone**；Ghidra 书签/重命名 **未写入** `.rep`，待桥可用后补做  
  证据路径：MCP 返回 `No running Ghidra instances`；用户可启动 Ghidra 桥后执行 `set_bookmark` / `save_program`  
  可信度：高（会话事实）

- 结论：**Ghidra MCP 已恢复**：`list_instances` 见工程 **`ZY_F100`**、程序 **`ZHIYUN-F100-full.bin`**；已 **`set_bookmark`**（`Startup`：`0x08006B6C`、`0x0800692A`；`Protocol`：`0x080041B0`；`PD_GPIO`：`0x0800CBFC`、`0x0800F0B0`）并 **`save_program`** 成功；反编译确认 **`scatterload_table_iterate`** @ `0x08006B20`、**`PD_GPIO_StateMachine`** @ `0x0800C4E0`  
  证据路径：MCP `get_current_program_info` / `decompile_function` / `set_bookmark` / `save_program`；项目 [`ZY_F100`](ZY_F100)  
  可信度：高

- 结论：23 B SPI 常量 **`0x080041B0`** 的 **xref** 仅 `SPI1_StartupSequence` 参数位；**`SPI1_StartupSequence` 仅 `main` 调用**；**`SPI1_SendConstBlockWithCS` 第二调用点**为 `FUN_0800422a`（3 B 栈块），与 23 B 无关  
  证据路径：MCP `get_xrefs_to` / `get_function_callers`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.1.1  
  可信度：高

- 结论：**第二向量表** `0x0800803C` 字 **`0x0800F0B1`** → **`0x0800F0B0`** SysTick stub；**`FUN_0800CC3C`** 重命名为 **`PD_PeriodicDispatchFromSysTick`**（每 20 tick 调 `PD_PowerAndProtocol_Block`；`>599` 分支同任务书）；书签 **`SysTick`** @ `0x0800803C`、`0x0800F0B0`；数据标签 **`SPI1_boot_blob_23`** @ `0x080041B0`；已 **`save_program`**  
  证据路径：`read_memory` / `decompile_function`；MCP `rename_function_by_address` / `create_label` / `set_bookmark` / `save_program`  
  可信度：高

- 结论：全镜像 **32 位小端**外设基址扫描：**I2C1/2、USART1–3、SPI2 = 0**；**`0x40005C00` 1 命中**；**SPI1/GPIOB/GPIOC** 与既有结论一致（详见 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.7）  
  证据路径：Python 扫描 `dump/ZHIYUN-F100-full.bin`  
  可信度：高（计数）；USART 零命中解释见 §3.7

- 结论：维修手册 **[§6 MCU 引脚定义](../Document/Zhiyun_F100_Repair_Notes.md)** 现为 **扩展版**（含 **OLED/SPI、PA8/9 W&C_PWM、PB15 ADIM、PB5 MO_PWM、PB14 EN、PB4 按键5、PB6/PB7 编码器** 等）；**PD 三针** 与 **`PD_ReadGPIOC13_IdrBit`/`PD_WriteGPIOB3_BSRR_BRR`/`PD_WriteGPIOB11_BSRR_BRR`** 仍 **高一致**；**PA8/PA9/PB15** 与 **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）掩码 **高一致**；**PB6/PB7** 与「I2C1 接 HUSB238」仍 **不相容**（OI-003）  
  证据路径：[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8  
  可信度：**高–中**（按行）；HUSB238 走线 **中**

- 结论：**万用表蜂鸣档**：**HUSB238** 的 **SDA/SCL** 与 **MCU 任意引脚** **未测得直接连通**；整机 **PD 为诱骗 20V** 档之逻辑（用户描述）与固件 **GPIO PD 状态机** 相容，**不由**本 MCU **I2C 访问 HUSB238**（与无 I2C 字面量、上条结论一致）  
  证据路径：用户测量；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.1.1  
  可信度：**高**（实测）；串联电阻/副板走线仍可能使蜂鸣档**假阴性**（见 §3.1.1 限制）

- 结论：**`PD_PowerAndProtocol_Block`**（`0x0800CBFC`）除 **`PD_GPIO_StateMachine`** 外，尚含 **`FUN_0800c156`**（多段延时协议）、**`FUN_0800d74e`**（5 路输入机）、**`FUN_0800d89c`**（大型 **OLED/UI** 状态机）、条件 **`FUN_0800d3d2`/`FUN_0800c8c0`** 等；**非**单一 GPIO 状态机  
  证据路径：Ghidra MCP `get_function_callees` / `decompile_function`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.9  
  可信度：**高**

- 结论：**USB Device** 路径成立：**`FUN_08001fbc`** 对 **`0x40005C00`** 实参执行 **RCC USB 时钟门控**；**`FUN_0800207c`** 初始化 USB 寄存器块与描述符；**`FUN_0800405c`→`FUN_08002b18`** 建立上下文；**`FUN_0800370c`** 经 **`FUN_08004144`** 与 **`DAT_08004158`→`0x20004860`** PWM 控制块交互（升级/DFU 与灯控共享数据面）  
  证据路径：MCP `decompile_function` @ `0x08001fbc`、`0x0800207c`、`0x0800405c`、`0x0800370c`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.9  
  可信度：**高**

- 结论（2026-04-05 续）：**`InitializeUsbDescriptorsAfterPwmInit`** 在 **清零 `+0x20` 共 16 B** 后调用 **`AES_InitWorkspaceTables`**：内含 **`GF256_InitExpLogTables_AesField`**（**×2 在 GF(2⁸)** 与 **`0x1b` 约化**）、**`AES_BuildSboxFromGFInverse`**（**`xor 0x63`**）、**`AES_InitRoundConstantsFromFlashTemplate`** — 与 **标准 AES S 盒预计算** 形态一致；**`UsbCtl_RequestDispatch_StateMachine`**（原 **`FUN_0800370c`**）**case 7** 自映像 **`0x08008000+uVar5`** 经 **`Scatterload_MemcpyBytes`** 取块，**16 B 一步** 调 **`AES_DecryptTransformBlock16_InPlace`**，再 **`FUN_08004144`** 写入；**IWDT** 在环内喂狗。故 **`05` 中「仅 USB 描述符/缓冲区清零」** 对 **`InitializeUsbDescriptorsAfterPwmInit`** **不完整**，应并列 **AES 表初始化**（服务 **加密固件下发**）。Ghidra：**`Memset32_ExpandBytePattern`** 等重命名 + **`UsbCtl_RequestDispatch_StateMachine`** **PLATE** + 书签 **`USB` @ `0x0800370C`**，`save_program`。  
  证据路径：MCP `decompile_function` @ `0x0800410E`、`0x08004B40`、`0x08004B70`、`0x08004F06`、`0x08004F3C`、`0x0800370C`；Ghidra `rename_function_by_address` / `set_plate_comment` / `set_bookmark` / `save_program`  
  可信度：**高**（GF/AES 指纹）；**中**（完整轮数/Nr、密钥派生细节未闭合）

- 结论：**`FUN_0800AD5C`** 重命名为 **`PD_SetDoneFlagHalfword`**；**`EXTI`/`AFIO` 基址字面量** 全镜像 **0 命中**；**用户万用表** 后：**PB4=右键** 与 **`FUN_0800a16c(1)`** **一致**；**编码器 A/B=PB6/PB7** 仍须 **另寻固件读路径**（**OI-006** 部分闭合）  
  证据路径：MCP；Python 扫描；用户测量；[`02_Hardware_Init.md`](02_Hardware_Init.md) §编码器  
  可信度：**高**（代码）；**硬件：高**

- 结论：**TMR1** 初始化 **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）中 **`FUN_0800a340`** 掩码 **`0x300`/`0x8000`** 对应 **PA8/PA9** 与 **PB15** 复用线；与用户 **§6** 之 **W_PWM/C_PWM（PA8/PA9）**、**ADIM（PB15）** **一致**（**OI-004** 引脚级加强）  
  证据路径：MCP `decompile_function` @ **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）；[`02_Hardware_Init.md`](02_Hardware_Init.md) §Timer；[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6  
  可信度：**高**（掩码+手册）；**亮度语义** 仍 **中**

- 结论：用户 **修订 §6 MCU 引脚定义**（扩展 **OLED/SPI、PWM 测试点、PB14 EN、PB5 MO_PWM、PB4 按键5、PB6/PB7 编码器** 等）；知识库已同步 **[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8** 分表、**OI-004/OI-006** 表述  
  证据路径：[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6；Git/工作区 diff  
  可信度：**高**（文档为用户主控）；与固件 **部分已闭合、部分仍 OI**

- 结论：**万用表**：**编码器插座 A/B 分别通 PB6/PB7**；**右键通 PB4**；**§6** 写明 **C 相接地**（公共端）。与 **`FUN_0800a16c(1)`→PB4** **一致**；**PA1** 非编码器 A → **旋转增量** 仍从 **PB6/PB7** 其它固件路径追（**OI-006** 软件半开）  
  证据路径：用户测量；[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6 第19–20条；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8.2  
  可信度：**高**（实测 + 文档）

- 结论：**OI-006 软件路径闭合**：**`TMR4`**（`0x40000800`）**编码器模式**（**`TMR_ConfigureEncoderMode_SMCR_CCMR`**，`SMS=3`）；**`Init_TMR4_EncoderQuadrature_PB6PB7`** @ `0x0800DE2E`；**`Encoder_DispatchDelta_FromTMR4`** @ `0x0800D3D2` 读 **`TMR4->CNT`**、死区 **`0x65`**，**唯一调用**自 **`PD_PowerAndProtocol_Block`**。**`Input_SamplePA1PB4_As2bit`** 仍为 **PA1+PB4** 2bit，与旋钮 **并行**。  
  证据路径：Ghidra MCP `decompile_function` / `get_xrefs_to` `0x40000824`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8.2；[`02_Hardware_Init.md`](02_Hardware_Init.md) §旋转编码器  
  可信度：**高**

- 结论：**OI-002 静态收窄**：**`SPI1_SendConstBlockWithCS`** 先 **`GPIO_SPI_SelectLine_0x40`**（**PB6**）再 **PB10 CS**；与 **§6 OLED/SPI** **相容**；型号仍待 LA。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.5  
  可信度：**高**（时序）；**中**（器件）

- 结论：**Phase 4（已由下条 supersede）**：**`FUN_0800C156`** 魔数子状态与 tick 初摘录；现 §3.10 已命名 **`PD_AttachSequence_StateMachine`** 并闭合 **PD GPIO** 子链（**非** ADC 主路径）。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.10  
  可信度：**高**（结构）；热/流 **仍以 `PD_GPIO_StateMachine`+ADC 为主**

- 结论：**`PD_AttachSequence_StateMachine`**（`0x0800C156`） callee 链已闭合：**`PD_GPIO_ReadPc13ToggleApplyPb3`** 即 §3.1 **PC13/PB3** 路径；**`PD_OnAttachDone_ResetEncoderAndUI`** 调 **`TMR4_EncoderReset_ClearStateAndReinitQuadrature`**（`0x0800D38C`，原 `FUN_0800d38c`）复位 **TMR4 编码器**；**无 ADC1 字面量** — **非**热/流保护主链（与 §3.2 **并列**）  
  证据路径：Ghidra MCP `decompile_function` / `rename_function_by_address` / `set_bookmark` / `save_program`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.10  
  可信度：**高**

- 结论：**OI-002 软件边界**：**`get_xrefs_to` @ `0x08004258`** 仅 **`main`** 与 **`SPI1_StartupSequence`**；**`FUN_0800d89c`** **不**直接调 SPI；**`UI_Framebuffer_CopyRect`** 仅写 **帧缓冲**  
  证据路径：MCP `get_xrefs_to` / `decompile_function`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.6  
  可信度：**高**

- 结论：**PB5（MO_PWM）** 对应 **`Init_TMR3_PWM_GPIOB5`**：**TMR3** `0x40000400`，**`FUN_0800a340` 掩码 `0x20`**；**外层** **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** 末尾调用 **`Init_TMR3_PB5_AndTickSyncLoop`**，其内先 **`Init_GPIO_Then_TMR3_PB5_PWM`**（**`Init_TMR3_PWM_GPIOB5`** 链）；与 **TMR4 PB6/PB7 编码器** 外设独立  
  证据路径：`decompile_function` @ `0x0800dc92`、`0x0800d006`；Python 字面量扫描；[`02_Hardware_Init.md`](02_Hardware_Init.md) §Timer  
  可信度：**高**（寄存器+掩码）；**中**（PWM→风扇语义）

- 结论：**OI-001**：全镜像 **唯一** `0x200002C4` 字 @ Flash **`0x0800696C`**；**`scatterload_rw_decompress_entry`** 调 **`FUN_080012E8`/`FUN_080013C8`**；**`FUN_080008EC`** 重命名为 **`Scatterload_FindRegionNode`**（region 树查找）；**OI-001** 单块 Flash 源仍开放  
  证据路径：Python 扫描 `dump/ZHIYUN-F100-full.bin`；Ghidra MCP；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §1.3.1  
  可信度：**高**（字面量唯一性）；**中**（压缩源边界）

- 结论：**OI-002**：23 B 与 **SSD1306** 命令 **`AE`/`81 FF`/`A8 3F`** 等 **高度同构**；首选 **板载 SSD1306/兼容 OLED**（§2.4/§2.5 已更新）  
  证据路径：datasheet 对照；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.4  
  可信度：**中–高**（opcode）；**中**（贴装）

- 结论：**`PD_GPIO_StateMachine`** 内 **ADC** 仅 **`ADC1_AverageSamples(3,4)`**，**无** ADC1 MMIO 直访；**`FUN_0800d89c`** Ghidra 重命名为 **`UI_OLED_MainStateMachine`**；Phase 3 产出 [`03_Function_Modules.md`](03_Function_Modules.md)  
  证据路径：Ghidra `decompile_function`；[`02_Hardware_Init.md`](02_Hardware_Init.md) §ADC；[`03_Function_Modules.md`](03_Function_Modules.md)  
  可信度：**高**

- 结论：**OI-005 加深**：**`UI_ModeRender_Dispatch`**（`+4` **vtable** 调用）、**`UI_ModeBitmap_CopyRows`**（`FUN_0800a244` 条带拷贝）、**`UI_ModeActiveContext_Clear`** / **`UI_ModeRender_PostProcess`** 形成 **绘制子链**；池字 **`0x080081C8`/`0x080082CC`** 抽样为 **点阵数据**；**模式字 2–7** 与说明书 **CCT/HSI/FX** **推测对应** 见 **`03` §1.5**  
  证据路径：Ghidra MCP `decompile_function` / `read_memory` @ `0x0800DB50`；[`03_Function_Modules.md`](03_Function_Modules.md) §1.5；[`ZHIYUN_F100_官方说明书.md`](../Document/Markdown/ZHIYUN_F100_官方说明书.md) §二–三  
  可信度：**高**（调用形态）；**中**（模式↔UI 语义）

- 结论：**OI-001** 再收紧：**`read_memory` @ `0x080014A0`** 列出 **SRAM 指针池**（`0x20000030`…`0x20000048`）；**`FUN_080012E8`/`FUN_080013C8`/`FUN_08001244`/`FUN_08000f3c`** 使用 **不同 handler 偏移**（`+0x18`/`+0x1c`/`+0x14`/`+0x10`）；**`FUN_0800154C`** 为 **区间数学** 非 Flash 地址；**`FUN_080068f8`** 为 **并行解压入口**（静态 **无 xref**）  
  证据路径：Ghidra MCP `decompile_function` / `read_memory`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §1.3.2；书签 **`Scatterload` @ `0x080014B0`**，`save_program`  
  可信度：**高**（池与 handler）；**中**（`FUN_080068f8` 可达性）

- 结论：**Phase 3**：**`Input_5Channel_TableStateMachine`**（`0x0800D74E`）、**`PD_HelperSeq_3000tick_ThenPc13Gpio`**（`0x0800C994`）、**`ProtocolLanes_BitExpandTick`**（`0x0800C692`）反编译并 **重命名**；**`Input_SamplePA1PB4_As2bit`∪`Input5Channel_ADC_IN8_BandClassify`** → **5 路表** 上游已叙述；**ADC1_AverageSamples** 仍 **仅四类 caller**，**无**独立「热敏」ADC 机  
  证据路径：Ghidra MCP；[`03_Function_Modules.md`](03_Function_Modules.md) §2；[`02_Hardware_Init.md`](02_Hardware_Init.md) ADC 节  
  可信度：**高**

- 结论：**PA1** 键掩码：**`DAT_0800a1fc`** 首项 **`GPIOA` + `IDR` 掩码 `0x2`**  
  证据路径：`read_memory` @ `0x0800A1FC`  
  可信度：**高**

- 结论：**PB14（EN）** — **固件路径已闭合（OI-004）**：**`MP3398_EN_SetHigh_BSRR_PB14`**（`0x0800A11A`）向 **`GPIOB_BSRR`** 写 **`mov.w r0,#0x4000`**（**非**池内 `0x00004000` 四字，故镜像扫描仍 **0 命中**）；**唯一调用** **`thunk_MP3398_EN_SetHigh_BSRR_PB14`** ← **`PD_OnAttachDone_ResetEncoderAndUI`**。**上电**：**`MP3398_EN_ADIM_InitLow_GPIOB_BRR`**（`0x0800A0DA`）经 **BRR** 先清 **PB14** 再清 **PB15**（**`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** 链）。Ghidra：**书签 `GPIO` @ `0x0800A11A`/`0x0800A0DA`**，`save_program`  
  证据路径：MCP `get_xrefs_to 0x40010C10` / `decompile_function` / `get_function_callers`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8.2；[`02_Hardware_Init.md`](02_Hardware_Init.md) §GPIO PB14  
  可信度：**高**（BSRR/BRR 指令级）；**中**（关灯/保护是否另有写线 — 可选 LA）

- 结论：**Phase 3 电源/灯光调度**：**`PD_PowerAndProtocol_Block`** 反编译 **顺序** 与 **§0** **mermaid** 已写入 [`03_Function_Modules.md`](03_Function_Modules.md)；**`FUN_0800c8c0`** Ghidra 重命名为 **`EncoderGated_LightOutputApplyChain`**（**`FUN_0800acfa`** + **`FUN_0800cd14`** + **`FUN_0800d044`**→**`FUN_0800dd22`** TMR1 通道写）  
  证据路径：MCP `decompile_function` @ `0x0800CBFC`/`0x0800C8C0`；`rename_function_by_address` / `set_decompiler_comment` / `set_bookmark` / `save_program`  
  可信度：**高**（调用图）；**中**（W/C/HSI 业务名）

- 结论：**OI-001 续推** — **`FUN_080016CC`** 为 **`dst,src,n` 字节拷贝**；**`FUN_080012E8`** 快路径 **`FUN_080016CC(DAT_080014b8,param_5,0x200)`** 与 **`FUN_08000f3c`** 内 **`FUN_080016cc(param_5,DAT_080014b8,0x200)`** 明确 **池指针 ↔ 工作缓冲** 方向；**仍无** 裸 Flash 比特流区间。**`Scatterload_ParallelDecompressEntry`**（`0x080068F8`）经 **`FUN_08000f3c`→`FUN_08001244`** 与主 **`scatterload_rw_decompress_entry`** 并列；**`get_xrefs_to`/`get_function_callers`/小端字面量 `f8680008`** 均为 **空**  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §1.3.3；Ghidra MCP `decompile_function` / `search_byte_patterns`；**`rename_function_by_address`** → **`Scatterload_ParallelDecompressEntry`**；书签 **`Scatterload` @ `0x080068F8`、`0x08000F3C`**，`save_program`  
  可信度：**高**（静态形态）；**中**（**OI-001** Flash 源块仍开放）

- 结论：**Phase 4 / PD 降额静态穷尽** — **`PD_GPIO_StateMachine`** 仅 **PC13/PB3/PB11 + `ADC1_AverageSamples(3,4)`**，**不含 PB14**；**`ADC1_AverageSamples`** 仍为 **4** 类 caller；**`GPIOB_BSRR`** `0x40010C10` 与 **PB14** 路径在 **`PD_GPIO_StateMachine` 外**（与 §3.8.2 **一致**）  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.11；MCP `decompile_function` @ `0x0800C4E0` / `get_function_callers` @ `0x0800CB7A` / `get_xrefs_to` @ `0x40010C10`  
  可信度：**高**

- 结论：**Phase 2（静态）** 标记 **100%**；**示波器/LA 待证** 项保留 **OI-004** / **Phase 5**，不阻塞本阶段  
  证据路径：任务书 §2 看板与 Phase 2 清单（2026-04-04）  
  可信度：**高**（口径定义）

- 结论：**OLED「型号」静态发掘** — 无 **规格书/丝印** 时，可收敛为 **SSD1306 族控制器 + 128×64 SPI**；**`DA 12`/`D9 22`/`A4`/`A6`/`AF`** 与 **开源 SSD1306 128×64 参考序列** **同寄存器语义**；**`AD 8B`** 与常见 **`8D 14` 电荷泵** 写法不同，**不**单独否定 **SSD1306 族**；**模组订货号** 仍 **需拆机 COG 丝印**；固件 **无** OLED 相关 ASCII  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.7；`strings`/`search_strings` `ZHIYUN-F100-full.bin`；Adafruit `Adafruit_SSD1306.cpp`（对照 **非** 产品证据）  
  可信度：**高**（控制器族+分辨率）；**中**（`AD 8B` 精确电气含义）；**低**（模组厂型号）

- 结论：**OLED 实物标识（用户）** — 玻璃喷码 **`V010015101302832`** / **`WV01000307420`**；FPC **`02832-MF1-B`**、**`16`** 线；正面 **`02830`**、**`3-12`**；玻璃非显示区 **22.3×18.8 mm**；主板座 **Pin16 地**、**Pin13→680 kΩ 下地**、多脚接电容；**屏侧 Pin16 未接**。与 **02832** 料号族 **一致**；**Pin13** 电阻用途 **待原理图**  
  证据路径：[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6.1；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.7.1–2.7.2  
  可信度：**高**（实测连通与尺寸）；**中**（喷码业务含义）；**低**（Pin13 网络名）

- 结论：**COG 顶标不可读（用户）** — **未** 观察到 **SSD1306 等** 芯片丝印；**芯片级** 型号 **无法** 由硬件目视闭合；**SSD1306 族 + 128×64** 仍以 **固件 23 B** 为主证据，**FPC/02832** 为 **辅证**  
  证据路径：[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6.1 COG 小节；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §2.7.3  
  可信度：**高**（观测事实）；**中**（推断仍依赖固件）

- 结论：**`EncoderGated_LightOutputApplyChain`** 子链已细化：**`LightOutput_PendingHSI_Dispatch`**（`0xB5` 子态 + **`DAT_0800ae18`**）、**`FUN_0800ae20`**（浮点 HSI→RGB）、**`FUN_0800abcc`**（**TMR2** 门控缓冲）、**`CCT_Slew_TableSplit_TMR1Shadow`**（CCT 表 + **`TMR1_PeriodHalfword_Write`**）、**`TMR1_CompareHalfword_Write`** / **`FUN_0800d044`**（ADC→比较半字）  
  证据路径：Ghidra MCP `decompile_function`；[`03_Function_Modules.md`](03_Function_Modules.md) §0.3.1；重命名 + 书签 **`Light` @ `0x0800C8C0`**，`save_program`  
  可信度：**高**（调用与寄存器影子写入形态）；**中**（光学/ RGB 协议细节）

- 结论：**OI-001** 在 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.4** 固定 **「静态已顶」** 与 **map / SRAM 转储 / 指令仿真** 三选一验证路径；与 §1.3.3 **无矛盾**  
  证据路径：`04` §1.3.4；本任务书 **OI 表**  
  可信度：**高**（方法论收口）

- 结论：**Phase 5** 初版 **`main` / `PD_PowerAndProtocol_Block`** 模块调用图已落盘（mermaid + 表）  
  证据路径：Ghidra MCP `get_function_callees`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md)  
  可信度：**高**（工具导出）

- 结论：**灯光子链** 再命名 **`HSI_FloatMath_ComputeRGBTriple`**、**`RGBTriple_PushPattern_TMR2Gated`**、**`EncoderGated_ADC_TMR1Compare_Apply`**；**`0x0800DED0`** 字面量池与 **`TMR1->CC1`/`CC3`、`TMR3->CC2`** 对齐（[`03` §0.3.2](03_Function_Modules.md)）  
  证据路径：Ghidra `read_memory` / `set_decompiler_comment` @ **`TMR1_CompareHalfword_Write`** / **`TMR1_PeriodHalfword_Write`**；SDK **`TMR_T`**  
  可信度：**高**（MMIO 偏移）；**中**（运行期 **`*DAT_0800ded0`** 选路）

- 结论：**全局指针** 重命名 **`g_LightApply_ADCContextState`**（Flash 首字 **`0x20000180`**）、**`g_LightApply_UIContextPtr`**（**`0x2000000C`**）、**`g_TMR_CCR_WriteTargetPtr`**、**`g_TMR_PeriodWriteTargetPtr`**；**`LightApply_ReadContextState0`** / **`LightApply_ReadContextState3`** 为 **状态字节** 薄读取  
  证据路径：Ghidra `read_memory` @ **`0x0800D1C4`**、`rename_global_variable`；[`03` §0.3.3](03_Function_Modules.md)  
  可信度：**高**（指针字面量）；**中**（结构体字段 **sizeof**）

- 结论：**`EncoderGated_ADC_TMR1Compare_Apply`** 内 **`pcVar1`** **字节偏移表**（**`+0`…`+0x30`**）与 **`LightApply_UpdateRampScratchFromADC`**（原 **`FUN_0800dee4`**）**子结构 `+0xC`…`+0x1C`** 已落盘；**`g_ADC_Average_u32`**（**`0x20000268`**）、**`g_SystemTickCounter_u32`**、**`g_LightModeAuxFlags_u16`** 命名；**`0x20000268`** 字面量 **镜像 4 命中**（`xxd`）  
  证据路径：[`03` §0.3.4](03_Function_Modules.md)；Ghidra `decompile_function` / `rename_*`；`xxd` **`dump/ZHIYUN-F100-full.bin`**  
  可信度：**高**（偏移与池）；**中**（字段业务名）

- 结论：**`LightApply_ClassifyADC_ToControlByte`**（**`ADC1_AverageSamples(4,6)`** = **IN4**、**6 次平均**）+ Flash 表 **`g_ADC_LightClassThresholdTable_u16`** @ **`0x0800D1DA`**；饱和返回 **`-1`** 与 **`EncoderGated`** **首字节 `0xFF`** 分支一致  
  证据路径：[`03` §0.3.5](03_Function_Modules.md)；Ghidra `decompile_function` / `rename_*` / **`PRE_COMMENT`**，`save_program`  
  可信度：**高**（指令与表）；**中**（**IN4** 与 **NTC/母线** 物理对应）

- 结论：**`ADC1_AverageSamples` 通道索引 → MCU 引脚** 已由 **`ADC1_SetChannelSampleTimeAndRegularRank1`**（`0x0800D50A`，原 **`FUN_0800d50a`**）**MMIO 偏移** 与 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) **`ADC_T.SMPTIM2`/`SMPTIM1`/`REGSEQ3`** **闭合**；**`Init_ADC_GPIO_AnalogChannels`** 掩码与 **IN3/IN4/IN8/IN9** **一致**；**`BatteryGauge_ADC9_UpdateFromSysTick600`**（`0x0800C2BA`）使用 **`ADC1_AverageSamples(9,4)`** → **IN9→PB1**（**`ADC1_AverageSamples` 第四类 caller**）。  
  证据路径：Ghidra MCP `decompile_function` @ `0x0800CB7A`/`0x0800D50A`/`0x0800CAA8`/`0x0800C2BA`；[`02_Hardware_Init.md`](02_Hardware_Init.md) §ADC；**`rename_function_by_address`** / **`PRE_COMMENT`** / **`set_bookmark`**（**`ADC`** @ `0x0800D50A`），`save_program`  
  可信度：**高**（寄存器 + 通道实参 + GPIO 表）；**中**（各 IN 的 **模拟前端** 分压网络）

- 结论：**PB1（19 脚）↔ 板级 `VBAT_ADC`**：[`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) **§6 第9条**（PB1→**VBAT_ADC** 测试点）、**§5 第17条**（测试点清单）；与 **`BatteryGauge_ADC9_UpdateFromSysTick600`**（**ADC1 IN9**）**闭合**。Ghidra **`BatteryGauge_ADC9_UpdateFromSysTick600`** @ **`0x0800C2BA`** **PRE_COMMENT** 已同步；`save_program`。  
  证据路径：维修笔记 §5–§6；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8.1；[`02_Hardware_Init.md`](02_Hardware_Init.md) §ADC caller 表；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5.1  
  可信度：**高**（硬件自述 + 固件通道）

- 结论：**Ghidra** 已 **`create_memory_block` SRAM `0x20000000–0x20004FFF`**（20KB）；**`LightApplyContext`**（**52 B**）应用于 **`0x20000180`**（与 **`03` §0.3.4** 偏移一致）；书签 **`LightApply`** @ **`0x20000180`**  
  证据路径：Ghidra MCP `run_script_inline`（Java 建结构体）、`apply_data_type`、`set_bookmark`、`save_program`；[`03_Function_Modules.md`](03_Function_Modules.md) §0.3.3–§0.3.4  
  可信度：**高**（`sizeof` 与反编译交叉）

- 结论：**`FUN_08009F84`/`FUN_08009F90`** 重命名为 **`PD_ReadGPIOC13_IdrBit`** / **`PD_WriteGPIOB3_BSRR_BRR`**；**`get_function_callers` 各仅 2**（**`PD_GPIO_StateMachine`**、**`PD_GPIO_ReadPc13ToggleApplyPb3`**）；修正 §3.5 旧表述「**仅** `C4E0` 路径」之歧义（见 **`04` §3.1.2**）。**`FUN_0800C40C`/`FUN_0800C2BA`** → **`PD_600tickAttachWindow_StateMachine`** / **`BatteryGauge_ADC9_UpdateFromSysTick600`**（**SysTick>599** → **ADC IN9** + **`UI_Framebuffer_CopyRect`**）。  
  证据路径：Ghidra MCP `get_function_callers`、`rename_function_by_address`、`set_decompiler_comment`、`save_program`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.1.2、§3.5.1、§3.8.1（**PB1**）  
  可信度：**高**（调用图）；**高**（**PB1→VBAT_ADC** 测试点，见 [`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) §6 第9条、§5 第17条）

- 结论：**`ADC1_AverageSamples`** 的 **`get_function_callers`（limit=100）** 穷尽为 **恰好 4** 处；**`FUN_0800D68E`** 重命名为 **`Input5Channel_ADC_IN8_BandClassify`**；**`ADC1_AverageSamples`** 增加 **PRE_COMMENT** + 书签 **`ADC` @ `0x0800CB7A`**；知识库新增 **`03` §0.3.6** 全表、**`02` §ADC** 穷尽性行、**`04`** 相关表项同步函数名  
  证据路径：Ghidra MCP `get_function_callers` / `rename_function_by_address` / `set_decompiler_comment` / `set_bookmark` / `save_program`；[`03_Function_Modules.md`](03_Function_Modules.md) §0.3.6；[`02_Hardware_Init.md`](02_Hardware_Init.md) §ADC  
  可信度：**高**（调用图与通道实参）

- 结论：**Phase 5** 增加 **[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5** — **MMIO/外设行为 ↔ 业务语义** 双表初稿（ADC1/TMR1/TMR3/TMR4/SPI1/GPIO/USB）  
  证据路径：`05` §5.1–§5.2  
  可信度：**高**（函数名与外设址）；**中**（用户语义列）

- 结论：**Phase 5** 续增 **`05` §5.3** — **`TMR_T` / `ADC_T` / `GPIO_T` / `SPI_T`** 与固件锚点 **对照表**（偏移来自 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h)）；Ghidra 书签 **`Phase5` @ `0x0800DED0`（CCR 池）**、**`0x0800D50A`（ADC 通道 helper）**，`save_program`  
  证据路径：`05` §5.3.1–§5.3.4；MCP `set_bookmark`  
  可信度：**高**（SDK 偏移）；**高**（与既有 `02`/`03` 结论一致）

- 结论：**OI-001** 在 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.4** 增补可检验假设 **`H-OI-001`**（**`FUN_080012E8`** 首次 **`+0x18` 槽** 调试/仿真与 Flash 源指针关系）  
  证据路径：`04` §1.3.4  
  可信度：**高**（方法论）；**待证**（仿真/断点实测）

- 结论：**`main` @ `0x08004092`** 实为 **USB Device 主循环数据泵**（**`USB_MainLoop_DataPump`**），**非** DMA1 硬件初始化；**DMA1 通道 1**（**`0x40020008`**）由 **`DMA1_Channel_ApplyCfgAndAddresses`**、**`Init_DMA1Ch1_TMR2CcrDMA`** 配置，**外设地址** **`TMR2+0x3C`**、**内存** **`0x20000A78`**，与 **`RGBTriple_PushPattern_TMR2Gated`** 及 **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** 初始化链闭合  
  证据路径：Ghidra MCP `decompile_function` @ `0x08004092`/`0x0800B2CC`/`0x0800DC1A`/`0x0800ABCC`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5.1、**§5.3.5**；`read_memory` @ `0x0800DEB8`/`0x0800DEBC`  
  可信度：**高**

- 结论：**TMR1 `BDT`（`+0x44` / `0x40012C44`）** 静态 **无** 直接引用；**`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**（`0x0800DD28`）不编程 **BDTR/MOE 死区互补**；冷暖 PWM 仍走 **`CC1`/`CC3`** 路径  
  证据路径：MCP `get_xrefs_to` @ `0x40012C44` **空**；`set_decompiler_comment` @ `0x0800DD28`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5.3.1 **`BDT`** 行  
  可信度：**高**（负向 + 初始化链）

- 结论：**Phase 5** 增补 **`05` §5.3.5**（**`DMA_Channel_T`**）、修正 **§1** 对 **`0x08004092`** 模块归类；Ghidra **`USB_MainLoop_DataPump`**、**`DMA1_Channel_ApplyCfgAndAddresses`**、**`Init_DMA1Ch1_TMR2CcrDMA`**、**`DMA_Channel_SetEnableCHCFG`**、**`DMA_Channel_SetCHNDATA`**、**`DMA1_IntSts_TestGatingBit`**；书签 **`Phase5` @ `0x0800B2CC`/`0x0800DC1A`/`0x08004092`**，`save_program`  
  证据路径：MCP `rename_*` / `set_bookmark` / `save_program`；`05` §5.3.5–§6  
  可信度：**高**

- 结论：**`main` 初始化链**（**`0x08002DF6`–`0x08003DE8`**）与 **USB/字模（`0x0800402C`–`0x0800429E`）** 已 **批重命名**（例：**`InitializeClock_PLLFlashSysTick`**、**`InitializeGPIOB_ClockAndPinMode`**、**`InitializeFlashPrefetchChain`**、**`EnableAHBPeriphClock_DMA1`**、**`SetUsbDeviceContextPointer`**、**`InitializeUsbDescriptorsAfterPwmInit`**、**`CopyFontGlyphRunsToWorkBuffer`**）；**`FUN_0800B1FC`** → **`ResetDmaChannelAndEnableAHB1`**  
  证据路径：Ghidra MCP `decompile_function` / `read_memory` @ **`0x08004FC8`**（**`RCM->APB2ENR`**）、**`0x08004FC4`**（**`RCM->AHBENR`**）；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) **§1**；书签 **`MainInit` @ `0x08002E74`**，`save_program`  
  可信度：**高**

- 结论：**RCM 三件套 + GPIO CRL/CRH 封装** 已命名：**`RCM_SetOrClearAHBENR_Bits`**（`0x08004FC4`→`0x40021014`）、**`RCM_SetOrClearAPB2ENR_Bits`**（`0x08004FC8`→`0x40021018`）、**`RCM_SetOrClearAPB1ENR_Bits`**（`0x08004FCC`→`0x4002101C`）、**`GPIO_WriteCRLCRH_FromPinMaskDescriptor`**（`0x08004FD0`）；旧版 [`02_Hardware_Init.md`](02_Hardware_Init.md) **§RCM** 将 **AHB/APB1** 与两函数 **交叉写反** 已纠正  
  证据路径：Ghidra MCP `read_memory` @ **`0x08004FC4`/`0x08004FC8`/`0x08004FCC`**；`rename_function_by_address`；[`02_Hardware_Init.md`](02_Hardware_Init.md) §RCM、§GPIO；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1  
  可信度：**高**（池字与寄存器址）

- 结论：**运行期 SPI 主泵** **`FUN_08004258`** → **`SPI1_PumpEightFramebufferSlices`**；**每通道 3 B 前缀** **`FUN_0800422a`** → **`SPI1_Send3ByteSlicePrefixForChannel`**（栈上组字后 **`SPI1_SendConstBlockWithCS(sp,3)`**，**非** 23 B 启动 blob）；**`get_xrefs_to` @ `0x08004258`**：**`main`**（两处）与 **`SPI1_StartupSequence`**  
  证据路径：Ghidra MCP `decompile_function` / `get_xrefs_to` / `set_decompiler_comment`；书签 **`Protocol` @ `0x08004258`**，`save_program`  
  可信度：**高**

- 结论：**`FUN_0800405c`** → **`UsbDevice_CompleteInitFromContext`**（**`SetUsbDeviceContextPointer`** 后继 **`FUN_08002b18`→`FUN_0800207c`** 等）；**`FUN_08009492`** → **`UI_SPI1_PumpEightFramebufferSlices`**（UI 侧 8×（3 B+`0x60`），经 **`FUN_08009e06`** 轮询 **SPI DR**，与 **`SPI1_PumpEightFramebufferSlices`** 形态对仗）；**`FUN_0800940c`** → **`UI_OLED_DisplayInitAndFlushFramebuffer`**（仅 **`UI_OLED_MainStateMachine`** 调用，末步调 **`UI_SPI1_PumpEightFramebufferSlices`**）  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1–§2；书签 **`USB` @ `0x0800405c`**，`save_program`  
  可信度：**高**（调用链）；**中**（UI 与 `main` 双泵是否分时/模式门控 — 未做运行时证明）

- 结论：**`FUN_080038c4`** → **`Init_GPIO_PortBCD_StartupExtended`**（**`RCM_SetOrClearAPB2ENR_Bits(0x1c,1)`** 使能 **IOPB+IOPC+IOPD**，多轮 **`GPIO_WriteCRLCRH_FromPinMaskDescriptor`**，末 **`GPIOC_Pin2Drive_BSRR_or_BRR(0)`**）；**`FUN_080038b0`**/**`FUN_0800389c`** → **`GPIOC_Pin2Drive_BSRR_or_BRR`** / **`GPIOB_Pin9Drive_BSRR_or_BRR`**（**`BSRR`/`BRR` 掩码 `0x4`/`0x200`**，分别为 **PC2** / **PB9** 语义）；**`InitializeStartupFlagsAndPrep`** 顺序：**`Init_GPIO_PortBCD_StartupExtended`** → **`GPIOB_Pin9Drive_BSRR_or_BRR(0)`** → 写 **`DAT_08003d18`** 标志  
  证据路径：Ghidra MCP `decompile_function` @ `0x080038C4`/`0x08003CD2`；[`02_Hardware_Init.md`](02_Hardware_Init.md) **`main` 摘录**；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1  
  可信度：**高**（指令与池字）；**中**（PB9/PC2 与维修手册网络名一一对应需原理图）

- 结论：**`FUN_080050dc`** → **`FMC_DelayFromStoredSysClkThenKeySequence`**：读 **SRAM `0x200004D4`**（**`SysClkHz_ReadCachedFromSRAM`** 暴露为 **SYSCLK Hz 缓存**）→ **`thunk_FUN_08002a92(sysclk/1000)`** 延时 → **`FUN_08002abc(0xffffffff,param,0)`** 走 **FMC 密钥/选项字写入路径**；**`get_function_callers`**：**`InitializeFlashPrefetchChain`**、**`FMC_ConfigureWaitStatesAndSysClkCache`**  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1 **`InitializeFlashPrefetchChain`**  
  可信度：**高**（调用与数据址）；**中**（延时标度是否严格 1 ms 量级 — 未示波器）

- 结论：**`FUN_08004618`** → **`FMC_ConfigureWaitStatesAndSysClkCache`**：以 **描述字节 `param_1`** 与 **目标等待周期 `param_2`** 写 **`FMC`**（`DAT_080047bc`→`0x40022000`）并 **轮询 `FMC_STS`**；忙等使用 **`TickVar_Get`**（读 **`0x200048F0`** 单调 tick 变量）与 **超时 `0x1389`**；成功路径则 **`RCM_ComputeSysClkHz`** → **`0x200004D4`**（与 **`SysClkHz_ReadCachedFromSRAM`** 同源缓存）并 **`FMC_DelayFromStoredSysClkThenKeySequence(0)`**。**唯一调用者**（静态）：**`InitializeClock_PLLFlashSysTick`**（`0x08002F9C`）  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1 **`InitializeClock_PLLFlashSysTick`** 表行  
  可信度：**高**（FMC 基址与调用链）；**中**（描述字节各位语义与 ST 官方 `FLASH_SetLatency` 模板的一一映射）

- 结论：**`FUN_0800475a`** → **`RCM_ComputeSysClkHz`**（PLL/HSI/HSE 路径表 **`FUN_080016ec`** + **`RCM->CFGR`** 类字段）；**`FUN_0800570c`** → **`TickVar_Get`**（供 FMC 等待环）  
  证据路径：Ghidra MCP `decompile_function`；书签 **`MainInit` @ `0x08004618`**，`save_program`  
  可信度：**高**（交叉引用 **`FMC_ConfigureWaitStatesAndSysClkCache`**）

- 结论：**`UI_OLED_MainStateMachine`**（`0x0800D89C`）**第一层 callee** 中 **帧缓冲/字模** 与 **提示/对话框** 已命名：**`UI_Framebuffer_CopyRect`**（`0x60` 行步长矩形拷）、**`UI_Framebuffer_Clear768`**（`0x300` 清零）、**`UI_Framebuffer_OrMergeRect`**（OR 合并）、**`UI_Framebuffer_DrawGlyphBand`**（条带位图）、**`UI_Framebuffer_DrawAsciiString`**（`6×12` 类字形，`FUN_0800a244` 条带拷）；**`UI_SPI_GpioIdlePattern`**（GPIO 片选空闲态）；**`UI_PromptGlobals_Reset`/`UI_PromptGlobals_GetStatus`**（`DAT_0800c640`→`0x20000234`）；**`UI_ActiveContext_Byte1_Get`**（`DAT_0800c7d0+1`）；**`UI_DialogTimed_SequenceA`/`UI_DialogTimed_SequenceB`**（清屏 + **`UI_Framebuffer_CopyRect`** + **`0x5DD` tick** 窗）。**`SequenceB`** 另调 **`UI_Framebuffer_DrawAsciiString`**  
  证据路径：Ghidra MCP `get_function_callees` / `decompile_function`；[`03_Function_Modules.md`](03_Function_Modules.md) §1.2–§1.4；书签 **`UI` @ `0x0800d89c`**，`save_program`  
  可信度：**高**（调用与缓冲址 **`DAT_08009808`**）；**中**（**SequenceA/B** 业务文案与说明书条目 **未**逐条对照）

- 结论：**`FUN_080042f4`** → **`RCM_SequencePLLHSEAndBusEnable_ApplyControlBlock`**：**唯一调用者** **`InitializeClock_PLLFlashSysTick`**（先于 **`FMC_ConfigureWaitStatesAndSysClkCache`**）；对 **`RCM` 基址 `0x40021000`** 写 **`CTRL`/`CFG`/`INT`** 及 **APB 时钟门控字**，并用 **`TickVar_Get`** 轮询 **HSE/PLL 就绪**（与 SDK **`RCM_T`**：`CTRL`、`CFG`、`INT`、**`APB1CLKEN`/`APB2CLKEN`** 等字段语义一致）。  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；[`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) **`RCM_T`**；书签 **`MainInit` @ `0x080042f4`**，`save_program`  
  可信度：**高**（MMIO 址 + 调用序）

- 结论：**`FUN_080047fc`** → **`RCM_SequenceSysClkMuxAndAPBPrescale_ApplyControlBlock`**：**唯一调用者** **`InitializeClock_PLLFlashSysTick`**（在 **`FMC_ConfigureWaitStatesAndSysClkCache`** **之后**）；池指针含 **`0x40021004`（`RCM` `CFG`）** 与 **`0x40007000`（PMU/备份域侧，与 SDK 外设图一致）**；写 **`CFG`** 的 **SCLKSEL / AHBPSC / APB 分频** 等并 **忙等就绪位**（**`TickVar_Get` 超时 `0x1389`** 形态同链上其它时钟序列）。  
  证据路径：Ghidra MCP `decompile_function` / `read_memory` @ **`0x080048c0`**；[`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) 外设基址；书签 **`MainInit` @ `0x080047fc`**，`save_program`  
  可信度：**高**（字面量址 + 调用序）；**中**（各位与 **`RCM_CFG_*` / PMU 寄存器** 字段一一命名需对照反汇编）

- 结论：**`FUN_0800e9dc`** → **`UI_ModeRender_LightApplyDiagnosticsAscii`**：**无直接 `bl` 交叉引用**（`get_xrefs_to` 空）；由 **`UI_ModeRender_Dispatch`**（`0x0800D612`–`0x0800D622` **`bx r1`**）经 **`DAT_0800d668`** 区 **vtable** 间接调用；**`FUN_08009828`** **唯一调用者**为本函数；功能：**`UI_Framebuffer_Clear768`** + **`UI_Framebuffer_DrawAsciiString`** + **`LightApply_ReadContextState0`** 分支 + 数值/参数行绘制。  
  证据路径：Ghidra MCP `disassemble_function` @ **`0x0800D600`**、**`0x0800E9DC`**；`get_function_callers` @ **`0x08009828`**；书签 **`UI` @ `0x0800e9dc`**，`save_program`  
  可信度：**高**（间接跳转与 **唯一** `FUN_08009828` caller）；**中**（屏上文案与说明书 **模式号** 的逐条映射）

- 结论：**`FUN_0800A1C0`** → **`PD_WriteGPIOB11_BSRR_BRR`**：**`GPIOB` 掩码 `0x800` → PB11（DC_IN）**；**`get_function_callers` 仅 `PD_GPIO_StateMachine`**（与 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.5 一致）。  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；书签 **`PD_GPIO` @ `0x0800A1C0`**，`save_program`  
  可信度：**高**

- 结论：**`FUN_08009FA0`** → **`GPIOA_Pin0_Drive_BSRR_or_BRR`**：**`DAT_0800a1e8`→`0x40010810`（`GPIOA->BSC`）**，**PA0** 置位/清零；**调用者**：**`PD_GPIO_ReadPc13ToggleApplyPb3`**、**`PD_ResetPc13ToggleContext_ClearGPIOA0`**、**`BatteryGauge_ADC9_UpdateFromSysTick600`**。  
  证据路径：Ghidra MCP `read_memory` @ **`0x0800A1E8`**；`get_function_callers`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.10  
  可信度：**高**（寄存器址）；**中**（**PA0** 与维修手册 §6 **未逐条命名** — 待原理图）

- 结论：**`FUN_0800C77C`** → **`PD_ResetPc13ToggleContext_ClearGPIOA0`**：序 **`FUN_08009e8e`** → **`GPIOA_Pin0_Drive_BSRR_or_BRR(0)`** → 置 **`DAT_0800c7d0[0]=1`**、**`[1]=0`**；**唯一调用者** **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**（PWM/GPIO 初始化链）。**`FUN_0800CBD0`** → **`PD_GPIO_ToggleHook_NoOp`**（**空桩**，**`PD_GPIO_ReadPc13ToggleApplyPb3`** 内条件调用）。  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；`save_program`  
  可信度：**高**（调用图）

- 结论：**`FUN_080047B6`** → **`SysClkHz_ReadCachedFromSRAM`**：返回 **`*(uint32_t*)0x200004D4`**；供 **`InitializeClock_PLLFlashSysTick`** 尾部与 **`FMC_DelayFromStoredSysClkThenKeySequence`** 共用 **SYSCLK Hz** 缓存（与 **`FMC_ConfigureWaitStatesAndSysClkCache`** 写入路径一致）。  
  证据路径：Ghidra MCP `decompile_function` @ **`0x080047B6`**；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1  
  可信度：**高**

- 结论：**原 `thunk_FUN_08005184` / `FUN_08005938`…`FUN_08005954` 池** 指向 **`IWDT_BASE` `0x40003000`**（**`apm32f103xb.h`**），为 **独立看门狗（IWDT）** 解锁/预分频/重装载/喂狗/启动序列，**不是** FMC **`0x40022000`** 密钥流。已重命名：**`Iwdg_UnlockConfigurePrescaleReloadAndStart`**（`0x08005184`）、**`IWDG_KR_Write`** / **`IWDG_PR_Write`** / **`IWDG_RLR_Write`** / **`IWDG_KR_ReloadAAAA`** / **`IWDG_KR_EnableWatchdogCCCC`**；**`main`** 第一层 **`thunk_Iwdg_UnlockConfigurePrescaleReloadAndStart`**、**`thunk_IWDG_KR_ReloadAAAA`**；**`FUN_08005100`** → **`InitializeFlashPrefetch_SecondStage`**（**`InitializeFlashPrefetchChain`** 尾段，**`RCM->APB2ENR`** bit0 + **`FUN_08002abc`**）。池字标签 **`pool_IWDT_*_PTR`**，书签 **`MainInit` @ `0x08005184`**，`save_program`。  
  证据路径：Ghidra MCP `disassemble_function` @ **`0x08005184`**/`0x08005938`；`xxd` 池 @ **`0x08005960`**→**`0x40003000`**；SDK **`IWDT_BASE`**；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1 补充段  
  可信度：**高**（外设基址与指令序列）；**中**（**`FUN_08002abc`** 各负立即数语义细拆）

- 结论：**USB Device 初始化链** 自 **`RCM_EnableAPB1_USBCLK_IfBaseIsUsbDevice`**（`0x08001FBC`）起，经 **`UsbDevice_ConfigureCoreRegistersAndEndpoints`**、**`UsbDevice_InitSessionAndConfigureCore`**、**`UsbDevice_SetDeviceCallbackPtr`** / **`UsbDevice_SetDescriptorTablePtr`** / **`UsbDevice_SetStringDescriptorPtr`**、**`UsbDevice_StartDeviceAfterDescriptors`**（**`UsbDevice_InitEndpointArrayAndHardware`**、**`UsbDevice_AssignEndpointDescriptorSlot`**、**`UsbDevice_ExecuteConnectEnableSequence`**）与 **`UsbDevice_CompleteInitFromContext`** 闭合；**`UsbDevice_CompleteInitFromContext`** 注释已更新。  
  证据路径：Ghidra MCP `decompile_function` / `rename_function_by_address`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §1、§5.1；书签 **`USB` @ `0x08001FBC`**，`save_program`  
  可信度：**高**

- 结论：**`HSI_FloatMath_ComputeRGBTriple`** 下游 **microlib** 浮点运行时（**`Microlib_UIntToFloat`**、**`Microlib_FloatMul`/`Add`/`Sub`/`Div`**、**`Microlib_DoubleMul`/`Add`/`Sub`**、**`Microlib_FloatLibmPowScale`** 等）与 **`RGBTriple_PushPattern_TMR2Gated`** 之 **`BlockZero_BufferUnaligned`**、**`BlockZero_BufferFromEnd`**、**`TMR_SetCounterEnable`**、**`NVIC_SetPendingIRQ`** 已 **批量重命名**；**`LightApply_UpdateRampScratchFromADC`**（`0x0800DEE4`）增加 **PRE_COMMENT**（**`pcVar1+0xC`** 子块 **+4/+8/+0xC/+0x10** 与 [`03` §0.3.4](03_Function_Modules.md) **一致**）。  
  证据路径：Ghidra MCP；[`03_Function_Modules.md`](03_Function_Modules.md) §0.3.1、§0.3.4；书签 **`Microlib` @ `0x0800B390`**、**`LightApply` @ `0x0800DEE4`**，`save_program`  
  可信度：**高**（指令形态）；**中**（libm 个别入口 **精确 C 库名**）

- 结论：**`FUN_0800d6da`** → **`Input5Channel_DispatchRowEvent`**（`DAT_0800d884` 表 **`0x14` 步进**，主行回调 + 偏移 **`+0x64`** 共享回调；事件码 **`0x10`…`0xF0`**）；**`FUN_0800d670`** → **`Input_SamplePA1PB4_As2bit`**；**`FUN_0800a0c8`** → **`GPIOB_Pin9_BSRR_BRR_ProtocolLane0`**（**lane0** 写 **`0x200`→PB9**，供 **`ProtocolLanes_BitExpandTick`**）；**`0x0800d1b2`** 已为 **`LightApply_ReadContextState3`**（与计划中的 **`FUN_0800d1b2`** 为同一地址）。  
  证据路径：Ghidra MCP `decompile_function` / `rename_function_by_address` / `set_bookmark`（**Input** @ `0x0800d6da`、**Protocol** @ `0x0800a0c8`）；[`03_Function_Modules.md`](03_Function_Modules.md) §2、[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.9  
  可信度：**高**（寄存器/调用形态）

- 结论：**`FUN_0800ECEA`** → **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**（关中断 → **`Init_DMA1Ch1_TMR2CcrDMA`** + **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`** → 开中断 → **`Init_TMR3_PB5_AndTickSyncLoop`**）；**`FUN_0800cc7e`** → **`Init_TMR3_PB5_AndTickSyncLoop`**（**`Init_GPIO_Then_TMR3_PB5_PWM`** + **`FUN_0800d70a`** 行槽 + **IWDG `0xAAAA`** 忙等环）。  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers`；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5.1、§5.3.5；书签 **`Timer` @ `0x0800ecea`**，`save_program`  
  可信度：**高**（callee 顺序）；**中**（**`Init_TMR3_PB5_AndTickSyncLoop`** 外层 **无限循环** 是否可达全部运行路径 — 未做运行时证明）

- 结论：**OI-001** 静态复核：**`get_xrefs_to` @ `0x080012E8`** 仅 **`scatterload_rw_decompress_entry`**；**`FUN_080012E8`** 反编译 **`node+0x18` handler** 与 **`param_5` 缓冲区** 参数形态与 §1.3.2 **一致**，**仍无** 裸露 Flash 源连续区间 — 见 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.5**。  
  证据路径：MCP `get_xrefs_to` / `decompile_function`  
  可信度：**高**（工具）；**OI-001 源块** 仍为 **中**（开放）

- 结论：**`FUN_0800d38c`**（唯一调用者 **`PD_OnAttachDone_ResetEncoderAndUI`**）重命名为 **`TMR4_EncoderReset_ClearStateAndReinitQuadrature`**：清零 **`DAT_0800d428`** 侧软件状态，调用 **`FUN_0800a074`**，再 **`Init_TMR4_EncoderQuadrature_PB6PB7`** — 与 PD 附着完成后 **编码器硬件复位** 语义一致。  
  证据路径：Ghidra MCP `decompile_function` / `get_function_callers` @ `0x0800D38C`；**`PRE_COMMENT`**、书签 **`Encoder` @ `0x0800D38C`**，`save_program`；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.10、`02` §Timer TMR4  
  可信度：**高**（调用图 + MMIO 链）

- 结论：**OI-001 / OI-004** 的执行级说明见 [`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) **§6**；**OI-002/OI-005** 策略已改为 **逆推优先**（见 **§4 Current Focus**、**§6 OI 表**），**不再**以屏厂规格书为门禁。  
  证据路径：`05` §6；[`04` §2](04_Protocol_Reverse.md) 策略注  
  可信度：**高**（任务书口径）

- 结论：**任务计划修订（2026-04-04）** — **OLED / 23 B / UI 模式** 改为 **固件逆推主线**：Phase 4 清单、Handoff、OI-002/OI-005 依赖列已同步；[`04` §2.0](04_Protocol_Reverse.md)、[`03`](03_Function_Modules.md) 头注、[`05` §6.1](05_Full_Reconstruction.md)、[`PROJECT_INDEX.md`](../PROJECT_INDEX.md) 知识库索引。  
  证据路径：本任务书 §3–§6；上述文件 diff  
  可信度：**高**（文档口径）

### 2026-04-05

- 结论：启动 **23 B**（`0x080041B0`）已在 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§2.4** 扩写为 **23 行逐字节** SSD1306 族语义表，并固定 **固件唯一发送链**（**`SPI1_StartupSequence`** → **`SPI1_SendConstBlockWithCS`**，同 §2.1.1）。  
  证据路径：`04` §2.4；Ghidra MCP `read_memory` @ `0x080041B0`；`decompile_function` @ `0x08004184`  
  可信度：**高**（命令 opcode）；**`AD 8B`** 电气细节 **中**（与 `04` §2.7.2 一致）

- 结论：**Flash 模式描述符指针池** @ **`0x0800DB5C`** 五字 **`0x20000190` / `0x200001CC` / `0x200001F8` / `0x200001AC` / `0x20000218`** 与 **`UI_OLED_MainStateMachine`** 中 **`UI_ModeRender_Dispatch`** 五路 **`DAT_0800db5c`…`db6c`** 分支 **一一对应**；**`g_UI_ModeRender_ActiveDescriptorPtr`**（`0x0800d668`）的 **`get_xrefs_to`**：**`UI_OLED_MainStateMachine`**、**`UI_ModeRender_PostProcess`**、**`UI_ModeRender_Dispatch`**、**`UI_ModeActiveContext_Clear`**。[`03_Function_Modules.md`](03_Function_Modules.md) **§1.5** 已增 **官方说明书** §二–§三 **模式字 ↔ 用户模式** 对照表（**CCT / HSI / FX / MAX**）。  
  证据路径：Ghidra `read_memory` @ `0x0800DB5C`；`decompile_function` @ `0x0800D89C`；`get_xrefs_to` @ `0x0800d668`；`03` §1.5  
  可信度：**高**（指针与状态机分支）；**说明书文案级** **中**（**`+0x12` 直接写** 见 **§1.5.1**，2026-04-06）

- 结论：**万用表** — 本轮 **未** 新增 **PA0 / PB12 / PB13** 测量需求；任务书 Handoff 所列仍保留 **供后续** 某脚出现 **未闭合 BSRR/IDR 疑点** 时使用。  
  证据路径：静态分析范围（本计划 **optional-hw**）  
  可信度：**高**（本轮范围声明）

- 结论：**Ghidra MCP 握手** — **`list_instances`** 见工程 **`ZY_F100`**、程序 **`ZHIYUN-F100-full.bin`**（**`connected: true`**）；**`get_function_by_address`** 抽样 **`PD_PowerAndProtocol_Block`**（`0x0800CBFC`）、**`UI_OLED_MainStateMachine`**（`0x0800D89C`）、**`Scatterload_DecompressRegionOrMemcpy`**（`0x080012E8`）与 Handoff **一致**。  
  证据路径：MCP `list_instances` / `get_function_by_address` / `get_current_program_info`  
  可信度：**高**

- 结论：**ADC1 采样环助手闭合（原 `FUN_0800d4f8`/`d5a4`/`d588`）** — 重命名为 **`ADC1_CR2_SetSwstartAdonMask`**、**`ADC1_RegBitsTestNonZero`**、**`ADC1_DR_ReadU16`**（**`ADC1_AverageSamples`** 内对 **`ADC1->REGDATA` +0x4C** 与 **状态轮询**）；**`ADC1_AverageSamples`** **`PRE_COMMENT`** + 书签 **`ADC` @ `0x0800CB7A`**（**`ADC1_AverageSamples_inner_helpers`**）。  
  证据路径：MCP `disassemble_function` @ **`0x0800CB7A`**；`rename_function_by_address` / `set_decompiler_comment` / `set_bookmark`；[`02_Hardware_Init.md`](02_Hardware_Init.md) §ADC **`ADC1_AverageSamples`**  
  可信度：**高**（指令与 [`apm32f103xb.h`](../SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h) **`REGDATA`**）

- 结论：**`FUN_0800DEA0`** → **`TMR4_ClearHalfwordAtPlus24_DisableCounter`**（**`DAT_0800dee0`→`TMR4_BASE`**，**`Encoder_ClearSoftwareStateAt20000D428`** 调用）；**`FUN_0800E47E`** → **`SharedU32_Read_DAT_0800e48c`**（**`FUN_0800e2dc`** / **`FUN_0800ed34`** 链读全局）。  
  证据路径：MCP `decompile_function` / `rename_function_by_address`；`save_program`  
  可信度：**高**（基址 **`0x40000800`**）；**中**（**`SharedU32_*`** 业务名）

- 结论：**`LightApplyContext`** 结构体字段 **`compareOrTarget_i32` / `rampDelta_i32` / `rampAcc_i32` / `pwmScaled_i32` / `tmrCompareWrittenLatch_u8` / `tickSnapshotAt30`**（**`modify_struct_field`**），与 [`03` §0.3.4](03_Function_Modules.md) **斜坡子块** 对齐。  
  证据路径：MCP `get_struct_layout` / `modify_struct_field`；`03` §0.3.3–§0.3.4  
  可信度：**高**

- 结论：**OI-005** — **`UI_ModeRender_Dispatch`** **`set_plate_comment`**（五路池 **`0x0800DB5C`…`db6c`**、**`UI_ModeEvent_*`** 交叉引用）；[`03` §1.5.4a](03_Function_Modules.md)；**`read_memory` @ `ram:20000190`** **失败**（静态镜像 **无** SRAM 预填）记入 **§1.5.4a**。**原 §1.5.4 官方说明书索引** 顺延为 **[§1.5.5](03_Function_Modules.md)**。  
  证据路径：MCP `set_plate_comment` / `read_memory`；`03` §1.5.4a / §1.5.5  
  可信度：**高**（PLATE）；**高**（RAM 负向）

- 结论：**OI-001** — **`get_xrefs_to` @ `0x080012E8`** **仍仅** **`scatterload_rw_decompress_entry`**（与 §1.3.5–§1.3.9 **一致**）。**f100-gdb-mcp**：**`monitor version`** **不支持**；**`target remote localhost:3333`** **协议错误**；**未** 取得 **`r0`/`x/`** 快照 — 记入 [`04` §1.3.10](04_Protocol_Reverse.md)。  
  证据路径：MCP `get_xrefs_to`；`open` / `call`（**f100-gdb-mcp**）；`04` §1.3.10  
  可信度：**高**（工具）；**OI-001 `src` 块** **仍中**

### 2026-04-06

- 结论：**UI 模式半字** `*(uint16*)(g_LightApply_UIContextPtr+0x12)`（SRAM **`0x2000001E`**）在 Ghidra **DATA xref** 中仅 **`PD_OnAttachDone_ResetEncoderAndUI`** **一处 `WRITE`**，值为 **`1`**（PD 附着完成后复位 UI 模式）。**`0x0800CA68`** 与 **`0x0800D1C8`** 同为 **`0x2000000C`** 池字；标签 **`flash_literal_g_light_apply_ui_ctx_ptr`**。  
  证据路径：[`03` §1.5.1](03_Function_Modules.md)；`decompile_function` @ `0x0800C974`；`get_xrefs_to` @ `0x2000001E`；Ghidra **`PRE_COMMENT`** / `save_program`  
  可信度：**高**（直接写）；**模式 2…7 的静态来源** **中**（**`Input_5Channel`** 行回调，间接）

### 2026-04-07

- 结论：**五行输入表** 基址为 **SRAM `0x20000020`**（**`*` Flash 字 `0x0800D884`**）；**`Input5Channel_SetRowPrimaryCallback`**（`0x0800D70A`）**唯一调用者** **`Init_TMR3_PB5_AndTickSyncLoop`**，按 **行 `0,2,3,4,1`** 从 **`0x0800CCF4`…`0x0800CD08`** 载入 **Thumb 主回调**（**`0x0800CBD3`…`0x0800CBE1`** 间隙桩）。  
  证据路径：[`03` §1.5.2](03_Function_Modules.md)；`disassemble_function` @ `0x0800CC7E`、`0x0800D6DA`；`get_function_callers` @ `0x0800D70A`；Ghidra **重命名** / **`PRE_COMMENT`** / 书签 **`Input` @ `0x0800CC90`**，`save_program`  
  可信度：**高**（表基址与注册序）；**行 ↔ DIM/HSI/FX/MAX/电源** **中**（待桩反汇编）

### 2026-04-05（续）

- 结论：**`0x0800CBxx` 间隙桩** — **`ProtocolLanes_MapEventCode_ToLaneByte_10_20_40`**（**`STRB`** → **`0x20001E14`**，**非** **`g_LightApply_UIContextPtr+0x12`**）；**Row2** → **`ProtocolLane_VtableDispatch_FromPtr20000184`**（**`0x20000184`** 链）。**模式半字 `+0x12`** 的 **间接写** 经 **`STRH [reg,#0x12]`** 在 **`UI_ApplyEventCode_ToModeHalfword_21_23_24`**（**`0x0800E5E6`**）与 **`UI_ModeHalfword_IncrementOrWrapAt4_Event24`**（**`0x0800E610`**）等处闭合；**`get_xrefs_to` @ `0x2000001E`** 仍 **仅** **`PD_OnAttachDone`** **DATA WRITE**（**间接寻址限制**）。  
  证据路径：[`03` §1.5.1–§1.5.2](03_Function_Modules.md)；Capstone；Ghidra MCP **`create_function`** / **`set_decompiler_comment`** / **`set_bookmark`** / **`save_program`**  
  可信度：**高**（指令与池字）；**中**（**`0x21`/`0x23`/`0x24`** ↔ 说明书按键文案）

### 2026-04-06

- 结论：**OI-005 续** — 静态 **`STRH [*,#0x12]`** 命中 **`0x0800E85A`**、**`0x0800E98E`**、**`0x0800EB02`** 三处 **vtable 风格** 分发器已建函数并注释：**`0x11`…`0x14`→`+0x1c`**；**`0x41`** 在 **`E85A`** 写 **`+0x12:=1`**，在 **`E98E`** 写 **`6`**，在 **`EB02`** 写 **`5`**；**`0x24`**（**`EB02`**）写 **`+0x12:=2`** 并链接 **`FUN_0800a11e`** / **`PD_SetDoneFlagHalfword`**。**`get_function_callers` 空**（间接调用，与 **§1.5.3** 一致）。  
  证据路径：[`03` §1.5.3](03_Function_Modules.md)；Capstone；Ghidra **`create_function`** / **`set_decompiler_comment`** / **`set_bookmark`** / **`save_program`**  
  可信度：**高**（分支与立即数）；**中**（**事件码 ↔ 说明书按键** 需 **抓屏/LA**）

### 2026-04-05（Ghidra 续分析计划落地）

- 结论：**五行表 Init 注册** 的 **Flash 池 `0x0800CCF4`…`0x0800CD08`** 与 **行 `0/2/3/4/1`**、**主回调入口** 已在 [`03` §1.5.2](03_Function_Modules.md) 以 **「注册序 ↔ 行号 ↔ 池字 ↔ 语义」** 表闭合：**行 0** → **`ProtocolLanes_MapEventCode_ToLaneByte_10_20_40`**；**行 2/3/4/1** → 汇入 **`ProtocolLane_OrMergeR0R1_VtableDispatch20000184`**（原 **`FUN_0800cbd6`**）→ **`ProtocolLane_VtableDispatch_FromPtr20000184`**。  
  证据路径：Ghidra MCP `decompile_function` @ **`0x0800CC7E`**、**`read_memory` @ `0x0800CCF0`**；[`03` §1.5.2](03_Function_Modules.md) 闭合表；**`rename_function_by_address`** / **`set_bookmark`（`Protocol` @ `0x0800CBD6`）** / **`save_program`**  
  可信度：**高**（池字与调用序）；**中**（行 ↔ 说明书 **DIM/HSI/FX** 文案，仍依事件码）

- 结论：**`0x41` 事件码** 在固件中存在 **三套互斥 vtable 入口**（**`UI_ModeRender_Dispatch` +4** 选择）：**`0x0800E85A`** → **`+0x12:=1`** + **`FUN_0800cf0e`/`cf2c`**；**`0x0800E98E`** → **`+0x12:=6`（FX）**；**`0x0800EB02`** → **`+0x12:=5`（HSI）**。Ghidra：**`set_decompiler_comment`** @ 三入口；**`set_bookmark`（`UI` @ `0x0800E98E`/`0x0800EB02`）**；[`03` §1.5.3](03_Function_Modules.md) 增补 **对照表**。  
  证据路径：`decompile_function` @ **`0x0800E85A`/`E98E`/`EB02`**；**`decompile_function` @ `0x0800D600`**（**+4 间接调用**）  
  可信度：**高**（立即数与互斥命题）；**中**（物理键 → 命中哪一 vtable **需运行时**）

### 2026-04-05（计划执行闭环）

- 结论：**OI-005** 五行表 **Row3 / Row4 / Row1** 在 Ghidra 中补齐 **独立符号**：**`ProtocolLane_Row3_Entry_NopThenOrMergeVtable`**（**`0x0800CBD4`**）、**`ProtocolLane_Row4_BranchToOrMergeVtable`**（**`0x0800CBDC`**）、**`ProtocolLane_Row1_BranchToOrMergeVtable`**（**`0x0800CBDE`**）；**`PRE_COMMENT`** 三处；书签 **`Protocol` @ `0x0800CBD4`**（桩簇说明）；[`03` §1.5.2](03_Function_Modules.md) 闭合表已同步 **显式函数名**（相对原「落在 `0x0800CBDC`…」表述）。  
  证据路径：MCP **`create_function`** / **`set_decompiler_comment`** / **`set_bookmark`**；`decompile_function` @ **`Init_TMR3_PB5_AndTickSyncLoop`**；[`03` §1.5.2](03_Function_Modules.md)  
  可信度：**高**（入口地址与注册池 **`0x0800CCF4…CD08`**）

- 结论：**OI-001** — 工作区 **`**/*.map`** 扫描 **0** 文件，**① 链接 map** 在仓库内 **不可用**；**`get_xrefs_to` @ `0x080012E8`** 复核为 **唯一** 调用边：**`scatterload_rw_decompress_entry`** @ **`0x0800693c`**（与 §1.3.5 **一致**）；[`04` §1.3.6](04_Protocol_Reverse.md) 已增 **负向 map 行** 与 **xref 复核行**。  
  证据路径：工作区 `Glob`；MCP `get_xrefs_to`；[`04` §1.3.6](04_Protocol_Reverse.md)  
  可信度：**高**（工具）；**Flash 压缩源单块** 仍为 **中**（待 **外部 map** 或 **调试器 H-OI-001** / **SRAM 转储**）

- 结论：**Ghidra** 会话 **`ZY_F100`** / **`ZHIYUN-F100-full.bin`** 已连接；**`list_bookmarks`** 与任务书 **Handoff** 书签集 **一致**（**95** 条，含 **`Scatterload`/`UI`/`Protocol`** 等）。本轮末执行 **`save_program`**。  
  证据路径：MCP `list_instances` / `list_bookmarks` / `save_program`  
  可信度：**高**

### 2026-04-08

- 结论：**Ghidra MCP** 已确认工程 **`ZY_F100`** / **`ZHIYUN-F100-full.bin`**；**`FUN_080012E8`** 重命名为 **`Scatterload_DecompressRegionOrMemcpy`**，**`PRE_COMMENT`** + 书签 **`Scatterload` @ `0x080012E8`**；**OI-001** 验证优先级写入 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.6**（**`.map` → 调试器 H-OI-001 → SRAM 转储**）。**`FUN_0800AD78`** → **`HSI_CommitPendingRGB_ToShadow`**（**`UI_ApplyEventCode_ToModeHalfword_21_23_24`** 在 **`0x21`/`0x23`** 路径尾链）；**`FUN_0800cf0e`/`FUN_0800cf2c`** → **`CCT_Context_ApplyPreset4000_100_OrClear`** / **`CCT_Context_WriteAuxHalfwordAtPlus4`**；**`save_program`**。[`03_Function_Modules.md`](03_Function_Modules.md) **§1.5.1** 增 **`UI_ApplyEventCode` 尾链** 表；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) **§6.4** 增 **最小可复现实验** 与 **风险简表**；**万用表** 本轮 **无** 新引脚疑点（**PA0/PB12/PB13** 仍为 **可选**）。  
  证据路径：MCP `list_instances` / `decompile_function` / `rename_function_by_address` / `set_bookmark` / `save_program`；`03` §1.5.1、`04` §1.3.6、`05` §6.4  
  可信度：**高**（工具与反编译）；**OI-001 源块** 仍为 **中**（待 **map/断点/转储**）

### 2026-04-05（Ghidra 续分析计划执行）

- 结论：**OI-001** — **`get_xrefs_to` @ `0x080012E8`** 复核仍为 **唯一** **`scatterload_rw_decompress_entry` @ `0x0800693C`**；**`FUN_080016CC`** → **`Scatterload_MemcpyBytes`**（**`PRE_COMMENT`**）；[`04` §1.3.7](04_Protocol_Reverse.md) 新增计划执行表；**OI-001** 单一 Flash 源区间仍 **开放**。  
  证据路径：MCP `get_xrefs_to` / `rename_function_by_address` / `set_decompiler_comment` / `save_program`；[`04` §1.3.7](04_Protocol_Reverse.md)  
  可信度：**高**（xref 与符号）；**中**（源块）

- 结论：**OI-005** — **`g_UI_ModeRender_ActiveDescriptorPtr`**（**`0x0800D668`**）**DATA `READ`** 共 **4** 处（**`UI_ModeRender_Dispatch`** / **`UI_ModeRender_PostProcess`** / **`UI_ModeActiveContext_Clear`** / **`ProtocolLane_VtableDispatch_FromPtr20000184`**）；**`UI_OLED_MainStateMachine`** 经 **池字实参** 调 **`UI_ModeRender_Dispatch`** 与 **直接池 xref** 可并存 — 已写入 [`03` §1.5](03_Function_Modules.md)。**`FUN_0800DE20`** → **`CCT_WriteHalfwordPair_ToDed8Ptr`**（**`DAT_0800ded8`** 双半字写）；**`03` §1.5.3** 表同步。  
  证据路径：MCP `get_xrefs_to` @ `0x0800D668`；`decompile_function` / `rename_function_by_address` @ `0x0800DE20`；[`03` §1.5](03_Function_Modules.md)  
  可信度：**高**（xref 与反编译）

- 结论：**Ghidra** **`list_instances`** 见 **`ZY_F100`**；**`list_bookmarks`** **96** 条（与此前 **95** 条为会话统计差 **±1**，以工程为准）；末 **`save_program`**。  
  证据路径：MCP `list_instances` / `list_bookmarks` / `save_program`  
  可信度：**高**

- 结论：**计划项复核（MCP）** — **`Scatterload_DecompressRegionOrMemcpy`**（`0x080012E8`）、**`Scatterload_MemcpyBytes`**（`0x080016CC`）、**`CCT_WriteHalfwordPair_ToDed8Ptr`**（`0x0800DE20`）与工程内命名 **一致**；**`get_xrefs_to` @ `0x080012E8`** 仍为 **`scatterload_rw_decompress_entry` @ `0x0800693C`** **唯一** 调用边；**`get_xrefs_to` @ `0x0800D668`** 仍为 **4** 处 **`READ`**（与 [`03` §1.5](03_Function_Modules.md)、[`04` §1.3.7](04_Protocol_Reverse.md) **无矛盾**）；**`save_program`**。  
  证据路径：MCP `get_current_program_info` / `get_function_by_address` / `get_xrefs_to` / `save_program`  
  可信度：**高**

### 2026-04-05（Ghidra 计划实施）

- 结论：**OI-005 / EB02 链** — 将原 **`FUN_0800a11e`/`FUN_0800ad8e`/`FUN_0800cf26`** 分别重命名为 **`MP3398_EN_ClearPB14_BRR_FromPool`**（**`0x0800A11E`**）、**`HSI_CopyPendingRGB_ToCommittedSlots`**（**`0x0800AD8E`**）、**`CCT_ReadFlagByteAtContextPlus8`**（**`0x0800CF26`**）；**`FUN_0800cf34`** → **`CCT_ResetTo2700K_SlewShadowAndClearEN`**；**`UI_ModeEvent_Apply_Codes21_22_23_24_41_ToggleOrSetMode5_2`**（**`0x0800EB02`**）增加 **`PRE_COMMENT`**（事件 **`0x24`** 与 **`0x22`** 分支摘要）。  
  证据路径：MCP `decompile_function` / `rename_function_by_address` / `set_decompiler_comment`；[`03` §1.5.3](03_Function_Modules.md)  
  可信度：**高**（指令与池字）；**中**（**PB14** 关灯语义与 **说明书按键** 仍属 **OI-004/OI-005 余量**）

- 结论：**Phase 5 / PD 附着** — **`FUN_0800c95a`** → **`PD_Attach_PostCCTResetChain`**；**`FUN_0800a154`** → **`GPIOA_BSRR_SetODRBit15_0x8000`**；**`FUN_0800ad6e`** → **`HSI_ClearPendingFlagAndGPIOA_BRR_Bit15`**；**`FUN_0800d03a`** → **`GPIOB_BRR_ClearPB8_0x100`**；**`FUN_0800d3a4`** → **`Encoder_ClearSoftwareStateAt20000D428`**；**`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`** @ **`0x0800DD28`**（文档与 Ghidra **统一**；历史自动符号 **`FUN_0800dd28`** 作废）；书签 **`PD_GPIO` @ `0x0800C95A`**。  
  证据路径：MCP `get_function_callees` @ `0x0800C156` / `0x0800CBFC`；`rename_function_by_address` / `set_bookmark`；[`03` §2.5](03_Function_Modules.md)；[`05` §5.3.1](05_Full_Reconstruction.md) **`BDT`** 行  
  可信度：**高**（调用序与 MMIO 池字）

- 结论：**OI-001 回归** — **`get_xrefs_to` @ `0x080012E8`**（**`Scatterload_DecompressRegionOrMemcpy`**）仍为 **唯一** **`scatterload_rw_decompress_entry` @ `0x0800693C`** **`UNCONDITIONAL_CALL`**；**Flash 压缩源单块** 仍 **不** 闭合静态。  
  证据路径：MCP `get_xrefs_to`；[`04` §1.3.7](04_Protocol_Reverse.md)  
  可信度：**高**（工具）；**源块** **中**（待 **map/H-OI-001**）

- 结论：**Ghidra 会话** — **`ZY_F100`** / **`ZHIYUN-F100-full.bin`** 已连接；**`list_bookmarks`** `category=UI` **9** 条与 Handoff **一致**；本轮末 **`save_program`**。  
  证据路径：MCP `list_instances` / `list_bookmarks`  
  可信度：**高**

- 结论：**TMR1 @ `0x0800DD28` 文档符号对齐（续）** — 主文统一 **`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`**；**`02` §Timer** 表中 **`Init_TMR3_PWM_GPIOB5`** 行并入 **同一 Markdown 表**（修复表体被空行打断）；**Handoff** 已增 **符号规范** 条。**`FUN_0800dd28`** 仅作 **考古** 出现于 **`05` §5.3.1 `BDT`** 行及 §5 **本条/上条** 历史括号。  
  证据路径：[`02_Hardware_Init.md`](02_Hardware_Init.md) §Timer；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.8；[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §5.3.1；本节 Handoff；工作区 `rg FUN_0800dd28`  
  可信度：**高**

### 2026-04-09（PD 附着链深化）

- 结论：**`FUN_0800c8d4`** → **`PD_Attach_CopyCCT_HSI_TripleCompareDescriptors`**：先 **`CCT_CopyActiveColorPairToBuffer`**（**`0x0800CF7E`**）把 **`DAT_0800cf9c`** 前两半字写入 **`DAT_0800ca74+4`**；再 **`HSI_FillDescriptorTripleHalfwords_FromMode`**（**`0x0800ADCA`**）按 **`DAT_0800ae1c+0x12`** 模式填充 **`DAT_0800ca74`** 三半字；再 **`CompareHalfwordArrays_0x14_Microlib`**（**`0x0800D344`**，`get_function_callers` **仅** 本函数）对 **`DAT_0800ca78`** vs **`DAT_0800ca74`** **`0x14`** 字节 **最多三次** 比较。**`PRE_COMMENT`** + 书签 **`PD_GPIO` @ `0x0800C8D4`**；[`03` §2.5](03_Function_Modules.md) 表更新。  
  证据路径：MCP `decompile_function` / `rename_function_by_address` / `set_decompiler_comment` / `set_bookmark` / `get_function_callers` @ `0x0800D344`；`save_program`  
  可信度：**高**（调用与池字）；**中**（**ca74/ca78** 与屏侧 **CCT/HSI 描述符** 的 **用户语义**）

- 结论：**OI-001 回归** — **`get_xrefs_to` @ `0x080012E8`** 仍为 **唯一** **`scatterload_rw_decompress_entry` @ `0x0800693C`**。  
  证据路径：MCP `get_xrefs_to`  
  可信度：**高**

### 2026-04-05（F100 逆向续分析计划闭环）

- 结论：**Ghidra MCP** 会话 **`ZY_F100`** 已连接；抽样 **`get_function_by_address`** 验证 **`UI_OLED_MainStateMachine`** @ **`0x0800D89C`**、**`UI_ModeRender_Dispatch`** @ **`0x0800D600`**、**`Scatterload_DecompressRegionOrMemcpy`** @ **`0x080012E8`** 与 Handoff **一致**；**`get_xrefs_to` @ `0x080012E8`**：**仅** **`scatterload_rw_decompress_entry`** **`UNCONDITIONAL_CALL`**。  
  证据路径：MCP `list_instances` / `get_function_by_address` / `get_xrefs_to`；**`save_program`**  
  可信度：**高**

- 结论：**OI-005** — 新增 [`03_Function_Modules.md`](03_Function_Modules.md) **§1.5.5**（原 **§1.5.4** 官方说明书索引，**2026-04-05** 起与 **§1.5.4a** PLATE 区分）：**官方说明书 §二 / §三** 与 **`UI_ModeEvent_*`** / **`UI_ApplyEventCode_*`** / 模式字 **`+0x12`** **索引表**；**`UI_ModeRender_Dispatch`** **`vtable +4`** 与 **§1.5.3** 互证。  
  证据路径：`decompile_function` @ **`0x0800D600`**；`03` §1.5.5  
  可信度：**高**（静态）；**中**（按键→事件码）

- 结论：**PD 附着** — [`03` §2.5.1](03_Function_Modules.md) 以 **`disassemble_function` @ `0x0800C8D4`** 固定 **`PD_Attach_CopyCCT_HSI_TripleCompareDescriptors`** 内 **三次** **`CompareHalfwordArrays_0x14_Microlib(ca78,ca74,0x14)`** 与 **`cbnz`** 早退语义（**以汇编为准**，纠 C 反编译冗余）。**`CompareHalfwordArrays_0x14_Microlib`** @ **`0x0800D344`** **`PRE_COMMENT`**（英文）已写入 Ghidra；**`save_program`**。  
  证据路径：MCP `disassemble_function` / `set_decompiler_comment` / `save_program`；`03` §2.5.1  
  可信度：**高**

- 结论：**OI-001 / Renode** — [`04` §1.3.7](04_Protocol_Reverse.md) 增补 **f100-gdb-mcp + Renode** 步骤（**`break *0x080012E8`**、**`interrupt`** / **`monitor machine Pause`**、`x/` 读 SRAM）；工作区 **`**/*.map`** **`Glob`** **0** 文件复核。**`continue` 后 GDB 异步**，本轮 **未** 稳定读出 **`0x2000000C`** 等 — **动态 SRAM** 证据 **中**。  
  证据路径：MCP `open` / `call`（`monitor version`、**`break *0x0800CBFC`**、`continue`）；`Glob` **`**/*.map`**；`04` §1.3.7  
  可信度：**高**（工具链）；**中**（RAM 字）

### 2026-04-05（续：Ghidra 小函数闭合 + GDB 冷读）

- 结论：**Ghidra** 重命名 — **`FUN_0800db7c`** → **`Init_TMR2_GPIO_ForRGBPattern_LightEngine`**（**`PRE_COMMENT`**：唯一 caller **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`**）；**`FUN_0800c60a`** → **`UI_WorkBuffer_SelectSlicePtr_FromPromptIndex`**（**`BatteryGauge_ADC9_UpdateFromSysTick600`** 与 **`UI_Framebuffer_OrMergeBand_FromPromptWorkSlice`** 共用）；**`FUN_0800e7c8`** → **`UI_Framebuffer_OrMergeBand_FromPromptWorkSlice`**；**`FUN_0800e620`** → **`UI_ModeRender_CCT234_CopyHeaderBand_AndOrMerge`**。**`save_program`**。  
  证据路径：MCP `get_function_callers` / `decompile_function` / `rename_function_by_address` / `set_decompiler_comment`；[`03` §1.2](03_Function_Modules.md)  
  可信度：**高**（调用图 + 行为）

- 结论：**f100-gdb-mcp** 新开会话、**未** `continue` 时 **`x/4wx 0x2000000C`** → **全 0**；**`x/2hx 0x2000001E`** → **0**；**`pc`** → **0** — 与 **冷复位/未跑散载** 或 **仿真未置位 PC** **相容**；**不能**替代 **`main` 后** SRAM 语义，仅作 **负向/环境** 记录。  
  证据路径：MCP `open` / `call`（`x/4wx`、`x/2hx`、`info registers pc`）；`close`  
  可信度：**高**（原始输出）；**业务语义** **低–中**（须跑起来后断点）

### 2026-04-05（逆向续分析计划实施：§1.3.8 + 五行 Init + §6.4）

- 结论：**OI-001 / f100-gdb-mcp** — **`monitor version`** 确认 **Renode 1.16.1**；**`break *0x080012E8`** → **`monitor start`** → **`continue`/`advance *0x080012e8`** 后 GDB **MI `*running`**，随后 **`call`**（**`info registers`**、**`monitor pause`**、**`interrupt`**）**多次空输出**，**未** 获得 **H-OI-001** 寄存器/SRAM 快照；**`set mi-async off`** 在 **inferior running** 时被拒。已写入 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.8**（**负向**）；**OI-001** Flash 源区间 **仍开放**。  
  证据路径：MCP **`open`** / **`call`** / **`list_sessions`**；[`04` §1.3.8](04_Protocol_Reverse.md)  
  可信度：**高**（桥接行为）；**动态 `src`** **低**（本轮无数据）

- 结论：**Ghidra / OI-005** — **`FUN_0800d422`** → **`Input5Channel_StoreInitOpaqueArgAtContextPlus8`**（**`0x0800D422`**，**`str` → `*(0x20001DE0)+8`**）；**`Init_TMR3_PB5_AndTickSyncLoop`** **`PRE_COMMENT`**（plate）；书签 **`Input` @ `0x0800D422`**；书签 **`UI` @ `0x0800E610`**（**`UI_ModeHalfword_IncrementOrWrapAt4_Event24`**）；**`save_program`**。[`03` §1.5.2](03_Function_Modules.md) 表行与 **`UI` 书签 @ `E610`** 已同步。  
  证据路径：MCP `disassemble_function` / `rename_function_by_address` / `set_decompiler_comment` / `set_bookmark` / `save_program`  
  可信度：**高**（汇编与池字 **`0x0800D428`** / **`0x0800CD0C`**）

- 结论：**Phase 5** — [`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) **§6.4** 增补 **f100-gdb-mcp** **六步**命令序与 **§1.3.8** 交叉引用；**Renode 路径** 对 **OI-001** 仍 **未** 闭环寄存器读。  
  证据路径：`05` §6.4；`04` §1.3.8  
  可信度：**高**（文档）；**实验可重复性** 依赖 **本机 GDB CLI** 或 **`.map`**

- 结论：**任务书「本轮优先任务」①（静态收口）** — **`scatterload_table_iterate`** @ **`0x08006B20`**：`disassemble_function` 固定 **`r1` 初值 `0x08006B6C`**、**`r4` 哨兵 `0x08006B8C`**、**池字 `0x08006B40`/`0x08006B44`**（**`0x2C`/`0x48`**）与 **`blx` 时 `r0=行+4`**；**`CRT_ScatterloadThenMain`**（**`0x08006B8C`**）→ **`CRT_PreScatterload_AlwaysOne`** → **`scatterload_table_iterate`** → **`main`** → **`CRT_MainReturn_BkptHangLoop`** / **`CRT_SoftwareBkptInfiniteLoop`**（**`BKPT #0xAB`**）。已写入 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.15**；Phase 1 清单增 **外层迭代器** 子项；**`save_program`**。  
  证据路径：MCP `disassemble_function` / `rename_function_by_address` / `set_plate_comment` / `save_program`；`04` §1.3.15  
  可信度：**高**（汇编与符号）；**OI-001 Flash `src` 单块** 仍 **中**（§1.3.4 不变）

### 2026-04-09（续分析计划执行：OI-001 调用约定汇编闭合 + GDB 复测 + OI-005 / 五行桩注释）

- 结论：**Ghidra 引导** — **`list_instances`**：**`ZY_F100`** / **`ZHIYUN-F100-full.bin`** 已连接；**`get_function_by_address`** 抽样：**`UI_OLED_MainStateMachine`**、**`Scatterload_DecompressRegionOrMemcpy`**、**`PD_PowerAndProtocol_Block`** 与 Handoff **一致**。  
  证据路径：MCP `list_instances` / `get_current_program_info` / `get_function_by_address`  
  可信度：**高**

- 结论：**OI-001** — **[`04` §1.3.9](04_Protocol_Reverse.md)**： **`disassemble_function` @ `0x0800692A`** 固定 **`scatterload_rw_decompress_entry` → `bl 0x080012E8`** 时 **`r0=*DAT_0800695c`（`0x200002C4`）**、**`r1`/`[sp]` 第五参同为 `param_2`**、**`r2=param_3<<9`**、**`r3=0`**；**Flash 压缩源单块** 仍 **不闭合**。**f100-gdb-mcp**：**`break *0x080012E8`** + **`continue`** → **`^running`**，后续 **`call`** **空输出**（**§1.3.8 负向** 复现）；**`close`** 会话。**主机** **`arm-none-eabi-gdb -batch`** **`continue`** **超时未停** — 记入 **`05` §6.4**。  
  证据路径：`04` §1.3.9；`05` §6.4；MCP **`open`** / **`call`** / **`close`**；Ghidra **`disassemble_function`** / **`read_memory`** @ **`0x0800695C`**  
  可信度：**高**（汇编）；**高**（GDB 负向）；**Flash `src`** **仍中**（待 **map/H-OI-001**）

- 结论：**OI-005** — **`UI_ModeRender_Dispatch`** @ **`0x0800D600`** 增加 **`PLATE`**：**描述符 `+4` 间接调用** 与 **`UI_ModeEvent_*` 内 `r1` 事件码** 分层说明；**`03` §1.5.5** 增 **2026-04-09** 行（**§1.5.4a** 为 **2026-04-05** PLATE 专节）。  
  证据路径：MCP `set_decompiler_comment` / `set_plate_comment`；`03` §1.5.4a / §1.5.5；`save_program`  
  可信度：**高**（静态）

- 结论：**五行桩** — **`ProtocolLane_Row2_Entry_JumpVtableDispatch`** @ **`0x0800CBD2`** **`PLATE`**（**Row2 → 0x0800CBD6** 链）；**Scatterload** 书签 **`Call_Scatterload_DecompressRegionOrMemcpy`** @ **`0x0800693C`**；**`scatterload_rw_decompress_entry`** **`PLATE`**（**r0/r1/栈参** 摘要）。**`save_program`**。  
  证据路径：MCP `set_decompiler_comment` / `set_bookmark` / `save_program`  
  可信度：**高**

- 结论：**万用表（可选）** — 本轮 **静态未新增** **PA0 / PB12 / PB13** 相对 **BSRR/IDR** 疑点；**不发起** 新测量请求（与 [`Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) **Handoff** 储备 **一致**）。  
  证据路径：本轮 Ghidra 仅 **注释/书签**，无新 GPIO 字面量歧义  
  可信度：**高**（本轮范围）

### 2026-04-10（任务书「下一刀」：OI-005 汇编锚点 + PB12/PB13 `IDR` 闭合）

- 结论：**OI-005** — **`UI_OLED_MainStateMachine`** 内 **五处** **`bl UI_ModeRender_Dispatch`** 的 **VA ↔ Flash 池字 ↔ 子进度首字** 已写入 [`03_Function_Modules.md`](03_Function_Modules.md) **§1.5**（**`0x0800D96A`…`0x0800DA4A`** 表）；**Ghidra 书签 `UI` @ `0x0800D96A`**；**`03` §1.2** 修正 **`LightApply_ReadContextState3`** 命名（替旧 **`FUN_0800d1b2`**）。  
  证据路径：`disassemble_function` @ **`0x0800D89C`**；MCP `set_bookmark` / `save_program`  
  可信度：**高**（指令与池）

- 结论：**PB12 / PB13** — **`GPIOB_IDR_ReadBit12_PB12`**（**`0x08009F7C`**）/ **`GPIOB_IDR_ReadBit13_PB13`**（**`0x08009F78`**）仅 **`get_function_callers` → `BatteryGauge_ADC9_UpdateFromSysTick600`**；读 **`GPIOB->IDR`**（池 **`0x0800A1E0`→`0x40010C08`**）**bit12/bit13**。全镜像 **`0x40010C08` 四字** **1** 命中。[`02_Hardware_Init.md`](02_Hardware_Init.md) **ADC 表** 与 **GPIOB IDR** 段已同步；**`BatteryGauge_*` `PLATE`**。  
  证据路径：MCP `rename_function_by_address` / `get_function_callers` / `set_decompiler_comment`；Python 扫描 `dump/ZHIYUN-F100-full.bin`  
  可信度：**高**（固件）；**中**（与维修笔记 **10k 网络** 的 **电气功能名** 仍待原理图）

### 2026-04-05（任务书续：OI-005 UI 上下文结构体 + `0x0800DB48`/`58` 池字）

- 结论：**Flash 池字语义闭合（静态）** — **`read_memory` @ `0x0800DB40`** 解析：**`0x0800DB48`→`0x20001DD4`**（**`UI_OLED_MainStateMachine`** 内 **`puVar2` 草稿区**）；**`0x0800DB58`→`0x2000000C`**（与 **`g_LightApply_UIContextPtr`** / **`flash_literal_g_light_apply_ui_ctx_ptr`** 同目标）。  
  证据路径：MCP `read_memory` @ **`0x0800DB40`**；Ghidra **`decompile_function` @ `0x0800D89C`**  
  可信度：**高**

- 结论：**Ghidra 数据类型** — **`UI_OLED_MainState_SubstateScratch`**：**12 B**（**`+6..+7` 占位** + **`+8` `tick_anchor_u32`**，与 **`puVar2+8`** 汇编一致；初建 10 B 曾致 **`tick_anchor_u32+2` 误读**，已用 **`add_struct_field` 修正**）；**`apply_data_type` @ `0x20001DD4`**。创建 **`LightApply_UIContext`**（**`+0x12` `mode_halfword_oled_render`**，**`+0x1c` `lane_or_subpage_byte_1c`** 等，**总长 32 B** — **仅覆盖已文档化前缀**）并 **`apply_data_type` @ `0x2000000C`**；Flash 池重命名 **`flash_literal_ptr_UI_OLED_MainState_SubstateScratch`**、**`flash_literal_ptr_LightApply_UIContext`**；**`UI_OLED_MainStateMachine`** **`PRE_COMMENT`**；**`save_program`**。[`03` §1.5.7](03_Function_Modules.md)。  
  证据路径：MCP `run_script_inline` / `apply_data_type` / `add_struct_field` / `rename_data` / `set_decompiler_comment` / `save_program`  
  可信度：**高**（**`+8` tick** 与汇编）；**中**（**`LightApply_UIContext` 总大小** 待 xref 扩表）

### 2026-04-05（OI-001：`renode_oi001_auto` 成功采集 → `04` §1.3.12）

- 结论：**OI-001 动态旁证闭环** — 本机 **`./scripts/renode_oi001_auto.sh`** 完整跑通，任务书 **§7** 两类产物齐备：**GDB 日志**（寄存器 / 栈 / **`x/4wx 0x200002c4`** / **`0x0800695C` 池字** / 两址指令窗）与根目录 **`renode_sram_20k.bin`**（20480 B）。**`0x0800695C`→`0x200002C4`** 与 SRAM **`[0, 0x20000230, 0, 0x20004888]`**（首 16 B）经 **日志与 `xxd` 交叉核对**一致；**`0x20001620` 栈窗 64 B 全 0** 为该次 **`interrupt`** 快照事实。**Flash 压缩源单块 `[src,src+L)`** 仍 **不闭合**（与 **§1.3.4 / OI-001 表** 一致）。  
  证据路径：[`scripts/out/renode_gdb_capture_20260405_171110.log`](../scripts/out/renode_gdb_capture_20260405_171110.log)；[`scripts/out/renode_auto_20260405_171106.log`](../scripts/out/renode_auto_20260405_171106.log)；[`renode_sram_20k.bin`](../renode_sram_20k.bin)；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.12**  
  可信度：**高**（文件级与字节级）；**`src`** **仍中**

### 2026-04-05（续：Agent 执行 `OI001_MODE=hbreak` / Monitor `AddHook` — `04` §1.3.14）

- 结论：**Renode + GDB** — **`hbreak` + `continue`** **超时无停机**；**`expect` + Ctrl-C** 亦**非** **`0x080012E8`** 语义命中。**Monitor `cpu AddHook` + `emulation RunFor` + `pause`** 可读 **`sysbus.cpu PC`**：多秒虚拟时间后 **PC 仍为 `0x080045A2` 一带**，落在 **`RCM_SequencePLLHSEAndBusEnable_ApplyControlBlock`**（**Ghidra** `get_function_by_address` @ **`0x080045A2`**），**`0x080012E8` 钩子不触发** — 与 **PLL/HSE 等待环在仿真中不满足** 的推断一致。**新增** **[`scripts/oi001_monitor_runfor_hook.py`](../scripts/oi001_monitor_runfor_hook.py)**；**`04` §1.3.14**；**Ghidra `PRE_COMMENT`** @ **`RCM_SequencePLLHSEAndBusEnable_ApplyControlBlock`**（**`0x080042F4`**）。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.14**；**`scripts/out/oi001_run3_pause.txt`**（**`strings`** 可读）；MCP **`set_decompiler_comment`** / **`save_program`**  
  可信度：**高**（工具链行为 + PC 归属）；**PLL 根因** **中–高**（仿真）

### 2026-04-05（逆向续推计划：OI-005 UI 绘制 + Phase5 灯光引擎 RCM 子簇 + OI-001 GDB 复测）

- 结论：**Ghidra 握手** — **`list_instances`** 工程 **`ZY_F100`**、程序 **`ZHIYUN-F100-full.bin`**（**`connected: true`**）；**`get_current_program_info`** 与 **`get_function_by_address`** 抽样 **`PD_PowerAndProtocol_Block`**（`0x0800CBFC`）、**`UI_OLED_MainStateMachine`**（`0x0800D89C`）、**`Scatterload_DecompressRegionOrMemcpy`**（`0x080012E8`）、**`USB_MainLoop_DataPump`**（`0x08004092`）命名与计划 **一致**。  
  证据路径：MCP `list_instances` / `get_current_program_info` / `get_function_by_address`  
  可信度：**高**

- 结论：**OI-005 / Phase 5** — 原 **`FUN_0800E67E`**（**唯一调用者** **`UI_ModeRender_CCT234_CopyHeaderBand_AndOrMerge`**）重命名为 **`UI_DrawAsciiTripleColumns_14_23_32`**：在帧缓冲列 **`0x14` / `0x23` / `0x32`** 绘制三组 **0…100** 数值 ASCII（经 **`FUN_08009828`** 组字 + **`FUN_080096d4`** 点阵写入）。**`PRE_COMMENT`** + 书签 **`UI` @ `0x0800E67E`**。  
  证据路径：MCP `decompile_function` / `get_function_callers` @ **`0x0800E67E`**；`rename_function_by_address` / `set_decompiler_comment` / `set_bookmark` / `save_program`  
  可信度：**高**（调用与列坐标立即数）

- 结论：**Phase 5 / 灯光引擎初始化** — **`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** 内原 **`FUN_0800ED34`** 为 **阻塞式 RCM 配置序列**（池指针 **`DAT_0800E484`→`0x40021000`** **`RCM` 基址**），失败路径 **死循环**；重命名为 **`Init_LightEngine_ClockTreeSequence_OrHalt`**。其下调 **`FUN_0800E00C`/`FUN_0800E2DC`/`FUN_0800EDE0`/`FUN_0800E4C4`** 分别重命名为 **`RCM_BlockingApplyDescriptor_WaitReadyBits`**、**`RCM_BlockingApplyDescriptorByte_Masked`**、**`RCM_BlockingApplyCfgWordMasks`**、**`LightEngineSeq_ReadTickU32`**（读 **`DAT_0800E4CC`→SRAM `0x20001E0C`** 单调 tick）；**`FUN_0800E416`** → **`RCM_LookupScaledDivider_FromCfgBits`**；**`FUN_0800E5CC`** → **`NOP_ReturnVoid`**（仅 **`thunk_FUN_0800E5C8`** 跳转）。**`PRE_COMMENT`** @ **`0x0800ED34`**；书签 **`MainInit` @ `0x0800ED34`**；**`save_program`**。  
  证据路径：MCP `decompile_function` / `read_memory` @ **`0x0800E480`**（**`0x40021000` 池字**）；`rename_function_by_address`  
  可信度：**高**（MMIO 基址 + 调用图）；**中**（各分支与 **APM32 `RCM_T`** 位域一一命名仍可依 SDK 细化）

- 结论：**OI-001** — **`get_xrefs_to` @ `0x080012E8`** 仍为 **`scatterload_rw_decompress_entry` @ `0x0800693C`** **唯一** **`UNCONDITIONAL_CALL`**（与 **`04` §1.3.9** **一致**）。**`scatterload_rw_decompress_entry`**（`0x0800692A`）增加 **`PRE_COMMENT`**（**`r0=*0x200002C4`**、**`r2=param_3<<9`**、**栈参**、**GDB MCP 负向** 摘要）。**f100-gdb-mcp**：**`open`** → **`target remote localhost:3333`** **成功**（inferior **已停**于 **`0x080045A6`**）；**`break *0x080012E8`** → **`continue`** → **`^running`**；**`interrupt`** / **`info registers r0 r1 …`** **`call`** **空输出** — 与 **`04` §1.3.8** 类 **负向** **一致**，**未** 在断点处取得 **寄存器/SRAM** 快照。**Flash 压缩源单块** 仍 **不闭合**。  
  证据路径：MCP `get_xrefs_to` / `disassemble_function` @ **`0x0800692A`**；`set_decompiler_comment`；f100-gdb-mcp **`open`** / **`call`**（**`target remote`** / **`break`** / **`continue`** / **`interrupt`**）；`save_program`  
  可信度：**高**（静态工具 + GDB 文本输出）；**动态 `src` 指针** **仍中**

### 2026-04-05（续推计划执行闭环：Ghidra MCP + `renode_oi001_capture` 修订）

- 结论：**Ghidra 基线** — **`list_instances`**：**`ZY_F100`** / **`ZHIYUN-F100-full.bin`**（**`connected: true`**）；**`get_function_by_address`** 抽样 **`PD_PowerAndProtocol_Block`**、**`UI_OLED_MainStateMachine`**、**`Scatterload_DecompressRegionOrMemcpy`** 与 Handoff **一致**；**`list_bookmarks`** **104** 条。  
  证据路径：MCP `list_instances` / `get_function_by_address` / `list_bookmarks`  
  可信度：**高**

- 结论：**OI-005** — 原 **`FUN_0800e5d0`/`e978`/`e7fc`/`e7de`/`eb68`/`ea7a`/`ead8`** 等重命名为 **`UI_ModeVtable_Clear768_State1_JumpPlusC`**、**`UI_ModeVtable_Clear768_State1_JumpPlusC_FX`**、**`UI_ModeScreen_DrawGlyphBands_FrameLayout`**、**`UI_Framebuffer_DrawBandRow_Call095d8_0x16`**、**`UI_ModeScreen_HSI_CopyRectAndNumericRows`**、**`UI_FormatUInt_DecimalDigits_ToBuffer`**、**`UI_ActiveDescriptor_ReadFieldPtrAtPlus8`**；**`0x0800E5D0`** 与 **`0x0800E978`** **指令级同构**（**`bx *(ctx+0xc)`** vtable 尾）。书签 **`UI` @ `0x0800E5D0`**；[`03` §1.5.6](03_Function_Modules.md)。**`save_program`**。  
  证据路径：MCP `disassemble_function` / `decompile_function` / `rename_function_by_address` / `set_bookmark` / `save_program`  
  可信度：**高**

- 结论：**Phase 5 / 灯光引擎** — **`FUN_0800e4d0`…`eeac`/`edbc`** 重命名为 **`TickSpinWait_UntilDelta`**、**`LightEngine_SysTickDelayDiv1000_ThenFMCBytePack`**、**`NVIC_AIRCR_MergePriorityGroupField`**（**`PRE_COMMENT`**：`0xE000ED0C`）、**`FMC_BytePackWriteByNegativeSlot`**、**`SysTick_CountdownSetup_FromParam`**（**`PRE_COMMENT`**：`0xE000E010`）、**`SysTick_CSR_OrClearBit2_EnableStyle`**、**`FMC_KeySequence_MultiSlotUnlockTail`**、**`LightEngine_RCMStep_NVICPrigroup_FMCUnlock`**；**`Init_LightEngine_PWM_DMA_TMR1_TMR3Sequence`** 的 callee 表含 **`LightEngine_RCMStep_NVICPrigroup_FMCUnlock`**。书签 **`MainInit` @ `0x0800EDBC`**。**`save_program`**。  
  证据路径：MCP `decompile_function` / `get_function_callees` @ **`0x0800ECEA`**；`rename_function_by_address` / `set_decompiler_comment`  
  可信度：**高**（MMIO 池字）；**中**（**FMC_BytePackWriteByNegativeSlot** 与 **Option 字节** 逐槽业务名）

- 结论：**OI-001 / 主机 GDB 脚本** — [`scripts/renode_oi001_capture.gdb`](../scripts/renode_oi001_capture.gdb) **去掉** 连接后 **`set mi-async off`**（避免 **inferior running** 时整段 **`source` 失败**）；**`04` §1.3.9** 后 **增补** 修订表 + 日志 **`scripts/out/renode_gdb_capture_20260405_152731.log`**（**失败形态**）、**`153357.log`**（**连接超时**）— **仍无** 断点寄存器快照，**Flash `src` 单块** **不闭合**。  
  证据路径：`scripts/renode_oi001_capture.gdb`；`scripts/out/*.log`；[`04` §1.3.9](04_Protocol_Reverse.md)  
  可信度：**高**（脚本与日志）；**动态 `src`** **仍中**

### 2026-04-05（执行计划：Ghidra MCP + 文档收口）

- 结论：**Ghidra `list_instances`** — 工程 **`ZY_F100`** / 程序 **`ZHIYUN-F100-full.bin`**，**`connected: true`**；**`get_xrefs_to` @ `0x080041B0`**：**唯一** **`SPI1_StartupSequence`** @ **`0x08004194`**；**`get_xrefs_to` @ `0x080012E8`**：**唯一** **`scatterload_rw_decompress_entry`** @ **`0x0800693C`**。  
  证据路径：MCP `list_instances` / `get_xrefs_to` / `read_memory` @ **`0x080041B0`**  
  可信度：**高**

- 结论：**OI-002 / §2.4** — **`read_memory`** 复核 **23 B** 与 [`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§2.1** hex **一致**；**[`04` §2.4.1](04_Protocol_Reverse.md)** 增补 **MCP 复核表**；**`set_disassembly_comment` @ `0x080041B0`**（指向 §2.4 + xref 摘要）；**`save_program`**。  
  证据路径：MCP `read_memory` / `set_disassembly_comment` / `save_program`  
  可信度：**高**

- 结论：**OI-005** — **[`03_Function_Modules.md`](03_Function_Modules.md) §1.5** 增补 **「说明书图示编号 ①–⑥ 与固件速查」** 子表（与官方 **§二、2.1** 布局图编号对齐，交叉 **`UI_ModeRender_Dispatch`** / 模式半字 **`+0x12`**）。  
  证据路径：[`03_Function_Modules.md`](03_Function_Modules.md) **§1.5.5** 后；[`ZHIYUN_F100_官方说明书.md`](../Document/Markdown/ZHIYUN_F100_官方说明书.md) **§二、2.1**  
  可信度：**高**（固件符号与模式字）；**中**（按键→事件码，仍见 **§1.5.3 余量**）

- 结论：**OI-001** — **[`04` §1.3.11](04_Protocol_Reverse.md)** 记录 **执行计划** 静态 **xref** 复核与 **动态** 仍走 **`scripts/renode_gdb_capture.sh`** / 任务书 **§7**（**本轮未**重跑 f100-gdb-mcp **`continue`**）。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.11**；MCP `get_xrefs_to` @ **`0x080012E8`**  
  可信度：**高**（静态）；**中**（Flash **`src`** 单块仍开放）

- 结论：**Phase 3 / D 簇** — **`decompile_function` @ `0x0800C8C0`**：**`EncoderGated_LightOutputApplyChain`** callee 为 **`LightOutput_PendingHSI_Dispatch`** → **`CCT_Slew_TableSplit_TMR1Shadow`** → **`EncoderGated_ADC_TMR1Compare_Apply`**，与 [`03_Function_Modules.md`](03_Function_Modules.md) **§0** 叙述 **一致**（**本轮无新增重命名**）。  
  证据路径：MCP `decompile_function` @ **`0x0800C8C0`**  
  可信度：**高**

### 2026-04-06（任务书 §4 优先项 4：OI-005 说明书编号对照收口）

- 结论：**官方说明书 §二 2.1 图示 ①–⑥ + §三 CCT/HSI/FX（及 MAX 模式字 7）** 与固件 **`LightApply_UIContext+0x12`**、**`UI_OLED_MainStateMachine` 五选一池 `0x0800DB5C`…`DB6C`** 的 **静态编号对照** 已收敛为 **[`03` §1.5.5.3](03_Function_Modules.md)** 一页表；明确 **`0x0800D668`** **仅** 指向 **SRAM 槽 `0x20000184`**（**非** 模式分支池），与 **`UI_ModeRender_Dispatch`** PLATE / **`LitPool_Ptr_UI_ModeRender_Globals_20000184`** **一致**。**`read_memory` @ `0x0800D668`** 首字 **`0x20000184`**；**`get_xrefs_to`** **4** 处 **READ** 与 Handoff **一致**。任务书 **§4 优先项 4**、**§6 OI-005** 已同步 **「静态对照已表列、余量按键→`r1`」**。  
  证据路径：Ghidra MCP `decompile_function` @ **`0x0800D600`**、`read_memory` / `get_xrefs_to` @ **`0x0800D668`**；[`03` §1.5.5.3](03_Function_Modules.md)；[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§4/§6**；[`ZHIYUN_F100_官方说明书.md`](../Document/Markdown/ZHIYUN_F100_官方说明书.md) **§二/§三**  
  可信度：**高**（池字、xref、反编译符号）；**中**（**物理键→事件码** 仍 **§1.5.3 余量**）

### 2026-04-06（续：Phase 6 入口三条文书闭合）

- 结论：**任务书 §1** 进入 Phase 6 的 **三条可核查条件** 已 **文书满足**：① **[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md)** 发布 **`05-MMIO-v1`**（**2026-04-06**），冻结 **§1–§3**+**§5**，**§6** 明示 **非冻结**；② **[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) §6** 增 **OI-001…006 书面取舍**；③ **§4** 增 **Ghidra 优先级 ①② 书签对照表**，并与 MCP **`list_bookmarks`**（**112** 条，**ZY_F100**）抽样 **互证**。**§1** 区分 **「Phase 6 准备」** 与 **可编译工程仍未创建**；**§2** Phase 6 看板 **状态** 更新为 **准备文书已齐 / 工程骨架未建**。  
  证据路径：[`05`](05_Full_Reconstruction.md) 文首冻结表；[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§1**/**§2**/**§4**/**§6**；Ghidra MCP `list_bookmarks`  
  可信度：**高**（文档与书签枚举）；**Phase 6 执行度** 仍以 **仓库构建产物出现** 为 **下一里程碑**

### 2026-04-06（OI-005：物理输入 → 底层 `param_2` / `merged` 静态表）

- 结论：**`Input_5Channel_TableStateMachine`** @ **`0x0800D74E`** 向 **`Input5Channel_DispatchRowEvent(row, param_2)`** 下发的 **`param_2`** 已 **静态枚举**为 **`0x10`（按下沿）**、**`0x20`/`0x80`（释放支路）**、**`0x40`（按住过窗）**、**`0x30`/`0x60`/`0x70`/`0xF0`（长按分级，tick>299）**；**`Input5Channel_DispatchRowEvent`** @ **`0x0800D6DA`** 对 **`base=*0x0800D884→0x20000020`** 做 **`row×0x14`** 索引并 **可选**调 **`base+100`（`+0x64`）共享回调**。  
  证据路径：Ghidra MCP `decompile_function` @ **`0x0800D74E`**、**`0x0800D6DA`**；[`03` §1.5.3.1](03_Function_Modules.md)  
  可信度：**高**

- 结论：**Row2–4/Row1** 经 **`ProtocolLane_OrMergeR0R1_VtableDispatch20000184`**（**`orrs r0,r1`**）将 **行回调实参** 并为 **`merged`**，再入 **`ProtocolLane_VtableDispatch_FromPtr20000184`**；若 **`*(uint32*)0x20000184`** 对象 **`*obj==1`** 则 **`(*(code**)(obj+8))(obj, merged)`**。**`merged` 与 `UI_ModeEvent_*` 的 `r1`（0x11…）不能静态等同**；**`UI_ModeEvent_*`** **无** Ghidra **直接 `bl` 调用边**（与既有 **§1.5.3** 一致）；**`dump` 全镜像** dword **`0x0800E85B`**、**`0x0800EB03`** 各 **唯一** 出现于 **`0x0800F34D`**、**`0x0800F339`**。  
  证据路径：`disassemble_function` / `decompile_function` @ **`0x0800CBD6`**、**`0x0800D5B4`**；Python 扫描 **`dump/ZHIYUN-F100-full.bin`**；[`03` §1.5.3.2](03_Function_Modules.md)  
  可信度：**高**（ORR 链、负向 xref）；**中**（**`obj+8` 包装层** 语义 **待 Renode `hbreak` 或实机**）

- 结论：**OI-005 余量收窄**：**物理扫描 → 底层 `0x10…` 族** **已文档闭合**；**说明书逐键 → `UI_ModeEvent` `r1` → 三套 `0x41`** 仍 **开放**（**任务书 §6 OI-005** 书面取舍 **不变**）。Ghidra：**`Input_5Channel_TableStateMachine` PRE_COMMENT** + **书签 `Input` @ `0x0800D74E`**，**`save_program`**。  
  证据路径：MCP `set_decompiler_comment` / `set_bookmark` / `save_program`（**ZY_F100**）  
  可信度：**高**（工程写回）

### 2026-04-06（Phase 6 文书审计 + 优先补强；不动工程骨架）

- 结论：**Phase 6 入口三条** 与仓库 **无冲突**：**`05-MMIO-v1`** 文首冻结段仍在；**§6 开放项书面取舍** 表完整；**Ghidra** `list_instances` → **`ZY_F100` / `ZHIYUN-F100-full.bin`**，`list_bookmarks` **113** 条（与 **§4** 表 **112** 为 **±1 枚举漂移**，**非** Handoff 符号/地址冲突）。  
  证据路径：[`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) 文首；[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§4**/**§6**；Ghidra MCP `list_instances` / `list_bookmarks`  
  可信度：**高**

- 结论：**OI-001** — **[`04` §1.3.16](04_Protocol_Reverse.md)** 增补 **一页式可核查摘要**（散载驱动 / 表区间 / Entry0·Entry1 / CRT / **仍开放 `src` 单块**），与 **§1.0 `.map` 不可用**、任务书 **§7** 脚本索引 **交叉引用**。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) **§1.3.16**  
  可信度：**高**（文档归纳）；**Flash `src`** **仍开放**（**中**，闭合路径不变）

- 结论：**OI-002 / 23 B** — MCP `read_memory` @ **`0x080041B0`**（**32 B**）前 **23** 字节 **仍为** **`ae…af`**；`get_xrefs_to` **仅** **`SPI1_StartupSequence` `[PARAM]`**；`get_function_callers` @ **`SPI1_StartupSequence`** **仅** **`main`**。[`04` §2.4.1](04_Protocol_Reverse.md) 增 **2026-04-06 复核段**。  
  证据路径：Ghidra MCP `read_memory` / `get_xrefs_to` / `get_function_callers`；[`04` §2.1](04_Protocol_Reverse.md)  
  可信度：**高**

- 结论：**OI-005** — **`get_xrefs_to` @ `UI_ModeRender_Dispatch` `0x0800D600`**：**五处**调用 **全部**在 **`UI_OLED_MainStateMachine`**；**第二实参 `r1`** **静态闭合**为 **循环内 `0x0800D94E`–`0x0800D950`** 自 **`*(g_LightApply_UIContextPtr+0x12)`** 载入之 **模式半字**（与 **vtable 内 `UI_ModeEvent_*` 的 `r1`（`0x11`…）** **区分**）。[`03` §1.5.3.3](03_Function_Modules.md)；Ghidra **`set_plate_comment` @ `0x0800D600`** + **`save_program`**。  
  证据路径：MCP `get_xrefs_to` / `disassemble_function` @ **`0x0800D89C`**；[`03` §1.5.3.3](03_Function_Modules.md)  
  可信度：**高**；**物理键→`param_2` / `tag`** **仍中**（**`UI_ModeEvent r1` 已由 §1.5.3.2.1 与 `merged` 闭合** — 见下条）。

- 结论：**（2026-04-06 续）Markdown 抢救** — 本机曾误跑「全局 `**`/`\`\`」替换脚本，**`03_Function_Modules.md` / `04_Protocol_Reverse.md`** 出现 **表格与行内代码混排损坏**；已用 **表格单元 `|**…**|` 批修** + **手修**（如 **§1.5.2** 行 4 列、**`04` §1.1** CRT 行）。**全文尚未**逐段校对；若本地有 **Time Machine / Cursor Local History / Git**，建议 **对照恢复** 大段 prose。  
  证据路径：仓库 diff；本节记录  
  可信度：**高**（过程事实）；**技术内容**仍以 **Ghidra `save_program`** 与 **本节上文 MCP 结论** 为准

### 2026-04-06（Phase 6 执行回合 AE：六项待办闭环）

- 结论：**灯光链从 `TMR2` 单点推进到 `TMR2 + DMA1 + TMR3` 同步影子** — `phase6_light_pd_skeleton.c` 新增 `Init_TMR3_PWM_FanPB5`、`Init_DMA1Ch1_TMR2CcrDMA`，`RGBTriple_PushPattern_TMR2Gated` 现回写 `tmr3_cc2_shadow` 并驱动 `DMA1_Channel1` 传输计数。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h)  
  可信度：**高**（源码可审计）

- 结论：**USB Device 数据面上下文化收敛，guard 边界未变** — `phase6_usb_device` 新增 `context_ptr_shadow`、`ep0_max_packet_shadow`、`descriptors_ready`；`phase6_startup_chain` 的 USB 三锚点已同步写入并调用 `Phase6_UsbDevice_CompleteInitFromContext`；`PHASE6_USB_FW_UPGRADE_ENABLED` 仍为 `0`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

- 结论：**验收门禁扩展并通过** — `phase6_acceptance` / host test 新增 `tmr3_pwm_apply_count`、`dma1_ch1_transfer_count` 与 USB context readiness 断言；构建与两项 host test 均通过。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)  
  可信度：**高**（断言与构建结果）

- 结论：**G2 动态证据复跑成功** — `renode_phase6_pd_breaktrace` 与 `renode_phase6_capture` 产物分别落 `logs/renode-phase6/20260406_201541/`、`logs/renode-phase6/20260406_201545/`；采样包含 `PD_GPIO` 影子推进、`spi_flush_trigger_count=1`、`startup_tx_count=23` 与 `last_byte=0xAF`。  
  证据路径：[`../logs/renode-phase6/20260406_201541/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_201541/gdb_phase6_pd_breaktrace.log)、[`../logs/renode-phase6/20260406_201545/gdb_phase6_capture.log`](../logs/renode-phase6/20260406_201545/gdb_phase6_capture.log)  
  可信度：**高**（Renode + 本机 GDB）

- 结论：**任务书 §4「本轮优先任务」①** — **Entry1**（**`0x20000000` / `0x54C`**）**handler Thumb 入口** 静态闭合为 **`0x08006964`**（**`0x08006B7C` + signed −0x217**）；**主外层环** **`cmp r1,r2` @ `0x08006978`**，**`r3`** 为 **目的写游标**，**`r1`** 为 **Flash 比特流读游标**；返回 **`r0+=0xC` → `0x08006B8C`** 与 **散表尾 `r4`** 一致。**实现为微库解码头**，**非**单条 **`memset`**；**OI-001** **单一 Flash `[src,src+L)`** **仍开放**。Ghidra：**`create_function` → `Scatterload_TableEntry1_DecompressToSRAM_20000000_54C`**、书签 **`Scatterload` @ `0x08006964`**，**`save_program`**。  
  证据路径：[`04` §1.3.17](04_Protocol_Reverse.md)；`arm-none-eabi-objdump` @ **`dump/ZHIYUN-F100-full.bin`** `0x6964`–`0x69e0`；Ghidra MCP **`disassemble_function` @ `0x08006B20`**、**`create_function`/`set_bookmark`/`save_program`**（**ZY_F100**）  
  可信度：**高**（handler/表字/指令）；**Flash 源块** **中**（与 §1.3.4–§1.3.7 一致）

- 结论：**OI-005（续）** — **`merged ≡ UI_ModeEvent_*` 第二实参 `r1`**：**`ProtocolLane_OrMergeR0R1_VtableDispatch20000184`**（**`orrs r0,r1` @ `0x0800CBD6`**）输出 **`merged`**；**`ProtocolLane_VtableDispatch_FromPtr20000184`**（**`mov r1,r0` @ `0x0800D5C0`**）在 **`bx *(obj+8)`** 前将 **`merged` 固定传入 `r1`**。**`merged = (uint8)(*(ushort*)(0x20000020+row×0x14)) | param_2`**（**`param_2`** 见 §1.5.3.1）。**否定**先前「**`obj+8` 包装层改写 `r1`**」假设。**余量**：**`tag` 半字** 运行态初值；**物理键→`param_2`**。  
  证据路径：Ghidra **`disassemble_function` @ `0x0800D6DA`、`0x0800CBD6`、`0x0800D5B4`**；**`set_decompiler_comment` @ `0x0800CBD6`/`0x0800D5B4`**，**`save_program`**；[`03` §1.5.3.2.1](03_Function_Modules.md)  
  可信度：**高**（指令链）；**`tag`** **中**（建议 SRAM 快照）

### 2026-04-06（任务书：Phase 6 主轴 + 行为验收口径）

- 结论：**工作模式** — 以 **Phase 6 可编译重构** 为**主要任务**；**静态逆向 / Ghidra / `03`–`05` 增补** **仅当** 重构实现或验收**遇阻**时再启动，结论回写专项文档与 FINDINGS。**Phase 6 第一期验收**以 **用户可感知行为等价** 为主（开关机、色温、HSL、亮灯、充电等）；**USB 固件升级**（加密下发 / 写 Flash）**不纳入**第一期验收；**插入 USB 时的 PD 诱骗充电** **须** 与规格 **高还原** 对齐（与「暂缓 USB 升级」区分）。**二进制/段级/哈希一致** 为**次要**门禁。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§1「Phase 6 验收口径与当前工作模式」**、**§2–§4**、**§7–§9**  
  可信度：**高**（文书修订）

### 2026-04-06（续：Phase 6 §7 工程索引）

- 结论：**PlatformIO** 工程 **[`ZHIYUN_F100_Firmware/zhiyun_f100_apm32/`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/)** 已写入任务书 **§7 Phase 6**（`platformio.ini`、**ST-Link**、Geehy **`apm32f103xb_flash.ld`**、**`src/geehy_sdk/`** 链 **`SDK/APM32F10x_SDK_V2.0.0`**、**[`link_geehy_sdk.sh`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/scripts/link_geehy_sdk.sh)**）；**§2** Phase 6 完成度、**§3** 骨架项、**§4** 里程碑、**§9** 维护记录同步。  
  证据路径：仓库目录与 [`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§7**  
  可信度：**高**

### 2026-04-06（Phase 6 执行回合 B：可编译重构 + 验收证据回写）

- 结论：**代码实现** — `zhiyun_f100_apm32` 已完成一轮 Phase 6 可编译收敛：`main` 默认去除硬编码演示注入（改为 `PHASE6_BOOTSTRAP_SEQUENCE_ENABLE` 可选自举）；`phase6_light_pd_skeleton` 增加阶段枚举、`PD_HelperSeq_3000tick_ThenPc13Gpio` 3000-tick 门控与 CCT/HSI 可维护计算；`phase6_usb_guard` 增加 enable/block 读数接口；`phase6_acceptance` 扩展 USB guard 与 PD 子路径计数字段。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/main.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/main.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)  
  可信度：**高**（源码可审计）

- 结论：**构建与测试** — 运行 `chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 6.8%`、`Flash 3.1%`）；主机侧 `phase6_ui_event_router_host_test` 重新编译并执行，退出码 `0`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) **「本轮执行证据」**  
  可信度：**高**（命令可复现）

- 结论：**阻塞补缺策略** — 本回合未出现必须依赖 Ghidra/f100-gdb-mcp 的实现阻塞，故遵循任务书“遇阻再逆向补缺”原则，未触发新增 MCP 逆向作业。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§1「Phase 6 验收口径与当前工作模式」**  
  可信度：**高**

### 2026-04-06（Phase 6 执行回合 C：主链收敛 + 验收门槛升级）

- 结论：**主行为链收敛** — `PD_PowerAndProtocol_Block` 调用顺序保持与 `05-MMIO-v1` 一致；`phase6_light_pd_skeleton` 新增可审计影子字段（`usb_attached`、`tmr1_period_shadow`、`tmr1_compare_shadow`、`hsi_rgb_shadow`、`encoder_accum_shadow`、`adc_level_shadow`），并将 CCT/HSI 路径显式标注“待证实”，用于后续示波器/实机补证。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h)  
  可信度：**高**（源码可审计）

- 结论：**输入采样与调度合并稳态化** — `Phase6_InputEvent` 新增 `has_adc_update`，修复 ADC=0 合法值下的软件注入/硬件采样合并歧义；`scheduler` 在 `PHASE6_HOST_TEST` 模式下提供无硬件采样桩，支持主机侧端到端验收测试。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c)  
  可信度：**高**

- 结论：**USB 升级隔离纳入验收门槛** — 验收报表增加 `usb_upgrade_policy_guarded`，`PH6-BHV-005` 通过判据要求“USB 插入 + PD 子路径 + 升级 guard 生效”；若 `usb_upgrade_enabled != 0` 则直接判 `FAIL`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

- 结论：**新增主机侧最小闭环测试** — 新增 `phase6_scheduler_acceptance_host_test`，覆盖 `PH6-BHV-001~005` 软件门槛与 USB guard；保留 `phase6_ui_event_router_host_test` 事件路由测试。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_ui_event_router_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_ui_event_router_host_test.c)  
  可信度：**高**

- 结论：**构建与测试复验** — `pio run` 成功（`RAM 7.0%`、`Flash 3.2%`）；两项 host test 均退出码 `0`。`phase6_behavior_acceptance_checklist.md` 已同步到“可执行模板 + 实测命令 + 证据字段”。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**高**（命令可复现）

### 2026-04-06（Phase 6 执行回合 H：计划落地复验）

- 结论：**构建链复验通过** — 在工程根执行 `chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run`，构建成功（`RAM 7.0%`、`Flash 3.5%`），可编译基线保持稳定。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/scripts/link_geehy_sdk.sh`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/scripts/link_geehy_sdk.sh)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/platformio.ini`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/platformio.ini)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**高**（命令可复现）

- 结论：**行为验收软件门槛复验通过** — `phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 均重新编译并执行，退出码均为 `0`；`PH6-BHV-001~005` 软件路径门槛保持通过。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_ui_event_router_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_ui_event_router_host_test.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**高**（断言驱动）

- 结论：**Phase 6 任务状态回写完成** — 任务书 `§3 Phase 6` 中“模块重写 / USB 第一期策略 / 行为验收 / 二次开发基线”已由“待开始”更新为“已完成”；二进制对照仍保持“次要、非第一期门禁”。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) **§3 Phase 6 清单**  
  可信度：**高**（文书可核查）

### 2026-04-06（Phase 6 执行回合 I：SDK 范式收敛与验收判据去魔法数）

- 结论：**可编译重构收敛** — `phase6_light_pd_skeleton`、`phase6_input_adapter` 将核心 MMIO 访问从裸地址宏切换为 Geehy SDK 外设结构体字段（`RCM/GPIO/TMR/ADC`），保持 `05-MMIO-v1` 约束下的最小访问集，不引入新增硬件行为假设。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c)  
  可信度：**高**（源码可审计）

- 结论：**验收判据稳定性提升** — `Phase6_StageId` 上提到公共头后，`phase6_acceptance` 用 `PHASE6_STAGE_UI_SPI_FLUSH` 替代阶段魔法数，避免实现与验收判据漂移。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.h)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)  
  可信度：**高**

- 结论：**主机测试兼容修复** — `PHASE6_HOST_TEST` 路径下改为条件包含 `apm32f10x.h`，修复 host test 构建报错并保持嵌入式构建路径不变。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_input_adapter.c)  
  可信度：**高**

- 结论：**复验通过** — 运行 `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 7.0%`、`Flash 3.5%`）；`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 均退出码 `0`，`PH6-BHV-001~005` 软件门槛维持通过。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)  
  可信度：**高**（命令可复现）

### 2026-04-06（Phase 6 收口执行计划落地：回合 J/K）

- 结论：**`PH6-BHV-005` 动态补证增强** — `renode_phase6_pd_breaktrace.sh` 增加 **断点后单步采样**（`PHASE6_PD_STEP_INSN`）与 **helper 断点开关**（`PHASE6_TRACE_HELPER_BP`，默认 `0`），可稳定停在 `PD_GPIO_StateMachine` 并采集 entry/post-step 双快照。  
  证据路径：[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)；运行目录 [`../logs/renode-phase6/20260406_144126/`](../logs/renode-phase6/20260406_144126/)  
  可信度：**高**（脚本与日志可复核）

- 结论：**J 回合关键观测** — 在 `PD_GPIO_StateMachine` 入口后单步 64 条指令，`g_phase6_pd_gpio_count` **0→1**，`g_phase6_pd_pb3_state`/`g_phase6_pd_pb11_state` **0→1**，`g_phase6_protocol_lane_expand_count` **1→2**；形成 `PH6-BHV-005` 的 G2 级动态证据。  
  证据路径：[`../logs/renode-phase6/20260406_144126/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_144126/gdb_phase6_pd_breaktrace.log)  
  可信度：**高**（断点与寄存器/影子计数一致）

- 结论：**回归断言补强** — `phase6_scheduler_acceptance_host_test` 增加负向场景：初始化后不得误判 PASS、USB 注入前 `BHV-005` 不得通过；保留原正向闭环断言。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)  
  可信度：**高**（测试源码可审计）

- 结论：**次要二进制对照（第一轮）已执行** — 原厂 `dump/ZHIYUN-F100-full.bin` 与构建产物 `firmware.bin` 完成向量表、镜像长度、关键常量对照：原厂 `131072B`、`SP=0x20001620`、`PC=0x08006be9`；重构 `4844B`、`SP=0x20005000`、`PC=0x08000145`；23B 启动 SPI 常量仅原厂命中（偏移 `0x41B0`）。  
  证据路径：本轮仓库根 Python 对照命令输出；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 K  
  可信度：**高**（原始二进制直接读取）

- 结论：**构建与主机测试复验通过** — `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 7.0%`、`Flash 3.5%`）；`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 退出码均 `0`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 H/I/J/K 段落  
  可信度：**高**（命令可复现）

### 2026-04-06（Phase 6 第一期计划收口：回归 + 规格审计 + 文书对齐）

- 结论：**G1 回归通过** — `zhiyun_f100_apm32` 下 `./scripts/link_geehy_sdk.sh && pio run` 成功；两项 Phase 6 host test 退出码 `0`。  
  证据路径：本轮终端输出；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 M  
  可信度：**高**

- 结论：**`05` §2 与 `PD_PowerAndProtocol_Block` 调用序一致** — 审计 `phase6_light_pd_skeleton.c`：10 个直接 callee 与 [`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §2 表一致；`Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` 仅首次进入块时门控调用（与 OEM `main` 初始化位置不同、无额外 MMIO 语义）。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md) 新增「§2 顺序对照」节  
  可信度：**高**（源码与冻结表可 diff）

- 结论：**G2 动态补证复跑** — `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`；产物目录 [`../logs/renode-phase6/20260406_150719/`](../logs/renode-phase6/20260406_150719/)（breaktrace 运行后 **`LATEST`** 可能随后续脚本被覆盖）；快照中 `g_phase6_pd_gpio_count=1`，`PB3`/`PB11` 影子为 `1`。另跑 `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh` → [`../logs/renode-phase6/20260406_150928/`](../logs/renode-phase6/20260406_150928/)（`phase6_sram_20k.bin`、`gdb_phase6_capture.log`）。  
  证据路径：同目录 `gdb_phase6_pd_breaktrace.log`  
  可信度：**高**（Renode+本机 GDB，非 MCP）

- 结论：**万用表可及验收步骤已落文档** — 仅直流/通断级，不声称 PWM；锚定 `04` §3 与任务书 §1。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)「实机万用表可及步骤」  
  可信度：**高**（文书级）

- 结论：**USB 升级隔离与 SPI 第二期边界已写入基线** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`；`UI_SPI1_PumpEightFramebufferSlices` 第一期为计数占位。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)  
  可信度：**高**

- 结论：**任务书 §2/§3/§4 与 Phase 6 文档门禁表述已对齐** — Phase 6 第一期里程碑标记为已完成；Handoff「下一刀」更新为第二期候选。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md)  
  可信度：**高**

### 2026-04-06（Phase 6 文书计划复验：回合 N）

- 结论：**G1 再次通过** — `zhiyun_f100_apm32` 下 `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 7.0%`、`Flash 3.5%`）；`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 退出码 `0`；`PH6-BHV-001~005` 与 `Phase6_AcceptanceReport` 字段映射未改代码语义。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 N；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md) 回合 N  
  可信度：**高**

- 结论：**规格冻结无需升级** — 本轮未引入与 `05-MMIO-v1` 冲突的 MMIO 语义变更，**不**触发 `05-MMIO-v2`。  
  证据路径：同上 `phase6_mmio_mapping.md` 回合 N 段  
  可信度：**高**

- 结论：**G2 动态补证再次执行** — `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`（仓库根）；产物 [`../logs/renode-phase6/20260406_152442/`](../logs/renode-phase6/20260406_152442/)（`LATEST` 指向该次 breaktrace）。`post-step` 快照：`g_phase6_pd_gpio_count=1`，`g_phase6_pd_pb3_state=1`，`g_phase6_pd_pb11_state=1`。  
  证据路径：同目录 `gdb_phase6_pd_breaktrace.log`  
  可信度：**高**（Renode+本机 GDB，非 F100 GDB MCP）

- 结论：**遇阻逆向未触发** — 构建、G1、G2 均无需新增 Ghidra 取证或修订 `03`/`04`/`05` 冻结正文。  
  证据路径：本轮无新增反编译落盘需求  
  可信度：**高**（流程性记录）

### 2026-04-06（Phase 6 启动链收敛：回合 P）

- 结论：**启动链接线收敛已落地** — 新增 `phase6_startup_chain` 模块，将 `05` §1 的关键 `main` 初始化符号以锚点函数串行化：`InitializeClock_PLLFlashSysTick`、`InitializeGPIOB_ClockAndPinMode`、`EnableAHBPeriphClock_DMA1`、`SetUsbDeviceContextPointer`、`InitializeUsbControlEndpointDefaults`、`InitializeUsbDescriptorsAfterPwmInit`；`main` 改为 `Phase6_Scheduler_Init()` 后显式执行 `Phase6_Scheduler_RunStartupStage()`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/main.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/main.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c)  
  可信度：**高**

- 结论：**验收口径同步到启动阶段** — `phase6_acceptance` 新增 `startup_chain_completed/startup_step_count/startup_last_step_id` 字段，并将 `PH6-BHV-001` 通过条件绑定“启动链已完成”，避免仅凭周期循环误判通过。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)  
  可信度：**高**

- 结论：**G1/G2 复验通过** — `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 7.2%`、`Flash 3.7%`）；两项 host test 退出码 `0`。`TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh` 产物 [`../logs/renode-phase6/20260406_154347/`](../logs/renode-phase6/20260406_154347/)：`post-step` 观测 `pd_gpio_count=1`、`PB3/PB11=1`；`PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh` 产物 [`../logs/renode-phase6/20260406_154350/`](../logs/renode-phase6/20260406_154350/)：`TMR1_CTRL1=0x81`、`TMR1_ARR=0x04B0`、`TMR1_CCR1/CCR3=0x18`。  
  证据路径：同目录 `gdb_phase6_pd_breaktrace.log`、`gdb_phase6_capture.log`；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 P  
  可信度：**高**（Renode+本机 GDB）

- 结论：**USB 写 Flash 风险边界未变化** — `phase6_usb_guard` 继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本轮无写 Flash 路径引入。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.c)  
  可信度：**高**

### 2026-04-06（Phase 6 终局收敛：回合 R）

- 结论：**启动链从锚点推进到可核查寄存器序列** — `InitializeClock_PLLFlashSysTick_Phase6Anchor` 现执行 `HSI` 就绪、`FMC CTRL1` 等待/预取、`PLL` 切换与 `SysTick` 装载；同时新增 `Phase6_SPI1_EnsureReady/SetCs/Transmit`，用于统一启动与运行期 SPI 事务。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.h)  
  可信度：**高**

- 结论：**运行期 SPI1/OLED 全量帧泵已落地** — `UI_SPI1_PumpEightFramebufferSlices` 由计数桩改为 `8×(3B+96B)` 真实发送；`UI_ModeRender_PostProcess` 与描述符池映射输出 `ui_descriptor_index`/`ui_framebuffer_checksum`，可做验收断言。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_ui_event_router.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_ui_event_router.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_ui_event_router.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_ui_event_router.h)  
  可信度：**高**

- 结论：**灯光路径一致性断言收紧** — `RGBTriple_PushPattern_TMR2Gated` 新增 `TMR2/DMA` 影子图案推进（`tmr2_dma_push_count`、`tmr2_dma_last_words`），`PH6-BHV-003` 从“仅 RGB 非零”收紧为“RGB + TMR2/DMA 图案推进成立”。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)  
  可信度：**高**

- 结论：**USB Device 稳定数据面已接入，升级链仍 guard 隔离** — 新增 `phase6_usb_device` 提供 `SOF/EP0 setup/in/out` 计数，`PH6-BHV-005` 需满足 USB 数据面心跳与 guard 同时成立；写Flash仍保持禁用。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_device.h)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

- 结论：**G1 回归通过** — `pio run` 成功（`RAM 11.8%`、`Flash 5.1%`）；`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 退出码均为 `0`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) 回合 R、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md) 回合 R  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 S，SPI1/OLED 节拍收敛）

- 结论：**运行期帧泵触发时机已收敛** — `UI_SPI1_PumpEightFramebufferSlices` 由“每轮强制全量刷新”改为“脏帧触发 + 16 tick 保底刷新”，单次触发仍保持 `8×(3B+96B)` 事务，不改变 `PD_PowerAndProtocol_Block` 十步顺序。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)  
  可信度：**高**

- 结论：**新增动态证据位点** — 增加 `g_phase6_spi_flush_trigger_count` / `g_phase6_spi_flush_skip_count`，并接入 `renode_phase6_capture.sh`、`renode_phase6_pd_breaktrace.sh`，用于 Renode+本机 GDB 复核运行期帧泵节拍。  
  证据路径：[`../scripts/renode_phase6_capture.sh`](../scripts/renode_phase6_capture.sh)、[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)  
  可信度：**高**

- 结论：**USB 写 Flash 风险边界不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0` 持续生效，本轮未接触升级写 Flash 链。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 W，帧泵分支计数闭合）

- 结论：**G2 取证链收敛** — `renode_phase6_pd_breaktrace.sh` 的 flush 分支注入从“入口前一次注入”改为“命中 flush 断点后二次注入”，并将可选计数断点改为软件断点，规避硬件断点槽位限制导致的长等待。  
  证据路径：[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)  
  可信度：**高**

- 结论：**最小可观测增强已落地（不改业务语义）** — `UI_SPI1_PumpEightFramebufferSlices` 将 `g_phase6_spi_flush_trigger_count++` 前移到 trigger 分支入口，使 GDB 在 SPI 发送循环完成前即可确认 trigger 分支已被选中；flush 事务与 MMIO 顺序保持不变。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)  
  可信度：**高**

- 结论：**trigger/skip 计数均获得非零快照** —  
  - 强制 skip：`logs/renode-phase6/20260406_181002/` 中 `g_phase6_spi_flush_skip_count=1`；  
  - 强制 trigger：`logs/renode-phase6/20260406_181024/` 中 `g_phase6_spi_flush_trigger_count=1`。  
  该结果将差距从“仅函数级命中”收敛到“可区分帧泵节拍分支并复现”。  
  证据路径：[`../logs/renode-phase6/20260406_181002/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_181002/gdb_phase6_pd_breaktrace.log)、[`../logs/renode-phase6/20260406_181024/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_181024/gdb_phase6_pd_breaktrace.log)  
  可信度：**高**

- 结论：**G1 回归仍通过且 USB guard 未放开** — `./scripts/link_geehy_sdk.sh && pio run` 成功（`RAM 11.9%`、`Flash 5.2%`），两项 host test 退出码 `0`；`PHASE6_USB_FW_UPGRADE_ENABLED=0` 不变，本轮未触发写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 AD，取证链稳定化）

- 结论：**Renode/GDB 取证链加入硬超时保护** — `renode_phase6_capture.sh` 与 `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_GDB_TIMEOUT_SECONDS`；在 `PHASE6_PD_BREAK_IGNORE=20` 场景下会话超时退出码 `124`，可自动终止卡住会话并回收。  
  证据路径：[`../scripts/renode_phase6_capture.sh`](../scripts/renode_phase6_capture.sh)、[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)；终端执行记录  
  可信度：**高**

- 结论：**断点策略收敛为默认软件断点** — `phase6_pd_breaktrace` 默认 `PHASE6_PD_SOFT_BP=1`，降低硬件断点槽位与状态切换导致的不确定性；同时统一输出 `call_ticks/oled_last_flush_tick/ui_frame_dirty`，使 main/PD/SPI 同窗采样可比对。  
  证据路径：同上脚本源码  
  可信度：**高**

- 结论：**G2 双分支复验通过** —  
  - `logs/renode-phase6/20260406_195020/`：强制 trigger 后 `g_phase6_spi_flush_trigger_count=1`；  
  - `logs/renode-phase6/20260406_195035/`：强制 skip 后 `g_phase6_spi_flush_skip_count=1`；  
  - 两轮均观测 `post-step` 的 `g_phase6_pd_gpio_count=1`、`g_phase6_pd_pb3_state=1`、`g_phase6_pd_pb11_state=1`。  
  证据路径：[`../logs/renode-phase6/20260406_195020/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_195020/gdb_phase6_pd_breaktrace.log)、[`../logs/renode-phase6/20260406_195035/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_195035/gdb_phase6_pd_breaktrace.log)  
  可信度：**高**

- 结论：**capture 采样稳定产出但 main 周期计数仍待收敛** — `logs/renode-phase6/20260406_195008/` 稳定命中 PD 入口并读到 `spi1_startup_tx_count=23`；当前窗口中 `main_boot_phase_counter8/main_iwdg_reload_count` 仍可能为 `0`，后周期非零推进继续列为下一切口。  
  证据路径：[`../logs/renode-phase6/20260406_195008/gdb_phase6_capture.log`](../logs/renode-phase6/20260406_195008/gdb_phase6_capture.log)  
  可信度：**高（现象）/中（后周期解释）**

- 结论：**USB guard 风险边界未变化** — `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；本轮仅脚本与文书收敛，未触及 USB 写 Flash 实现。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（主板 OLED 座子：硬件交叉证据）

- 结论：**主板 16pin 屏幕座与 MCU 的连通性**与 [`02_Hardware_Init.md`](02_Hardware_Init.md) §SPI1 中 **`PA5/PA6/PA7`、`PB10`（主 CS）、`PB2`（至屏座）** 的固件侧证据 **一致**，可作黑屏/接触不良排查时的 **板级网表锚点**（不新增 MMIO 语义）。  
  证据路径：[`../Document/Zhiyun_F100_Repair_Notes.md`](../Document/Zhiyun_F100_Repair_Notes.md) **§6.1**（pin8→`PB10`，pin9→`PB2`，pin10–12→`PA6/PA5/PA7`；pin7→`PB0` 接地）；[`02_Hardware_Init.md`](02_Hardware_Init.md) `Init_SPI1` / `GPIO_SPI_CS_PB10_AssertLow` / `GPIO_SPI_SelectLine_0x40`（`PA6` 辅助线）  
  可信度：**高（连通性）**；**`PB2` 在单次 OLED SPI 事务中的电气角色**若需写成与 `PB10`/`PA6` 同级结论，仍须 **Ghidra 寄存器写读链** 或 **LA**（当前标 **待证实**）

### 2026-04-06（Ghidra MCP：`SPI1` / `UI_SPI1` 帧泵 callee 对照）

- 结论：**Ghidra MCP `get_function_callees` / `decompile_function`** — `SPI1_PumpEightFramebufferSlices`（`0x08004258`）**直接**调用 **`SPI1_Send3ByteSlicePrefixForChannel`**（`0x0800422a`）、**`SPI1_SendBlockWithCS_GPIOAlt`**（`0x08004208`），与 [`02_Hardware_Init.md`](02_Hardware_Init.md) 运行期事务分型表一致；**`UI_SPI1_PumpEightFramebufferSlices`**（`0x08009492`）为 **8×（3 B + 0x60）** 循环，入口受 **`*DAT_0800980c`** 门控（为 0 则早退），循环体调用 **`FUN_08009e06`**（按长度发字节，语义上对齐 `SPI1_TransferBytes` 包装）、**`FUN_0800a028`/`FUN_0800a038`**（片选线 setup）、**`thunk_FUN_0800a162`**（片选/teardown），与 **`main`** 路径的 **`SPI1_PumpEightFramebufferSlices`** **对仗**（见 [`05_Full_Reconstruction.md`](05_Full_Reconstruction.md) §2）。  
  证据路径：MCP `get_function_callees`（`program=ZHIYUN-F100-full.bin`）；`decompile_function @ 0x08009492`  
  可信度：**高（调用图与循环形态）**；**`FUN_08009e06` 等符号**仍建议仅在 Ghidra 内逐步重命名，**禁止**整段反编译粘贴进 Phase6 产品源码

### 2026-04-06（Phase6：scheduler_ticks 与 call_ticks 同窗，对照 OEM `/20` SysTick）

- 结论：**GDB 快照** 在 `PD_GPIO_StateMachine` 断点窗口内读得 **`g_phase6_scheduler_ticks` == `g_phase6_call_ticks`**（例：`0x1`），与 `Phase6_Scheduler_RunOnce` **每圈一次** `PD_PowerAndProtocol_Block` 的 **结构** 一致；**不等于** OEM **`PD_PeriodicDispatchFromSysTick`** 的 **每 20 SysTick 一次**（`04` §3.5）之 **时间域等价**。`scripts/renode_phase6_capture.sh` 已追加 **SysTick 寄存器**（`0xE000E010`…）与 **tick 同窗** 段。  
  证据路径：[`../logs/renode-phase6/20260406_205715/gdb_phase6_capture.log`](../logs/renode-phase6/20260406_205715/gdb_phase6_capture.log)；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md) **OEM 时基对照**  
  可信度：**高（1:1 计数）**；**与原厂 wall-clock 对齐** 仍 **待证实**

### 2026-04-06（SysTick LOAD=0x1193F 与 `tick_delta`>99 / Phase6 100 门槛 — 可复核表）

- 结论：**Renode+GDB** 读 **`SysTick->LOAD`（`0xE000E014`）= `0x0001193F`**，与 **72MHz** 下 **`72000000/1000 − 1`** 一致；与 Phase6 [`phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c) 中 **`(SystemCoreClock/1000)-1`** 闭合。**OEM** `PD_PeriodicDispatchFromSysTick` 为 **每 20 次** SysTick 路径进入调 PD（`04` §3.5.1）；**`main`** 中 **`tick_delta > 99`** 与 Phase6 **`PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS = 100`** 为 **同一数字门槛** 的 **行为级**对齐，**不**自动等价于「~100ms」或「100 个 SysTick 周期」除非单独证明 `tick_delta` 粒度与 Phase6 忙等迭代一一对应。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md) **SysTick LOAD ↔ tick_delta/Step8/IWDG 可复核关系表**；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.5.1  
  可信度：**高（LOAD 公式）**；**`tick_delta` 物理单位与 SysTick 1:1** 仍 **待证实**（Ghidra 数据流）

### 2026-04-06（Phase6 G2：`renode_phase6_capture` 与 main 周期快照 — 回合 AG）

- 结论：**GDB `if 2 > 0` 被解析为重定向**（`>` 非比较运算符），已改为 **`if (2 > 0)`** 形式；否则内层 `disable`/`monitor start`/后周期采样块行为不可靠。  
  证据路径：[`../scripts/renode_phase6_capture.sh`](../scripts/renode_phase6_capture.sh)（`if (${PHASE6_CAPTURE_BREAK_ON_PD} == 1)` / `if (${PHASE6_POST_BREAK_RUN_SECONDS} > 0)`）  
  可信度：**高**

- 结论：**默认 `PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=100`** 时，在 **`PD_GPIO_StateMachine` 首次命中** 且 **`g_phase6_scheduler_ticks==1`** 的快照中 **`main_boot_phase_counter8`/`main_iwdg_reload_count` 为 0** 属**预期**（尚未跨过 100 次 `RunOnce` 门限）；Renode 下首个 `RunOnce` 可能在 **UI_SPI1 帧泵** 内长时间占用，**`monitor start` 后周期**也未必推进 PC（取证链限制，非业务逻辑否定）。  
  证据路径：动态复跑与 GDB 日志对照；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c)  
  可信度：**高（门限语义）**；**Renode 单核步进** 仍标 **待证实**

- 结论：**可选构建 `env:apm32f103cb_renode_g2`**（`-DPHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=1`）专用于 **Renode/G2 同屏证据**：在 **`PD_GPIO` 首次断点**窗口即可见 **`main_boot_phase_counter8=1`、`main_iwdg_reload_count=1`**（与 **nominal `100`** 量产语义不同，**仅**作仿真快照，不替代出厂 cadence）。配合 **`PHASE6_FW_BIN`/`PHASE6_FW_ELF`** 指向 `.pio/build/apm32f103cb_renode_g2/firmware.{bin,elf}`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/platformio.ini`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/platformio.ini) `[env:apm32f103cb_renode_g2]`；`logs/renode-phase6/LATEST/gdb_phase6_capture.log`（用户本机复验）  
  可信度：**高（可复现构建+脚本）**

- 结论：**脚本增强** — 默认 `PHASE6_POST_BREAK_RUN_SECONDS=2`、`PHASE6_CAPTURE_PD_IGNORE=0`、`PHASE6_RENODE_SPI_FALLBACK_INJECT=1`、`PHASE6_RENODE_SPI_STS_FORCE=1`；命中 PD 后可选写 **`g_phase6_spi1_status_fallback_mode`** 与 **`SPI1->STS`（0x40013008）**，与 `renode_phase6_pd_breaktrace.sh` 的 **`PHASE6_FORCE_SPI_STS_READY`** 同思路。  
  证据路径：[`../scripts/renode_phase6_capture.sh`](../scripts/renode_phase6_capture.sh) 头部注释与 GDB 段  
  可信度：**高**

- 结论：**`Phase6_LightPd_ReadTraceSnapshot`** 中 **`scheduler_ticks`** 已改为读 **`g_phase6_scheduler_ticks`**（此前误用 `g_phase6_call_ticks`，与 GDB 标签「scheduler_ticks」不一致）。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)  
  可信度：**高**

- 结论：**USB guard** — 本轮 **未** 解除 `phase6_usb_guard`，**未** 引入写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（终局收敛：PD_PeriodicDispatchFromSysTick /20 实装 + AHB SRAM 锚点 — 回合 AH）

- 结论：**调度结构对齐** — 实机新增 [`phase6_pd_systick_dispatch.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_pd_systick_dispatch.c)：`SysTick_Handler` 调用 `Phase6_PD_PeriodicDispatchFromSysTick()`，`g_phase6_systick_entry_count` 自增，**`% 20 == 0`** 时 **`Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded()` + `PD_PowerAndProtocol_Block()`**，与 **`04` §3.5.1**（`count % 0x14 == 0`）一致；**`g_phase6_pd_irq_ready`** 在 [`phase6_scheduler.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c) `RunStartupStage` 末尾置位，避免启动完成前 ISR 内进入 PD。  
  证据路径：同上；[`phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)「OEM 时基对照」修订段  
  可信度：**高（分频语义）**；**`PD_600tickAttachWindow_StateMachine` / `BatteryGauge_ADC9_UpdateFromSysTick600`** 仍 **未**并入本 ISR — **待证实**

- 结论：**主机验收兼容** — `PHASE6_HOST_TEST` 下 [`phase6_scheduler.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_scheduler.c) 每 `RunOnce` **循环 20 次** `Phase6_PD_PeriodicDispatchFromSysTick()`，保持 **1 PD / 迭代**，`phase6_scheduler_acceptance_host_test` 链接新增 `phase6_pd_systick_dispatch.c`。  
  证据路径：同上  
  可信度：**高**

- 结论：**启动链** — [`phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c) 在 `InitializeClock_PLLFlashSysTick_Phase6Anchor` 之后增加 **`EnableAHBPeriphClock_SRAM_Phase6Anchor`**（`RCM->AHBCLKEN` **SRAMEN**），对齐 **`05` §1** `EnableAHBPeriphClock_SRAM`。  
  证据路径：同上；[`phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md) 启动链差距矩阵  
  可信度：**高**

- 结论：**SPI/OLED 节拍文书化** — `PHASE6_OLED_FORCE_REFRESH_TICKS` 与 `UI_SPI1_PumpEightFramebufferSlices` 注释标明以 **`g_phase6_call_ticks`（PD 调用次数）** 为域；**trace** 增加 **`systick_entry_count` / `pd_call_ticks`**；**Renode 脚本** 打印 **`g_phase6_systick_entry_count`**。  
  证据路径：[`phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)；[`../scripts/renode_phase6_capture.sh`](../scripts/renode_phase6_capture.sh)  
  可信度：**高**

- 结论：**USB** — **未**解除 `phase6_usb_guard`；**未**引入写 Flash；专验章节增加 **Preflight 合并 Gate** 表述（[`phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)）。  
  证据路径：[`phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 AI，启动链时钟证据闭环）

- 结论：**启动链时钟路径从“占位标记”收敛为“可核验影子”** — `InitializeClock_PLLFlashSysTick_Phase6Anchor` 新增 `SystemCoreClockUpdate()`，并将 `clock_hsi_ready/clock_pll_ready/clock_pll_selected/fmc_latency_shadow/sysclk_hz_shadow/systick_reload_shadow` 纳入 `Phase6_StartupTrace`。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_startup_chain.h)  
  可信度：**高**

- 结论：**PH6-BHV-001 门槛收紧** — 验收增加启动时钟就绪条件（HSI/PLL/SysTick 重装值），避免后续回归退化到“主循环通过但时钟步骤失真”。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_acceptance.h)  
  可信度：**高**

- 结论：**G1 回归通过** — `pio run` 两环境均成功（`RAM 13.1%`、`Flash 6.4%`），host 验收测试（含新增启动时钟断言）退出码 `0`。  
  证据路径：本轮构建与测试日志；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/test/phase6_scheduler_acceptance_host_test.c)  
  可信度：**高**

- 结论：**USB 写 Flash 风险边界不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本轮未放通、未触碰升级写 Flash 链。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 AJ，PD helper 3000-tick 时基动态补证）

- 结论：**取证脚本支持直接观测 `pd_attach_window_count` 并支持可选 `systick` 边界注入** — `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_FORCE_SYSTICK_ENTRY` 与 `g_phase6_pd_attach_window_count` entry/post-step 输出，便于验证 `PD_HelperSeq_3000tick_ThenPc13Gpio` 时基语义。  
  证据路径：[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)  
  可信度：**高**

- 结论：**动态补证命中 3000-tick helper 窗口跃迁** — 在 `PHASE6_FORCE_SYSTICK_ENTRY=3000` 场景下，`g_phase6_pd_attach_window_count` 由 `0 -> 1`，同时 `pd_gpio_count` 与 `PB3/PB11` 影子由 `0 -> 1`；`g_phase6_systick_entry_count` 读数为 `0x0bcc (3020)`，与注入后的 SysTick 域窗口一致。  
  证据路径：[`../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log)  
  可信度：**高（边界场景）**

- 结论：**该证据缩短了“helper 时基仅静态对齐”的差距，但仍保留自然窗口待证实项** — 当前为边界注入取证，后续仍需在不注入 `PHASE6_FORCE_SYSTICK_ENTRY` 的自然调度下复现同类跃迁，以降低场景依赖。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**中（自然场景仍待补证）**

- 结论：**USB 风险边界保持不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本回合仅取证脚本与文书更新，未触及写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 AJ，PD helper 3000-tick 时基动态补证）

- 结论：**取证脚本支持直接观测 `pd_attach_window_count` 并支持可选 `systick` 边界注入** — `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_FORCE_SYSTICK_ENTRY` 与 `g_phase6_pd_attach_window_count` entry/post-step 输出，便于验证 `PD_HelperSeq_3000tick_ThenPc13Gpio` 时基语义。  
  证据路径：[`../scripts/renode_phase6_pd_breaktrace.sh`](../scripts/renode_phase6_pd_breaktrace.sh)  
  可信度：**高**

- 结论：**动态补证命中 3000-tick helper 窗口跃迁** — 在 `PHASE6_FORCE_SYSTICK_ENTRY=3000` 场景下，`g_phase6_pd_attach_window_count` 由 `0 -> 1`，同时 `pd_gpio_count` 与 `PB3/PB11` 影子由 `0 -> 1`；`g_phase6_systick_entry_count` 读数为 `0x0bcc (3020)`，与注入后的 SysTick 域窗口一致。  
  证据路径：[`../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log)  
  可信度：**高（边界场景）**

- 结论：**该证据缩短了“helper 时基仅静态对齐”的差距，但仍保留自然窗口待证实项** — 当前为边界注入取证，后续仍需在不注入 `PHASE6_FORCE_SYSTICK_ENTRY` 的自然调度下复现同类跃迁，以降低场景依赖。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**中（自然场景仍待补证）**

- 结论：**USB 风险边界保持不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本回合仅取证脚本与文书更新，未触及写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-06（Phase 6 第二期切口：回合 AK，PD helper 时基代码收敛与回归）

- 结论：**`PD_HelperSeq_3000tick_ThenPc13Gpio` 从“PD 调用计数窗口”收敛为“`SysTick` 入口计数窗口”** — 3000 tick 判定改为基于 `g_phase6_systick_entry_count`，不再使用 `g_phase6_call_ticks`。该改动对齐了 tick 语义，避免因 `PD_PeriodicDispatchFromSysTick` 的 `/20` 分频导致 helper 窗口被放大约 20 倍。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_light_pd_skeleton.c)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_pd_systick_dispatch.c`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_pd_systick_dispatch.c)  
  可信度：**高（实现与分频关系）**

- 结论：**G1 回归通过且未扩展 USB 风险面** — `./scripts/link_geehy_sdk.sh && pio run` 通过（`RAM 13.1%`、`Flash 6.4%`；双环境 `2 succeeded`），两项 host test 退出码 `0`；`PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变，本轮未触及写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

- 结论：**残余缺口保持显式“待证实”** — `PD_HelperSeq_3000tick_ThenPc13Gpio` 与 `PD_600tickAttachWindow_StateMachine` 的并行关系仍未闭合到逐周期等价，下一步需继续用 Renode + 本机 GDB 在同窗采样 `g_phase6_systick_entry_count`、`pd_attach_window_count` 与 `PB3/PB11` 影子。  
  证据路径：[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §3.1.2/§3.9/§3.11，[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)  
  可信度：**中（待动态补证）**

### 2026-04-06（Phase 6 第二期切口：回合 AL，PD helper 自然窗口复验）

- 结论：**自然短窗口下 3000-tick helper 未触发，结果与时基设计一致** — 在不注入 `PHASE6_FORCE_SYSTICK_ENTRY` 的运行中，`g_phase6_systick_entry_count` 保持 `0x14`，`g_phase6_pd_attach_window_count` 维持 `0`；同窗 `pd_gpio_count` 与 `PB3/PB11` 仍从 `0 -> 1`，说明 PD GPIO 子链正常推进但窗口尚未跨阈值。  
  证据路径：[`../logs/renode-phase6/20260406_232907/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232907/gdb_phase6_pd_breaktrace.log)  
  可信度：**高（现象）**

- 结论：**AJ+AL 形成互补证据** — AJ 在边界注入场景命中 `pd_attach_window_count: 0 -> 1`，AL 在自然短窗口维持 `0`，共同支持“helper 受 SysTick 域阈值门控”而非“随每次 PD 调用必增”。  
  证据路径：[`../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log)、[`../logs/renode-phase6/20260406_232907/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232907/gdb_phase6_pd_breaktrace.log)  
  可信度：**高（互证）**

- 结论：**USB 风险边界保持不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本回合仅动态取证与文书回写，未触及写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-07（Phase 6 第二期切口：回合 AM，自然窗口首次 3000-tick helper 命中）

- 结论：**在自然调度（无 systick 注入）场景下完成 3000-tick helper 首次命中** — 使用 `PHASE6_PD_BREAK_IGNORE=149` 后，entry 快照 `g_phase6_systick_entry_count=0x0bb8 (3000)` 且 `g_phase6_pd_attach_window_count=0`；post-step 跨过 helper 后 `g_phase6_pd_attach_window_count: 0 -> 1`。  
  证据路径：[`../logs/renode-phase6/20260407_000042/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260407_000042/gdb_phase6_pd_breaktrace.log)  
  可信度：**高**

- 结论：**PD 子链与 helper 触发同窗一致** — 同一会话中 `pd_gpio_count` 继续递增（`0x95 -> 0x96`），`PB3/PB11` 维持高，说明 helper 触发未破坏 PD GPIO 主链推进。  
  证据路径：同上日志  
  可信度：**高**

- 结论：**AJ+AM 闭合“边界注入 + 自然窗口”双证据链** — AJ 证明时基阈值可被命中，AM 证明自然调度下同样可达，从而将 `PD_HelperSeq_3000tick_ThenPc13Gpio` 时基收敛提升到可复现动态结论。  
  证据路径：[`../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260406_232423/gdb_phase6_pd_breaktrace.log)、[`../logs/renode-phase6/20260407_000042/gdb_phase6_pd_breaktrace.log`](../logs/renode-phase6/20260407_000042/gdb_phase6_pd_breaktrace.log)  
  可信度：**高（互证）**

- 结论：**USB 风险边界保持不变** — `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本回合未触及写 Flash。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/src/phase6/phase6_usb_guard.h)  
  可信度：**高**

### 2026-04-07（Phase 6 文书：回合 AN，终局对齐计划落地 — 空板定位与矩阵收口）

- 结论：**空开发板（STM32F103C8 + ST-Link）角色已写入基线** — 明确与量产 **APM32F103CBT6 / 128KB Flash** 的差异、烧录记录模板、以及「仅调试辅助、非终局验收」口径。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)「空开发板与量产目标 MCU」  
  可信度：**高（文书）**

- 结论：**PD/SysTick 证实/待证实矩阵收口** — `/20` 分频与 **3000-tick helper**（SysTick 域）标为已证实；**`PD_600tickAttachWindow_StateMachine`** 与 Phase6 当前 ISR 的合并关系保持 **待证实**，与 `04` §3.5.1 及既有 FINDINGS **回合 AK/AM** 一致，避免无证据合并两窗。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)「PD / SysTick 时基 — 证实与待证实矩阵」  
  可信度：**高（口径）；600-tick 并入仍为待证实**

- 结论：**SPI1/OLED 第二期闭合矩阵** — 将启动帧、运行期帧泵、UI 池、USB 边界以表格形式固定「已达 / 仍缺」，与 `05`/`04` 锚点交叉引用；**不**声称 SSD1306 全序列已逐字节等价 OEM。  
  证据路径：同上 `phase6_mmio_mapping.md`「SPI1 / OLED 终局闭合矩阵」  
  可信度：**高（文书）**

- 结论：**终局门禁与空板** — 验收清单增加「终局充分条件 vs 空板不充分条件」及 Flash 容量风险，与任务书 §1 终局对齐。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)「终局门禁与空开发板」  
  可信度：**高（文书）**

- 结论：**USB guard 文书复核** — `PHASE6_USB_FW_UPGRADE_ENABLED` 仍为 `0`；本轮无写 Flash、无 guard 解除。  
  证据路径：[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)「第二期：USB 官方升级…」2026-04-07 条  
  可信度：**高**

### 2026-04-07（续：射日 F407 + CH549/WCH-Link，PIO 工程按资料初始化）

- 结论：**侧车资料已映射到可编译 PIO 工程** — 以射日「例 1」与 `USER/system_stm32f4xx.c` 为据，固件采用 **HSE 25 MHz** PLL **M=25/N=336/P=2/Q=7** → **168 MHz**，**PA0** 500 ms 翻转；与量产 APM32 目标 **解耦**，仅供后续引脚级互证扩展。  
  证据路径：[`../射日科技STM32F407资料/2，例程代码/库函数版本/例1 LED灯/USER/system_stm32f4xx.c`](../射日科技STM32F407资料/2，例程代码/库函数版本/例1%20LED灯/USER/system_stm32f4xx.c)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/main.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/main.c)  
  可信度：**高（与厂商例程时钟注释一致）**

- 结论：**板卡选用 `black_f407ve` / `black_f407vg`** — 相对 `genericSTM32F407V*`，官方板 JSON **upload.protocols** 含 **cmsis-dap**，PIO **不再**报「Unknown upload protocol」；**WCH-Link DAP** 与 `upload_protocol = cmsis-dap`、`debug_tool = cmsis-dap` 对齐。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/platformio.ini`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/platformio.ini)；本机 `pio run -e sheritech_f407ve_cmsis_dap` **SUCCESS**  
  可信度：**高**

- 结论：**原理图文档型号为 VET6；VGT6 为可选环境** — 资料 PDF 名为 **STM32F407VET6**（512KB）；若实物为 **VGT6**（1MB），使用环境 **`sheritech_f407vg_cmsis_dap`**（`black_f407vg`）。  
  证据路径：[`../射日科技STM32F407资料/4，原理图&封装库/STM32F407VET6_V1.2(1).pdf`](../射日科技STM32F407资料/4，原理图&封装库/STM32F407VET6_V1.2(1).pdf)（文件名）、[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) §7  
  可信度：**高（文书）**；丝印以用户实物为准

- 结论：**任务书 / PROJECT_INDEX 已同步** — §1 资源分工、§7 新建「STM32F407 射日核心板 + WCH-Link」、`PROJECT_INDEX.md` §2.4。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md)、[`../PROJECT_INDEX.md`](../PROJECT_INDEX.md)  
  可信度：**高**

### 2026-04-07（续：实物 MCU 确认为 STM32F407VGT6）

- 结论：**PIO 默认环境改为 `sheritech_f407vg_cmsis_dap`**（`black_f407vg`，1MB，`board_upload.maximum_size = 1048576`）；`sheritech_f407ve_cmsis_dap` 仅作 VET6 备选；ST-Link 环境更名为 **`sheritech_f407vg_stlink`**。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/platformio.ini`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/platformio.ini)、[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) §7  
  可信度：**高（用户确认丝印 + 本机 `pio run` SUCCESS）**

### 2026-04-07（Phase 6 第二期切口：回合 AO，F407 I2C/SPI 从机模拟最小可用）

- 结论：**`I2C_SLAVE_EMU` / `SPI_SLAVE_EMU` 从预留状态升级为最小可用协议级 emu** — 新增 `src/emu_bus.c` 与 `include/emu_bus.h`，支持 `CFG/STATUS/OFF` 与 `XFER` 事务路径，其中 I2C 支持寄存器镜像（`REG_WRITE/REG_READ`），SPI 支持固定响应脚本循环回放。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/emu_bus.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/emu_bus.c)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/include/emu_bus.h`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/include/emu_bus.h)  
  可信度：**高（代码级）**

- 结论：**桥接协议与统计链已接入 emu 计数** — `GET_CAPS` 将 I2C/SPI emu 标记为 implemented；`GET_STATS` 新增 `emu_i2c_*` / `emu_spi_*` 计数；`RESET_STATS` 同步清零 emu 统计。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_stats.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/probe_stats.c)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/include/probe_stats.h`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/include/probe_stats.h)  
  可信度：**高（代码级）**

- 结论：**Phase6 对比脚本与场景文档已落地（3 个场景）** — 新增 `scripts/f407_phase6_compare_cases.py`（I2C 镜像、SPI 回放、Phase6 场景共存）与 `phase6_f407_compare_cases.md`（步骤、通过条件、记录模板），用于后续实机取证回填。  
  证据路径：[`../scripts/f407_phase6_compare_cases.py`](../scripts/f407_phase6_compare_cases.py)、[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_f407_compare_cases.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_f407_compare_cases.md)  
  可信度：**高（脚本/文书）**

- 结论：**风险边界保持不变** — I2C/SPI emu 明确为协议级模拟，不代表真实物理总线时序；USB 写 Flash guard 未触碰。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/probe_capability_matrix.md`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/probe_capability_matrix.md)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Firmware_Reverse_Guide.md`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Firmware_Reverse_Guide.md)  
  可信度：**高（文书）**

### 2026-04-07（F103 量产逆向 + F407 侧车现状摘要回填）

- 结论：**逆向进度与主轴一致** — `Phase 1/2` 已完成、`Phase 6` 第一期已收口；当前主目标仍是向“与量产行为一致并可替换量产”收敛。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) §2/§3/§4；[`../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md`](../ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md)  
  可信度：**高（文书一致）**

- 结论：**当前阻塞口径保持不变** — 外部仅万用表、无示波器/LA；Renode 对量产镜像不作为唯一可靠验证路径；`.map` 不可得仍限制 OI-001 某些静态闭环。  
  证据路径：[`REVERSE_TASK_BOOK.md`](REVERSE_TASK_BOOK.md) §1「外部实测条件约束」/§4；[`04_Protocol_Reverse.md`](04_Protocol_Reverse.md) §1.0  
  可信度：**高**

- 结论：**Ghidra 故障检测关键链再复核** — `PD_GPIO_StateMachine @ 0x0800C4E0` 内调用 `PD_ReadGPIOC13_IdrBit @ 0x08009F84`、`PD_WriteGPIOB3_BSRR_BRR @ 0x08009F90`、`PD_WriteGPIOB11_BSRR_BRR @ 0x0800A1C0`，门限常量覆盖 `0x65/0x15/0x7d1/0xD17`。  
  证据路径：Ghidra MCP `decompile_function/disassemble_function/get_xrefs_to`（`ZHIYUN-F100-full.bin`）  
  可信度：**高（函数+反编译双证）**

- 结论：**寄存器访问证据补齐** — `0x0800A1E4` 常量池可解析到 `0x40011008/0x40010810/0x40010C10/0x40010C14`，分别对应 PD 相关 `IDR/BSRR/BRR` 读写路径；`SPI1_TransferBytes @ 0x08005664` 通过 `0x40013008` 轮询 `SPI1` 收发。  
  证据路径：Ghidra MCP `read_memory/disassemble_function`（`0x0800A1E4`、`0x08005664`）  
  可信度：**高**

- 结论：**OLED 协议入口证据补齐** — `SPI1_StartupSequence @ 0x08004184` 使用 `0x080041B0` 启动 23B（`AE 00 10 40 81 FF A1 A8 3F AD 8B C8 D3 00 DA 12 D9 22 DB 2B A4 A6 AF`），运行期由 `SPI1_PumpEightFramebufferSlices @ 0x08004258` 执行分片帧泵。  
  证据路径：Ghidra MCP `read_memory(0x080041B0)`、`decompile_function(0x08004184/0x08004258)`、`get_xrefs_to(0x080041B0)`  
  可信度：**高**

- 结论：**F407 当前能力边界与缺口明确** — 现有 `I2C_SLAVE_EMU/SPI_SLAVE_EMU` 为协议级 emu；尚未实现真实 OLED 总线 sniffer（边沿->事务->字节流全链）。本轮后续实施转向“新增接线 + sniffer + 自动绕过 FSM”。  
  证据路径：[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Firmware_Reverse_Guide.md`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/F407_Probe_Firmware_Reverse_Guide.md)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/bridge_protocol.c)、[`../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/emu_bus.c`](../ZHIYUN_F100_Firmware/stm32f407vgt6_probe/src/emu_bus.c)  
  可信度：**高**

