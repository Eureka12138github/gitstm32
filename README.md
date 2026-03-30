# STM32 Standard Peripheral Library Templates

> A collection of 50+ STM32F103 project templates based on Standard Peripheral Library (SPL) V3.5.0

## 📦 Project Structure

```text
STM32-StdPeriph-Templates/
├── 00-Common/           # Common resources (templates, wiring diagrams)
├── 01-Basic/            # Basic peripherals (GPIO, OLED, sensors)
├── 02-Communication/    # Communication protocols (UART, I2C, SPI)
├── 03-Timer/            # Timers (Interrupts, PWM, Encoders)
├── 04-ADC-DMA/          # Data acquisition (ADC, DMA)
├── 05-Advanced/         # Advanced features (RTC, Low-power, Watchdog)
└── 99-Tools/            # Tools and resources
```

## 🚀 Quick Start

### Prerequisites
- **IDE**: Keil MDK-ARM v5.x
- **Device**: STM32F103C8T6/ZET6/VCT6
- **Debugger**: DAP-Link / ST-Link V2 / J-Link

### Get Started

```bash
# Clone repository
git clone https://github.com/Eureka12138github/gitstm32.git
# Open project
cd gitstm32
# Open .uvprojx file in Keil MDK
```

## 📋 Template Categories

### Basic Peripherals
LED Blink, LED Chaser, Buzzer, Button Control, IR Sensor Counter, OLED Display (English/Chinese/UI Framework)

### Communication Protocols
UART (TX/RX, Hex/Text Packets), I2C (Software/Hardware - MPU6050), SPI (Software/Hardware - W25Q64 FLASH)

### Timers & PWM
Basic Timer Interrupts, PWM (LED Dimming, Servo, Motor), Input Capture (Frequency Measurement), Encoder Interface, Complex Button Detection (Single/Double/Triple Click, Long Press)

### ADC & DMA
Single/Multi-channel ADC, DMA Transfer, ADC+DMA Combined Acquisition

### Advanced Features
Backup Registers, RTC Real-Time Clock, System Clock Configuration, Low-Power Modes (Sleep/Stop/Standby), Independent/Window Watchdog, Internal FLASH, Chip ID

### Tools & Resources
Download Tools (FlyMcu/ST-Link), Troubleshooting Guides, VOFA+ Debugging, DS18B20 Temperature Module

## 💡 Usage Notes

- Each template is a standalone Keil project - open `.uvprojx` to get started
- Refer to `00-Common/1-1 Wiring Diagrams/` for hardware connections
- All build artifacts (`Objects/`, `Listings/`, `*.uvoptx`) are gitignored

## 📚 Reference

- **Primary Course**: [Jiang Xie Ke Ji STM32 Tutorial](https://www.bilibili.com/list/ml1454946110)
- **Library**: STM32 Standard Peripheral Library V3.5.0

## 📄 License

MIT License

---

**Happy Coding with STM32!** 🚀
