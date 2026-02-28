# STM32F756ZG Bare-Metal Driver Development

A comprehensive collection of peripheral drivers for the STM32F756ZG microcontroller, developed from the ground up using direct register-level programming. This project builds a strong foundation in embedded firmware development by working at the hardware level without relying on STM32 HAL or middleware libraries.

## 🎯 Project Objectives

- Master ARM Cortex-M7 architecture fundamentals
- Understand STM32F7 memory organization (Flash, DTCM, SRAM1, SRAM2)
- Implement accurate register structures based on RM0385 reference manual
- Master peripheral clock control using RCC
- Perform safe and deterministic bit-level register manipulation
- Build clean, reusable, and modular driver abstractions
- Develop production-quality embedded firmware patterns

## 🏗️ Architecture & Design Philosophy

This project emphasizes:

- **Hardware Awareness**: Deep understanding of Cortex-M memory model
- **Bus Architecture**: Working with AHB and APB bus protocols
- **Clock Management**: Proper peripheral clock gating and RCC configuration
- **Register Safety**: Correct use of `volatile` for hardware registers and proper offset alignment
- **Clean Code**: Minimal abstraction overhead with modular, scalable driver structure
- **Verification**: All register structures verified against STM32F7 reference manual (RM0385)

## 📋 Design Principles

✓ **No HAL/Middleware**: Pure register-level implementation  
✓ **Minimal Abstraction**: Direct hardware control with clean macros  
✓ **Deterministic Behavior**: Predictable register access patterns  
✓ **Modular Structure**: Reusable and scalable driver architecture  
✓ **Production Ready**: Follows embedded systems best practices  

## 🔧 Implemented Peripherals

| Peripheral | Status | Notes |
|-----------|--------|-------|
| RCC | ✅ Complete | Clock control and system configuration |
| GPIO | ✅ Complete | Digital I/O control |
| EXTI | 📋 Planned | External interrupts |
| USART/UART | 🔄 In Progress | Serial communication |
| SPI | 📋 Planned | Serial peripheral interface |
| I2C | 📋 Planned | Inter-integrated circuit |
| Timers | 📋 Planned | General-purpose and advanced timers |
| DMA | 📋 Planned | Direct memory access |
| NVIC | 📋 Planned | Nested vectored interrupt controller |

## 📱 Target Hardware

- **MCU**: STM32F756ZG
- **Core**: ARM Cortex-M7 @ 216 MHz
- **Flash**: 1 MB
- **RAM**: 316 KB (DTCM + SRAM)

## 🚀 Getting Started

```bash
# Clone the repository
git clone https://github.com/Likith025/STM32F756ZG-Bare-Metal-Driver-Development.git
cd STM32F756ZG-Bare-Metal-Driver-Development

# Review the architecture and memory maps
# Start with RCC driver implementation
# Follow the register structure patterns
```


## 📖 Reference Documentation

- **STM32F756ZG Datasheet**: Official device specifications
- **RM0385 Reference Manual**: Complete register definitions and peripheral descriptions
- **ARM Cortex-M7 Technical Reference Manual**: CPU architecture details

## 💡 Learning Path

1. Understand the STM32F7 memory map and address spaces
2. Learn RCC configuration and clock tree management
3. Master GPIO operations and register patterns
4. Implement interrupt handling with NVIC
5. Build advanced peripherals (Timers, UART, SPI, I2C)
6. Explore DMA for efficient data transfers

## 🤝 Contributing

This is a personal learning project, but suggestions and improvements are welcome. Feel free to open issues for bug reports or feature requests.

## 📝 License

This project is provided for educational purposes.

---

**Purpose**: A growing driver foundation aimed at strengthening low-level embedded firmware development skills and building reusable peripheral drivers from scratch.
