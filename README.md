# STM32F103C8T6 Standard Library Learning Notes

This repository contains my personal learning notes and project templates for the **STM32F103C8T6** microcontroller, developed using the **Standard Peripheral Library (SPL)**.

> **️ Important Note:** This is primarily a **personal learning repository**. The code is based on the excellent tutorial by **Jiang Xie Ke Ji (江协科技)**. I have added my own comments and understanding to the code for self-study purposes, but I made it public to share my learning journey.

---

###  Course Reference

This project strictly follows the "STM32 Introductory Tutorial - 2023 Edition" by **Jiang Xie Ke Ji**. If you are looking for the video tutorials, please visit their Bilibili channel.

-   **Course Link:** [Jiang Xie Ke Ji STM32 Tutorial on Bilibili](https://www.bilibili.com/list/ml1454946110?oid=206940155&bvid=BV1th411z7sn)
-   **Focus:** Understanding the underlying registers and standard library functions.

---

### ️ Tech Stack

-   **MCU:** STM32F103C8T6 (Blue Pill)
-   **Library:** STM32 Standard Peripheral Library (SPL)
-   **IDE:** Keil MDK-ARM
-   **Language:** C

###  Repository Structure

-   **`/Template`**: A clean project template with startup files and system configuration.
-   **`/Examples`**:
    -   `GPIO`: LED control, Key input.
    -   `TIM`: PWM generation, Input capture.
    -   `USART`: Serial communication.
    -   `ADC/DMA`: Analog sensor reading.

###  Quick Start

1.  Clone the repository.
2.  Open the `.uvprojx` file in Keil.
3.  Build and Flash using an ST-Link debugger.

---
*Happy Coding & Learning!*
