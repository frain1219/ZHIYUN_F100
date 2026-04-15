# 逆向工具指南（给 Agent 的“选工具”速查）

> 本文件只描述**该用什么工具做什么**、**能产出什么证据**、以及必要的**边界/红线**。不规定推进流程与交付判定（以任务书与各阶段文档为准）。

---

## 0) 总原则（避免“会做但不可用”）

- **优先产出可复核证据**：伪代码/反汇编片段、xrefs/调用路径、类型布局、寄存器/SRAM 快照、脚本与日志。
- **先静态后动态（通常）**：静态用于定位/假设；动态用于确认“运行时事实”（寄存器、内存变化、到达路径）。
- **别把工具输出当结论**：任何“检测/猜测”（如 crypto/anti-analysis）都要用 `xrefs`、调用路径与上下文二次验证。

---

## 1) Ghidra MCP：静态分析（结构化、可复用证据）

### 1.1 什么时候用

- **你要回答“代码里有什么/在哪里/谁引用谁”**：函数、字符串、常量、段、交叉引用、调用关系。
- **你要形成可协作知识库**：重命名、注释、书签、批量标注、类型应用（结构体/枚举）。
- **你要做版本/变体比对**：函数 diff、相似函数搜索、模糊匹配。

### 1.2 能给出的“证据形态”

- **伪代码/反汇编**：单函数或批量导出，便于归档与复盘。
- **引用与路径**：某地址/函数的 callers/callees、调用图深度遍历。
- **类型事实**：结构体布局、字段访问上下文、类型应用后的反编译质量变化。
- **差异证据**：两程序同一功能的 diff/相似度列表。

### 1.3 最小闭环（必须能跑通）

- **打开**：`open_program(path, auto_analyze=false|true)`
- **分析**：`run_analysis(...)`（必要时 `reanalyze(...)`）
- **固化工程状态**：做完一轮重命名/注释/类型应用后 `save_program(...)`，避免证据与工程状态漂移。

### 1.4 “目标 → 该用哪些能力”（不限定具体细节，按需组合）

- **想快速总览固件**：函数清单、字符串清单、段/入口点。
  - 常用：`list_functions_enhanced`、`list_strings`、`list_segments`、`get_entry_points`
- **想锁定某功能点（字符串/常量/特征）**：先搜索，再追 xrefs 与调用路径。
  - 常用：`search_strings` / `search_byte_patterns` → `get_xrefs_to` / `get_xrefs_from` → `get_function_call_graph`
- **想解释某函数在干什么**：伪代码 + 关键跳转/调用 + 上下文引用。
  - 常用：`decompile_function`（不稳则 `force_decompile`）+ `get_function_callers/callees` + `get_function_jump_targets`
- **想把“猜测”变成团队可用资产**：重命名/注释/书签/批量标注，并保存工程。
  - 常用：`rename_function(_by_address)`、`rename_variable(s)`、`set_decompiler_comment`、`set_bookmark`、`batch_`* + `save_program`
- **想把“内存形状”变成类型证据**：创建/导入结构体 → 应用到地址 → 看字段访问与反编译改善。
  - 常用：`create_struct` / `import_data_types` → `apply_data_type` → `get_struct_layout` / `analyze_struct_field_usage`
- **想做跨版本差异/复用定位**：函数 diff 与相似度搜索。
  - 常用：`diff_functions`、`find_similar_functions(_fuzzy)`、`bulk_fuzzy_match`

### 1.5 边界与注意事项（约束，但不限制发挥）

- **输出性质**：MCP 输出是**分析证据（文本/JSON）**，不是可直接编译的 C 工程；可编译重构以 SDK 工程手写/半自动为准。
- **多地址空间**：涉及地址参数时，必要时用 `addressSpace:address`（如 `mem:0x...`）消歧。
- **“检测类工具”**：如 IOC/crypto/anti-analysis/恶意行为检测，只当线索；必须用 xrefs/调用路径/上下文证实。
- **脚本化扩展**：内置能力不够再用脚本（列脚本/运行脚本/inline 脚本），但应优先保证输出可复核、可复跑。

---

## 2) Renode + 本机 GDB 脚本：动态采集（寄存器/SRAM 运行时事实）

### 2.1 什么时候用

- **你要回答“运行时到底发生了什么”**：寄存器值、SRAM 内容/影子变量、是否到达某 PC、断点命中轨迹。
- **无 `.map` 仍需闭合路径**：辅助闭合散载/解压/启动路径的证据链。

### 2.2 红线（必须遵守）

- **不要依赖 F100 GDB MCP**：按任务书 Handoff 使用仓库内 `.sh`/`.gdb`/`.resc` 与 `arm-none-eabi-gdb`。

### 2.3 工作方式（允许迭代，不强行规定路径）

- 在终端中**编写/修改/重跑**脚本与 `.resc`，反复运行 Renode + GDB。
- 以日志与二进制快照为准：直到 `logs/renode-oi001/LATEST`（或 Phase 6 的 `logs/renode-phase6/`）产物满足分析需要。
- 已知限制与细节参考：`04_Protocol_Reverse.md` §1.3.8、`05_Full_Reconstruction.md` §6.4。

### 2.4 脚本与产物入口（“用哪个脚本拿什么证据”）


| 入口                                       | 用途（证据）                                        | 主要产物位置                                      |
| ---------------------------------------- | --------------------------------------------- | ------------------------------------------- |
| `scripts/oi001_capture_all.sh`           | 一键聚合：Renode 自动运行 + 采集/监控                      | `logs/renode-oi001/<时间戳>/`                  |
| `scripts/renode_gdb_capture.sh`          | GDB 批处理入口：连接 Renode 并 `source` 对应 `.gdb`      | `logs/renode-oi001/<时间戳>/gdb_capture.log` 等 |
| `scripts/renode_phase6_capture.sh`       | Phase 6 固件快照：向量表 SP/PC + 寄存器 + SRAM 20KB dump | `logs/renode-phase6/<时间戳>/`                 |
| `scripts/renode_phase6_pd_breaktrace.sh` | Phase 6 PD 断点追踪：命中次数 + SRAM 影子状态              | `logs/renode-phase6/<时间戳>/`                 |
| `Renode/apm32_f100_gdb_first.resc`       | 手动两终端方案（Renode + GDB）                         | 按运行目录落盘                                     |


> 备注：`OI001_MODE=hbreak` 在 Renode 1.16.1 下可能常超时（见 `04_Protocol_Reverse.md` §1.3.14）；需要时再试，不要把它当默认路径。

### 2.5 交给 Agent 分析的材料（标准输入）

- **优先**：`logs/renode-oi001/LATEST` 指向目录下的 `.log` / `.bin`
- **Phase 6**：`logs/renode-phase6/<时间戳>/` 下的快照与追踪日志

---

## 3) 实物逆向工具：F407 探针侧车 + F103 量产板（补齐动态证据 + 还原度对比）

### 3.1 什么时候用

- **Renode/GDB 覆盖不到或可信度不足时**：需要真实电气环境下的数字边沿、OLED 总线事务、ADC 电压、以及“接线/副板存在性”带来的状态机行为。
- **要做“行为一致性对照”**：将重构固件与量产固件在同一组观测点上对比（事件统计、总线抓包、场景触发结果），形成还原度证据。
- **要做“解阻塞实验”**：在不改 F103 量产固件的前提下，使用侧车的短时注入/协议级 emu/逻辑级 emu，验证假设或绕过缺失外设。

### 3.2 组成与定位

- **F103 核心板**：运行原版量产固件（主控职责不变）。
- **F407 核心板（探针固件）**：默认高阻监听，白名单引脚允许短时注入；用于监听/注入/模拟/取证。

参考资料（必须读）：

- 接线与安全规则：`Document/APM32F103CB_to_STM32F407V_Wiring.md`
- 探针固件工程：`ZHIYUN_F100_Firmware/stm32f407vgt6_probe/`（能力说明见 `docs/`）

### 3.3 能给出的“证据形态”

- **数字监听证据**：1ms 轮询快照 + 关键线 EXTI 边沿事件（带 `t_ms/t_us` 时间戳）；`GET_STATUS`/`SUBSCRIBE` 形成可回放事件流。
- **可观测统计**：`GET_STATS` 给出队列深度、丢帧/丢样、EXTI/轮询来源拆分、OLED 抓包计数等，使“可靠度”可量化。
- **OLED 取证**：`OLED_SNIFF_`* 识别总线类型并导出边沿/事务/字节流（`OLED_SNIFF_DUMP edge|txn|bytes`），用于离线解码与对比。
- **ADC 证据**：`ADC_READ`/`ADC_STREAM_`* 采样真实模拟量（注意模拟线不串 100 欧）。
- **注入/脚本证据**：`GPIO_SET/RELEASE/PULSE`、`SEQ_PLAY`、`SCENE_RUN` 形成可复现实验输入；必要时结合 `RESET_STATS` 进行 A/B 对照。
- **模拟/仿真（补齐外设）**：
  - 逻辑级：`EMU_PL5500_STAT`、`EMU_MP3398_`*
  - 协议级：`I2C_SLAVE_EMU`、`SPI_SLAVE_EMU`
  - 注意：这些是逻辑/协议级，不等价真实功率级与物理时序。

### 3.4 红线与安全边界（必须遵守）

- **断电改线**：修改杜邦线必须断电，禁止热插拔。
- **必须共地**：两板至少 1 根 GND（建议多根）。
- **默认高阻**：F407 默认高阻输入；仅白名单引脚允许短时注入，并且必须能 `GPIO_RELEASE` 回到高阻。
- **禁止长期对驱**：禁止让 F407 推挽输出长期对驱 F103 输出。
- **电阻规则**：数字线默认串联 100 欧（每条线只串一次）；模拟采样线禁止串联 100 欧。
- **禁接信号**：不要把以下信号用杜邦线直连到 F407 普通 GPIO（见接线文档详细规则）：`NRST`、`PA11/PA12`（USB D-/D+）、`PD0/PD1`（HSE）、`PC14/PC15`（LSE）。
- **只监听通道不可注入**：例如 OLED 抓取线按文档要求只监听，不允许注入输出。

### 3.5 当功能不足时：允许扩展探针固件（鼓励，但要守约束）

- **允许自主规划增强能力**：当现有命令/观测点不足以支撑逆向取证或对比验证时，agent 可以直接在 `ZHIYUN_F100_Firmware/stm32f407vgt6_probe/` 内规划并实现新增能力（例如新增监听引脚、增加更高分辨率的采样/缓冲、扩展 OLED/总线解码、增加新的场景脚本、补充统计字段、导出更适合离线分析的日志格式等）。
- **必须保持安全默认**：新增功能不得破坏“上电全引脚高阻、仅白名单可注入、可随时 `GPIO_RELEASE` 回高阻”的安全基线；新增注入能力必须显式白名单化。
- **必须可观测、可复现、可回滚**：新增能力应同时提供（或复用）`GET_CAPS/GET_STATS/SELF_CHECK` 等可观测证据；命令与场景应可脚本化重复执行；需要提供关闭/停止/回退到纯监听的路径。
- **必须遵守接线规范**：任何新增的引脚/注入策略都要与 `Document/APM32F103CB_to_STM32F407V_Wiring.md` 的禁接项、串联电阻规则、只监听通道等约束一致。
- **新增接线必须同步记录**：一旦新增/调整了侧车接线（引脚映射、串联电阻、只监听/可注入属性、禁接项等），必须同步更新 `Document/APM32F103CB_to_STM32F407V_Wiring.md`，并确保探针固件的 `LIST_PINS/SELF_CHECK/GET_CAPS` 等输出与文档一致。