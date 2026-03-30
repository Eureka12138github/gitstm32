# STM32F103C8T6 Standard Library Templates

This repository contains my learning notes and project templates for the **STM32F103C8T6** microcontroller, developed using the **Standard Peripheral Library (SPL)**.

Unlike the HAL library, these projects focus on register-level understanding and bare-metal programming.

## 🛠️ Tech Stack
- **MCU**: STM32F103C8T6 (ARM Cortex-M3)
- **Library**: STM32 Standard Peripheral Library (SPL)
- **IDE**: Keil MDK-ARM 
- **Language**: C (C99)

## 📂 Project Structure
- **`/Template`**: A clean project template with startup files and system configuration ready to use.
- **`/Examples`**:
    - `GPIO`: LED blinking, Key input.
    - `TIM`: PWM generation, Input capture.
    - `USART`: Serial communication with printf retargeting.
    - `ADC/DMA`: Analog sensor reading.

## 🚀 Quick Start
1. Clone the repo.
2. Open the `.uvprojx` file in Keil.
3. Build and Flash using an ST-Link debugger.

---
> **Note**: This repository is for educational purposes. I am migrating to HAL/LL libraries for newer projects, but keeping this for legacy understanding.
