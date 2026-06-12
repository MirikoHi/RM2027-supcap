# Supcap

项目: 超级电容能量管理（STM32G4xx）

## 项目简介

本仓库为基于 STM32G474CEU6  MCU 的超级电容管理固件实现，包含电容组管理、功率管理、采样与保护、通信（CAN/FDCAN）、无线充电（WPT）集成等功能模块。

## 主要特性

- 电容组容量估算与保护
- 裁判系统（Referee）功率交互与限制
- 多路 ADC 采样与校准
- 增量 PID 控制环路用于功率/电流管理
- CAN/FDCAN 通信（包含两种反馈消息格式）
- 基于 HRTIM 的 PWM/切换控制

## 硬件要求

- MCU: STM32G474CEU6 
- 调试/烧录器: 支持 SWD 的编程器
- 外围: 电容组、无线充电模块（可选）等

## 开发环境与依赖

- GNU Arm Embedded Toolchain (arm-none-eabi-gcc)
- Make / mingw32-make
- CMSIS 与 STM32 HAL 驱动（仓库内 Drivers/STM32G4xx_HAL_Driver）
- 可选: OpenOCD 或 ST-Link Utility 用于烧写

## 构建

在 Windows 下使用提供的 VSCode 任务或命令行构建：

```bash
# 在仓库根目录
mingw32-make -j24
```

## 烧写/下载（示例）

使用 ST-Link CLI / OpenOCD 等工具烧写生成的固件 `build/<your-target>.elf` 或 `build/<your-target>.bin`。

## 代码结构

- `Core/` - 应用代码
  - `cppInc/` - C++ 头文件（模块接口）
  - `cppSrc/` - C++ 源文件
  - `Inc/` - C 头文件
  - `Src/` - C 源文件
- `Drivers/` - HAL 与设备驱动 （ stm32cubemx 生成）
- `build/` - 构建输出
- `Hardware/` - 硬件相关工程文件

## 关键文件

- `Core/cppInc/` 中包含模块接口：`CapArray.hpp`, `Communication.hpp`, `Config.hpp`, `Modulation.hpp`, `PID.hpp`, `PowerManager.hpp`, `Protection.hpp`, `SampleManager.hpp`, `SystemState.hpp`
- 链接脚本: `STM32G474XX_FLASH.ld`
- 启动汇编: `startup_stm32g474xx.s`

