# 智云 F100 固件逆向工程项目目录

> **项目目标**: 对智云F100棒灯的APM32F103CBT6固件进行逆向分析
> **创建日期**: 2026-04-04
> **文档版本**: v1.1（任务书「关键发现」已迁至独立附录文件，见 **§5.6**）

---

## 目录

- [1. 项目概述](#1-项目概述)
- [2. 主控芯片信息](#2-主控芯片信息)
- [3. SDK资源目录](#3-sdk资源目录)
- [4. 文档资源目录](#4-文档资源目录)
- [5. 逆向知识库目录](#5-逆向知识库目录)
  - [5.6 任务书与关键发现附录](#56-任务书与关键发现附录)
- [6. 快速查找指南](#6-快速查找指南)

---

## 1. 项目概述

智云F100是一款支持最高100W双色温/RGB输出的棒灯产品，本项目旨在对其固件进行逆向分析，理解其工作原理和功能实现。

### 1.1 硬件架构

- **主板**: 充放电管理、电源路径管理、主控MCU
- **副板**: 降压/升压驱动、混色电路

### 1.2 核心功能

- 双色温/RGB LED驱动
- Type-C PD输入（最大100W）
- 动态电源路径管理(DPPM)
- 电池充放电管理
- OLED显示与用户交互

---

## 2. 主控芯片信息

### 2.1 APM32F103CBT6 规格参数


| 参数    | 值                           |
| ----- | --------------------------- |
| 型号    | APM32F103CBT6               |
| 系列    | APM32F103xB (128KB Flash系列) |
| 封装    | LQFP48                      |
| 内核    | ARM Cortex-M3               |
| 最高频率  | 96MHz                       |
| Flash | 128KB                       |
| SRAM  | 20KB                        |
| GPIO  | 37个                         |
| 定时器   | 3个通用定时器 + 1个高级定时器           |
| ADC   | 2个12位ADC (16通道)             |
| USART | 3个                          |
| SPI   | 2个                          |
| I2C   | 2个                          |
| USB   | USB 2.0 FS Device           |
| CAN   | CAN 2.0B                    |


### 2.2 SDK型号命名规则

APM32F103系列后缀含义：


| 后缀     | Flash容量   | CBT6归属       |
| ------ | --------- | ------------ |
| x4     | 16KB      | ❌ 非CBT6      |
| x6     | 32KB      | ❌ 非CBT6      |
| x8     | 64KB      | ❌ 非CBT6      |
| **xB** | **128KB** | ✅ **CBT6专用** |
| xC     | 256KB     | ❌ 非CBT6      |
| xD     | 384KB     | ❌ 非CBT6      |
| xE     | 512KB     | ❌ 非CBT6      |


### 2.3 CBT6专用资源标识

在SDK中，CBT6专用资源文件命名包含 `103xb` 标识：

- **启动文件**: `startup_apm32f103xb.s`
- **头文件**: `apm32f103xb.h`
- **链接脚本**: `apm32f103xb_flash.ld`

### 2.4 STM32F407 侧车（引脚监听/激励预备 · 非量产主控）


| 路径                                                                                     | 说明                                                                                                  |
| -------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| [射日科技STM32F407资料/](射日科技STM32F407资料/)                                                   | F407V 核心板原理图（如 `4，原理图&封装库/STM32F407VET6_V1.2(1).pdf`）、Keil 例程（**HSE 25 MHz**、例 1 **PA0** LED）       |
| [CH549G下载器/](CH549G下载器/)                                                               | WCH-Link 相关驱动、CH549 原理图 PDF；DAP 模式配合 **CMSIS-DAP** 烧录（异常恢复见 `烧录固件工具/LED灯闪烁解决方法.txt`）                |
| [ZHIYUN_F100_Firmware/stm32f407vgt6_probe/](ZHIYUN_F100_Firmware/stm32f407vgt6_probe/) | **PlatformIO**（`stm32cube`）：默认 **STM32F407VGT6**（`black_f407vg`）+ **cmsis-dap**；任务书 **§7** 有环境与命令说明 |


---

## 3. SDK资源目录

SDK路径: [SDK/](SDK/)

### 3.1 APM32F10x_SDK_V2.0.0

路径: [SDK/APM32F10x_SDK_V2.0.0/](SDK/APM32F10x_SDK_V2.0.0/)

#### 3.1.1 核心库文件 ✅ CBT6专用


| 资源类型          | 文件路径                                                                                                                                                            | 说明           |
| ------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------ |
| **头文件**       | [Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h)                       | CBT6寄存器定义    |
| **启动文件(ARM)** | [Libraries/Device/Geehy/APM32F10x/Source/arm/startup_apm32f103xb.s](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/arm/startup_apm32f103xb.s) | Keil MDK启动代码 |
| **启动文件(IAR)** | [Libraries/Device/Geehy/APM32F10x/Source/iar/startup_apm32f103xb.s](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/iar/startup_apm32f103xb.s) | IAR启动代码      |
| **启动文件(GCC)** | [Libraries/Device/Geehy/APM32F10x/Source/gcc/startup_apm32f103xb.S](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/gcc/startup_apm32f103xb.S) | GCC启动代码      |
| **链接脚本**      | [Libraries/Device/Geehy/APM32F10x/Source/gcc/apm32f103xb_flash.ld](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/gcc/apm32f103xb_flash.ld)   | GCC链接脚本      |
| **系统初始化**     | [Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c)               | 系统时钟初始化      |
| **系统头文件**     | [Libraries/Device/Geehy/APM32F10x/Include/system_apm32f10x.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/system_apm32f10x.h)             | 系统配置头文件      |
| **总头文件**      | [Libraries/Device/Geehy/APM32F10x/Include/apm32f10x.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f10x.h)                           | 设备总头文件       |


#### 3.1.2 标准外设驱动库 ✅ CBT6可用

路径: [SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/)


| 外设    | 头文件                                                                                                         | 源文件                                                                                                         | 用途      |
| ----- | ----------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | ------- |
| GPIO  | [inc/apm32f10x_gpio.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_gpio.h)   | [src/apm32f10x_gpio.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_gpio.c)   | GPIO控制  |
| RCM   | [inc/apm32f10x_rcm.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_rcm.h)     | [src/apm32f10x_rcm.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_rcm.c)     | 复位与时钟管理 |
| TMR   | [inc/apm32f10x_tmr.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_tmr.h)     | [src/apm32f10x_tmr.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_tmr.c)     | 定时器/PWM |
| ADC   | [inc/apm32f10x_adc.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_adc.h)     | [src/apm32f10x_adc.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_adc.c)     | 模数转换    |
| SPI   | [inc/apm32f10x_spi.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_spi.h)     | [src/apm32f10x_spi.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_spi.c)     | SPI通信   |
| I2C   | [inc/apm32f10x_i2c.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_i2c.h)     | [src/apm32f10x_i2c.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_i2c.c)     | I2C通信   |
| USART | [inc/apm32f10x_usart.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_usart.h) | [src/apm32f10x_usart.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_usart.c) | 串口通信    |
| DMA   | [inc/apm32f10x_dma.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_dma.h)     | [src/apm32f10x_dma.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_dma.c)     | DMA传输   |
| FMC   | [inc/apm32f10x_fmc.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_fmc.h)     | [src/apm32f10x_fmc.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_fmc.c)     | Flash控制 |
| USB   | [inc/apm32f10x_usb.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_usb.h)     | [src/apm32f10x_usb.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_usb.c)     | USB设备   |
| CAN   | [inc/apm32f10x_can.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_can.h)     | [src/apm32f10x_can.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/src/apm32f10x_can.c)     | CAN通信   |


#### 3.1.3 CMSIS核心库 ✅ CBT6可用

路径: [SDK/APM32F10x_SDK_V2.0.0/Libraries/CMSIS/](SDK/APM32F10x_SDK_V2.0.0/Libraries/CMSIS/)


| 文件         | 路径                                                                                          | 说明            |
| ---------- | ------------------------------------------------------------------------------------------- | ------------- |
| core_cm3.h | [Core/Include/core_cm3.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/CMSIS/Core/Include/core_cm3.h) | Cortex-M3核心定义 |


#### 3.1.4 示例代码 ✅ CBT6相关

路径: [SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/)


| 示例类别  | 路径                                                                      | 用途        |
| ----- | ----------------------------------------------------------------------- | --------- |
| GPIO  | [GPIO/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/GPIO/)   | GPIO控制示例  |
| TMR   | [TMR/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/TMR/)     | 定时器/PWM示例 |
| ADC   | [ADC/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/ADC/)     | ADC采样示例   |
| SPI   | [SPI/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/SPI/)     | SPI通信示例   |
| I2C   | [I2C/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/I2C/)     | I2C通信示例   |
| USART | [USART/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/USART/) | 串口通信示例    |
| USB   | [USB/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/USB/)     | USB设备示例   |
| DMA   | [DMA/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/DMA/)     | DMA传输示例   |
| FMC   | [FMC/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/FMC/)     | Flash操作示例 |
| PMU   | [PMU/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/PMU/)     | 电源管理示例    |
| RTOS  | [RTOS/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/RTOS/)   | 实时操作系统示例  |


#### 3.1.5 开发板支持 ✅ CBT6相关

路径: [SDK/APM32F10x_SDK_V2.0.0/Boards/Board_APM32F103_MINI/](SDK/APM32F10x_SDK_V2.0.0/Boards/Board_APM32F103_MINI/)


| 文件          | 路径                                                                                          | 说明     |
| ----------- | ------------------------------------------------------------------------------------------- | ------ |
| Board.h     | [Board.h](SDK/APM32F10x_SDK_V2.0.0/Boards/Board_APM32F103_MINI/Board_APM32F103_MINI.h)      | 开发板头文件 |
| board_delay | [inc/board_delay.h](SDK/APM32F10x_SDK_V2.0.0/Boards/Board_APM32F103_MINI/inc/board_delay.h) | 延时函数   |


#### 3.1.6 非CBT6资源 ❌ 仅供参考

以下资源不属于CBT6，仅供逆向参考：


| 资源类型        | 文件                                                                                        | 说明            |
| ----------- | ----------------------------------------------------------------------------------------- | ------------- |
| F103xC头文件   | `apm32f103xc.h`                                                                           | 256KB Flash版本 |
| F103xD头文件   | `apm32f103xd.h`                                                                           | 384KB Flash版本 |
| F103xE头文件   | `apm32f103xe.h`                                                                           | 512KB Flash版本 |
| F105/F107系列 | `apm32f105*.h`, `apm32f107*.h`                                                            | 互联型产品         |
| F107示例      | [Examples/Board_APM32F107_Mini/](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F107_Mini/) | F107开发板示例     |


### 3.2 APM32_USB_SDK_v1.1

路径: [SDK/APM32_USB_SDK_v1.1/](SDK/APM32_USB_SDK_v1.1/)

#### 3.2.1 USB设备库 ✅ CBT6可用


| 资源      | 路径                                                                                                                  | 说明      |
| ------- | ------------------------------------------------------------------------------------------------------------------- | ------- |
| USB设备驱动 | [Libraries/APM32F10x/](SDK/APM32_USB_SDK_v1.1/Libraries/Device/Geehy/APM32F10x/)                                    | USB设备驱动 |
| CDC示例   | [Examples/APM32F1xx/Device_Examples/USBD_CDC/](SDK/APM32_USB_SDK_v1.1/Examples/APM32F1xx/Device_Examples/USBD_CDC/) | 虚拟串口示例  |
| MSC示例   | [Examples/APM32F1xx/Device_Examples/USBD_MSC/](SDK/APM32_USB_SDK_v1.1/Examples/APM32F1xx/Device_Examples/USBD_MSC/) | 大容量存储示例 |
| HID示例   | [Examples/APM32F1xx/Device_Examples/USBD_HID/](SDK/APM32_USB_SDK_v1.1/Examples/APM32F1xx/Device_Examples/USBD_HID/) | 人机接口示例  |


#### 3.2.2 非CBT6资源 ❌ 仅供参考


| 资源类型   | 路径                                                                               | 说明          |
| ------ | -------------------------------------------------------------------------------- | ----------- |
| F0xx系列 | [Libraries/APM32F0xx/](SDK/APM32_USB_SDK_v1.1/Libraries/Device/Geehy/APM32F0xx/) | Cortex-M0系列 |
| F4xx系列 | [Libraries/APM32F4xx/](SDK/APM32_USB_SDK_v1.1/Libraries/Device/Geehy/APM32F4xx/) | Cortex-M4系列 |
| L0xx系列 | [Libraries/APM32L0xx/](SDK/APM32_USB_SDK_v1.1/Libraries/Device/Geehy/APM32L0xx/) | 低功耗系列       |
| E10x系列 | [Libraries/APM32E10x/](SDK/APM32_USB_SDK_v1.1/Libraries/Device/Geehy/APM32E10x/) | 增强型系列       |


### 3.3 中间件库

路径: [SDK/APM32F10x_SDK_V2.0.0/Middlewares/](SDK/APM32F10x_SDK_V2.0.0/Middlewares/)


| 中间件      | 路径                                                          | 说明        |
| -------- | ----------------------------------------------------------- | --------- |
| FatFs    | [FatFs/](SDK/APM32F10x_SDK_V2.0.0/Middlewares/FatFs/)       | 文件系统      |
| FreeRTOS | [FreeRTOS/](SDK/APM32F10x_SDK_V2.0.0/Middlewares/FreeRTOS/) | 实时操作系统    |
| LwIP     | [LwIP/](SDK/APM32F10x_SDK_V2.0.0/Middlewares/LwIP/)         | TCP/IP协议栈 |


---

## 4. 文档资源目录

文档路径: [Document/](Document/)

### 4.1 Markdown文档

路径: [Document/Markdown/](Document/Markdown/)


| 文档                    | 文件                                                                   | 用途           | 重要程度  |
| --------------------- | -------------------------------------------------------------------- | ------------ | ----- |
| **APM32F103 CBT6规格书** | [APM32F103 CBT6规格书.md](Document/Markdown/APM32F103%20CBT6规格书.md)     | 主控芯片完整规格     | ⭐⭐⭐⭐⭐ |
| **IC_MCU逻辑关系推测**      | [IC_MCU_逻辑关系推测.md](Document/Markdown/IC_MCU_逻辑关系推测.md)               | 各IC与MCU交互分析  | ⭐⭐⭐⭐⭐ |
| HUSB238规格书            | [husb238规格书.md](Document/Markdown/husb238规格书.md)                     | Type-C PD控制器 | ⭐⭐⭐⭐  |
| MP3398E规格书            | [MP3398E-规格书.md](Document/Markdown/MP3398E-规格书.md)                   | LED驱动芯片      | ⭐⭐⭐⭐  |
| PL5500规格书             | [pl5500规格书.md](Document/Markdown/pl5500规格书.md)                       | 充放电控制器       | ⭐⭐⭐⭐  |
| MP2491CGQB-Z规格书       | [MP2491CGQB-Z规格书.md](Document/Markdown/MP2491CGQB-Z规格书.md)           | 降压转换器        | ⭐⭐⭐   |
| TP6841S6规格书           | [TP6841S6规格书.md](Document/Markdown/TP6841S6规格书.md)                   | 降压稳压器        | ⭐⭐⭐   |
| HSBB4115规格书           | [HSBB4115%20规格书.md](Document/Markdown/HSBB4115%20规格书.md)             | P-MOSFET     | ⭐⭐    |
| WMQ30P04T1规格书         | [Way-on-WMQ30P04T1规格书.md](Document/Markdown/Way-on-WMQ30P04T1规格书.md) | P-MOSFET     | ⭐⭐    |
| 官方说明书                 | [ZHIYUN_F100_官方说明书.md](Document/Markdown/ZHIYUN_F100_官方说明书.md)       | 用户操作指南       | ⭐⭐⭐   |


### 4.2 PDF文档

路径: [Document/PDF/](Document/PDF/)

PDF文档与Markdown文档内容对应，提供原始规格书参考。

### 4.3 维修笔记


| 文档       | 文件                                                                  | 用途            |
| -------- | ------------------------------------------------------------------- | ------------- |
| **维修笔记** | [Zhiyun_F100_Repair_Notes.md](Document/Zhiyun_F100_Repair_Notes.md) | 硬件拓扑逆向、DPPM分析 |


---

## 5. 逆向知识库目录

知识库路径: [Cursor_Knowledge_Base/](Cursor_Knowledge_Base/)

### 5.1 硬件初始化分析


| 文档          | 文件                                                               | 内容                  |
| ----------- | ---------------------------------------------------------------- | ------------------- |
| **硬件初始化分析** | [02_Hardware_Init.md](Cursor_Knowledge_Base/02_Hardware_Init.md) | GPIO/SPI/ADC初始化代码分析 |


### 5.2 协议与 PD/GPIO 证据（Phase 4）


| 文档       | 文件                                                                     | 内容                                                            |
| -------- | ---------------------------------------------------------------------- | ------------------------------------------------------------- |
| **协议逆向** | [04_Protocol_Reverse.md](Cursor_Knowledge_Base/04_Protocol_Reverse.md) | 散表/__scatterload、23 字节 SPI 常量、PD 相关 GPIO 与 `FUN_0800C4E0` 状态机 |


### 5.3 功能模块分析（Phase 3）


| 文档         | 文件                                                                     | 内容                                       |
| ---------- | ---------------------------------------------------------------------- | ---------------------------------------- |
| **功能模块分析** | [03_Function_Modules.md](Cursor_Knowledge_Base/03_Function_Modules.md) | OLED/UI 状态机；**逆推优先**（见 `04` §2.0、任务书 §4） |


### 5.4 逆向日志


| 文档       | 文件                                                     | 内容         |
| -------- | ------------------------------------------------------ | ---------- |
| **逆向日志** | [reverse_log.md](Cursor_Knowledge_Base/reverse_log.md) | 系统启动与向量表分析 |


### 5.5 完整重构（Phase 5，轻量）


| 文档            | 文件                                                                           | 内容                                                                                                                                      |
| ------------- | ---------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| **模块调用图（初版）** | [05_Full_Reconstruction.md](Cursor_Knowledge_Base/05_Full_Reconstruction.md) | `main` / `PD_PowerAndProtocol_Block` 直接 callee 与 mermaid；**§5.1 MMIO↔业务** + **§5.3 SDK 寄存器字段偏移表**；**冻结 `05-MMIO-v1` @ 2026-04-06**（见文首） |


### 5.6 任务书与关键发现附录


| 文档                      | 文件                                                                                                                             | 内容                                                                  |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------- |
| **逆向任务书**               | [REVERSE_TASK_BOOK.md](Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md)                                                             | 阶段看板、当前进度 Handoff、OI、文件与脚本索引；**§5 仅保留短索引**                          |
| **关键发现记录（附录）**          | [REVERSE_TASK_BOOK_FINDINGS.md](Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md)                                           | 按日「结论 / 证据路径 / 可信度」流水（**新发现优先追加于此**）                                |
| **Phase 6 行为验收清单**      | [phase6_behavior_acceptance_checklist.md](ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_behavior_acceptance_checklist.md) | PH6-BHV-001~005 判定口径、执行证据、待证实项与验证建议                                 |
| **Phase 6 MMIO 映射清单**   | [phase6_mmio_mapping.md](ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_mmio_mapping.md)                                   | `05-MMIO-v1` 到可编译代码符号与 MMIO 写入点映射（含待证实标签）                           |
| **Phase 6 二次开发预备基线**    | [phase6_secondary_dev_baseline.md](ZHIYUN_F100_Firmware/zhiyun_f100_apm32/docs/phase6_secondary_dev_baseline.md)               | 模块职责、构建/验收命令、待证实项与二次开发建议顺序                                          |
| **Phase 6 Renode 采样脚本** | [renode_phase6_capture.sh](scripts/renode_phase6_capture.sh)                                                                   | 直接加载 `zhiyun_f100_apm32` 构建产物，采集 TMR1/PC13/PB3/PB11 等关键寄存器与 SRAM 快照 |
| **Phase 6 PD 断点追踪脚本**   | [renode_phase6_pd_breaktrace.sh](scripts/renode_phase6_pd_breaktrace.sh)                                                       | 对 `PD_GPIO_StateMachine` 下断并记录命中与关键状态，定位 `PH6-BHV-005` 动态阻塞点        |


## 6. 快速查找指南

### 6.1 按功能分类的资源索引

#### GPIO相关

- [GPIO驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_gpio.h)
- [GPIO示例](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/GPIO/)
- [GPIO初始化分析](Cursor_Knowledge_Base/02_Hardware_Init.md)

#### PWM/定时器相关

- [TMR驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_tmr.h)
- [TMR示例](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/TMR/)
- [PWM初始化分析](Cursor_Knowledge_Base/02_Hardware_Init.md)

#### ADC相关

- [ADC驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_adc.h)
- [ADC示例](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/ADC/)
- [ADC初始化分析](Cursor_Knowledge_Base/02_Hardware_Init.md)

#### SPI相关

- [SPI驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_spi.h)
- [SPI示例](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/SPI/)
- [SPI初始化分析](Cursor_Knowledge_Base/02_Hardware_Init.md)

#### I2C相关

- [I2C驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_i2c.h)
- [I2C示例](SDK/APM32F10x_SDK_V2.0.0/Examples/Board_APM32F103_Mini/I2C/)
- [HUSB238规格书](Document/Markdown/husb238规格书.md)

#### USB相关

- [USB驱动库](SDK/APM32F10x_SDK_V2.0.0/Libraries/APM32F10x_StdPeriphDriver/inc/apm32f10x_usb.h)
- [USB设备示例](SDK/APM32_USB_SDK_v1.1/Examples/APM32F1xx/Device_Examples/)

### 6.2 常用文件路径速查


| 用途                       | 文件路径                                                                                                                                                                                     |
| ------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **协议逆向（散表/SPI/PD GPIO）** | [Cursor_Knowledge_Base/04_Protocol_Reverse.md](Cursor_Knowledge_Base/04_Protocol_Reverse.md)                                                                                             |
| **CBT6寄存器定义**            | [SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Include/apm32f103xb.h)                       |
| **CBT6启动代码**             | [SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/arm/startup_apm32f103xb.s](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/arm/startup_apm32f103xb.s) |
| **系统初始化**                | [SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c](SDK/APM32F10x_SDK_V2.0.0/Libraries/Device/Geehy/APM32F10x/Source/system_apm32f10x.c)               |
| **芯片规格书**                | [Document/Markdown/APM32F103 CBT6规格书.md](Document/Markdown/APM32F103%20CBT6规格书.md)                                                                                                       |
| **硬件拓扑分析**               | [Document/Zhiyun_F100_Repair_Notes.md](Document/Zhiyun_F100_Repair_Notes.md)                                                                                                             |
| **IC交互分析**               | [Document/Markdown/IC_MCU_逻辑关系推测.md](Document/Markdown/IC_MCU_逻辑关系推测.md)                                                                                                                 |
| **硬件初始化分析**              | [Cursor_Knowledge_Base/02_Hardware_Init.md](Cursor_Knowledge_Base/02_Hardware_Init.md)                                                                                                   |
| **逆向日志**                 | [Cursor_Knowledge_Base/reverse_log.md](Cursor_Knowledge_Base/reverse_log.md)                                                                                                             |
| **任务书 / 关键发现附录**         | [Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md](Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md) / [REVERSE_TASK_BOOK_FINDINGS.md](Cursor_Knowledge_Base/REVERSE_TASK_BOOK_FINDINGS.md)          |


### 6.3 外设地址速查表


| 外设      | 基地址  | 偏移     | 完整地址       |
| ------- | ---- | ------ | ---------- |
| TMR2    | APB1 | 0x0000 | 0x40000000 |
| TMR3    | APB1 | 0x0400 | 0x40000400 |
| TMR4    | APB1 | 0x0800 | 0x40000800 |
| SPI2    | APB1 | 0x3800 | 0x40003800 |
| USART2  | APB1 | 0x4400 | 0x40004400 |
| USART3  | APB1 | 0x4800 | 0x40004800 |
| I2C1    | APB1 | 0x5400 | 0x40005400 |
| I2C2    | APB1 | 0x5800 | 0x40005800 |
| CAN     | APB1 | 0x6400 | 0x40006400 |
| AFIO    | APB2 | 0x0000 | 0x40010000 |
| EINT    | APB2 | 0x0400 | 0x40010400 |
| GPIOA   | APB2 | 0x0800 | 0x40010800 |
| GPIOB   | APB2 | 0x0C00 | 0x40010C00 |
| GPIOC   | APB2 | 0x1000 | 0x40011000 |
| ADC1    | APB2 | 0x2400 | 0x40012400 |
| ADC2    | APB2 | 0x2800 | 0x40012800 |
| TMR1    | APB2 | 0x2C00 | 0x40012C00 |
| SPI1    | APB2 | 0x3000 | 0x40013000 |
| USART1  | APB2 | 0x3800 | 0x40013800 |
| DMA     | AHB  | 0x0000 | 0x40020000 |
| RCM     | AHB  | 0x1000 | 0x40021000 |
| Flash接口 | AHB  | 0x2000 | 0x40022000 |
| CRC     | AHB  | 0x3000 | 0x40023000 |


---

## 附录：资源状态标识说明


| 标识       | 含义                      |
| -------- | ----------------------- |
| ✅ CBT6专用 | APM32F103CBT6专用资源，必须使用  |
| ✅ CBT6可用 | APM32F103CBT6可用的资源，兼容使用 |
| ❌ 非CBT6  | 不属于CBT6的资源，仅供逆向参考       |


---

> **提示**: 本目录文件旨在帮助后续agent快速定位所需资源。如需更新，请同步修改本文档。