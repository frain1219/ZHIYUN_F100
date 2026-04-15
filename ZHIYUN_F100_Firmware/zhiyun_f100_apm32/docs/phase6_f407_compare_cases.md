# Phase6 F407 对比用例（副板绕过 + OLED 抓取）

## 目标

在 Renode 不能稳定承载量产镜像完整行为验证的前提下，使用 F407 侧车提供可重复、可回溯的物理监听/注入证据链。

本文件覆盖三类能力：

- 软件链路自检（命令/统计/日志可用性）
- 副板缺失检测绕过（自动 FSM）
- OLED 链路识别与抓取（边沿/事务/字节流）

脚本入口：`scripts/f407_phase6_compare_cases.py`

## 前置条件

- F407 已烧录 `stm32f407vgt6_probe` 最新固件。
- 接线已按 `Document/APM32F103CB_to_STM32F407V_Wiring.md` 完成（含 OLED 必选三线）。
- 主机串口可访问（例如 `/dev/tty.usbmodem*`）。
- Python 依赖：`python3 -m pip install pyserial`

## 阶段 A：未接线软件自检

### Case A1 能力与引脚表

- 命令：`PING` -> `GET_CAPS` -> `LIST_PINS` -> `SELF_CHECK`
- 通过标准：
  - `GET_CAPS` 含 `SELF_CHECK`、`I2C_SLAVE_EMU`、`SPI_SLAVE_EMU`
  - `LIST_PINS` 返回扩展监听/注入引脚
  - `SELF_CHECK` 返回引脚分类与 EXTI watch 列表

### Case A2 统计链路

- 命令：`RESET_STATS` -> `SCENE_LIST` -> `SCENE_RUN KEY_RIGHT_TAP` -> `SCENE_STOP` -> `GET_STATS`
- 通过标准：
  - 命令返回 `ok/implemented=1`
  - `GET_STATS` 结构完整、无解析异常

## 阶段 B：统一接线后对比

### Case B1 副板缺失绕过（自动 FSM）

- 目标：无副板真硬件条件下，F103 不再“开机即关机”。
- 命令：
  - `BYPASS_FSM STATUS`
  - `BYPASS_FSM ON`
  - `SUBSCRIBE ON`
  - 启动 F103 后执行 `GET_STATUS`、`GET_STATS`
- 通过标准：
  - `BYPASS_FSM` 状态为启用
  - `GET_STATS` 中 bypass 计数增长（触发/成功/超时字段）
  - 侧车日志存在对应事件/快照，且 F103 进入稳定开机流程

### Case B2 OLED 总线类型判定

- 目标：明确 OLED 链路为 SPI/I2C 中哪一种。
- 命令：
  - `OLED_SNIFF_STATUS`
  - `OLED_SNIFF_START`
  - 触发 F103 开机与 UI 刷新
  - `OLED_SNIFF_STATUS`
  - `OLED_SNIFF_STOP`
- 通过标准：
  - 返回明确 `bus_type`（预期 SPI）
  - 能观察到事务计数与字节计数增长

### Case B3 OLED 抓包导出可分析数据

- 目标：导出后续解码所需记录（时间戳 + 事务/边沿/字节流）。
- 命令：
  - `OLED_SNIFF_START`
  - 触发模式切换/编码器操作
  - `OLED_SNIFF_DUMP edge 256`
  - `OLED_SNIFF_DUMP txn 64`
  - `OLED_SNIFF_DUMP bytes 1024`
  - `OLED_SNIFF_STOP`
- 通过标准：
  - `edge` 数据含 `t_us + line + level`
  - `txn` 数据含 `start/end + cs 窗口 + payload_len`
  - `bytes` 数据可被后续脚本重放/解码

## 运行方式

```bash
python3 ./scripts/f407_phase6_compare_cases.py /dev/tty.usbmodemXXXX 115200
```

输出目录：

- `logs/f407-phase6/<timestamp>/caseA_software_check.log`
- `logs/f407-phase6/<timestamp>/caseB1_bypass_fsm.log`
- `logs/f407-phase6/<timestamp>/caseB2_oled_bus_type.log`
- `logs/f407-phase6/<timestamp>/caseB3_oled_capture.log`
- `logs/f407-phase6/<timestamp>/summary.json`

## 证据记录模板（必须回填）

- 执行时间：
- 串口：
- F103 镜像版本：
- F407 固件版本：
- 接线版本（引用 `APM32F103CB_to_STM32F407V_Wiring.md` 提交）：
- Case A1 结论（通过/失败 + 关键输出）：
- Case A2 结论（通过/失败 + 关键输出）：
- Case B1 结论（通过/失败 + bypass 统计 + 开机稳定性）：
- Case B2 结论（bus_type + 判定依据）：
- Case B3 结论（edge/txn/bytes 文件名与条目数）：
- 风险与回滚（是否恢复 `BYPASS_FSM OFF`、是否回退到纯监听模式）：
- 异常说明（若有）：

# Phase6 F407 对比用例（I2C/SPI emu + 软件自检）

## 目标

在 Renode 对量产镜像仿真存在限制的前提下，使用 F407 侧车固件提供可重复的协议级证据。
本文件聚焦两阶段：

- 未接线阶段：只做软件层自检，确保命令与统计链路可用。
- 统一接线后：执行 I2C/SPI/场景并发对比，产出可回溯日志。

脚本入口：`scripts/f407_phase6_compare_cases.py`

## 前置条件

- F407 已烧录 `stm32f407vgt6_probe` 最新固件。
- 主机串口可访问（例如 `/dev/tty.usbmodem*`）。
- Python 依赖：`python3 -m pip install pyserial`

## 阶段 A：未接线软件自检（新增）

### Case A1: 能力与引脚表自检

- 目标：确认软件层能力暴露完整，且扩展 IO 已纳入引脚表。
- 关键命令：
  - `PING`
  - `GET_CAPS`
  - `LIST_PINS`
  - `SELF_CHECK`
- 通过标准：
  - `GET_CAPS` 中包含 `SELF_CHECK`。
  - `LIST_PINS` 可见扩展组：`PG5/PG6/PG7`、`PF7~PF12`。
  - `SELF_CHECK` 返回 `pins_total`、`pins_injectable`、`pins_monitor_only`、`exti_watch`。

### Case A2: 协议与统计链路自检

- 目标：未接线时验证命令执行和统计字段更新路径。
- 关键命令：
  - `RESET_STATS`
  - `SCENE_LIST`
  - `SCENE_RUN KEY_RIGHT_TAP`
  - `SCENE_STOP`
  - `GET_STATS`
- 通过标准：
  - 命令返回 `ok/implemented=1`。
  - `GET_STATS` 返回结构完整，不出现解析错误。

## 阶段 B：统一接线后对比用例

### Case 1: I2C 寄存器镜像一致性

- 目标：验证 `I2C_SLAVE_EMU` 的寄存器写入、读取、事务路径可重复。
- 关键命令：
  - `I2C_SLAVE_EMU CFG 0x3C`
  - `I2C_SLAVE_EMU REG_WRITE 0x10 0x5A`
  - `I2C_SLAVE_EMU XFER r 0x10`
  - `GET_STATS`
- 通过标准：
  - 返回值与写入一致。
  - `emu_i2c_req_count`、`emu_i2c_hit_count` 增长，`timeout` 不异常。

### Case 2: SPI 固定帧回放一致性

- 目标：验证 `SPI_SLAVE_EMU` 脚本循环回复与请求匹配。
- 关键命令：
  - `SPI_SLAVE_EMU CFG PE5 0 0x11,0x22,0x33,0x44`
  - `SPI_SLAVE_EMU XFER 0xAA,0xBB,0xCC,0xDD`
  - `SPI_SLAVE_EMU XFER 0x01,0x02,0x03`
  - `GET_STATS`
- 通过标准：
  - `rx` 数组与脚本循环一致。
  - `emu_spi_req_count`、`emu_spi_hit_count` 增长，`timeout` 不异常。

### Case 3: Phase6 场景并发路径

- 目标：验证 emu 命令与 `SCENE_RUN`/`SUBSCRIBE`/`GET_STATUS` 并发可用。
- 关键命令：
  - `SUBSCRIBE ON`
  - `SCENE_RUN KEY_RIGHT_TAP`
  - `I2C_SLAVE_EMU XFER r 0x10`
  - `SPI_SLAVE_EMU XFER 0x55,0x66`
  - `GET_STATUS`
  - `GET_STATS`
- 通过标准：
  - 可持续输出 `event`/`snapshot`。
  - `emu_i2c_*`、`emu_spi_*` 与事件统计同时增长，无异常清零。

## 运行方式

```bash
python3 ./scripts/f407_phase6_compare_cases.py /dev/tty.usbmodemXXXX 115200
```

输出目录：

- `logs/f407-phase6/<timestamp>/caseA_software_check.log`
- `logs/f407-phase6/<timestamp>/case1_i2c.log`
- `logs/f407-phase6/<timestamp>/case2_spi.log`
- `logs/f407-phase6/<timestamp>/case3_phase6_scene.log`
- `logs/f407-phase6/<timestamp>/summary.json`

## 记录模板

- 执行时间：
- 串口：
- 固件版本：
- Case A1 结论（通过/失败 + 证据）：
- Case A2 结论（通过/失败 + 证据）：
- Case 1 结论（通过/失败 + 证据）：
- Case 2 结论（通过/失败 + 证据）：
- Case 3 结论（通过/失败 + 证据）：
- 异常说明（若有）：

