# STM32F756ZG Bare Metal Drivers

Comprehensive bare-metal driver library for STM32F7 series microcontrollers, developed using direct register-level programming without relying on vendor HAL libraries.

---

## Overview

This project implements a modular and scalable embedded firmware driver layer for STM32F756ZG, focusing on:

- Direct hardware control via memory-mapped registers
- Minimal abstraction overhead
- Clean and reusable driver APIs
- Separation between driver and application layers

---

## Architecture

The firmware is structured into layered components:

### Driver Layer
Low-level peripheral drivers responsible for direct register manipulation.

- GPIO Driver
- Timer Driver
- UART Driver
- NVIC (Interrupt Controller)

### BSP (Board Support Package)
Handles board-specific configurations.

- Peripheral configuration
- Hardware initialization support

---

## Project Structure

    stm32f7xx_driver/
    │
    ├── driver/
    │   ├── driver_inc/     # Header files (APIs, macros, register definitions)
    │   └── driver_src/     # Driver implementations
    │
    ├── bsp/                # Board-level configuration
    │
    ├── Src/                # Example / test application code
    │
    └── Startup/            # Startup and linker files

---

## Features

- Bare-metal driver implementation (no HAL/LL)
- Modular and reusable driver design
- Peripheral configuration through structured APIs
- Interrupt handling via NVIC
- Timer-based operations including PWM generation

---

## Supported Peripherals

- GPIO  → Digital input/output control  
- TIMER → Time base generation  
- PWM   → Duty cycle control using timers  
- UART  → Serial communication  
- NVIC  → Interrupt management  

---

## Usage

### GPIO Initialization

    GPIO_Handle_t gpio;

    gpio.pGPIOx = GPIOA;
    gpio.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_5;
    gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUTPUT;

    GPIO_Init(&gpio);

---

### PWM Configuration

    TimerPWM_Init(&timer_handle, CHANNEL_1);
    TimerPWM_SetDutyCycle(&timer_handle, CHANNEL_1, 50); // 50% duty cycle

---

## Design Philosophy

- Transparency → All register-level operations are explicit  
- Modularity → Drivers are independent and reusable  
- Efficiency → No unnecessary abstraction layers  
- Scalability → Easy to extend with new peripherals  

---

## Limitations

- No RTOS integration  
- No DMA support  
- Basic clock configuration  
- Minimal error handling  

---

## Future Improvements

- Add support for SPI, I2C, ADC  
- Integrate DMA  
- Improve validation and error handling  
- Add low-power mode support  
- Introduce unit testing  

---

## Target Hardware

- STM32F756ZG Nucleo Board  
- ARM Cortex-M7 architecture  

---

## Intended Audience

- Embedded firmware engineers  
- Students learning bare-metal programming  
- Developers preparing for firmware interviews  

---

## Key Learning Outcomes

- Understanding memory-mapped I/O  
- Writing peripheral drivers from scratch  
- Interrupt handling using NVIC  
- Timer and PWM configuration  
- UART communication  

---

## Documentation

This documentation is generated using Doxygen and includes:

- Driver APIs  
- Function-level descriptions  
- Source code references  

---

## Author

Likith R  
Embedded Firmware Developer