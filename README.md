# STM32F756ZG Bare-Metal Driver Development

This repository contains bare-metal peripheral drivers for the STM32F756ZG,
implemented using direct register-level programming without STM32 HAL or middleware.

The focus of this project is to understand how peripherals work at the hardware
level by using the reference manual and building drivers from scratch.

---

## Implemented Drivers

| Peripheral | Status   | Description                      |
|------------|----------|----------------------------------|
| RCC        | Complete | Peripheral clock configuration   |
| GPIO       | Complete | Digital input/output             |
| EXTI       | Complete | External interrupt handling      |
| USART      | Complete | Serial communication             |
| NVIC       | Complete | Interrupt controller             |
| Timers     | Complete | General-purpose + PWM            |
| SPI        | Planned  | Serial peripheral interface      |
| I2C        | Planned  | Inter-integrated communication   |
| DMA        | Planned  | Data transfer engine             |

---

## Project Structure

```
.
├── driver/
│   ├── driver_src/    # Peripheral driver implementations
│   └── driver_inc/    # Header files and register definitions
├── bsp/               # Board support / hardware-specific setup
├── Src/               # Application code
└── Startup/           # Startup code and vector table
```
---

## Key Features

- Direct register-level programming (no HAL)
- Register structures implemented based on RM0385
- Proper use of `volatile` for hardware registers
- Modular driver design for reuse across projects

---

## Target Hardware

- MCU: STM32F756ZG
- Core: ARM Cortex-M7
- Board: STM32 Nucleo-144
- Max Clock: 216 MHz

---

## Build & Run

```bash
git clone https://github.com/Likith025/STM32F756ZG-Bare-Metal-Driver-Development.git
cd STM32F756ZG-Bare-Metal-Driver-Development
```

Build using STM32CubeIDE or GCC ARM toolchain.
Flash to STM32 Nucleo board and test using example applications.

---

## Examples

| Peripheral | Test                                              |
|------------|---------------------------------------------------|
| USART      | Data transmission at 115200 baud (polling + IT)   |
| GPIO       | LED toggle verified on board                      |
| EXTI       | Button interrupt triggers handler                 |
| Timer/PWM  | Duty cycle control verified using timer registers |

---

## References

- [RM0385 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0385-stm32f75xxx-and-stm32f74xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32F756ZG Datasheet](https://www.st.com/en/microcontrollers-microprocessors/stm32f756zg.html)
- ARM Cortex-M7 Technical Reference Manual

## Documentation

Generate locally using Doxygen:
```bash
doxygen Doxyfile
```
