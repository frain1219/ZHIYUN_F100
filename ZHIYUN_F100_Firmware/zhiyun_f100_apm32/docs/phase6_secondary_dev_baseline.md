# Phase 6 二次开发预备基线（第一期收口，第二期开启）

## 第二期开启说明（当前状态）

- **状态**：Phase 6 第一期间（G1/G2 + 文档基线）已收口，当前进入第二期收敛执行。
- **第二期目标**：
  1. 收敛 `SPI1/OLED` 运行期帧泵触发与节拍，提升与原厂行为对照精度；
  2. 强化 Renode + 本机 GDB 动态证据链，推动“待证实”项向“可复现实证”转化；
  3. 保持 USB 升级路径 guard 边界，先完成文书与专验准备，再评估是否进入实现。
- **第二期任务（当前）**：
  - [进行中] `SPI1/OLED` 运行期帧泵节拍收敛与计数证据化；
  - [进行中] 断点脚本取证（`renode_phase6_capture` / `renode_phase6_pd_breaktrace`）；
  - [进行中] 文书回写（`phase6_mmio_mapping`、验收清单、FINDINGS）；
  - [待开始] USB 官方升级链专验任务条目与 guard 解除条件评审。

## 基线定位

本文件定义 Phase 6 第一阶段结束后的可维护开发基线，用于后续功能迭代（灯效、UI、保护策略）前的统一入口。  
基线口径遵循：

- `Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md` §1/§3/§7
- `Cursor_Knowledge_Base/05_Full_Reconstruction.md` 冻结 `05-MMIO-v1`
- `docs/phase6_behavior_acceptance_checklist.md`
- `docs/phase6_mmio_mapping.md`

## 空开发板与量产目标 MCU（文书基线）

| 项目 | 智云 F100 量产目标 | 常见 STM32F103C8 最小系统（空板） |
|------|-------------------|----------------------------------|
| MCU | **APM32F103CBT6**（Cortex-M3，**128 KB Flash**，20 KB SRAM） | **STM32F103C8**（标称 **64 KB Flash**，20 KB SRAM；部分批次灰市标 C8 可读满 128 KB，**不作量产依据**） |
| 本工程默认构建 | [`platformio.ini`](platformio.ini) `env:apm32f103cb` → `genericSTM32F103CB` + Geehy **128 KB** 链接脚本 | 若仅 64 KB 器件，须**单独** `board`/链接脚本与 **长度门禁**，**禁止**在不知情下烧录 **128 KB** 级 `ZHIYUN-F100-full.bin` |
| 外设 | OLED、PD、灯驱、编码器等按 `05-MMIO-v1` 接线 | **无**外设时引脚浮空，ADC/附着检测与整机**不等价** |

**烧录记录模板（协作方每次实验填写）**：

| 字段 | 说明 |
|------|------|
| 镜像路径 | 例如 `dump/ZHIYUN-F100-full.bin` 或 `.pio/build/apm32f103cb/firmware.bin` |
| 镜像长度（字节） | 须 **≤** 目标 MCU 可用 Flash；OEM 全镜像为 **128 KB 档** |
| MCU 丝印 / `DBGMCU_IDCODE` / 读回 Flash 容量 | 与镜像匹配；C8 与 CB 混用前须书面记录 |

**空板 + ST-Link 的定位**：可用于 **启动链不断电**、**HardFault 初判**、**GDB 读 Phase6 验收计数**（若烧录本仓库构建产物）；**不能**作为 **G1/G2/G3 中「用户可感知与原厂一致」** 或 **终局代替 `ZHIYUN-F100-full.bin`** 的充分条件 —— 终局须 **F100 整机** 或 **Renode 外设模型 + 文书** 闭环，见 [`phase6_behavior_acceptance_checklist.md`](phase6_behavior_acceptance_checklist.md)「终局门禁与空开发板」。

## 门禁分级口径（与验收清单一致）

- **G1（当前门禁）**：可编译 + host test + `Phase6_AcceptanceReport` 通过，确认第一期行为路径可达。
- **G2（补证进行中）**：Renode/GDB 动态快照用于补强关键路径（尤其 `PH6-BHV-005`）。
- **G3（定量/波形类 — 与 `REVERSE_TASK_BOOK.md` §1 对齐）**：**外部仅万用表、无示波器/LA**，**不排期**仪器补强。**光学/占空比/引脚时序** 等以 **固件逆推 + 寄存器影子 + 行为级验收** 文档化；未闭合处保留「待证实」时，**补证路径**须为 **逆向/Renode/GDB**，**不得**挂起等待示波器。

## 模块职责（当前）

- `src/phase6/phase6_scheduler.c`
  - 主循环调度；
  - 聚合硬件输入与软件注入输入；
  - 实机：`PD_PowerAndProtocol_Block` 由 **`SysTick` /20 路径** 触发（见 `phase6_pd_systick_dispatch.c`）；主机验收路径每 `RunOnce` 模拟 20 次 SysTick 入口；
  - 回填 `Phase6_AcceptanceReport`。
- `src/phase6/phase6_pd_systick_dispatch.c`
  - `SysTick_Handler` → `Phase6_PD_PeriodicDispatchFromSysTick`（`/20` 分频 + USB guard + `PD_PowerAndProtocol_Block`）；
  - 导出 `g_phase6_systick_entry_count`（GDB/Renode 证据）。
- `src/phase6/phase6_input_adapter.c`
  - 输入硬件最小初始化（AFIO/GPIOC/ADC1/TMR4）；
  - 编码器增量（`TMR4->CNT`）、ADC 低通/阈值、PC13 USB 去抖；
  - PB3/PB11 输出口初始化为低电平，供 PD 状态机驱动。
- `src/phase6/phase6_light_pd_skeleton.c`
  - `PD_PowerAndProtocol_Block` 调用序；
  - `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15` + `TMR1_*Halfword_Write`；
  - PD GPIO 读写封装（PC13/PB3/PB11）；
  - CCT/HSI/调光路径与 trace 快照输出。
- `src/phase6/phase6_acceptance.c`
  - PH6-BHV-001~005 软件判定逻辑；
  - USB guard 证据读数与策略验证。
- `src/phase6/phase6_usb_guard.c`
  - 第一期间禁止 USB 固件升级路径（不写 Flash）。
- `src/phase6/phase6_startup_chain.c`
  - 启动链锚点顺序与 trace；
  - `SPI1_StartupSequence` 真实发送 23B 启动帧（仅启动帧，不含运行期全量帧泵）。

## 启动链差距矩阵（回合 P）

对齐锚点：[`05_Full_Reconstruction.md`](../../../Cursor_Knowledge_Base/05_Full_Reconstruction.md) §1 `main` 直接 callee。

| 原厂主链符号（05 §1） | Phase 6 当前落点 | 本轮处理 | 仍缺口 |
|---|---|---|---|
| `InitializeClock_PLLFlashSysTick` | `phase6_startup_chain.c` 中 `InitializeClock_PLLFlashSysTick_Phase6Anchor` | 建立同名语义锚点并纳入启动阶段顺序 | PLL/FMC/SysTick 逐寄存器一致性（待证实） |
| `InitializeGPIOB_ClockAndPinMode` | `InitializeGPIOB_ClockAndPinMode_Phase6Anchor` | 启用 `AFIO/IOPB` 时钟，避免无锚点 GPIO 写 | 全端口初始化细节未对齐 |
| `EnableAHBPeriphClock_SRAM` | `EnableAHBPeriphClock_SRAM_Phase6Anchor` | `RCM->AHBCLKEN` **SRAMEN**（`05` §1） | 仅时钟门控锚点；SRAM 本体行为无额外扩展 |
| `EnableAHBPeriphClock_DMA1` | `EnableAHBPeriphClock_DMA1_Phase6Anchor` | 显式使能 `DMA1` 时钟，补齐启动链语义点 | DMA1 通道全配置仍由后续业务链触发 |
| `SetUsbDeviceContextPointer` | `SetUsbDeviceContextPointer_Phase6Anchor` | 记录 USB 上下文锚点（不进入升级写 Flash） | USB Device 全初始化链未实现 |
| `InitializeUsbControlEndpointDefaults` | `InitializeUsbControlEndpointDefaults_Phase6Anchor` | 增加 EP0 默认包长锚点 | 端点寄存器实配仍缺 |
| `InitializeUsbDescriptorsAfterPwmInit` | `InitializeUsbDescriptorsAfterPwmInit_Phase6Anchor` | 保留“PWM 后 USB 描述符初始化”顺序位置 | 描述符/AES 真实链路未纳入第一期 |
| `main -> PD_PowerAndProtocol_Block` 周期核 | `main.c` + `phase6_scheduler.c` | 明确拆分 `Init` 与 `RunStartupStage`，再进入周期 `RunOnce` | 与原厂更多启动子链仍有差距 |

说明：本矩阵只收录“文书已定义、当前可验证、且不引入新增硬件假设”的改造点；SPI1/OLED 运行期全量帧泵与 USB 升级链继续按第一期边界保持隔离。

## 构建与验收命令

在工程根 `ZHIYUN_F100_Firmware/zhiyun_f100_apm32/` 执行：

1. `chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh`
2. `pio run`
3. `cc -std=c11 -Wall -Wextra -I src test/phase6_ui_event_router_host_test.c src/phase6/phase6_ui_event_router.c -o ./.pio/phase6_ui_event_router_host_test`
4. `./.pio/phase6_ui_event_router_host_test`
5. `cc -std=c11 -Wall -Wextra -DPHASE6_HOST_TEST=1 -I src test/phase6_scheduler_acceptance_host_test.c src/phase6/phase6_scheduler.c src/phase6/phase6_acceptance.c src/phase6/phase6_light_pd_skeleton.c src/phase6/phase6_startup_chain.c src/phase6/phase6_ui_event_router.c src/phase6/phase6_usb_guard.c src/phase6/phase6_usb_device.c src/phase6/phase6_pd_systick_dispatch.c -o ./.pio/phase6_scheduler_acceptance_host_test`
6. `./.pio/phase6_scheduler_acceptance_host_test`

## 最新复验记录（2026-04-06，执行回合 H）

- 构建复验：`pio run` 通过（`RAM 7.0%`、`Flash 3.5%`）。
- 主机测试复验：`phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 均退出码 `0`。
- 策略复验：`PHASE6_USB_FW_UPGRADE_ENABLED` 继续保持 `0`，USB 升级链仍处于第一期隔离态。

## 最新复验记录（2026-04-06，执行回合 I）

- 范式收敛：Phase 6 核心链从裸地址 MMIO 访问收敛到 Geehy SDK 外设结构体字段访问，减少地址散落并提升可维护性。
- 判据收敛：`phase6_acceptance.c` 使用 `PHASE6_STAGE_UI_SPI_FLUSH` 枚举替代阶段魔法数，保证主循环完成判据与代码同源。
- 主机测试兼容：`PHASE6_HOST_TEST` 路径下对 `apm32f10x.h` 采用条件包含，维持 host test 无 SDK 头依赖构建能力。
- 复验结果：`pio run` 与两项 host test 均通过，`PH6-BHV-001~005` 软件门槛保持通过，USB 升级隔离策略未改变。

## 最新复验记录（2026-04-06，执行回合 J/K）

- 动态补证脚本增强：
  - `scripts/renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_PD_STEP_INSN` 与 `PHASE6_TRACE_HELPER_BP`，支持命中 `PD_GPIO_StateMachine` 后分步采样。
  - 在 `logs/renode-phase6/20260406_144126/` 观测到 `pd_gpio_count` 与 `PB3/PB11` 影子由 0 递增到 1。
- 回归断言增强：
  - `test/phase6_scheduler_acceptance_host_test.c` 增加负向断言（初始化阶段不可误判 PASS、USB 注入前 `BHV-005` 不得通过）。
  - 全量复跑 `pio run` + 两项 host test 通过。
- 次要二进制对照落地：
  - 已完成原厂 `dump/ZHIYUN-F100-full.bin` 与 `firmware.bin` 的向量表/镜像长度/关键常量对照；
  - 结果仅用于第二期收紧参考，不改变第一期门禁。

## 最新复验记录（2026-04-06，执行回合 N：文书计划复验）

- **G1**：`./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）；`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`；`PH6-BHV-001~005` 与 `Phase6_AcceptanceReport` 字段对齐未变（见 `phase6_acceptance.c` + 调度 host 测试断言）。
- **G2**：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`（仓库根）→ `logs/renode-phase6/20260406_152442/`；`pd_gpio_count` 与 `PB3`/`PB11` 影子 post-step 由 0 递增至 1。
- **规格**：`05-MMIO-v1` 无修订；`phase6_mmio_mapping.md` 与 `05` §2 顺序对照仍有效。

## 最新复验记录（2026-04-06，执行回合 M：Phase 6 计划收口）

- **G1**：`./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）；`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **G2**：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh` → 产物 `logs/renode-phase6/20260406_150719/`（`LATEST` 指针已更新）；`pd_gpio_count`、`PB3`/`PB11` 影子与 `PH6-BHV-005` 一致。
- **文书**：`phase6_mmio_mapping.md` 增加 `05` §2 顺序对照；`phase6_behavior_acceptance_checklist.md` 增加万用表可及步骤与回合 M 证据；本文件增加 USB 边界与 SPI/OLED 第二期说明。
- **第一期基线**：G1+G2 与文档已对齐；G3 波形类 **非**门禁（任务书 §1）。

## 最新复验记录（2026-04-06，执行回合 L）

- 代码可维护性收敛（行为保持）：
  - `phase6_light_pd_skeleton.c` 将输入事件吸收逻辑抽取为 `Phase6_Input_ApplyPendingEvent`；
  - `phase6_light_pd_skeleton.c` 将 PD GPIO 置位/清位逻辑抽取为 `Phase6_PD_SetOutputs`；
  - `phase6_usb_guard.c` 增补第一期隔离口径注释，继续保持 USB 升级链占位态。
- 动态补证复验：
  - 命令：`TRACE_RUN_SECONDS=4 PHASE6_PD_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物：`logs/renode-phase6/20260406_144847/`
  - 结果：`pd_gpio_count` 与 `PB3/PB11` 影子状态从 0 递增至 1，`PH6-BHV-005` G2 证据复现。
- 编译与测试复验：
  - `./scripts/link_geehy_sdk.sh && pio run` 通过（`RAM 7.0%`、`Flash 3.5%`）；
  - 两项 host test 均退出码 `0`。

## 最新复验记录（2026-04-06，执行回合 P：启动链收敛）

- 启动链重构：
  - 新增 `src/phase6/phase6_startup_chain.c/.h`，将 `05` §1 启动链关键符号以锚点函数串行落地；
  - `main.c` 显式调用 `Phase6_Scheduler_RunStartupStage()`，`scheduler` 保留兜底调用；
  - `phase6_acceptance` 新增启动链状态字段并纳入 `BHV-001` 通过条件。
- 回归结果：
  - `./scripts/link_geehy_sdk.sh && pio run` 通过（`RAM 7.2%`、`Flash 3.7%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- G2 复跑：
  - `TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh` → `logs/renode-phase6/20260406_154347/`；
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh` → `logs/renode-phase6/20260406_154350/`。
- 边界维持：
  - USB 升级 guard 仍为 `PHASE6_USB_FW_UPGRADE_ENABLED=0`；
  - 启动链收敛未引入 `05-MMIO-v1` 外新增 MMIO 语义。

## 最新复验记录（2026-04-06，执行回合 R：终局收敛第一刀）

- 启动链收紧：
  - `phase6_startup_chain.c` 将 `InitializeClock_PLLFlashSysTick_Phase6Anchor` 从 mark-only 改为可核查寄存器序列（HSI/PLL/FMC/SysTick 有界等待）；
  - SPI 低层能力统一为 `Phase6_SPI1_EnsureReady/SetCs/Transmit`，启动帧与运行期全量刷屏共用。
- SPI/OLED + UI 渲染：
  - `phase6_light_pd_skeleton.c` 实装 `UI_SPI1_PumpEightFramebufferSlices` 的运行期 `8×(3B+96B)` 事务；
  - `phase6_ui_event_router.c` 增加描述符池，`UI_ModeRender_PostProcess` 输出 `ui_descriptor_index` 与 `ui_framebuffer_checksum`。
- 灯光一致性收紧：
  - `RGBTriple_PushPattern_TMR2Gated` 新增 `TMR2/DMA` 影子图案推送计数（`tmr2_dma_push_count/last_words`）并作为验收断言。
- USB 数据面基线：
  - 新增 `phase6_usb_device.c/.h`，在 guard 不变前提下提供稳定 `SOF/EP0` 数据面计数；
  - `phase6_acceptance` 将 USB 数据面计数纳入 `PH6-BHV-005` 门槛，仍与写Flash路径隔离。
- 回归结果：
  - `pio run`：`SUCCESS`（`RAM 11.8%`、`Flash 5.1%`）；
  - 两项 host test：退出码 `0`。

## 最新复验记录（2026-04-06，执行回合 S：SPI1/OLED 节拍收敛）

- 运行期帧泵策略收敛：
  - `UI_SPI1_PumpEightFramebufferSlices` 改为“脏帧触发 + 16 tick 保底刷新”，单次触发仍是 `8×(3B+96B)`；
  - 新增 `g_phase6_spi_flush_trigger_count` / `g_phase6_spi_flush_skip_count` 两个节拍计数。
- 脚本链路同步（非 MCP）：
  - `scripts/renode_phase6_capture.sh`、`scripts/renode_phase6_pd_breaktrace.sh` 均加入上述计数读取位点；
  - 本轮采样目录：`logs/renode-phase6/20260406_164208/`、`logs/renode-phase6/20260406_164220/`、`logs/renode-phase6/20260406_164319/`。
- 当前证据结论：
  - `PH6-BHV-005` 的 PD 子路径推进继续可复现（`pd_gpio_count` 与 `PB3/PB11` 影子递增）；
  - 帧泵节拍计数在当前断点窗口仍为 0，属于“待证实（断点位置未跨到 flush 阶段）”，下一步需在 `UI_SPI1_PumpEightFramebufferSlices` 附近追加断点采样。
- 风险边界：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；
  - 本轮不涉及写 Flash。

## 最新复验记录（2026-04-06，执行回合 T：UI flush 函数级断点补证）

- 脚本补强：
  - `scripts/renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_TRACE_FLUSH_BP`（默认开启）与 `PHASE6_FLUSH_STEP_INSN`；
  - 在 `PD_GPIO_StateMachine` 断点取证后，追加命中 `UI_SPI1_PumpEightFramebufferSlices` 并单步采样。
- 执行与产物：
  - `TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 PHASE6_TRACE_FLUSH_BP=1 PHASE6_FLUSH_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
  - `logs/renode-phase6/20260406_172503/`
- 证据结论：
  - `PH6-BHV-005` 的 PD 影子推进继续可复现（`pd_gpio_count`、`PB3/PB11` 由 0 到 1）；
  - 已进入 `UI_SPI1_PumpEightFramebufferSlices -> Phase6_SPI1_Transmit` 路径，证明运行期帧泵执行点可命中；
  - `spi_flush_trigger/skip` 在当前窗口仍为 0，保留“待证实”，需在后续回合扩展 flush 后段采样深度。

## 最新复验记录（2026-04-06，执行回合 U：flush 计数写回点工程化）

- 脚本能力：
  - `renode_phase6_pd_breaktrace.sh` 增加可选开关 `PHASE6_TRACE_FLUSH_COUNTER_BP`（默认关闭）；
  - 可在 `UI_SPI1_PumpEightFramebufferSlices+0x84/+0xA8` 设置断点，分别对应 trigger/skip 计数写回阶段。
- 复验结果：
  - 默认参数复跑可稳定完成，不引入长时间卡住；
  - `PD` 与 `UI flush` 函数级命中保持稳定；
  - 运行期计数跃迁仍待证实，但已从“未知点位”收敛为“已知写回地址 + 可选断点工具”。
- 口径：
  - 本回合仅增强取证路径，不改业务语义，不触及 `05-MMIO-v1` 冻结语义边界。

## 最新复验记录（2026-04-06，执行回合 V：分支可控化尝试）

- 脚本新增分支注入开关（均为可选）：
  - `PHASE6_FORCE_FLUSH_SKIP`、`PHASE6_FORCE_FLUSH_TRIGGER`、`PHASE6_FORCE_SPI_STS_READY`；
  - 目标是减少“等待自然分支”导致的会话不确定性。
- 当前结果：
  - 可稳定命中 `UI_SPI1_PumpEightFramebufferSlices` 并完成寄存器注入；
  - 计数写回断点仍存在会话耗时不确定，需后续继续收敛执行策略（如更短 run 窗口、分步断点序列）。
- 风险边界：
  - 本回合仍未触发任何 USB 写 Flash 路径；
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变。

## 最新复验记录（2026-04-06，执行回合 W：节拍证据闭合）

- 代码与脚本最小收敛：
  - `phase6_light_pd_skeleton.c` 将 `g_phase6_spi_flush_trigger_count++` 前移到 trigger 分支入口（仅可观测时序调整，不改业务事务序）；
  - `renode_phase6_pd_breaktrace.sh` 改为在 flush 断点后二次应用分支强制注入，并将可选计数断点改为软件断点。
- G1 回归：
  - `./scripts/link_geehy_sdk.sh && pio run` 通过（`RAM 11.9%`、`Flash 5.2%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- G2 关键证据（均为仓库根，非 MCP）：
  - 强制 skip：`logs/renode-phase6/20260406_181002/` 观察到 `spi_flush_skip_count=1`；
  - 强制 trigger：`logs/renode-phase6/20260406_181024/` 观察到 `spi_flush_trigger_count=1`。
- 结论：
  - 第二期最小切口已把“帧泵分支计数长期为 0”的阻塞收敛为“trigger/skip 均可复现非零”；
  - USB 升级 guard 继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，未触及写 Flash。

## 最新复验记录（2026-04-06，执行回合 Z：main 周期语义动态可见）

- 脚本增强（非 MCP）：
  - `renode_phase6_capture.sh` 新增 main 周期字段采样：`g_phase6_main_boot_phase_counter8`、`g_phase6_main_iwdg_reload_count`；
  - 新增默认开关 `PHASE6_CAPTURE_BREAK_ON_PD=1`，以 `PD_GPIO_StateMachine` 为采样停机点，避免停在启动链导致“误零值”。
  - `renode_phase6_pd_breaktrace.sh` 同步增加 main 周期字段输出（entry/post-step）。
- 动态证据：
  - `logs/renode-phase6/20260406_184558/`：`main_boot_phase_counter8=1`、`main_iwdg_reload_count=1`，并在 post-step 观测到 `pd_gpio_count=1`、`PB3/PB11=1`；
  - `logs/renode-phase6/20260406_184633/`：在同一快照中同时观测到 main 周期语义计数、`TMR1` 影子与 SPI1 启动 23B 计数。
- 结论：
  - “主循环 Step8/IWDG 语义”从仅静态/host 断言推进到 Renode+GDB 可复现动态证据；
  - USB 升级 guard 保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，仍未涉及写 Flash。

## 最新复验记录（2026-04-06，执行回合 AC：SPI1 启动等待噪声收敛）

- 代码收敛：
  - `phase6_startup_chain.c` 将 SPI1 发送等待调整为“事件优先 + 首次超时降级直发（fallback）”，避免 Renode 下 `wait_timeout_count` 按字节增长。
- 复验结果：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.1%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`：退出码 `0`；
  - `logs/renode-phase6/20260406_191647/`：`startup_tx_count=23`、`startup_done=1`、`last_byte=0xAF`、`wait_timeout_count=1`。
- 与终局等价距离关系：
  - 本轮把“仿真环境每字节超时噪声”收敛为“一次性降级指示”，提升了 SPI1 启动链与 OEM 行为对照时的可判读性；
  - 仍需后续补证 fallback 在真实硬件上的触发条件（待证实）。
- 风险边界：
  - USB 升级 guard 未变（`PHASE6_USB_FW_UPGRADE_ENABLED=0`）；
  - 本轮未引入 USB 写 Flash 路径。

## 最新复验记录（2026-04-06，执行回合 AD：取证链稳定化）

- 脚本基线收敛：
  - `renode_phase6_capture.sh` 与 `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_GDB_TIMEOUT_SECONDS`，用于卡住场景自动回收；
  - `phase6_pd_breaktrace` 默认改为 `PHASE6_PD_SOFT_BP=1`，降低会话不确定性；
  - 两脚本统一输出 flush 节拍上下文字段：`call_ticks` / `oled_last_flush_tick` / `ui_frame_dirty`。
- 复验矩阵（非 MCP）：
  - `logs/renode-phase6/20260406_195008/`：capture 稳定输出启动帧与 PD 入口快照；
  - `logs/renode-phase6/20260406_195020/`：强制 trigger，`spi_flush_trigger_count=1`；
  - `logs/renode-phase6/20260406_195035/`：强制 skip，`spi_flush_skip_count=1`；
  - `PHASE6_PD_BREAK_IGNORE=20` 场景触发超时退出码 `124`，说明硬超时防护生效。
- 与终局等价距离关系：
  - 本轮将“断点卡住导致无产出”的风险收敛为“超时可回收 + 至少产出可复核日志”，并保持 trigger/skip 双分支可重放；
  - `main` 周期计数在当前窗口仍存在零值样本，后周期非零推进仍作为下一最小切口。
- 风险边界：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 继续保持；
  - 本轮未触及 USB 写 Flash。

## 已知待证实项

- `TMR1` 输出链与原厂在预装载/更新节拍上的逐位一致性：**无示波器** — 以 **Ghidra/寄存器写入路径对照 + Renode 寄存器快照** 为主，**不等待**波形实测。
- CCT/HSI 到实际光学输出（亮度、色温）的定量映射系数：**无光学测量条件** — 以 **说明书 + `03`/`05` + 固件表** 逆推，**OI-004** 口径结案。
- 插 USB 场景下 `PC13/PB3/PB11` 与诱骗充电时序：**无 LA/示波器** — 以 **`PD_GPIO_StateMachine` 静态链 + Renode/G2 快照 + 万用表可测的直流/通断** 为限，其余以逆向表述。

## USB 固件升级边界（第一期已确认）

- [`phase6_usb_guard.h`](../src/phase6/phase6_usb_guard.h)：`PHASE6_USB_FW_UPGRADE_ENABLED` **恒为 `0`**；`UsbCtl_RequestDispatch_StateMachine` 类路径仅经 `Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded()` 占位，**不写 Flash**。
- [`phase6_pd_systick_dispatch.c`](../src/phase6/phase6_pd_systick_dispatch.c)：在每次 **`PD_PowerAndProtocol_Block()`** 调用**之前**执行 `Phase6_UsbCtl_RequestDispatch_StateMachine_Guarded()`（与 OEM SysTick 路径上的 PD 周期一致；**非**每主循环一次）。
- **第二期**：若实现官方升级链，须单独任务书条目 + 实机验证；**不得**在未解除 guard 时合并写 Flash 代码路径。

### 第二期：USB 官方升级入口条件与风险（专验前保持 guard）

- **入口条件（未满足前禁止将 `PHASE6_USB_FW_UPGRADE_ENABLED` 置 1）**：
  1. **文书**：`04`/`05` 中 **`UsbCtl_RequestDispatch_StateMachine`**、**AES 工作区**（`InitializeUsbDescriptorsAfterPwmInit` 后 **`AES_InitWorkspaceTables`** 等）、**FMC 解锁/编程序列** 与 **`05` §1** USB 直接 callee 链 **静态闭合**，变更冻结段须 **`05-MMIO-v2`** 级升版。
  2. **二进制**：官方 `ZHIYUN-F100-full.bin` 与构建产物在 **向量表、映像长度、关键常量** 上的对照脚本通过（延续第一期次要对照口径，可按任务书收紧）。
  3. **专验**：**仿真**（Renode + 本机 GDB，**不用** F100 GDB MCP 驱动）证明 **无** 非预期 **Flash 写入**；**实机** 在隔离电源/可恢复刷写环境下完成 **升级流程与回滚**；**任务书 §1** 终局口径书面批准。
  4. **Guard 解除程序**：在 `phase6_usb_guard.h` 中 **显式**将 `PHASE6_USB_FW_UPGRADE_ENABLED` 置 **`1`** 前，须 **PR/任务书** 记录 **preflight_ready** 判定与 **flash_write_attempt_count==0** 的基线快照。
- **风险**：提前解除 guard → 误写 Flash / 设备变砖 / 量产批次污染；与灯控、USB 共享 SRAM 控制块（如 `05` §1 所述 USB 上下文指针与 PWM 后描述符初始化顺序）时须 **同窗回归**；**AES 密钥材料** 若进入 SRAM 须防 **侧信道/日志泄露**（专验条目）。
- **当前执行面**：`PHASE6_USB_FW_UPGRADE_ENABLED` **恒为 0**；[`phase6_usb_guard.c`](../src/phase6/phase6_usb_guard.c) 中 **`blocked_count` / `preflight_*` / `flash_write_attempt_count`** 仅作**策略与验收镜像**，**不**表示已开放升级数据面。
- **Preflight 合并 Gate（代码层）**：任何将 `PHASE6_USB_FW_UPGRADE_ENABLED` 置 `1` 的变更须**同一 PR** 附带：`pio run` 通过、`phase6_*` host test 通过、Renode+GDB 脚本在 **`flash_write_attempt_count==0`** 基线下复跑、以及任务书 **§1 终局**书面批准（与上表「入口条件」一致）。
- **2026-04-07 文书复核（回合 AN）**：[`phase6_usb_guard.h`](../src/phase6/phase6_usb_guard.h) 仍为 **`PHASE6_USB_FW_UPGRADE_ENABLED (0)`**；本轮**未**解除 guard、**未**新增写 Flash 路径；专验入口条件与上表一致，变更须先任务书批准。

### Renode G2 与主循环门限（专验用构建，非量产默认）

- **[`platformio.ini`](../platformio.ini)**：`[env:apm32f103cb_renode_g2]` 通过 **`-DPHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=1`** 生成固件，用于 **`scripts/renode_phase6_capture.sh`** 在 **`PD_GPIO` 首次断点** 窗口读出 **非零** `main_boot_phase_counter8` / `main_iwdg_reload_count`（缩短 G2 取证与 **100 次 `RunOnce`** 的耦合）。
- **量产默认**：`env:apm32f103cb` 仍使用源码中 **默认 `100`**，与 Ghidra **`tick_delta > 99`** 行为级对齐；**不得**将 `renode_g2` 镜像作为「已等价原厂 IWDT 节拍」的声明依据。

## SPI1 / OLED 全量帧泵（第一期仅启动帧落地）

- 第一期验收以 **`05` §2** 调用序到达、`UI_SPI1_PumpEightFramebufferSlices` 阶段计数与 **G1/G2** 证据为主；
- `SPI1_StartupSequence`（23B）已在启动链落地并可做 G2 计数补证；运行期 **8×(3 B+96 B)** 全量刷屏与原厂二进制一致仍属 **第二期** 收紧项。
- 规格锚点：[`04_Protocol_Reverse.md`](../../../Cursor_Knowledge_Base/04_Protocol_Reverse.md) §2.3/§2.4；[`05_Full_Reconstruction.md`](../../../Cursor_Knowledge_Base/05_Full_Reconstruction.md) §5.1 **SPI1** 行。
- 增量实现时：新文件模块承载，避免 `phase6_light_pd_skeleton.c` 单文件膨胀。

### Ghidra 对照（SPI1 双路径，只读备忘）

- **`SPI1_PumpEightFramebufferSlices`**（`0x08004258`）直接 callee：**`SPI1_Send3ByteSlicePrefixForChannel`**、**`SPI1_SendBlockWithCS_GPIOAlt`** — 与 [`02_Hardware_Init.md`](../../../Cursor_Knowledge_Base/02_Hardware_Init.md) 运行期事务表一致。
- **`UI_SPI1_PumpEightFramebufferSlices`**（`0x08009492`）：8×（3 B + 0x60）循环，入口受 **`*DAT_0800980c`** 门控；循环内 **`FUN_08009e06`**（定长字节发送）、**`FUN_0800a028`/`FUN_0800a038`**（片选线）、**`thunk_FUN_0800a162`** — 与 **`main`** 路径帧泵 **对仗**（`05` §2）。细节与 MCP 取证见 [`REVERSE_TASK_BOOK_FINDINGS.md`](../../../Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md) **§2026-04-06（Ghidra MCP：`SPI1` / `UI_SPI1` 帧泵 callee 对照）**；**禁止**将反编译整段粘贴为产品源码。

## 建议二次开发顺序

1. 在不改变 `phase6_usb_guard` 前提下扩展灯光算法（优先 CCT/HSI 参数表）。
2. 逐步替换 UI/OLED 占位实现，但保持 `UI_OLED_MainStateMachine` 调用序不变。
3. 每次改动都复跑 `PH6-BHV-001~005`，失败时先回归 `phase6_mmio_mapping` 对照。
4. 若出现规格空洞，再触发 Ghidra/F100-GDB 补证并回写 `03`~`05` 或 FINDINGS。

## 最新复验记录（2026-04-06，执行回合 AE：终局收敛六项待办并行闭环）

- **启动链与 `main`（Todo#2）**：
  - `phase6_startup_chain` 继续承接 `05` §1 锚点，并把 USB context/EP0/descriptor 初始化同步到 `phase6_usb_device`；
  - `Phase6_UsbDevice_CompleteInitFromContext()` 在 guard 下完成 Device 数据面就绪，不触及升级写 Flash。
- **灯光引擎 `TMR3 + DMA1/TMR2`（Todo#3）**：
  - `phase6_light_pd_skeleton.c` 新增 `Init_TMR3_PWM_FanPB5()` 与 `Init_DMA1Ch1_TMR2CcrDMA()`；
  - `RGBTriple_PushPattern_TMR2Gated()` 现在同时驱动 `TMR2->CC3`、`DMA1_Channel1` 与 `TMR3->CC2` 影子，新增验收字段 `tmr3_pwm_apply_count` / `tmr3_cc2_shadow` / `dma1_ch1_transfer_count`。
- **UI/OLED 与 SPI 帧泵（Todo#1/#4）**：
  - 保持运行期 `UI_SPI1_PumpEightFramebufferSlices` 的 trigger/skip 节拍统计；
  - 通过 `logs/renode-phase6/20260406_201541/` 再次命中 flush trigger 分支，`spi_flush_trigger_count=1`。
- **USB Device + guard（Todo#5）**：
  - `phase6_usb_device` 新增 `context_ptr_shadow`、`ep0_max_packet_shadow`、`descriptors_ready`；
  - `PHASE6_USB_FW_UPGRADE_ENABLED` 继续保持 `0`，无写 Flash 路径引入。
- **终局验收扩展（Todo#6）**：
  - `phase6_scheduler_acceptance_host_test` 新增 TMR3/DMA 与 USB context 就绪断言；
  - 构建与两项 host test 全部通过：`pio run`（`RAM 12.3%`、`Flash 5.8%`），`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。

## 最新复验记录（2026-04-06，执行回合 AH：终局收敛计划落地）

- **PD / SysTick 调度（`04` §3.5.1）**：
  - 新增 [`phase6_pd_systick_dispatch.c`](../src/phase6/phase6_pd_systick_dispatch.c)：`SysTick_Handler` → `Phase6_PD_PeriodicDispatchFromSysTick`，每 **20** 次 SysTick 入口调用一次 `PD_PowerAndProtocol_Block`（与 OEM 分频一致）；`g_phase6_pd_irq_ready` 在启动链完成后置位，避免过早进入 PD；
  - 主机验收：`PHASE6_HOST_TEST` 下每 `RunOnce` **模拟 20 次** SysTick 入口，保持 **1 PD / 迭代** 与既有 `PH6-BHV-*` 断言兼容；
  - `Phase6_LightPd_TraceSnapshot` 增加 `systick_entry_count`、`pd_call_ticks`；`scripts/renode_phase6_pd_breaktrace.sh` 与 `renode_phase6_capture.sh` 输出 `g_phase6_systick_entry_count`。
- **启动链（`05` §1）**：
  - `EnableAHBPeriphClock_SRAM_Phase6Anchor`：`RCM->AHBCLKEN` **SRAMEN**（`PHASE6_STARTUP_STEP_AHB_SRAM`）。
- **验证**：`pio run` `SUCCESS`；`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- **USB**：`phase6_usb_guard` 未解除；无写 Flash。

## 最新复验记录（2026-04-06，执行回合 AF：A/B/C/D 全待办闭环）

- **阶段 A（启动链）**：
  - `phase6_startup_chain` 新增 `CheckMagicWord55AA` + `IWDT` 启动锚点，补齐 `05` §1 的 `magic -> iwdt` 语义；
  - 新增 `magic55aa_*`、`iwdt_*` trace 字段，纳入 `Phase6_AcceptanceReport`。
- **阶段 B（SPI1 main/PD 双路径）**：
  - `phase6_scheduler` 周期触发 `Phase6_MainLoop_RequestSpiPump`；
  - `UI_SPI1_PumpEightFramebufferSlices` 在保留旧逻辑基础上消费 main 请求，形成双触发路径且保持同一事务后端。
- **阶段 C（业务核验收收紧）**：
  - `phase6_acceptance` 新增 `mainloop_spi_pump_*` 与启动链补强字段；
  - `phase6_scheduler_acceptance_host_test` 新增断言并通过。
- **阶段 D（USB guard 专验准备，不解除 guard）**：
  - `phase6_usb_guard` 新增 `preflight_ready/preflight_fail_count/flash_write_attempt_count`；
  - guard 继续 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，维持零写 Flash。
- **验证结果**：
  - `pio run`：`SUCCESS`（`RAM 12.8%`、`Flash 6.1%`）；
  - host tests：均退出码 `0`；
  - Renode 证据：`logs/renode-phase6/20260406_203926/`、`logs/renode-phase6/20260406_203941/`。
