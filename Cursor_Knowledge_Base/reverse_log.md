# 智云 F100 固件逆向日志（第一阶段：系统启动与向量表）

## [系统参数]

- 芯片参考：APM32F103（规格书与 SDK 交叉验证）
- Flash 起始地址：`0x08000000`
- SRAM 起始地址：`0x20000000`
- SRAM 估算范围（20KB）：`0x20000000 - 0x20004FFF`
- 向量表基地址（运行时）：`0x08000000`（Cortex-M3 默认映射）
- 初始 Stack Pointer（向量表第 0 项）：`0x20001620`

> 说明：当前 Ghidra 项目语言显示为 `ARM:LE:32:v7`，但向量表内容与 Cortex-M3 启动模式一致（SP + Reset 向量）。

## [向量表前 16 项结构核对]

根据固件 `0x08000000` 处 64 字节原始数据解码（little-endian）：

| Index | 含义（Cortex-M3） | 原始值 | 备注 |
|---|---|---|---|
| 0 | Initial SP | `0x20001620` | 栈顶初值 |
| 1 | Reset_Handler | `0x08006BE9` | Thumb 入口为 `0x08006BE8` |
| 2 | NMI_Handler | `0x08006AB1` | |
| 3 | HardFault_Handler | `0x08006AB3` | |
| 4 | MemManage_Handler | `0x08006AB5` | |
| 5 | BusFault_Handler | `0x08006AB7` | |
| 6 | UsageFault_Handler | `0x08006AB9` | |
| 7 | Reserved | `0x00000000` | |
| 8 | Reserved | `0x00000000` | |
| 9 | Reserved | `0x00000000` | |
| 10 | Reserved | `0x00000000` | |
| 11 | SVC_Handler | `0x08006ABB` | |
| 12 | DebugMon_Handler | `0x08006ABD` | |
| 13 | Reserved | `0x00000000` | |
| 14 | PendSV_Handler | `0x08006ABF` | |
| 15 | SysTick_Handler | `0x08006AC1` | |

与 SDK 启动文件 `startup_apm32f103xc.s` 的前 16 项结构定义一致（SP, Reset, NMI...SysTick）。

## [关键锚点]

- `Reset_Handler`：`0x08006BE8`（向量表给出 `0x08006BE9`，去掉 Thumb bit 后为偶地址）
- `Reset_Handler` 第一跳子函数（疑似 `SystemInit`）：`0x080069DC`
  - 观察到对 `0x40021000`（RCM/RCC 基址）和 `0xE000ED08`（SCB->VTOR）写操作，符合时钟/向量表初始化特征
- `Reset_Handler` 第二跳：`0x08006BF8`（函数指针跳转，疑似 C Runtime 入口）
- `main` 疑似逻辑入口：`0x08002E74`
  - 由 `0x08006B8C` 调用后进入长循环，不返回，符合业务主循环特征

## [启动链追踪]

1. 向量表第 1 项指向 `Reset_Handler`
2. `Reset_Handler` 执行两次间接跳转：
   - 第一跳 -> `0x080069DC`（疑似 `SystemInit`）
   - 第二跳 -> `0x08006BF8`（疑似 `__main`/C Runtime）
3. C Runtime 链中出现 `0x08006B8C`，进一步调用 `0x08002E74`（疑似主逻辑）

### 关于 SP 初始化与 `.data/.bss` 拷贝

- **SP 初始化**：由向量表第 0 项 `0x20001620` 在异常进入 Reset 时自动装载到 MSP（硬件行为），未见额外手工 `MOV SP, ...`。
- **`.data/.bss` 初始化**：在当前 Ghidra 语义下未直接反编译出清晰拷贝循环；结合启动模板可推断由第二跳（疑似 `__main`）执行：
  - `.data` 从 Flash 拷贝到 SRAM
  - `.bss` 清零

> 中文注释：该段属于典型 C Runtime 初始化，通常发生在 `SystemInit` 之后、`main` 之前。

## [Ghidra 入口点一致性验证]

- Ghidra `get_entry_points` 当前返回了 ARM 异常向量风格标签（如 `Reset @ 00000000`），与 Cortex-M3 常见“向量表在 Flash 基址”的展示方式存在差异。
- 通过原始向量表读取可确认“实际启动入口”为 `0x08006BE8`（Reset 向量）。
- 结论：**工具标签入口** 与 **固件真实 Reset 入口** 在展示层存在偏差，但实质可对齐。

## [SVD 验证]

- 随机抽查地址：`0x40021000`
- 在当前工程中：
  - `list_segments` 可见该地址段名称为 `RCM`
  - 但在 Decompile（如 `0x080069DC`）中多数表现为数据指针常量（如 `DAT_xxx -> 0x40021000`），而非直接显示结构化符号 `RCC/RCM->寄存器`
- 结论：**部分符号化（内存块级）**，但 **伪代码层主要仍是原始地址/常量引用**。

## [阶段结论]

- 已锁定启动主链：`VectorTable -> Reset_Handler(0x08006BE8) -> SystemInit?(0x080069DC) -> C Runtime?(0x08006BF8/0x08006B8C) -> main?(0x08002E74)`。
- 已完成系统参数、关键锚点与 SVD 可见性记录。
- 下一阶段建议：在 Ghidra 中重建为 Cortex-M3 专用导入配置后，补齐 `.data/.bss` 具体拷贝循环地址与寄存器级注释。
