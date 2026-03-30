# STM32 Standard Peripheral Library Templates

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![STM32](https://img.shields.io/badge/STM32-F103-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f1-series.html)
[![Keil](https://img.shields.io/badge/IDE-Keil%20MDK-orange.svg)](https://www.keil.com/download/)

A collection of **54 functional templates** for STM32F103 microcontrollers based on Standard Peripheral Library V3.5.0.

> **Personal Learning Note:** This repository contains my learning notes and project templates. The code is based on the excellent tutorial by **Jiang Xie Ke Ji (江协科技)**, with my own comments and understanding added for self-study purposes.

---

## Project Structure

```text
gitstm32/
├── 00-Common/           # Common resources (3 templates)
├── 01-Basic/            # Basic peripherals (9 templates)
├── 02-Communication/    # Communication protocols (9 templates)
├── 03-Timer/            # Timers (12 templates)
├── 04-ADC-DMA/          # ADC & DMA (4 templates)
├── 05-Advanced/         # Advanced features (10 templates)
└── 99-Tools/            # Tools & resources (7 templates)
```

---

## Template Index

### 00-Common - Common Resources

| Name | Description |
| --- | --- |
| [1-0 代码结构模板](00-Common/1-0%20代码结构模板/ "1-0 Code Structure") | Code organization reference |
| [1-1 接线图](00-Common/1-1%20接线图/ "1-1 Wiring Diagrams") | Hardware connection diagrams |
| [2-1 STM32 工程模版](00-Common/2-1%20STM32工程模版/ "2-1 Project Template") | Keil MDK base project template |

---

### 01-Basic - Basic Peripherals

| Name | Function |
| --- | --- |
| [3-1 LED 闪烁](01-Basic/3-1%20LED闪烁/ "3-1 LED Blink") | GPIO output |
| [3-2 LED 流水灯](01-Basic/3-2%20LED流水灯/ "3-2 LED Chaser") | Sequential GPIO control |
| [3-3 蜂鸣器](01-Basic/3-3%20蜂鸣器/ "3-3 Buzzer") | Buzzer control |
| [3-4 按键控制 LED](01-Basic/3-4%20按键控制LED/ "3-4 Button Control") | GPIO input detection |
| [3-5 红外线控制蜂鸣器](01-Basic/3-5%20红外线控制蜂鸣器/ "3-5 IR Sensor") | IR sensor + buzzer |
| [4-1 OLED 显示屏](01-Basic/4-1%20OLED显示屏/ "4-1 OLED Display") | SSD1306 display driver |
| [4-2 OLED 显示屏 - 中文显示](01-Basic/4-2%20OLED显示屏%20-%20中文显示/ "4-2-OLED-Chinese") | Chinese character display |
| [4-3 轻量型 OLED UI（待优化）](01-Basic/4-3%20轻量型OLED%20UI（待优化）/ "4-3-OLED-UI") | Lightweight UI framework |
| [5-1 对射式红外传感器计次](01-Basic/5-1%20对射式红外传感器计次/ "5-1 IR Counter") | Object counting application |

---

### 02-Communication - Communication Protocols

| Name | Protocol | Device |
| --- | --- | --- |
| [9-1 串口发送](02-Communication/9-1%20串口发送/ "9-1 UART TX") | UART | - |
| [9-2 串口发送与接收](02-Communication/9-2%20串口发送与接收/ "9-2 UART TX/RX") | UART Full-Duplex | - |
| [9-3 串口收发 Hex 数据包](02-Communication/9-3%20串口收发%20Hex%20数据包/ "9-3 UART Hex") | UART Binary Protocol | - |
| [9-4 串口收发文本数据包](02-Communication/9-4%20串口收发文本数据包/ "9-4 UART Text") | UART Text Protocol | - |
| [10-1 软件 I2C 读写 MPU6050](02-Communication/10-1%20软件%20I2C%20读写%20MPU6050/ "10-1 SW I2C") | I2C (Software) | MPU6050 |
| [10-2 硬件 I2C 读写 MPU6050](02-Communication/10-2%20硬件%20I2C%20读写%20MPU6050/ "10-2 HW I2C") | I2C (Hardware) | MPU6050 |
| [11-1 软件 SPI 读写 W25Q64](02-Communication/11-1%20软件%20SPI%20读写%20W25Q64/ "11-1 SW SPI") | SPI (Software) | W25Q64 |
| [11-2 硬件 SPI 读写 W25Q64](02-Communication/11-2%20硬件%20SPI%20读写%20W25Q64/ "11-2 HW SPI") | SPI (Hardware) | W25Q64 |

---

### 03-Timer - Timers

| Name | Function |
| --- | --- |
| [6-1 定时器中断（内部触发）](03-Timer/6-1%20定时器中断（内部触发）/ "6-1-Timer-IRQ") | Basic timer interrupt |
| [6-1-1 按键单击双击三击长按](03-Timer/6-1-1%20按键单击双击三击长按/ "6-1-1-Button-Detect") | Complex button detection |
| [6-2 定时器中断（外部触发）](03-Timer/6-2%20定时器中断（外部触发）/ "6-2-External-Trigger") | External trigger |
| [6-3 PWM 驱动 LED 灯](03-Timer/6-3%20PWM%20驱动%20LED%20灯/ "6-3-PWM-LED") | PWM dimming |
| [6-4 PWM 驱动舵机](03-Timer/6-4%20PWM%20驱动舵机/ "6-4-Servo") | Servo control |
| [6-5 PWM 驱动直流电机](03-Timer/6-5%20PWM%20驱动直流电机/ "6-5-DC-Motor") | Motor speed control |
| [6-6 输入捕获模式测频率](03-Timer/6-6%20输入捕获模式测频率/ "6-6-Input-Capture") | Frequency measurement |
| [6-7 PWMI 测频率占空比](03-Timer/6-7%20PWMI%20测频率占空比/ "6-7-PWMI") | PWM analysis |
| [6-8 编码器接口测速](03-Timer/6-8%20编码器接口测速/ "6-8-Encoder") | Quadrature decoder |
| [6-9 定时器测量函数执行时间](03-Timer/6-9%20定时器测量函数执行时间/ "6-9-Execution-Time") | Performance profiling |
| [5-2 旋转编码器计次](03-Timer/5-2%20旋转编码器计次/ "5-2-Encoder-Count") | Encoder counting |

---

### 04-ADC-DMA - Data Acquisition

| Name | Function |
| --- | --- |
| [7-1 AD 单通道](04-ADC-DMA/7-1%20AD%20单通道/ "7-1 Single ADC") | Single-channel ADC |
| [7-2 AD 多通道](04-ADC-DMA/7-2%20AD%20多通道/ "7-2 Multi ADC") | Multi-channel scanning |
| [8-1 DMA 数据转运](04-ADC-DMA/8-1%20DMA%20数据转运/ "8-1 DMA Transfer") | DMA transfer |
| [8-2 DMA+AD 多通道](04-ADC-DMA/8-2%20DMA+AD%20多通道/ "8-2 DMA+ADC") | ADC + DMA combined |

---

### 05-Advanced - Advanced Features

| Name | Function |
| --- | --- |
| [12-1 读写备份寄存器](05-Advanced/12-1%20读写备份寄存器/ "12-1 BKP Registers") | Backup registers |
| [12-2 实时时钟](05-Advanced/12-2%20实时时钟/ "12-2 RTC") | RTC real-time clock |
| [13-1 修改主频](05-Advanced/13-1%20修改主频/ "13-1 System Clock") | System clock configuration |
| [13-2 睡眠模式 + 串口发送 + 接收](05-Advanced/13-2%20睡眠模式%20+%20串口发送%20+%20接收/ "13-2-Sleep-Mode") | Sleep low-power mode |
| [13-3 停止模式 + 对射式红外传感器计次](05-Advanced/13-3%20停止模式%20+%20对射式红外传感器计次/ "13-3-Stop-Mode") | Stop low-power mode |
| [13-4 待机模式 + 实时时钟](05-Advanced/13-4%20待机模式%20+%20实时时钟/ "13-4-Standby-Mode") | Standby low-power mode |
| [14-1 独立看门狗](05-Advanced/14-1%20独立看门狗/ "14-1 IWDG") | Independent watchdog (IWDG) |
| [14-2 窗口看门狗](05-Advanced/14-2%20窗口看门狗/ "14-2 WWDG") | Window watchdog (WWDG) |
| [15-1 读取内部 FLASH](05-Advanced/15-1%20读取内部%20FLASH/ "15-1 Internal FLASH") | Internal FLASH programming |
| [15-2 读取芯片 ID](05-Advanced/15-2%20读取芯片%20ID/ "15-2 Chip ID") | Unique device ID |

---

### 99-Tools - Tools & Resources

| Name | Purpose |
| --- | --- |
| [9-6 FlyMcu 串口下载&Stlink Utility](99-Tools/9-6%20FlyMcu%20串口下载&Stlink%20Utility/ "9-6 Download Tools") | Program download tools |
| [STM32 疑难杂症](99-Tools/STM32%20疑难杂症/ "Troubleshooting") | Troubleshooting guides |
| [VOFA 测试](99-Tools/VOFA%20测试/ "VOFA+ Debug") | VOFA+ serial debugger |
| [X-1 51 程序移植（DS18B20 温度读取）](99-Tools/X-1%2051程序移植（DS18B20温度读取）/ "51 Migration") | 8051 to STM32 migration |
| [模块学习](99-Tools/模块学习/ "Module Tutorials") | Module tutorials (DHT11, stepper motor) |
| [练习](99-Tools/练习/ "Exercises") | Practice projects (stopwatch, digital clock) |

---

## Quick Start

### Prerequisites

- **IDE**: Keil MDK-ARM v5.x
- **MCU**: STM32F103C8T6
- **Firmware Library**: STM32 Standard Peripheral Library V3.5.0
- **Debugger**: ST-Link V2 / J-Link / DAP-Link

### Usage Steps

1. **Open Project**: Navigate to template folder, double-click `project.uvprojx`
2. **Build**: Press `F7` - ensure 0 Error, 0 Warning
3. **Connect Hardware**: Connect development board via debugger, USB power
4. **Flash**: Press `F8` or `Ctrl+F8` to download program
5. **Run**: Reset board or press `Ctrl+F5`

---

## Git Workflow

### .gitignore Configuration

The following are gitignored:

- Keil build artifacts (`.o`, `.axf`, `.hex`, `Objects/`, `Listings/`)
- User config files (`.uvoptx`, `.uvguix.*`)
- Debug logs and temporary files

### Recommended Workflow

```bash
git clone https://github.com/Eureka12138github/gitstm32.git
cd gitstm32
git status
git add .
git commit -m "Your changes"
git push
```

---

---

## Resources

- [STM32F103 Reference Manual RM0008](https://www.st.com/resource/en/reference_manual/rm0008.pdf)
- [Standard Peripheral Library User Manual](https://www.st.com/resource/en/user_manual/um0736.pdf)
- [Jiang Xie Ke Ji STM32 Tutorial](https://www.bilibili.com/list/ml1454946110?oid=206940155&bvid=BV1th411z7sn)

---

**Happy Coding & Learning!** 🚀
