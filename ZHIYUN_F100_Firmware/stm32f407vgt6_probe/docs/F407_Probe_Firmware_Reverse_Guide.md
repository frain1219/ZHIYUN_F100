# F407 逆向侧车固件说明

> 工程路径：`ZHIYUN_F100_Firmware/stm32f407vgt6_probe/`  
> 定位：STM32F407 作为 F103 量产固件逆向侧车，用于监听、注入、协议级模拟与取证。

## 1. 设计原则

- 默认安全：上电全引脚高阻，只有白名单可注入。
- 可观测优先：所有队列覆盖/丢包都可在 `GET_STATS` 中观察。
- 可复现实验：命令与场景脚本可重复执行并留痕。
- 逆向导向：所有能力服务于“解阻塞、对比验证、行为证据”。

## 2. 当前实现能力

### 2.1 监听（Digital）

- 1ms 轮询 `LIST_PINS` 全表。
- 关键线 EXTI 边沿捕获（`PE0/PG2/PG5/PG6/PG7`），以及 OLED 抓取线（`PE8~PE12`）。
- EXTI 事件记录已改为端口感知（按 `port+pin` 归属），避免同编号不同端口误记。
- 事件包含 `t_ms` 与 `t_us`。
- `GET_STATUS` 输出快照；`SUBSCRIBE ON` 可推送 `event/snapshot`。

### 2.2 注入（Digital drive）

- `GPIO_SET/GPIO_RELEASE/GPIO_PULSE/PIN_CFG`。
- `SEQ_PLAY/SEQ_ABORT` 多步脚本回放。
- `SCENE_LIST/SCENE_RUN/SCENE_STOP` 内置场景触发。

### 2.3 模拟量（ADC）

- `ADC_READ`：`PF3/PF4/PF5/PF6` 单次读取。
- `ADC_STREAM_START/STOP` 周期采样上报。
- `ADC_TRIGGER` 阈值触发事件。

### 2.4 外围行为模拟（逻辑级）

- `EMU_PL5500_STAT`：`OFF/TRICKLE/FAST/DONE`。
- `EMU_MP3398_BIND` + `EMU_MP3398_PWM/ADIM/ENABLE`。
- 说明：为逻辑级模拟，不代表真实功率级行为。

### 2.5 总线 emu（协议级）

- `I2C_SLAVE_EMU`：寄存器镜像与单次事务。
- `SPI_SLAVE_EMU`：脚本循环回复与单次事务。
- 说明：仅协议级，不覆盖物理电气时序。

### 2.6 协议与统计

- USB CDC 文本行协议（`\r\n` 分隔）。
- `GET_CAPS` 暴露能力集与限制。
- `GET_STATS/RESET_STATS` 观测与清零计数。
- 新增 `SELF_CHECK`：未接线阶段的软件层自检汇总。
- 新增 `BYPASS_FSM ON|OFF|STATUS|RESET`：副板缺失自动绕过 FSM（默认关闭）。
- 新增 `OLED_SNIFF_START/STOP/STATUS/DUMP`：OLED 监听、总线类型识别与抓包导出。

## 3. 常用命令

- 基础：`PING`、`HELP`、`LIST_PINS`、`GET_STATUS`、`SUBSCRIBE ON|OFF`
- 可观测：`GET_CAPS`、`GET_STATS`、`RESET_STATS`、`SELF_CHECK`
- 注入：`GPIO_SET/GPIO_RELEASE/GPIO_PULSE/PIN_CFG`
- 脚本：`SEQ_PLAY/SEQ_ABORT`、`SCENE_LIST/SCENE_RUN/SCENE_STOP`
- ADC：`ADC_READ`、`ADC_STREAM_START/STOP`、`ADC_TRIGGER`
- emu：`EMU_PL5500_STAT`、`EMU_MP3398_*`、`I2C_SLAVE_EMU`、`SPI_SLAVE_EMU`
- 副板绕过：`BYPASS_FSM ON|OFF|STATUS|RESET`
- OLED 抓取：`OLED_SNIFF_START`、`OLED_SNIFF_STOP`、`OLED_SNIFF_STATUS`、`OLED_SNIFF_DUMP edge|txn|bytes [limit]`

## 4. 未接线阶段建议流程

1. `PING`
2. `GET_CAPS`
3. `LIST_PINS`
4. `SELF_CHECK`
5. `RESET_STATS`
6. `SCENE_RUN KEY_RIGHT_TAP`
7. `GET_STATS`

通过标准：

- 命令全部可执行，无解析错误。
- `SELF_CHECK` 返回引脚计数与 EXTI 监视列表。
- `GET_STATS` 字段完整可读。

## 5. 关键限制

- **PF1 / PG1 在 STM32F407VGT6 LQFP100 封装中未引出**，引脚表中已移除。`BYPASS_FSM` 功能依赖 PF1（注入）和 PG1（监听 VS_EN），在当前硬件上无法正常工作，命令可执行但 `trigger_count` 不会增长。
- EXTI 仍受硬件 line 资源约束，不能无上限并发捕获。
- ADC 流当前为软件轮询，非 DMA 环形模式。
- USB 背压下仍可能丢帧，但丢帧可见。
- MP3398/I2C/SPI emu 为逻辑级或协议级，不等价真实电气行为。
- OLED sniffer 当前以 EXTI 边沿采样为主，极高频边沿可能丢样；需结合 `GET_STATS` 中 `oled_drop_count` 评估可靠度。

## 6. 对照与接线文档

- 能力矩阵：`ZHIYUN_F100_Firmware/stm32f407vgt6_probe/docs/probe_capability_matrix.md`
- 接线规范：`Document/APM32F103CB_to_STM32F407V_Wiring.md`
- Phase6 对比用例：`ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_f407_compare_cases.md`

## 7. 本轮发现、验证与回滚

### 7.1 发现了什么

- 新增 `BYPASS_FSM` 自动绕过能力（默认关闭，可命令启停），用于“无副板时防止开机即关机”场景。
- 新增 `OLED_SNIFF_*` 抓取能力，支持边沿记录、SPI 事务聚合与字节流导出，并给出 `bus_type` 判定字段。
- `GET_STATS` 扩展可观测计数：
  - `bypass_trigger/success/timeout/active_cycles`
  - `oled_edge/txn/byte/drop`
- 新增 OLED 接线建议并在文档中先定义后实施（`PE8~PE12` 监听映射）。

### 7.2 怎么验证

- 固件构建：在工程目录执行 `pio run`，确保 `sheritech_f407vg_cmsis_dap` 编译通过。
- 软件自检与对比用例：执行  
  `python3 ./scripts/f407_phase6_compare_cases.py /dev/tty.usbmodemXXXX 115200`
- 关注输出：
  - `logs/f407-phase6/<timestamp>/caseB1_bypass_fsm.log`
  - `logs/f407-phase6/<timestamp>/caseB2_oled_bus_type.log`
  - `logs/f407-phase6/<timestamp>/caseB3_oled_capture.log`
  - `logs/f407-phase6/<timestamp>/summary.json`

### 7.3 风险与回滚

- 风险：
  - 自动绕过 FSM 在接线错误或场景不匹配时可能误触发。
  - OLED 高速边沿在极端负载下可能出现采样丢失（`oled_drop_count` 增长）。
- 回滚：
  - 运行时回滚：`BYPASS_FSM OFF`、`OLED_SNIFF_STOP`，恢复纯监听/手动注入流程。
  - 脚本回滚：仅运行 `caseA_software_check`，跳过 Bypass/OLED 用例。
  - 工程回滚：恢复到不启用 `BYPASS_FSM` 与 `OLED_SNIFF_*` 命令的基线固件版本。