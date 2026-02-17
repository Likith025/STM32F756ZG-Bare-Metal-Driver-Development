# STM32F756ZG-Bare-Metal-Driver-Development
**Overview**

This project focuses on developing peripheral drivers for the STM32F756ZG microcontroller using direct register-level programming, without relying on STM32 HAL or middleware libraries.
The goal is to build a strong foundation in embedded firmware development by understanding hardware at the register and bus level.

**Objectives**

Understand ARM Cortex-M7 architecture fundamentals
Work with STM32F7 memory regions (Flash, DTCM, SRAM1, SRAM2)
Implement accurate register structures based on RM0385
Control peripheral clocks using RCC
Perform safe bit-level register manipulation
Build clean and reusable driver abstractions
Architecture Focus

**This project emphasizes:**
Cortex-M memory model awareness
AHB and APB bus architecture
Peripheral clock gating
Address-based peripheral identification
Proper use of volatile for hardware registers
Correct register offset alignment
All register structures are verified against the STM32F7 reference manual.

**Implemented Peripherals**
This section will be updated as new drivers are developed:
 ✅RCC (Clock control – base implementation)
 GPIO
 EXTI
 USART / UART
 SPI
 I2C
 Timers
 DMA
 NVIC configuration

**Design Principles**
No STM32 HAL or middleware usage
Minimal abstraction overhead
Clean macro-based peripheral mapping
Deterministic register access
Modular and scalable driver structure

**Target Hardware**
MCU: STM32F756ZG
Core: ARM Cortex-M7

**Purpose**
This repository serves as a growing driver foundation aimed at strengthening low-level embedded firmware development skills and building reusable peripheral drivers from scratch.
