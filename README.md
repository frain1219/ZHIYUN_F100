# ZHIYUN F100（智云F100） 固件逆向工程与重构开发

## 项目概述 (Project Overview)

本项目是关于智云（ZHIYUN）五倍 F100 棒灯产品的逆向工程与固件开发项目。一般称为智云 F100。同时也可以用作智云 FR100C、写趣味 F100 等棒灯产品的逆向参考。

This project is a reverse engineering and firmware development project for the ZHIYUN F100 (五倍 F100) LED light stick product. It is generally referred to as ZHIYUN F100. It can also be used as a reference for reverse engineering other LED light stick products such as ZHIYUN FR100C and Xie Qu Wei F100.

主要包含硬件分析、协议逆向、固件开发和仿真测试等内容。项目涉及 APM32F103 和 STM32F407 微控制器的应用。

It mainly includes hardware analysis, protocol reverse engineering, firmware development, and simulation testing. The project involves the application of APM32F103 and STM32F407 microcontrollers.

## 目录结构

```
ZHIYUN_F100/
├── Cursor_Knowledge_Base/   # 知识基础文档
├── Document/                # 文档资料
│   ├── Markdown/            # Markdown 格式文档
│   ├── OledDisplayMode/     # OLED 显示模式图片
│   └── PDF/                 # PDF 格式文档
├── Plug/                    # 插件工具
├── Renode/                  # Renode 仿真文件
├── SDK/                     # 开发SDK
│   ├── APM32F10x_SDK_V2.0.0/ # APM32F10x SDK
│   ├── APM32_USB_SDK_v1.1/  # APM32 USB SDK
│   └── HUSB238-main/        # HUSB238 相关代码
├── ZHIYUN_F100_Firmware/    # 固件代码
│   ├── stm32f407vgt6_probe/ # STM32F407 探针代码
│   └── zhiyun_f100_apm32/   # APM32 固件代码
├── ZY_F100.rep/             # 项目文件
├── dump/                    # 固件转储文件
├── .gitignore               # Git 忽略文件
├── PROJECT_INDEX.md         # 项目索引
├── ZY_F100.gpr              # 项目文件
└── convert_docx_to_md.py    # 文档转换脚本
```

## 主要功能模块

### 1. 硬件分析
- APM32F103 CBT6 微控制器分析
- 电源管理模块（MP2491CGQB-Z、MP3398E等）
- USB-C 接口控制（HUSB238）
- OLED 显示控制

### 2. 协议逆向
- 设备通信协议分析
- 固件结构分析
- 功能模块协议解析

### 3. 固件开发
- STM32F407 探针固件
- APM32F103 固件重写
- USB 设备功能实现

### 4. 仿真测试
- Renode 仿真环境搭建
- 固件功能测试
- 硬件行为模拟

## 开发环境

- **IDE**: PlatformIO (用于固件开发)
- **仿真工具**: Renode
- **逆向工具**: Ghidra (带SVD-Loader插件)
- **文档工具**: Markdown, PDF

## 快速开始

### 1. 环境搭建

1. 安装 PlatformIO
2. 安装 Renode 仿真环境
3. 安装 Ghidra 及 SVD-Loader 插件

### 2. 固件编译

```bash
# 编译 STM32F407 探针固件
cd ZHIYUN_F100_Firmware/stm32f407vgt6_probe
platformio run

# 编译 APM32 固件
cd ../zhiyun_f100_apm32
platformio run
```

### 3. 仿真测试

```bash
# 启动 Renode 仿真
cd Renode
renode apm32_f100.resc
```

## 文档资源

- **规格书**: `Document/PDF/` 目录下包含各种芯片和组件的规格书
- **硬件分析**: `Document/APM32F103CB_to_STM32F407V_Wiring.md`
- **维修笔记**: `Document/Zhiyun_F100_Repair_Notes.md`
- **逆向任务书**: `Cursor_Knowledge_Base/REVERSE_TASK_BOOK.md`

## 贡献指南

1. Fork 本项目
2. 创建功能分支
3. 提交更改
4. 发起 Pull Request

## 许可证

本项目采用 MIT 许可证。详见 LICENSE 文件。


---

**注意**: 本项目仅供学习和维修建议使用，请勿用于商业用途。

**Note**: This project is for learning and repair reference only, not for commercial use.