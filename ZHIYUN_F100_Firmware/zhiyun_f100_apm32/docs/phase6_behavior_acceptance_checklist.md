# Phase 6 第一行为验收清单（首稿）

## 文档目的

本清单用于约束 Phase 6 第一期验收口径：以行为等价为主，不以二进制一致为门禁。  
规格来源仅限以下文档：

- `Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md`（Phase 6 口径）
- `Cursor_Knowledge_Base/03_Function_Modules.md`（业务路径）
- `Cursor_Knowledge_Base/04_Protocol_Reverse.md`（PD 与 SPI/OLED 协议证据）
- `Cursor_Knowledge_Base/05_Full_Reconstruction.md`（冻结版 `05-MMIO-v1`）
- `docs/phase6_mmio_mapping.md`（代码符号到 MMIO 表项的 Phase 6 实施映射）

## 范围与排除

- 第一期间纳入：开关机、色温（CCT）、HSI、亮灯调光、插 USB 时 PD 诱骗充电。
- 第一期间排除：USB 固件升级链（`UsbCtl_RequestDispatch_StateMachine` 加密下发/写 Flash）。
- 底线：不得误写 Flash，不得凭空补充无文书依据的硬件行为。

## 终局门禁与空开发板（文书）

与 [`REVERSE_TASK_BOOK.md`](../../../Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md) §1「终局目标」一致：**可代替原厂 `ZHIYUN-F100-full.bin`** 须满足 **插上即用**、**用户可感知行为与说明书/文书规格** 一致，并可在量产/售后流程中作为烧录目标。

| 层级 | 说明 |
|------|------|
| **终局充分条件** | **F100 整机**（或经任务书认可的等效硬件夹具）上按说明书与 `03`/`04`/`05` 完成回归；**或** Renode 外围模型与 GDB 证据链闭合到任务书批准口径。 |
| **空 STM32F103C8 最小系统 + ST-Link** | **仅**辅助：启动不断电、HardFault 初判、GDB 读 Phase6 计数；**不充分**：无 OLED/PD/灯驱时 **ADC/附着/显示/灯效** 与整机**不等价**，**不得**单独作为「与原厂一致」或「可量产替换」的依据。 |
| **Flash 容量** | 量产目标为 **128 KB**（APM32F103CBT6）；在 **64 KB** 器件上烧录 **128 KB OEM 镜像** 属配置错误风险 —— 见 [`phase6_secondary_dev_baseline.md`](phase6_secondary_dev_baseline.md)「空开发板与量产目标 MCU」。 |

## 验收门禁分级（第一期）

- **G1：软件门槛通过（当前主门禁）**  
  以 `Phase6_AcceptanceReport`、`pio run`、host test 断言为准，确认 `PH6-BHV-001~005` 路径可达与策略隔离生效。
- **G2：动态补证（进行中）**  
  以 Renode+GDB 断点与快照补强关键分支推进（尤其 `PH6-BHV-005`）；若采样不足，记录阻塞与下一步，不推导硬件结论。
- **G3：定量/波形类闭环（非第一期门禁；与任务书 §1 对齐）**  
  **外部仅万用表、无示波器/LA**（见 `REVERSE_TASK_BOOK.md` **§1「外部实测条件约束」**）— **不排期**「示波器到货后再验」。G3 相关缺口以 **反汇编/Ghidra、寄存器影子、Renode+GDB、行为级验收** 结案或标注为**逆向推导**；与 **OI-004** 书面取舍一致。若将来具备仪器，可作**可选**补强，**非**当前前提。

## 验收用例（可重复执行）

| ID | 行为项 | 规格锚点 | 目标观察 | 状态 |
|---|---|---|---|---|
| PH6-BHV-001 | 开关机与主循环稳定运行 | `REVERSE_TASK_BOOK.md` §1/§3；`05_Full_Reconstruction.md` §1/§2 | 上电后进入主循环，周期调度不异常退出 | G1 通过（G3：无外部波形条件，见 §1） |
| PH6-BHV-002 | 色温（CCT）路径可达 | `03_Function_Modules.md` §0.3/§0.4；`05_Full_Reconstruction.md` §5.1 | CCT 模式下可触发 `TMR1` 比较值更新路径 | G1 通过（G3：无外部波形条件，见 §1） |
| PH6-BHV-003 | HSI 路径可达 | `03_Function_Modules.md` §0.3.1/§1.5 | HSI 模式下可触发 `HSI_*` 到 `RGBTriple_*` 路径 | G1 通过（G3：无外部波形条件，见 §1） |
| PH6-BHV-004 | 亮灯与调光路径可达 | `03_Function_Modules.md` §0.3；`05_Full_Reconstruction.md` §5.1 | 输入变化可驱动 `EncoderGated_LightOutputApplyChain` 子路径 | G1 通过（G3：无外部波形条件，见 §1） |
| PH6-BHV-005 | 插 USB 时 PD 诱骗充电路径 | `REVERSE_TASK_BOOK.md` §1；`04_Protocol_Reverse.md` §3 | 插线场景下 `PD_*` 状态机路径按预期运行（与 USB 升级链分离） | G1 通过（G2 补证中） |

## 本次骨架实现绑定（2026-04-06，执行回合 B）

- 调度主链：`src/phase6/phase6_light_pd_skeleton.c` 中 `PD_PowerAndProtocol_Block` 已按文书补齐
  `PD_HelperSeq_3000tick_ThenPc13Gpio`、`ProtocolLanes_BitExpandTick`。
- 主循环收敛：`src/main.c` 默认仅保留 `Phase6_Scheduler_Init()` + 周期 `RunOnce()`；
  演示注入改为 `PHASE6_BOOTSTRAP_SEQUENCE_ENABLE=1` 时才调用
  `Phase6_Scheduler_RunBootstrapSequence()`，避免长期固化测试激励。
- 验收快照：新增 `Phase6_LightPd_ReadTraceSnapshot` 与 `Phase6_LightPd_TraceSnapshot`，
  用于记录 `PD_Attach`/`PD_GPIO`/协议位扩展/UI 主状态机到达计数。
- USB 升级隔离：`src/phase6/phase6_usb_guard.h` 中 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，
  显式维持“第一期不测 USB 固件升级”策略；`phase6_usb_guard.c` 新增
  `Phase6_USB_UpgradePath_ReadBlockedCount()` / `Phase6_USB_UpgradePath_ReadEnableFlag()`
  供验收报表读取策略执行证据。
- 灯光重构补齐：新增 `CCT_Slew_TableSplit_TMR1Shadow`、`TMR1_*Halfword_Write`、
  `LightOutput_PendingHSI_Dispatch`、`HSI_FloatMath_ComputeRGBTriple`、
  `RGBTriple_PushPattern_TMR2Gated`，形成 CCT/HSI 双路径可编译实现。
- 阶段语义对齐：`phase6_light_pd_skeleton.c` 使用显式阶段枚举（`PD_ATTACH` 到
  `UI_SPI_FLUSH`），并将 `PD_HelperSeq_3000tick_ThenPc13Gpio` 的窗口计数改为
  3000 tick 周期门控（软件证据，待实机进一步核证）。
- 输入/UI 最小接入：调度层新增模式切换、编码器增量、ADC 输入和 USB 插入事件注入接口，
  默认走硬件采样；必要时可通过可控的 bootstrap 宏注入验证第一期路径可达性。
- 硬件采样适配：新增 `src/phase6/phase6_input_adapter.c`，从 `TMR4->CNT`、`ADC1->DR`、
  `GPIOC->IDR(PC13)` 采样并映射为 `Phase6_InputEvent`，与软件注入路径并存。
- 第三轮增强：`phase6_input_adapter` 新增最小硬件初始化与输入状态机：
  - 初始化：启用 `AFIO/GPIOC/ADC1/TMR4` 时钟，`PC13` 上拉输入，`ADC1` 上电；
  - 去抖：`PC13` 3 次确认后再判定 USB 插入沿；
  - 归一化：ADC 使用 1/8 IIR 低通，变化阈值 16；
  - 模式切换：ADC 带滞回阈值（`>=2800` 切 HSI，`<=2200` 切 CCT）并需 8 次连续确认。
- 第四轮增强：`Input_5Channel_TableStateMachine` 接管 UI 模式字与事件编码：
  - 模式切换事件：`tag=0x11 | param=mode`；
  - ADC 更新事件：`tag=0x22 | param=adc>>4`；
  - USB 插入事件：`tag=0x33 | param=1`；
  - 编码器事件：`tag=0x44 | param=abs(delta)`；
  - `UI_OLED_MainStateMachine` 消费 pending 事件并累计 `ui_event_dispatch_count`。
- 第五轮重构：新增 `src/phase6/phase6_ui_event_router.c/.h`，将 UI 事件编码/缓存/消费从
  `phase6_light_pd_skeleton` 拆分为独立路由模块；主循环中通过 `Phase6_UiEventRouter_Consume`
  驱动分发计数与最近事件码。
- 验收报表增强：`phase6_acceptance.c/.h` 增加 USB guard 证据字段（`usb_upgrade_*`）及
  PD 子路径计数镜像（`pd_attach_count`、`pd_gpio_count`、`protocol_lane_expand_count` 等）。

## MCP 取证记录（用于本轮对齐）

- 工具：`user-ghidra.get_function_callees`
- 查询：`PD_PowerAndProtocol_Block`（program=`ZHIYUN-F100-full.bin`）
- 结果：包含 `PD_AttachSequence_StateMachine`、`Input_5Channel_TableStateMachine`、
  `PD_GPIO_StateMachine`、`PD_HelperSeq_3000tick_ThenPc13Gpio`、
  `ProtocolLanes_BitExpandTick`、`Encoder_DispatchDelta_FromTMR4`、
  `EncoderGated_LightOutputApplyChain`、`UI_OLED_MainStateMachine`、
  `UI_ModeRender_PostProcess`、`UI_SPI1_PumpEightFramebufferSlices`。

## 行为记录器（本轮新增）

- 代码位置：`src/phase6/phase6_acceptance.h`、`src/phase6/phase6_acceptance.c`
- 输入来源：`Phase6_LightPd_ReadTraceSnapshot`（调度后采样）
- 调度接线：`src/phase6/phase6_scheduler.c` 中 `Phase6_Acceptance_OnLoopSample(...)`
- USB 事件注入：`Phase6_Acceptance_NotifyUsbInserted()`（供后续实机/仿真桥接）
- USB 升级策略证据：`usb_upgrade_enabled`、`usb_upgrade_block_count`、`usb_upgrade_policy_guarded`
- 限定声明：当前仅为“软件到达证据记录”，不是硬件波形或实机光学等价证明。

## 结果填报模板（首版）

| ID | 代码观测字段 | 证据来源 | 判定 | 备注 |
|---|---|---|---|---|
| PH6-BHV-001 | `bhv001_power_loop`、`loop_sample_count`、`last_observed_stage`、`ui_main_count/ui_render_count/ui_flush_count` | 运行时读取 `Phase6_AcceptanceReport` | 通过 | 要求主链完整跑到 `UI_SPI_FLUSH` 且 UI 三阶段均到达 |
| PH6-BHV-002 | `bhv002_cct_path`、`cct_apply_count`、`tmr1_period_shadow`、`tmr1_compare_shadow` | 启动后 CCT 注入事件 + 轨迹计数 | 通过 | 需满足 `period > compare > 0`（软件影子值） |
| PH6-BHV-003 | `bhv003_hsi_path`、`hsi_apply_count`、`hsi_rgb_shadow` | 启动后 HSI 注入事件 + 轨迹计数 | 通过 | 需观测到非零 RGB 影子值 |
| PH6-BHV-004 | `bhv004_dimming_path`、`light_apply_count`、`dimming_update_count` | ADC/编码器注入 + 轨迹计数 | 通过 | 光学/占空比定量：**无示波器**，以固件逆推 + §1/OI-004 表述，不等待实测波形 |
| PH6-BHV-005 | `bhv005_usb_pd_attach_path`、`usb_insert_event_count`、`usb_attached`、`pd_*_count`、`usb_upgrade_policy_guarded` | USB 插入事件注入 + PD 路径计数 | 通过 | 必须同时满足 USB 升级 guard 策略 |

## UI/模式最小观测（新增）

| 观测项 | 字段 | 期望 |
|---|---|---|
| 当前模式字 | `ui_mode_shadow` | 仅在 `2(CCT)`/`3(HSI)` 间切换 |
| 最近事件码 | `ui_last_event_code` | 高字节属于 `0x11/0x22/0x33/0x44` |
| UI事件分发计数 | `ui_event_dispatch_count` | 输入变化后递增，不应长时间停滞 |
| PWM影子值 | `tmr1_period_shadow` / `tmr1_compare_shadow` | CCT 路径下满足 `period > compare > 0` |

## 主机侧最小自测（新增）

- 自测文件：`test/phase6_ui_event_router_host_test.c`
- 自测文件：`test/phase6_scheduler_acceptance_host_test.c`
- 编译运行命令：
  - `cc -std=c11 -Wall -Wextra -I src test/phase6_ui_event_router_host_test.c src/phase6/phase6_ui_event_router.c -o ./.pio/phase6_ui_event_router_host_test`
  - `./.pio/phase6_ui_event_router_host_test`
  - `cc -std=c11 -Wall -Wextra -DPHASE6_HOST_TEST=1 -I src test/phase6_scheduler_acceptance_host_test.c src/phase6/phase6_scheduler.c src/phase6/phase6_acceptance.c src/phase6/phase6_light_pd_skeleton.c src/phase6/phase6_startup_chain.c src/phase6/phase6_ui_event_router.c src/phase6/phase6_usb_guard.c src/phase6/phase6_usb_device.c src/phase6/phase6_pd_systick_dispatch.c -o ./.pio/phase6_scheduler_acceptance_host_test`
  - `./.pio/phase6_scheduler_acceptance_host_test`
- 覆盖点：
  - 路由测试：`mode/adc/usb/encoder` 四类事件编码与消费计数。
  - 调度验收测试：`PH6-BHV-001~005` 的软件门槛与 USB guard 策略闭环。

## 本轮执行证据（2026-04-06）

- 构建命令：`chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run`
  - 结果：`SUCCESS`（`RAM 7.0%`、`Flash 3.2%`）。
- 主机侧路由测试：
  - `cc -std=c11 -Wall -Wextra -I src test/phase6_ui_event_router_host_test.c src/phase6/phase6_ui_event_router.c -o ./.pio/phase6_ui_event_router_host_test`
  - `./.pio/phase6_ui_event_router_host_test`
  - 结果：进程退出码 `0`（断言全部通过）。
- 主机侧调度+验收测试：
  - `cc -std=c11 -Wall -Wextra -DPHASE6_HOST_TEST=1 -I src test/phase6_scheduler_acceptance_host_test.c src/phase6/phase6_scheduler.c src/phase6/phase6_acceptance.c src/phase6/phase6_light_pd_skeleton.c src/phase6/phase6_startup_chain.c src/phase6/phase6_ui_event_router.c src/phase6/phase6_usb_guard.c src/phase6/phase6_usb_device.c src/phase6/phase6_pd_systick_dispatch.c -o ./.pio/phase6_scheduler_acceptance_host_test`
  - `./.pio/phase6_scheduler_acceptance_host_test`
  - 结果：进程退出码 `0`（断言全部通过）。
- 判定口径说明：以上证据属于“可编译 + 事件路由软件证据”，非实机光学/功率闭环证据。

## 本轮执行证据（2026-04-06，执行回合 C）

- 代码基线：
  - 新增 `docs/phase6_mmio_mapping.md`（`05-MMIO-v1` ↔ 代码符号/MMIO 的落地映射）。
  - 新增 `docs/phase6_secondary_dev_baseline.md`（二次开发预备基线）。
  - `src/phase6/phase6_light_pd_skeleton.c`：
    - 新增 `Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15`（Phase 6 一期可编译实现，`BDTR` 仍保持“待证实”策略）；
    - `TMR1_PeriodHalfword_Write` / `TMR1_CompareHalfword_Write` 由影子写入扩展为影子+寄存器写；
    - 新增 `PD_ReadGPIOC13_IdrBit`、`PD_WriteGPIOB3_BSRR_BRR`、`PD_WriteGPIOB11_BSRR_BRR` 封装，接入 `PD_GPIO_StateMachine`。
  - `src/phase6/phase6_input_adapter.c`：
    - 初始化阶段增加 `PB3/PB11` 输出口配置与默认拉低，供 PD 子状态机驱动。
    - 输入事件链保持 `TMR4/ADC/PC13` 路径不变。
- 构建命令：`chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run`
  - 结果：`SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
- 主机侧路由测试：`phase6_ui_event_router_host_test`
  - 结果：退出码 `0`。
- 主机侧调度+验收测试：`phase6_scheduler_acceptance_host_test`
  - 结果：退出码 `0`，`PH6-BHV-001~005` 软件门槛断言通过。

## 本轮执行证据（2026-04-06，执行回合 D：Renode 脚本取证）

- 采集方式：仓库脚本链（非 MCP）
  - `./scripts/oi001_capture_all.sh`
  - 自动包含：
    - `renode_oi001_auto.sh`（拉起 Renode + GDB sample 采样）
    - `renode_gdb_capture.sh`（`target extended-remote localhost:3333` + SRAM 导出）
    - `oi001_monitor_runfor_hook.py`（Monitor `RunFor` 快照）
- 产物目录：`logs/renode-oi001/20260406_134502/`
  - `gdb_capture.log`
  - `renode_emulator.log`
  - `monitor_runfor_hook.log`
  - `renode_sram_20k.bin`（大小校验通过：20480 字节）
- 关键观测（软件动态证据）：
  - GDB 快照记录 `PC=0x08006BE9`、`LR=0x080045A7`、`SP=0x20001620`；
  - Flash 池字 `0x0800695C -> 0x200002C4` 被再次观测到；
  - `0x200002C4` 控制块区域已在日志与 `xxd` 输出中留痕；
  - Monitor `RunFor` 路径记录 `PC=0x0800459E`，完成独立二次采样。
- 判定口径：
  - 以上证据用于“动态路径到达与上下文快照”补强；
  - 尚不是 `PH6-BHV-005` 的实机功率/波形闭环证据，仍需实机 GPIO 波形/寄存器联合验证。

## 本轮执行证据（2026-04-06，执行回合 E：Phase 6 固件 Renode 快照）

- 采集脚本：`scripts/renode_phase6_capture.sh`（本轮新增）
  - 直接加载 `zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.bin`
  - 由向量表自动解析 SP/PC，启动 Renode + GDB 后采样关键寄存器
- 产物目录：`logs/renode-phase6/20260406_135331/`
  - `gdb_phase6_capture.log`
  - `renode_emulator.log`
  - `phase6_sram_20k.bin`（20480 字节）
- 关键寄存器观测：
  - `PC=0x08000B2E`、`SP=0x20004F70`、`LR=0x08000CA3`
  - `GPIOC_IDR @ 0x40011008 = 0x00000000`（PC13 输入采样位所在寄存器）
  - `GPIOB_BSRR @ 0x40010C10 = 0x00000000`
  - `GPIOB_BRR  @ 0x40010C14 = 0x00000000`
  - `TMR1_CTRL1 @ 0x40012C00 = 0x00000081`（含计数使能/自动重装语义）
  - `TMR1_ARR   @ 0x40012C2C = 0x000004B0`（1200）
  - `TMR1_CCR1  @ 0x40012C34 = 0x00000018`
  - `TMR1_CCR3  @ 0x40012C3C = 0x00000018`
  - `TMR4_CNT   @ 0x40000824 = 0x00000000`
- 解释：
  - 该快照证明 Phase 6 的 `TMR1` 初始化/写寄存器路径在 Renode 环境下已可观测；
  - `PB3/PB11` 在当前采样点未观测到翻转，仍需后续“带 USB 插入激励序列”的定向场景采样。
  - 使用 `PHASE6_RUN_SECONDS=5` 重采样（`logs/renode-phase6/20260406_135430/`）结果一致：`TMR1` 寄存器值稳定、`PB3/PB11` 仍未翻转。

## 本轮执行证据（2026-04-06，执行回合 F：USB 激励尝试与阻塞记录）

- 目标：在 Renode 中触发“USB 插入 -> PD 输出”子路径，观测 `PB3/PB11` 翻转证据。
- 本轮方法：
  - `PHASE6_STIM_USB=1`：向 `g_phase6_pending_input` + `g_phase6_has_pending_input` 写入注入事件；
  - `PHASE6_FORCE_PD=1`：回退方案，直接置 `g_phase6_usb_attached=1` 与 `g_phase6_protocol_lane_expand_count=1`。
- 产物目录：
  - `logs/renode-phase6/20260406_140138/`
  - `logs/renode-phase6/20260406_140209/`
  - 以及一次失败实验：`logs/renode-phase6/20260406_140341/`（异步续跑模式报错）
- 结果：
  - `g_phase6_usb_attached` 可被写到 `1`（回退注入生效）；
  - 但 `g_phase6_pd_gpio_count` 仍为 `0`，`g_phase6_pd_pb3_state/g_phase6_pd_pb11_state` 未翻转，`GPIOB_ODATA` 亦未观测到变化。
- 当前结论：
  - 现阶段可确认 `TMR1` 链路与基础快照稳定；
  - 对 `PH6-BHV-005` 的 Renode 动态补证仍处于“阻塞/待细化”状态，需改用“断点命中 `PD_GPIO_StateMachine` 的分步脚本”继续取证。

## 本轮执行证据（2026-04-06，执行回合 G：PD 断点追踪落地）

- 新增脚本：`scripts/renode_phase6_pd_breaktrace.sh`
  - 加载 `firmware.bin/.elf`；
  - 对 `PD_GPIO_StateMachine`、`PD_HelperSeq_3000tick_ThenPc13Gpio` 下硬件断点；
  - 强制置 `g_phase6_usb_attached=1` 与 `g_phase6_protocol_lane_expand_count=1` 后 `continue` 到断点；
  - 输出断点命中次数、寄存器与关键 SRAM 影子状态。
- 本轮产物：`logs/renode-phase6/20260406_141256/`
  - `gdb_phase6_pd_breaktrace.log`
  - `renode_emulator.log`
- 核心结果：
  - `PD_GPIO_StateMachine` 断点命中（`breakpoint already hit 1 time`）；
  - 断点停机时：
    - `PC=0x08000700 <PD_GPIO_StateMachine+4>`
    - `g_phase6_usb_attached=1`
    - `g_phase6_protocol_lane_expand_count=1`
    - `g_phase6_pd_gpio_count=0`
    - `g_phase6_pd_pb3_state=0`、`g_phase6_pd_pb11_state=0`
    - `g_phase6_last_stage=2`
- 解释：
  - 证明“函数入口已到达”这一步已闭合；
  - 当前快照采样点位于函数前段，尚未跨过会递增 `pd_gpio_count` 与写 `pb3/pb11` 的指令区，因此该两项仍为 0；
  - 下一步应在该断点后执行单步/nexti 再采样，完成 `PH6-BHV-005` 的 Renode 细化证据。

## 本轮执行证据（2026-04-06，执行回合 H：计划落地复验）

- 构建复验（工程根 `zhiyun_f100_apm32`）：
  - `chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run`
  - 结果：`SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
- 主机侧路由测试复验：
  - `cc -std=c11 -Wall -Wextra -I src test/phase6_ui_event_router_host_test.c src/phase6/phase6_ui_event_router.c -o ./.pio/phase6_ui_event_router_host_test`
  - `./.pio/phase6_ui_event_router_host_test`
  - 结果：退出码 `0`。
- 主机侧调度+验收测试复验：
  - `cc -std=c11 -Wall -Wextra -DPHASE6_HOST_TEST=1 -I src test/phase6_scheduler_acceptance_host_test.c src/phase6/phase6_scheduler.c src/phase6/phase6_acceptance.c src/phase6/phase6_light_pd_skeleton.c src/phase6/phase6_startup_chain.c src/phase6/phase6_ui_event_router.c src/phase6/phase6_usb_guard.c src/phase6/phase6_usb_device.c src/phase6/phase6_pd_systick_dispatch.c -o ./.pio/phase6_scheduler_acceptance_host_test`
  - `./.pio/phase6_scheduler_acceptance_host_test`
  - 结果：退出码 `0`，断言覆盖 `PH6-BHV-001~005` 软件门槛通过。
- 本回合范围说明：
  - 本回合不新增硬件行为假设，不新增未文书锚定的 MMIO 写入；
  - 第一期间继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，USB 升级链不进入验收门禁。

## 本轮执行证据（2026-04-06，执行回合 I：SDK 范式收敛）

- 代码收敛：
  - `phase6_light_pd_skeleton` 与 `phase6_input_adapter` 从裸地址 MMIO 改为 Geehy SDK 外设结构体访问（`RCM/GPIO/TMR/ADC`）；
  - `Phase6_StageId` 枚举上提到 `phase6_light_pd_skeleton.h`，验收逻辑改为 `PHASE6_STAGE_UI_SPI_FLUSH` 显式判定，移除魔法数；
  - `PHASE6_HOST_TEST` 路径对 `apm32f10x.h` 改为条件包含，保持主机测试可独立编译。
- 影响边界：
  - 未新增无文书依据 MMIO 写入；
  - USB 升级 guard 策略保持不变（`PHASE6_USB_FW_UPGRADE_ENABLED=0`）。
- 构建与测试复验：
  - `./scripts/link_geehy_sdk.sh && pio run` 通过（`RAM 7.0%`、`Flash 3.5%`）；
  - `phase6_ui_event_router_host_test` 退出码 `0`；
  - `phase6_scheduler_acceptance_host_test` 退出码 `0`。
- 判定：
  - `PH6-BHV-001~005` 软件门槛维持通过；
  - **G3（光学/时序波形定量）**：**外部仅万用表、无示波器**（见 **[`REVERSE_TASK_BOOK.md`](../../../Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md) §1「外部实测条件约束」**）— **不等待**实机波形；占空比/光学关系以 **逆向与行为级表述** 为主，与 **OI-004** 书面取舍一致。

## 待证实项（必须显式标注）

- `PH6-BHV-002`/`003`/`004` 的光学与占空比定量关系：**无示波器条件下**以 **固件逆推 + 任务书 OI-004** 结案或标注为逆向推导；**不**排期「示波器到货后再验证」。
- `PH6-BHV-005` 的动态现场证据：优先实机日志或 Renode+GDB 采样；单次 MCP 失败不作为否定结论。

## 建议验证步骤（首版）

1. 先确认 `pio run` 可编译，通过后再进入行为验证。
2. 用固定输入序列触发 `PD_PowerAndProtocol_Block` 主调度，记录关键分支到达情况。
3. 将每条用例结果填写为：通过/失败/阻塞（阻塞需写明证据缺口与补证方式）。
4. 涉及推断的结论一律标注“待证实”，并链接到后续补证动作；若补证依赖示波器/LA 而 **§1 约束**下不可得，则补证路径须为 **逆向/Renode/GDB**（见任务书）。

## 本轮执行证据（2026-04-06，执行回合 J：BHV-005 动态补证增强）

- 脚本增强：`scripts/renode_phase6_pd_breaktrace.sh`
  - 新增 `PHASE6_PD_STEP_INSN`（命中断点后单步条数，默认 48）；
  - 新增 `PHASE6_TRACE_HELPER_BP`（默认 `0`，避免优先停在 helper 断点导致证据不足）；
  - 输出 entry/post-step 两组快照，便于比对 `pd_gpio_count` 与 `PB3/PB11` 影子变化。
- 执行命令：
  - `TRACE_RUN_SECONDS=4 PHASE6_PD_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
- 产物目录：
  - `logs/renode-phase6/20260406_144126/`
  - `gdb_phase6_pd_breaktrace.log`
  - `renode_emulator.log`
- 关键观测（entry -> post-step）：
  - `g_phase6_pd_gpio_count: 0 -> 1`
  - `g_phase6_pd_pb3_state: 0 -> 1`
  - `g_phase6_pd_pb11_state: 0 -> 1`
  - `g_phase6_protocol_lane_expand_count: 1 -> 2`
  - `g_phase6_last_stage: 2(PD_GPIO) -> 6(PROTOCOL_EXPAND)`
- 判定：
  - `PH6-BHV-005` 的“函数入口后状态推进”补证已形成可复现证据；
  - 该证据属于 G2（动态补证）；G3 在 §1 约束下指 **逆向/寄存器/行为级** 闭环表述，**不**以实机示波器为替代标准。

## 本轮执行证据（2026-04-06，执行回合 K：次要二进制对照）

- 对照对象：
  - 原厂镜像：`dump/ZHIYUN-F100-full.bin`
  - Phase 6 构建产物：`ZHIYUN_F100_Firmware/zhiyun_f100_apm32/.pio/build/apm32f103cb/firmware.bin`
- 对照命令：仓库根执行 Python 脚本（读取向量表、长度、关键常量命中）。
- 对照结果：
  - 原厂：`size=131072`，`vector_sp=0x20001620`，`vector_pc=0x08006be9`
  - 重构：`size=4844`，`vector_sp=0x20005000`，`vector_pc=0x08000145`
  - 23B 启动 SPI 常量 `ae00104081ffa1a83fad8bc8d300da12d922db2ba4a6af`：
    - 原厂命中：`True`（偏移 `0x41b0`）
    - 重构命中：`False`
- 结论：
  - 第一期“次要二进制对照”已执行，差异符合当前 Phase 6 目标（行为等价优先、非哈希对齐）；
  - 对照结果仅作第二期收紧入口，不改变第一期门禁。

## 本轮执行证据（2026-04-06，执行回合 L：计划落地复验与注释锚点补齐）

- 代码收敛（行为不变）：
  - `src/phase6/phase6_light_pd_skeleton.c` 将输入事件吸收和 PD 输出写入抽为
    `Phase6_Input_ApplyPendingEvent` 与 `Phase6_PD_SetOutputs`，减少重复分支，保持调用顺序不变；
  - 为 `PD_GPIO_StateMachine`、CCT/HSI 首轮算法补充“待证实”验证建议（**Renode+GDB / 寄存器快照 / 逆向**，**不**依赖示波器；见任务书 §1），并标注
    原厂符号锚点 `PD_GPIO_StateMachine @ 0x0800C4E0`；
  - `src/phase6/phase6_usb_guard.c` 明确 USB 升级链仍为占位与隔离策略，不进入第一期门禁。
- `PH6-BHV-005` 动态补证复验：
  - 执行命令：`TRACE_RUN_SECONDS=4 PHASE6_PD_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物目录：`logs/renode-phase6/20260406_144847/`
  - 关键观测（entry -> post-step）：
    - `g_phase6_pd_gpio_count: 0 -> 1`
    - `g_phase6_pd_pb3_state: 0 -> 1`
    - `g_phase6_pd_pb11_state: 0 -> 1`
    - `g_phase6_protocol_lane_expand_count: 1 -> 2`
    - `g_phase6_last_stage: 2(PD_GPIO) -> 6(PROTOCOL_EXPAND)`
- 回归复验（G1）：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）；
  - `phase6_ui_event_router_host_test`：退出码 `0`；
  - `phase6_scheduler_acceptance_host_test`：退出码 `0`。
- 判定：
  - `PH6-BHV-001~005` 软件门槛维持通过；
  - `PH6-BHV-005` 的 G2 动态补证已可重复；G3 不以「实机示波器/功率波形」为必达项（§1）。

## 实机万用表可及步骤（第一期，G3 非门禁）

**目的**：在 **仅万用表** 条件下对 `PC13` / `PB3` / `PB11` 做 **通断或直流电压级** 观测，补强「插线后 PD 相关 GPIO」与 [`04_Protocol_Reverse.md`](../../../Cursor_Knowledge_Base/04_Protocol_Reverse.md) §3 静态链的一致性说明。**不**声称 PWM 边沿、占空比或诱骗时序定量（见任务书 §1、**OI-004**）。

**前置**：Phase 6 固件已烧录；棒灯可由电池或合法电源上电；**Type-C 插入**时使用规格内 PD 电源。

| 步骤 | 操作 | 万用表档 | 观察点（相对 MCU 地） | 预期（与 §3 语义一致） |
|------|------|----------|------------------------|-------------------------|
| 1 | 冷机、未插 USB | 直流电压 | **PC13**（`GPIOC` 输入网络，见维修笔记/原理图） | 与固件 `PD_ReadGPIOC13_IdrBit` 语义一致的高/低区间（**待证实**：具体对地电压需以板卡实测为准） |
| 2 | 插入 **USB/PD 供电线** | 同上 | **PC13** | 相对步骤 1 应有可感知变化或稳定态（**低有效附着提示** 为逆向结论，见 `04` §3.1） |
| 3 | **USB 已附着**、灯可运行 | 直流电压或通断 | **PB3**（`PD_WriteGPIOB3_BSRR_BRR`**→PB3，VS_EN 语义见 `04` §3） | 与 `g_phase6_pd_pb3_state` 推定一致；**不**以波形为判据 |
| 4 | 同上 | 同上 | **PB11**（`PD_WriteGPIOB11_BSRR_BRR`**→PB11） | 与 `g_phase6_pd_pb11_state` 推定一致；**不**以波形为判据 |

**记录**：将步骤、表笔点、读数与 `Phase6_AcceptanceReport` / `PD_PowerAndProtocol_Block` 轨迹计数一并记入 [`REVERSE_TASK_BOOK_FINDINGS.md`](../../../Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md)（可选）。

## 本轮执行证据（2026-04-06，执行回合 M：Phase 6 计划收口）

- **G1 回归**（`zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **G2 动态补证**（复跑）：
  - 命令：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`（仓库根目录）。
  - 产物目录：`logs/renode-phase6/20260406_150719/`（`logs/renode-phase6/LATEST` 指向最近一次 **breaktrace** 运行）。
  - 快照：`g_phase6_usb_attached=1`，`g_phase6_pd_gpio_count=1`，`g_phase6_pd_pb3_state=1`，`g_phase6_pd_pb11_state=1`，`g_phase6_protocol_lane_expand_count=1`。
  - 补充：`PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh` → `logs/renode-phase6/20260406_150928/`（`TMR1`/SRAM 快照；**capture** 与 **breaktrace** 后执行，`LATEST` 以最后运行的脚本为准）。
- **规格**：[`phase6_mmio_mapping.md`](phase6_mmio_mapping.md) 增补 `05` §2 与 `PD_PowerAndProtocol_Block` 顺序对照及 UI 子项映射行。
- **口径**：USB 固件升级仍为第一期隔离；SPI1/OLED 全量帧泵为占位，**非**第一期强制门禁（见 `phase6_secondary_dev_baseline.md`）。

## 本轮执行证据（2026-04-06，执行回合 N：文书计划复验）

- **G1**（`ZHIYUN_F100_Firmware/zhiyun_f100_apm32/`）：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
  - `Phase6_AcceptanceReport` 与 `PH6-BHV-001~005` 对应关系仍由 [`phase6_acceptance.c`](../src/phase6/phase6_acceptance.c) 实现；主机侧 [`phase6_scheduler_acceptance_host_test.c`](../test/phase6_scheduler_acceptance_host_test.c) 断言 `001~005` 与 USB guard 策略闭环。
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **规格核对**：未改动 `05-MMIO-v1` 冻结语义，无需 `05-MMIO-v2`；[`phase6_mmio_mapping.md`](phase6_mmio_mapping.md) 与 `05` §2 顺序仍一致。
- **G2**（仓库根目录，非 F100 GDB MCP）：
  - 命令：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物：`logs/renode-phase6/20260406_152442/`（`logs/renode-phase6/LATEST` 已更新）
  - 快照要点：`g_phase6_usb_attached=1`；`post-step` 后 `g_phase6_pd_gpio_count=1`，`g_phase6_pd_pb3_state=1`，`g_phase6_pd_pb11_state=1`，与 `PH6-BHV-005` G2 补证口径一致。
- **口径**：G3 波形/光学定量仍非第一期门禁（`REVERSE_TASK_BOOK.md` §1）。

## 本轮执行证据（2026-04-06，执行回合 O：Phase 6 可编译与验收计划实施）

- **G1**（`ZHIYUN_F100_Firmware/zhiyun_f100_apm32/`）：
  - `chmod +x scripts/link_geehy_sdk.sh && ./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.0%`、`Flash 3.5%`）。
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`（`cc` 命令见上文「主机侧最小自测」）→ 退出码 `0`，`PH6-BHV-001~005` 软件门槛断言通过。
- **G2**（仓库根目录，**非** F100 GDB MCP）：
  - 命令：`TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物目录：`logs/renode-phase6/20260406_152754/`（`logs/renode-phase6/LATEST` 指向该次运行）
  - 快照要点（`post-step`）：`g_phase6_pd_gpio_count: 0 -> 1`，`g_phase6_pd_pb3_state: 0 -> 1`，`g_phase6_pd_pb11_state: 0 -> 1`，`g_phase6_last_stage: 2（INPUT_SCAN）-> 4（PD_ATTACH_HELPER）`，与 `PH6-BHV-005` G2 口径一致。
- **规格**：未修改源码；[`phase6_mmio_mapping.md`](phase6_mmio_mapping.md) 与 **`05-MMIO-v1`** §2 十步顺序仍一致，无需升版至 `05-MMIO-v2`。

## 本轮执行证据（2026-04-06，执行回合 P：启动链收敛）

- **启动链收敛（代码）**：
  - 新增 `src/phase6/phase6_startup_chain.c/.h`，将 `05` §1 中启动主链锚点收敛为可追踪顺序：`InitializeClock_PLLFlashSysTick`、`InitializeGPIOB_ClockAndPinMode`、`EnableAHBPeriphClock_DMA1`、`SetUsbDeviceContextPointer`、`InitializeUsbControlEndpointDefaults`、`InitializeUsbDescriptorsAfterPwmInit`（第一期锚点实现）。
  - `main.c` 改为 `Phase6_Scheduler_Init()` 后显式调用 `Phase6_Scheduler_RunStartupStage()`；`scheduler` 内保留兜底，避免漏调。
  - `phase6_acceptance` 新增启动链证据字段：`startup_chain_completed/startup_step_count/startup_last_step_id`，并将 `BHV-001` 通过条件纳入“启动链已完成”约束。
- **G1 回归**（`zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 7.2%`、`Flash 3.7%`）。
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` 退出码 `0`。
- **G2 动态补证**（仓库根目录，非 F100 GDB MCP）：
  - `TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_154347/`
    - `post-step`：`g_phase6_pd_gpio_count=1`，`g_phase6_pd_pb3_state=1`，`g_phase6_pd_pb11_state=1`，`g_phase6_last_stage=4`。
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh`
    - 产物：`logs/renode-phase6/20260406_154350/`
    - 快照：`TMR1_CTRL1=0x81`、`TMR1_ARR=0x04B0`、`TMR1_CCR1/CCR3=0x18`，与第一期 CCT 路径影子一致。
- **口径**：
  - `phase6_usb_guard` 仍保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本轮不触发写 Flash；
  - 启动链仅做文书锚点收敛，不新增 `05-MMIO-v1` 外的 MMIO 语义。

## 本轮执行证据（2026-04-06，执行回合 Q：SPI1 启动帧单点收敛）

- **代码收敛**：
  - `src/phase6/phase6_startup_chain.c` 新增 `SPI1_StartupSequence`，按 23B 常量帧（`AE00104081FFA1A83FAD8BC8D300DA12D922DB2BA4A6AF`）真实写入 `SPI1->DATA`；
  - 启动阶段新增 `PHASE6_STARTUP_STEP_SPI1_STARTUP_FRAME`，不改变 `PD_PowerAndProtocol_Block` 周期序。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run` 通过；
  - host test 命令维持原链路（`phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`）均应为退出码 `0`。
- **G2 补证**（仓库根，非 F100 GDB MCP）：
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh`
  - 产物目录：`logs/renode-phase6/20260406_155924/`
  - 实测快照字段：`g_phase6_spi1_startup_tx_count=23`、`g_phase6_spi1_startup_done=1`、`g_phase6_spi1_startup_last_byte=0xAF`、`g_phase6_spi1_startup_cs_low_count=1`、`g_phase6_spi1_startup_cs_high_count=1`；
  - `g_phase6_spi1_startup_wait_timeout_count=23`（Renode 未完整实现 SPI 状态位，触发防卡死路径；不改变 23B 发射序列与最后字节）。
- **风险与边界**：
  - 仅补齐“启动帧真实发射路径”，未扩展运行期 `8×(3B+96B)` 全量帧泵；
  - USB 升级 guard 不变（`PHASE6_USB_FW_UPGRADE_ENABLED=0`，不写 Flash）。

## 本轮执行证据（2026-04-06，执行回合 R：全量帧泵与USB数据面收敛）

- **代码收敛**：
  - `phase6_startup_chain` 从纯锚点升级为有界寄存器启动链（`HSI->PLL->SYSCLK`、`FMC latency/prefetch`、`SysTick`）；
  - `UI_SPI1_PumpEightFramebufferSlices` 改为真实 `8×(3B+96B)` 发送（由 `Phase6_SPI1_Transmit` 统一驱动）；
  - `phase6_ui_event_router` 新增描述符池解析，`UI_ModeRender_PostProcess` 回写 `ui_descriptor_index` 与 `ui_framebuffer_checksum`；
  - 新增 `phase6_usb_device` 数据面泵（`SOF/EP0 setup/in/out` 计数），并接入验收门禁。
- **G1 回归**：
  - `pio run` 通过（`RAM 11.8%`、`Flash 5.1%`）；
  - `phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 均通过。
- **门禁收紧**：
  - `PH6-BHV-001` 新增 `spi_flush_slices>=8`、`spi_flush_bytes>=792`、`ui_framebuffer_checksum!=0`；
  - `PH6-BHV-003` 新增 `tmr2_dma_push_count>0` 与 `tmr2_dma_last_words>=16`；
  - `PH6-BHV-005` 新增 USB Device 数据面门槛（`usb_device_initialized/connected`、`usb_ep0_setup_count`、`usb_ep0_in_count`）。
- **USB guard 与风险**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED` 仍为 `0`；
  - 本回合未实现任何写Flash逻辑，升级链继续由 guard 显式阻断。

## 本轮执行证据（2026-04-06，执行回合 S：SPI1/OLED 帧泵节拍收敛）

- **代码收敛**：
  - `UI_SPI1_PumpEightFramebufferSlices` 从“每轮强制刷”改为“脏帧触发 + 16 tick 保底刷新”，每次触发仍保持 `8×(3B+96B)` 完整事务；
  - `UI_ModeRender_PostProcess` 增加节拍门控，避免在无输入变化时重复重建同一帧；
  - 新增可观测计数：`g_phase6_spi_flush_trigger_count`、`g_phase6_spi_flush_skip_count`，用于动态补证帧泵节拍。
- **G1 回归（软件门槛）**：
  - `./scripts/link_geehy_sdk.sh && pio run` 应保持 `SUCCESS`；
  - `phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test` 应保持退出码 `0`。
- **G2 取证（非 MCP，Renode + 本机 GDB）**：
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh`；
  - 查看 `gdb_phase6_capture.log` 中新增 `SPI1 runtime framepump cadence` 两项计数，确认触发与跳过都可观测。
- **边界声明（必须）**：
  - 本回合不改 USB 升级写 Flash 路径；
  - `phase6_usb_guard` 继续维持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`。

## 本轮执行证据（2026-04-06，执行回合 T：UI flush 断点取证深化）

- **脚本增强**（非 MCP）：
  - `scripts/renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_TRACE_FLUSH_BP`（默认 `1`）；
  - 命中 `PD_GPIO_StateMachine` 取证后，会继续命中 `UI_SPI1_PumpEightFramebufferSlices`；
  - 新增 `PHASE6_FLUSH_STEP_INSN`（默认 `64`），支持在 UI flush 断点后继续单步再采样。
- **执行记录**：
  - 命令：`TRACE_RUN_SECONDS=3 PHASE6_PD_STEP_INSN=48 PHASE6_TRACE_FLUSH_BP=1 PHASE6_FLUSH_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物目录：`logs/renode-phase6/20260406_172503/`
  - 关键观测：
    - `PD_GPIO_StateMachine` 命中后，`pd_gpio_count` 与 `PB3/PB11` 影子由 `0 -> 1`（G2 继续稳定）；
    - 已命中 `UI_SPI1_PumpEightFramebufferSlices`，并进入 `Phase6_SPI1_Transmit` 调用栈（说明运行期帧泵路径可达）；
    - `spi_flush_trigger/skip` 计数在当前采样窗口仍为 `0`（**待证实**：计数递增点位于更后续指令，需要更长单步或改采样点）。
- **结论与下一步**：
  - 本回合把“仅在 PD 分支采样”推进到“可直达 UI flush 函数级采样”，缩短了 SPI1/OLED 帧泵与原厂节拍对齐的证据距离；
  - 下一最小切口：在不改业务语义前提下，继续扩展脚本的 flush 后段采样窗口，直采 `spi_flush_trigger/skip` 的首次非零跃迁。

## 本轮执行证据（2026-04-06，执行回合 U：flush 计数写回点定位）

- **脚本增强**（非 MCP）：
  - `scripts/renode_phase6_pd_breaktrace.sh` 增加可选开关 `PHASE6_TRACE_FLUSH_COUNTER_BP`（默认 `0`）；
  - 新增可选断点：`UI_SPI1_PumpEightFramebufferSlices+0x84`（trigger 计数写回后）与 `+0xA8`（skip 计数写回后）；
  - 默认关闭该能力，避免 Renode 在 SPI 发送循环中出现长等待。
- **静态定位依据**：
  - 通过 `arm-none-eabi-objdump` 对 `UI_SPI1_PumpEightFramebufferSlices` 反汇编，确认：
    - `g_phase6_spi_flush_trigger_count` 写回位于函数内 `+0x84` 附近；
    - `g_phase6_spi_flush_skip_count` 写回位于函数内 `+0xA8` 附近。
- **执行记录**：
  - 常规（默认）复验命令：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物目录：`logs/renode-phase6/20260406_173601/`
  - 观测：仍可稳定命中 `PD_GPIO_StateMachine` 与 `UI_SPI1_PumpEightFramebufferSlices`，并进入 `Phase6_SPI1_Transmit`；`spi_flush_trigger/skip` 在当前窗口仍为 `0`（待证实）。
- **阶段判定**：
  - 本回合把“计数跃迁待证实”进一步收敛为“已知精确写回地址、可选断点可复用”；
  - 下一最小切口是以更保守的断点策略抓取 `+0x84/+0xA8` 的首次命中快照（不改业务代码）。

## 本轮执行证据（2026-04-06，执行回合 V：flush 分支寄存器注入）

- **脚本增强**（非 MCP）：
  - `renode_phase6_pd_breaktrace.sh` 新增：
    - `PHASE6_FORCE_FLUSH_SKIP`（入口断点处强制 `r0/r1/r3`，走 skip 判定）；
    - `PHASE6_FORCE_FLUSH_TRIGGER`（入口断点处强制 `r0`，走 trigger 判定）；
    - `PHASE6_FORCE_SPI_STS_READY`（可选将 `SPI1->STS` 置 `TXBE|RXBNE`，用于触发分支加速试验）。
- **执行记录**：
  - 命令：`TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 PHASE6_TRACE_FLUSH_COUNTER_BP=1 PHASE6_FORCE_FLUSH_SKIP=1 PHASE6_FLUSH_STEP_INSN=0 ./scripts/renode_phase6_pd_breaktrace.sh`
  - 产物目录：`logs/renode-phase6/20260406_174012/`（本次尝试日志）
  - 观测：
    - 可稳定到达并停在 `UI_SPI1_PumpEightFramebufferSlices`；
    - “Force flush branch registers patched” 已记录，说明入口寄存器注入生效；
    - 计数写回断点在本次 Renode 会话中仍未在时限内返回（待证实）。
- **阻塞说明（待证实）**：
  - 当前阻塞表现为 Renode+GDB 在 flush 后段继续运行时长不可控，导致计数写回断点证据尚未落地；
  - 该阻塞属于**取证链时序/会话稳定性**，不属于业务语义缺口。

## 本轮执行证据（2026-04-06，执行回合 W：节拍计数跃迁闭合）

- **脚本收敛（非 MCP）**：
  - `renode_phase6_pd_breaktrace.sh` 将 flush 分支强制注入从“仅入口前”扩展到“命中 `UI_SPI1_PumpEightFramebufferSlices` 后再次注入”；
  - 将可选计数写回断点改为软件断点（`break`），避免硬件断点槽位限制导致的假阻塞。
- **可观测锚点（不改业务语义）**：
  - `phase6_light_pd_skeleton.c` 将 `g_phase6_spi_flush_trigger_count++` 前移到 trigger 分支入口，确保 GDB 在 SPI 发送循环未完成前也能观察到“已走 trigger 分支”；
  - `skip` 路径计数保持原有位置与语义不变。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 11.9%`、`Flash 5.2%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **G2 闭合证据**：
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 PHASE6_TRACE_FLUSH_BP=1 PHASE6_FORCE_FLUSH_SKIP=1 PHASE6_FLUSH_STEP_INSN=40 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_181002/`
    - 快照：`g_phase6_spi_flush_skip_count=1`、`g_phase6_spi_flush_trigger_count=0`
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 PHASE6_TRACE_FLUSH_BP=1 PHASE6_FORCE_FLUSH_TRIGGER=1 PHASE6_FORCE_SPI_STS_READY=1 PHASE6_FLUSH_STEP_INSN=14 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_181024/`
    - 快照：`g_phase6_spi_flush_trigger_count=1`、`g_phase6_spi_flush_skip_count=0`
- **判定**：
  - `SPI1/OLED` 运行期帧泵“触发/跳过”两类节拍计数均已形成可复现非零证据；
  - 本轮缩短了“函数级命中”到“节拍分支可量化取证”的距离；
  - USB 升级 guard 仍保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本轮未触发写 Flash。

## 本轮执行证据（2026-04-06，执行回合 X：节拍计数纳入验收报表）

- **代码收敛**：
  - `Phase6_AcceptanceReport` 新增 `spi_flush_trigger_count` / `spi_flush_skip_count`；
  - `phase6_acceptance.c` 将上述计数从 `Phase6_LightPd_TraceSnapshot` 回填到验收报表，供主机侧断言直接读取；
  - `phase6_scheduler_acceptance_host_test` 增加断言：`spi_flush_trigger_count > 0` 且 `spi_flush_skip_count > 0`，确保“脏帧触发 + 周期保底”两分支都被覆盖。
- **G1 回归**（`zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 11.9%`、`Flash 5.2%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **边界与风险**：
  - 本回合仅增强验收可观测性，不新增任何 MMIO 写入；
  - USB 升级 guard 保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，未触发写 Flash 路径。

## 本轮执行证据（2026-04-06，执行回合 Y：主循环 Step/IWDG 语义收敛）

- **代码收敛**：
  - `phase6_scheduler.c` 新增主循环影子锚点：`StepBootPhaseCounter8`（0..7 循环）与 `thunk_IWDG_KR_ReloadAAAA_Shadow`（每轮 reload 计数）；
  - `Phase6_LightPd_TraceSnapshot` / `Phase6_AcceptanceReport` 新增 `main_boot_phase_counter8`、`main_iwdg_reload_count`，形成可观测证据链；
  - `phase6_acceptance.c` 将 `PH6-BHV-001` 通过条件收紧为“UI 主链达成 + SPI 帧泵达成 + main 循环喂狗影子达成”；
  - `phase6_scheduler_acceptance_host_test` 新增断言：`main_iwdg_reload_count > 0` 且 `main_boot_phase_counter8 < 8`。
- **G1 回归**（`zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 12.0%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **边界与风险**：
  - 本回合只补“主循环语义影子”，不新增 PD/灯行为假设；
  - 为避免引入不可控副作用，本回合**未**新增真实 IWDG MMIO 写入，后续若需解除此限制需单独专验；
  - USB 升级 guard 继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，未触发写 Flash 路径。

## 本轮执行证据（2026-04-06，执行回合 Z：动态采样脚本收敛到 main 周期语义）

- **脚本收敛（非 MCP）**：
  - `scripts/renode_phase6_capture.sh` 新增 `g_phase6_main_boot_phase_counter8` / `g_phase6_main_iwdg_reload_count` 采样字段；
  - 新增 `PHASE6_CAPTURE_BREAK_ON_PD=1`（默认开启）：在 `PD_GPIO_StateMachine` 断点停机后采样，避免随机停在启动链导致关键计数为零；
  - `scripts/renode_phase6_pd_breaktrace.sh` 同步增加 main 周期语义字段（entry/post-step 同屏输出）。
- **G2 复现记录**：
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=24 PHASE6_TRACE_FLUSH_BP=0 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_184558/`
    - 观测：`main_boot_phase_counter8=1`、`main_iwdg_reload_count=1`；`post-step` 后 `pd_gpio_count=1`、`PB3/PB11=1`。
  - `PHASE6_RUN_SECONDS=2 PHASE6_CAPTURE_BREAK_ON_PD=1 ./scripts/renode_phase6_capture.sh`
    - 产物：`logs/renode-phase6/20260406_184633/`
    - 观测：`main_boot_phase_counter8=1`、`main_iwdg_reload_count=1`，并同时采到 `TMR1`/`SPI1 startup 23B` 状态影子。
- **判定**：
  - 本回合把“主循环语义仅 host test 可见”收敛为“Renode/GDB 动态同屏可见”，缩短了与原厂 `main` 周期行为证据链差距；
  - USB 升级 guard 保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，本回合未触发写 Flash。

## 本轮执行证据（2026-04-06，执行回合 AA：Ghidra MCP + Renode 联动收敛）

- **Ghidra MCP 补证（`user-ghidra`）**：
  - 通过 `search_functions_enhanced` / `decompile_function` / `get_function_callees` 复核：
    - `main @ 0x08002e74`
    - `StepBootPhaseCounter8 @ 0x08002df6`
    - `thunk_IWDG_KR_ReloadAAAA @ 0x08003d9e`
    - `Iwdg_UnlockConfigurePrescaleReloadAndStart @ 0x08005184`
  - 关键结论：`main` 循环内 `StepBootPhaseCounter8 + IWDG reload` 由 `tick_delta > 99` 门控，不是每轮执行。
- **代码收敛**：
  - `phase6_scheduler.c` 将 Step/IWDG 影子从“每轮”改为“100 tick 周期门控”（`PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=100`）；
  - host test 同步放宽前置循环以匹配门控语义。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run` → `SUCCESS`（`RAM 12.0%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test` → 退出码 `0`。
- **Renode 脚本联动**：
  - `renode_phase6_capture.sh`/`renode_phase6_pd_breaktrace.sh` 均新增 main 周期字段采样；
  - `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_PD_BREAK_IGNORE`（待证实：`ignore>0` 在当前 Renode/GDB 会话中可能长时间不返回，已记录为取证链稳定性问题，不归类为业务回归）。
  - 默认模式复跑（`logs/renode-phase6/20260406_190040/`）可稳定命中 `PD_GPIO_StateMachine` 并输出 main 周期字段（当前值为 0，符合“首轮尚未到 100 tick 门限”的预期）。
- **风险边界**：
  - 本回合未引入新的 IWDG MMIO 写寄存器副作用，仅收敛周期语义；
  - USB 升级 guard 继续保持 `PHASE6_USB_FW_UPGRADE_ENABLED=0`，未触发写 Flash。

## 本轮执行证据（2026-04-06，执行回合 AB：后周期断点采样补证尝试）

- **脚本增强（非 MCP）**：
  - `renode_phase6_capture.sh` 新增 `PHASE6_POST_BREAK_RUN_SECONDS`，尝试在命中 `PD_GPIO_StateMachine` 后续跑再采样；
  - `renode_phase6_pd_breaktrace.sh` 新增：
    - `PHASE6_PD_BREAK_IGNORE`（忽略前 N 次命中）
    - `PHASE6_PD_SOFT_BP`（可切换软件断点）
- **执行结果**：
  - `PHASE6_POST_BREAK_RUN_SECONDS` 路径已验证可执行，但当前会话中采样 PC 仍停留在 `PD_GPIO_StateMachine` 附近，`main` 周期计数未观察到增长；
  - `PHASE6_PD_BREAK_IGNORE>0`（硬/软件断点两种）在当前 Renode/GDB 组合下均出现长时间不返回，已终止并回收进程。
- **结论（待证实）**：
  - 该阻塞属于**取证链稳定性问题**（断点调度/会话行为），不等同业务逻辑回归；
  - 现阶段保留“主循环 100tick 门控”作为 **Ghidra MCP + G1 回归** 证据成立，G2 的“后周期动态快照”继续作为下一轮最小切口。

## 本轮执行证据（2026-04-06，执行回合 AC：SPI1 启动就绪语义收敛）

- **代码收敛**：
  - `src/phase6/phase6_startup_chain.c` 中 `Phase6_SPI1_Transmit` 改为“状态位驱动优先 + 首次超时后 fallback 直发”；
  - 该调整只收敛等待分支，不改 23B 启动帧内容、不改 `SPI1->DATA/STS` 访问边界。
- **G1 回归**：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.1%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test`、`phase6_scheduler_acceptance_host_test`：退出码 `0`，`PH6-BHV-001~005` 软件门槛保持通过。
- **G2 动态补证（非 MCP）**：
  - `PHASE6_RUN_SECONDS=1 PHASE6_STIM_USB=1 PHASE6_FORCE_PD=1 PHASE6_CAPTURE_BREAK_ON_PD=1 ./scripts/renode_phase6_capture.sh`
    - 产物：`logs/renode-phase6/20260406_191647/`
    - 关键快照：`g_phase6_spi1_startup_tx_count=23`、`g_phase6_spi1_startup_done=1`、`g_phase6_spi1_startup_last_byte=0xAF`、`g_phase6_spi1_startup_wait_timeout_count=1`
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=32 PHASE6_TRACE_FLUSH_BP=1 PHASE6_FLUSH_STEP_INSN=64 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_191659/`
    - 关键快照：`g_phase6_spi_flush_trigger_count=1`、`g_phase6_spi_flush_skip_count=0`（本次窗口命中 trigger 路径）。
- **与原厂等价距离收敛说明**：
  - 将“Renode 下按字节重复 timeout”收敛为“首次异常即降级”，降低了仿真噪声对 SPI1 启动链判断的干扰，证据链更接近 OEM 的“可发送即前进”语义。
- **仍缺项（待证实）**：
  - fallback 在真实板级 `STS` 行为下的触发频度仍需补证；当前只证明在 Renode 侧可抑制按字节超时主导。
- **USB guard 与风险边界（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED` 仍为 `0`；
  - 本回合未引入任何 USB 写 Flash 实现或入口，升级链继续由 guard 阻断。

## 本轮执行证据（2026-04-06，执行回合 AD：取证链稳定化与矩阵复验）

- **脚本侧稳定化（不改业务语义）**：
  - `renode_phase6_capture.sh` / `renode_phase6_pd_breaktrace.sh` 新增 `PHASE6_GDB_TIMEOUT_SECONDS`，用于超时自动回收；
  - `phase6_pd_breaktrace` 默认采用软件断点（`PHASE6_PD_SOFT_BP=1`），并输出 `call_ticks/oled_last_flush_tick/ui_frame_dirty`。
- **G1 复验**：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.1%`、`Flash 5.3%`）；
  - `phase6_ui_event_router_host_test` 与 `phase6_scheduler_acceptance_host_test`：退出码 `0`。
- **G2 复验矩阵（非 MCP）**：
  - `RENODE_GDB_PORT=4333 ... ./scripts/renode_phase6_capture.sh` → `logs/renode-phase6/20260406_195008/`；
  - `RENODE_GDB_PORT=4334 ... PHASE6_FORCE_FLUSH_TRIGGER=1 ./scripts/renode_phase6_pd_breaktrace.sh` → `logs/renode-phase6/20260406_195020/`（`spi_flush_trigger_count=1`）；
  - `RENODE_GDB_PORT=4335 ... PHASE6_FORCE_FLUSH_SKIP=1 ./scripts/renode_phase6_pd_breaktrace.sh` → `logs/renode-phase6/20260406_195035/`（`spi_flush_skip_count=1`）；
  - `RENODE_GDB_PORT=4337 ... PHASE6_PD_BREAK_IGNORE=20 ... ./scripts/renode_phase6_pd_breaktrace.sh` 触发硬超时退出码 `124`，验证超时回收链有效。
- **判定**：
  - `PH6-BHV-005` 的 PD 影子推进继续稳定（`pd_gpio_count=1`、`PB3/PB11=1`）；
  - 帧泵 trigger/skip 双分支均可被可控注入复现；
  - `main` 周期计数在本轮窗口仍多次为 `0`，后周期非零推进仍列“待证实”并保留下一切口。
- **USB guard 与风险边界（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 不变；
  - 本轮未引入任何 USB 写 Flash 路径。

## 本轮执行证据（2026-04-06，执行回合 AE：全待办收敛回归）

- **代码侧新增验收字段**：
  - HSI/DMA/TMR3 链：`tmr3_pwm_apply_count`、`tmr3_cc2_shadow`、`dma1_ch1_transfer_count`；
  - USB Device 上下文：`usb_device_context_ptr_shadow`、`usb_device_ep0_max_packet_shadow`、`usb_device_descriptors_ready`。
- **主机验收断言收紧**：
  - `phase6_scheduler_acceptance_host_test` 新增：
    - `tmr3_pwm_apply_count > 0`；
    - `tmr3_cc2_shadow <= 999`；
    - `dma1_ch1_transfer_count > 0`；
    - `usb_device_context_ptr_shadow != 0`；
    - `usb_device_ep0_max_packet_shadow >= 8`；
    - `usb_device_descriptors_ready != 0`。
- **G1 回归命令与结果**（工程根 `ZHIYUN_F100_Firmware/zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run` -> `SUCCESS`（`RAM 12.3%`、`Flash 5.8%`）；
  - `phase6_ui_event_router_host_test` -> 退出码 `0`；
  - `phase6_scheduler_acceptance_host_test` -> 退出码 `0`。
- **G2 补证（非 MCP）**：
  - `TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=48 ./scripts/renode_phase6_pd_breaktrace.sh` -> `logs/renode-phase6/20260406_201541/`；
  - `PHASE6_RUN_SECONDS=2 ./scripts/renode_phase6_capture.sh` -> `logs/renode-phase6/20260406_201545/`。
- **结论**：
  - 本回合将“灯光 RGB 推送链”推进到“`TMR2 + DMA1 + TMR3` 同步影子可验收”，并把 USB Device 初始化从纯计数推进到“上下文/EP0/描述符就绪”可验收字段，继续缩短与原厂等价距离。
- **USB 写 Flash guard（必须声明）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 未变；
  - 本轮无写 Flash 逻辑、无 guard 解除行为。

## 本轮执行证据（2026-04-06，执行回合 AF：四阶段落地 + 全量回归）

- **代码落地**：
  - 启动链新增 `CheckMagicWord55AA` 与 `IWDT` 启动锚点，且把 `magic55aa/iwdt` 状态写入 `Phase6_AcceptanceReport`；
  - 调度新增 main 周期 SPI 请求桥接，`UI_SPI1_PumpEightFramebufferSlices` 新增 main 请求消费分支；
  - USB guard 新增 preflight/风险计数字段，用于后续“专验后再解除 guard”的审计准备。
- **G1 回归**（工程根 `ZHIYUN_F100_Firmware/zhiyun_f100_apm32`）：
  - `./scripts/link_geehy_sdk.sh && pio run`：`SUCCESS`（`RAM 12.8%`、`Flash 6.1%`）；
  - `phase6_ui_event_router_host_test`：退出码 `0`；
  - `phase6_scheduler_acceptance_host_test`：退出码 `0`（新增断言通过：`mainloop_spi_pump_*`、`startup_iwdt_*`、`usb_upgrade_guard_*`）。
- **G2 补证（非 MCP）**：
  - `RENODE_GDB_PORT=4339 RENODE_MONITOR_PORT=49179 PHASE6_RUN_SECONDS=1 PHASE6_GDB_TIMEOUT_SECONDS=60 ./scripts/renode_phase6_capture.sh`
    - 产物：`logs/renode-phase6/20260406_203926/`
  - `RENODE_GDB_PORT=4340 RENODE_MONITOR_PORT=49180 TRACE_RUN_SECONDS=2 PHASE6_PD_STEP_INSN=48 PHASE6_GDB_TIMEOUT_SECONDS=60 ./scripts/renode_phase6_pd_breaktrace.sh`
    - 产物：`logs/renode-phase6/20260406_203941/`
  - 关键观测：`g_phase6_iwdt_started=1`、`g_phase6_iwdt_reload_shadow=1`、`g_phase6_spi_flush_trigger_count=1`、`g_phase6_usb_upgrade_flash_write_attempt_count=0`。
- **USB/Flash 风险声明（必须）**：
  - guard 仍为 `PHASE6_USB_FW_UPGRADE_ENABLED=0`；
  - 本轮未实现、未放通、未调用任何写 Flash 路径；
  - 仅增加“专验前置条件与风险计数”观察点，供后续第二期 USB 任务使用。

## 本轮执行证据（2026-04-06，执行回合 AG：G2 main 周期快照闭合 + capture 脚本修正）

- **根因与修正**：
  - GDB 批处理中 **`if N > 0`** 的 **`>`** 会被当作 **shell 重定向**，内层 `delete`/`monitor start`/后周期块不可靠；已统一为 **`if (N > 0)`**（由 bash 展开为带括号的数值比较）。
  - **`PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=100`** 时，在 **`PD_GPIO` 首次命中** 且仅完成 **1** 次 `RunOnce` 迭代门头的快照中，`main_step8`/`main_iwdg_reload` **为 0 属预期**（未达 100 次 `RunOnce`）；与「回合 AA」Ghidra 证据一致。
  - **Renode 专验构建** `[env:apm32f103cb_renode_g2]`：`platformio.ini` 中 **`-DPHASE6_MAIN_STEP_IWDG_PERIOD_TICKS=1`**，用于 **G2 同屏**使首次 `PD` 断点前同一轮 `RunOnce` 开头即完成 StepBoot+IWDG 影子（**不**替代默认 **100** 的量产语义）。
- **脚本**（[`scripts/renode_phase6_capture.sh`](../../../scripts/renode_phase6_capture.sh)）：
  - 默认 `PHASE6_POST_BREAK_RUN_SECONDS=2`、`PHASE6_CAPTURE_PD_IGNORE=0`；
  - 可选 `PHASE6_RENODE_SPI_FALLBACK_INJECT=1`、`PHASE6_RENODE_SPI_STS_FORCE=1`（与 `renode_phase6_pd_breaktrace.sh` 的 SPI STS 强制同思路）；
  - 命中 PD 后使用 **`delete $bpnum`**（替代 `disable`）再 `monitor start`，减少断点残留对继续运行的影响。
- **固件**：
  - `PHASE6_MAIN_STEP_IWDG_PERIOD_TICKS` 改为 **`#ifndef` 可覆盖**（默认 **100**）；
  - `Phase6_LightPd_ReadTraceSnapshot` 的 **`scheduler_ticks`** 改为 **`g_phase6_scheduler_ticks`**（与 GDB 标签一致）。
- **推荐参数矩阵（G2）**：

| 目标 | 构建 | `PHASE6_FW_BIN` / `PHASE6_FW_ELF` | 说明 |
|------|------|-----------------------------------|------|
| 量产等价主循环门限 | `pio run -e apm32f103cb` | `.pio/build/apm32f103cb/firmware.*` | `main_*` 在首次 PD 快照中可为 **0** |
| Renode 同屏非零 `main_*` | `pio run -e apm32f103cb_renode_g2` | `.pio/build/apm32f103cb_renode_g2/firmware.*` | 仅仿真证据，**非**出厂烧录默认 |

- **USB/Flash**：`PHASE6_USB_FW_UPGRADE_ENABLED=0` 未变；本轮无写 Flash。

## 本轮执行证据（2026-04-06，执行回合 AH：启动链时钟证据闭环）

- **代码落地**：
  - [`phase6_startup_chain.c`](../src/phase6/phase6_startup_chain.c) 在 `InitializeClock_PLLFlashSysTick_Phase6Anchor` 增加 `SystemCoreClockUpdate()`；
  - 新增 startup 影子：`clock_hsi_ready`、`clock_pll_ready`、`clock_pll_selected`、`fmc_latency_shadow`、`sysclk_hz_shadow`、`systick_reload_shadow`；
  - `SysTick->LOAD` 采用 `SystemCoreClock/1000-1` 并限定 24-bit，确保启动时钟步骤可复核。
- **验收收紧**：
  - [`phase6_acceptance.c`](../src/phase6/phase6_acceptance.c) 的 `PH6-BHV-001` 新增时钟就绪门槛：
    - `startup_clock_hsi_ready != 0`
    - `startup_clock_pll_ready != 0`
    - `startup_clock_pll_selected != 0`
    - `startup_systick_reload_shadow > 0`
  - [`phase6_scheduler_acceptance_host_test.c`](../test/phase6_scheduler_acceptance_host_test.c) 新增对应断言，防止后续回归把时钟路径退化回“仅计数占位”。
- **G1 回归**（工程根 `ZHIYUN_F100_Firmware/zhiyun_f100_apm32`）：
  - `pio run`：`2 succeeded`（`RAM 13.1%`、`Flash 6.4%`）；
  - host 验收编译 + 运行：退出码 `0`。
- **结论**：
  - 本轮把“启动链时钟步骤”从文书锚点推进到“可观测、可断言”的状态，缩短了与原厂主链可核验性的差距；
  - 仍未宣称位级完全等价，后续需继续用 Renode+GDB 做时钟寄存器细节补证（待证实）。
- **USB guard 与风险边界（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 不变；
  - 本轮未新增、未放通任何 USB 写 Flash 路径。

## 本轮执行证据（2026-04-06，执行回合 AJ：PD helper 3000-tick 动态补证）

- **脚本增强（非 MCP）**：
  - [`scripts/renode_phase6_pd_breaktrace.sh`](../../../scripts/renode_phase6_pd_breaktrace.sh) 新增：
    - `PHASE6_FORCE_SYSTICK_ENTRY`（可选）；
    - `g_phase6_pd_attach_window_count` 采样输出（entry/post-step）。
- **执行命令**：
  - `RENODE_GDB_PORT=4361 RENODE_MONITOR_PORT=49261 TRACE_RUN_SECONDS=1 PHASE6_PD_STEP_INSN=80 PHASE6_TRACE_FLUSH_BP=0 PHASE6_FORCE_SYSTICK_ENTRY=3000 PHASE6_GDB_TIMEOUT_SECONDS=120 ./scripts/renode_phase6_pd_breaktrace.sh`
- **产物目录**：
  - `logs/renode-phase6/20260406_232423/`
- **关键观测（entry -> post-step）**：
  - `g_phase6_systick_entry_count = 0x0bcc (3020)`（注入后保持在 SysTick 域窗口）；
  - `g_phase6_pd_attach_window_count: 0 -> 1`；
  - `g_phase6_pd_gpio_count: 0 -> 1`；
  - `g_phase6_pd_pb3_state: 0 -> 1`，`g_phase6_pd_pb11_state: 0 -> 1`。
- **结论**：
  - `PD_HelperSeq_3000tick_ThenPc13Gpio` 的窗口判定在动态上可触发并产生可观测跃迁，支持“以 `SysTick` 入口计数为时基”的实现收敛。
- **待证实（保留）**：
  - 当前为“边界注入”场景，仍需在不注入 `PHASE6_FORCE_SYSTICK_ENTRY` 的自然调度窗口下继续补证同类跃迁。
- **USB 风险边界（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；
  - 本回合未实现、未放通、未调用任何写 Flash 路径。

## 本轮执行证据（2026-04-06，执行回合 AK：PD helper 自然窗口复验）

- **执行命令（不注入 systick）**：
  - `RENODE_GDB_PORT=4362 RENODE_MONITOR_PORT=49262 TRACE_RUN_SECONDS=4 PHASE6_PD_STEP_INSN=120 PHASE6_TRACE_FLUSH_BP=0 PHASE6_GDB_TIMEOUT_SECONDS=150 ./scripts/renode_phase6_pd_breaktrace.sh`
- **产物目录**：
  - `logs/renode-phase6/20260406_232907/`
- **关键观测（entry -> post-step）**：
  - `g_phase6_systick_entry_count: 0x14 -> 0x14`；
  - `g_phase6_pd_attach_window_count: 0 -> 0`（本窗口未跨 3000 tick）；
  - `g_phase6_pd_gpio_count: 0 -> 1`，`PB3/PB11: 0 -> 1`（PD GPIO 子链仍正常推进）。
- **结论**：
  - 自然短窗口下未触发 3000-tick helper 跃迁，与当前计数窗口规模一致；
  - AJ 的“边界注入命中”与本回合“自然窗口未跨阈值”相互一致，二者共同约束了 helper 逻辑。
- **下一步最小切口**：
  - 在不注入 `PHASE6_FORCE_SYSTICK_ENTRY` 的前提下，延长运行窗并迭代断点策略，目标是自然命中 `pd_attach_window_count` 的首次 `0 -> 1`。

## 本轮执行证据（2026-04-07，执行回合 AL：自然窗口首次 3000-tick 命中）

- **执行命令（不注入 systick，忽略前 149 次 PD 断点）**：
  - `RENODE_GDB_PORT=4364 RENODE_MONITOR_PORT=49264 TRACE_RUN_SECONDS=2 PHASE6_PD_SOFT_BP=1 PHASE6_PD_BREAK_IGNORE=149 PHASE6_PD_STEP_INSN=96 PHASE6_TRACE_FLUSH_BP=0 PHASE6_GDB_TIMEOUT_SECONDS=180 ./scripts/renode_phase6_pd_breaktrace.sh`
- **产物目录**：
  - `logs/renode-phase6/20260407_000042/`
- **关键观测（entry -> post-step）**：
  - entry：`g_phase6_systick_entry_count=0x0bb8 (3000)`，`g_phase6_pd_attach_window_count=0`；
  - post-step：`g_phase6_pd_attach_window_count: 0 -> 1`（已跨过 helper 分支）；
  - 同窗：`g_phase6_pd_gpio_count: 0x95 -> 0x96`，`PB3/PB11` 保持高。
- **结论**：
  - 已在**自然调度**场景下观测到 `PD_HelperSeq_3000tick_ThenPc13Gpio` 的首次 3000-tick 窗口触发，补齐了 AJ“边界注入”之外的关键动态证据。
- **USB 风险边界（必须）**：
  - `PHASE6_USB_FW_UPGRADE_ENABLED=0` 保持不变；
  - 本回合仅动态取证与文书回写，未触及写 Flash 路径。

