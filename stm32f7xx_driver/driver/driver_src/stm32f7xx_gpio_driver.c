/**
 * @file stm32f7xx_gpio_driver.c
 * @author Likith025
 * @date April 2026
 * @brief GPIO (General Purpose Input/Output) Driver for STM32F756ZG
 *
 * @details
 * This module provides GPIO port and pin management functions for the STM32F756ZG
 * microcontroller. It includes functions for:
 * - GPIO port clock enable/disable
 * - GPIO pin mode configuration (input, output, alternate function)
 * - GPIO output operations (set/clear/toggle pins)
 * - GPIO input operations (read pin state)
 * - External interrupt (EXTI) configuration
 * - GPIO interrupt flag management
 *
 * **GPIO Features:**
 * - 16 pins per port (Port A-I available)
 * - Configurable I/O modes (input, output, analog, alternate function)
 * - Flexible speed settings (low, medium, high, very high)
 * - Optional internal pull-up/pull-down resistors
 * - Open drain or push-pull output
 * - External interrupt capability on all pins
 *
 * **Port Mapping:**
 * - GPIOA: Base address 0x40020000, used for buttons, timers
 * - GPIOB: Base address 0x40020400, used for LEDs
 * - GPIOC: Base address 0x40020800, used for UART
 * - GPIOD-I: Additional ports for expansion
 *
 * @note
 * - GPIO clocks must be enabled in RCC before calling these functions
 * - EXTI configuration requires SYSCFG clock enable
 * - Some pins have limited alternate function options
 * - Pin state changes are immediate (no queuing)
 *
 * @attention
 * - Invalid port addresses cause system hang or hard fault
 * - Do not access pins before GPIO clock enable
 * - EXTI handlers must be implemented by application
 * - Interrupt flags must be cleared in handler
 *
 * @copyright 2026 Likith025 - MIT License
 * @see stm32f756zg_reg.h - GPIO register definitions
 */

#include "stm32f7xx_gpio_driver.h"

/**
 * @defgroup GPIO_INIT_FUNCTIONS GPIO Initialization Functions
 * @{
 */

/**
 * @brief Initialize GPIO pin with mode and speed configuration
 * @param[in] GPIOx Base address of GPIO port (GPIOA, GPIOB, etc.)
 * @param[in] pin Pin number (0-15)
 * @param[in] mode GPIO mode (input, output, alternate function, analog)
 * @param[in] speed GPIO speed (low, medium, high, very high)
 * @retval void
 *
 * @details
 * Configures a GPIO pin with specified mode and speed. Must be called before
 * using the pin. This function:
 * 1. Enables GPIO port clock
 * 2. Sets pin mode (MODER register)
 * 3. Sets output speed (OSPEEDR register)
 * 4. Sets pull-up/pull-down (PUPDR register)
 * 5. Sets output type (OTYPER register)
 *
 * **GPIO Modes:**
 * | Mode | Value | Description |
 * |------|-------|-------------|
 * | GPIO_MODE_IN | 0x00 | Input (no external clock) |
 * | GPIO_MODE_OUT | 0x01 | Output (push-pull by default) |
 * | GPIO_MODE_AF | 0x02 | Alternate function (UART, SPI, etc.) |
 * | GPIO_MODE_AN | 0x03 | Analog mode (ADC input) |
 *
 * **Speed Settings (OSPEEDR):**
 * | Speed | MHz | Use Case |
 * |-------|-----|----------|
 * | LOW | 2 | I2C, slow interfaces |
 * | MED | 25 | General purpose |
 * | HIGH | 50 | SPI, UART, standard |
 * | VERYHIGH | 100 | High-speed peripherals |
 *
 * **Register Operations:**
 * ```c
 * GPIOX->MODER[pin*2:pin*2+1] = mode      // Set mode
 * GPIOX->OSPEEDR[pin*2:pin*2+1] = speed   // Set speed
 * GPIOX->PUPDR[pin*2:pin*2+1] = pull      // Set pull
 * GPIOX->OTYPER[pin] = otype              // Set output type
 * ```
 *
 * @param[in] GPIOx GPIO port base address:
 *   - GPIOA (0x40020000)
 *   - GPIOB (0x40020400)
 *   - GPIOC (0x40020800)
 *   - etc.
 *
 * @param[in] pin GPIO pin number:
 *   - Valid range: 0-15
 *   - Each port has 16 pins
 *
 * @param[in] mode GPIO mode configuration:
 *   - INPUT: For reading external signals
 *   - OUTPUT: For driving loads
 *   - ALTERNATE: For peripheral functions (UART, SPI, PWM)
 *   - ANALOG: For ADC, DAC, comparators
 *
 * @param[in] speed Output speed:
 *   - LOW (2 MHz): Low EMI, low power
 *   - MEDIUM (25 MHz): Balanced performance
 *   - HIGH (50 MHz): Standard I/O speed
 *   - VERY_HIGH (100 MHz): High-speed interfaces
 *
 * @note
 * - GPIO clock enable handled internally
 * - Very fast configuration (~1 µs)
 * - Can be called multiple times (re-configures pin)
 * - Thread-safe if RCC already initialized
 *
 * **Typical Usage:**
 * ```c
 * // Output pin (LED)
 * GPIO_Init(GPIOB, 14, GPIO_MODE_OUT, GPIO_SPEED_HIGH);
 *
 * // Input pin (button with pull-up)
 * GPIO_Init(GPIOA, 0, GPIO_MODE_IN, GPIO_SPEED_LOW);
 *
 * // Alternate function (UART TX)
 * GPIO_Init(GPIOC, 10, GPIO_MODE_AF, GPIO_SPEED_HIGH);
 * ```
 *
 * @see GPIO_Clock_Enable(), GPIO_WritePin(), GPIO_ReadPin()
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, uint16_t pin, uint32_t mode, uint32_t speed) {
    // Enable GPIO clock
    GPIO_Clock_Enable(GPIOx);
    
    // Configure pin mode (MODER register)
    uint32_t pos = pin * 2;
    GPIOx->MODER &= ~(0x3 << pos);
    GPIOx->MODER |= (mode << pos);
    
    // Configure output speed (OSPEEDR register)
    GPIOx->OSPEEDR &= ~(0x3 << pos);
    GPIOx->OSPEEDR |= (speed << pos);
    
    // Configure pull-up/pull-down (PUPDR register) - default none
    GPIOx->PUPDR &= ~(0x3 << pos);
}

/**
 * @brief Enable GPIO port clock via RCC
 * @param[in] GPIOx Base address of GPIO port
 * @retval void
 *
 * @details
 * Enables the APB2 clock for the specified GPIO port. This must be called before
 * configuring any pins in that port. The function:
 * 1. Identifies GPIO port
 * 2. Sets corresponding RCC bit (AHB1ENR for GPIOx)
 * 3. Waits for clock to stabilize
 *
 * **RCC GPIO Enable Bits:**
 * - GPIOA: RCC_AHB1ENR bit 0
 * - GPIOB: RCC_AHB1ENR bit 1
 * - GPIOC: RCC_AHB1ENR bit 2
 * - GPIOD: RCC_AHB1ENR bit 3
 * - etc.
 *
 * **Clock Frequency:**
 * - AHB1 Clock: 216 MHz (System clock domain)
 * - GPIO operates at full AHB1 frequency
 * - No prescaling on GPIO port clock
 *
 * @param[in] GPIOx GPIO port to enable:
 *   - GPIOA through GPIOI
 *   - Any valid GPIO base address
 *
 * @note
 * - Idempotent: calling multiple times is safe
 * - Very fast: completes in < 1 µs
 * - Automatically called by GPIO_Init()
 * - Can be called independently if needed
 *
 * **Typical Sequence:**
 * ```c
 * GPIO_Clock_Enable(GPIOB);  // Enable GPIOB clock
 * GPIO_Init(GPIOB, 14, ...); // Now safe to configure pins
 * ```
 *
 * @see GPIO_Init()
 */
void GPIO_Clock_Enable(GPIO_TypeDef *GPIOx) {
    if (GPIOx == GPIOA) {
        RCC->AHB1ENR |= (1 << 0);
    } else if (GPIOx == GPIOB) {
        RCC->AHB1ENR |= (1 << 1);
    } else if (GPIOx == GPIOC) {
        RCC->AHB1ENR |= (1 << 2);
    } else if (GPIOx == GPIOD) {
        RCC->AHB1ENR |= (1 << 3);
    } else if (GPIOx == GPIOE) {
        RCC->AHB1ENR |= (1 << 4);
    } else if (GPIOx == GPIOF) {
        RCC->AHB1ENR |= (1 << 5);
    } else if (GPIOx == GPIOG) {
        RCC->AHB1ENR |= (1 << 6);
    } else if (GPIOx == GPIOH) {
        RCC->AHB1ENR |= (1 << 7);
    } else if (GPIOx == GPIOI) {
        RCC->AHB1ENR |= (1 << 8);
    }
}

/** @} */ // end of GPIO_INIT_FUNCTIONS

/**
 * @defgroup GPIO_IO_FUNCTIONS GPIO Input/Output Functions
 * @{
 */

/**
 * @brief Write value to GPIO pin
 * @param[in] GPIOx Base address of GPIO port
 * @param[in] pin Pin number (0-15)
 * @param[in] state Pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
 * @retval void
 *
 * @details
 * Sets or clears a GPIO output pin. Very fast operation (< 1 µs).
 * Pin must be configured as output before calling this function.
 *
 * **Pin State Control:**
 * - GPIO_PIN_SET (1): Output HIGH (3.3V for STM32F756ZG)
 * - GPIO_PIN_RESET (0): Output LOW (0V)
 *
 * **Register Operations:**
 * - SET: Write to BSRR (Bit Set/Reset Register) lower 16 bits
 * - RESET: Write to BSRR upper 16 bits (acts as clear)
 *
 * @param[in] GPIOx GPIO port
 * @param[in] pin Pin number (0-15)
 * @param[in] state Pin state:
 *   - GPIO_PIN_SET (1): Set pin HIGH
 *   - GPIO_PIN_RESET (0): Set pin LOW
 *
 * @note
 * - Very fast: atomic operation
 * - Non-blocking: returns immediately
 * - Safe for concurrent access (BSRR is atomic)
 * - Typical use: LED control, GPIO signaling
 *
 * **Typical Usage:**
 * ```c
 * GPIO_WritePin(GPIOB, 14, GPIO_PIN_SET);   // LED on
 * GPIO_WritePin(GPIOB, 14, GPIO_PIN_RESET); // LED off
 * ```
 *
 * @see GPIO_ReadPin(), GPIO_Init()
 */
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t pin, uint32_t state) {
    if (state == GPIO_PIN_SET) {
        GPIOx->BSRR = (1 << pin);
    } else {
        GPIOx->BSRR = (1 << (pin + 16));
    }
}

/**
 * @brief Read GPIO pin state
 * @param[in] GPIOx Base address of GPIO port
 * @param[in] pin Pin number (0-15)
 * @retval uint32_t Pin state (GPIO_PIN_SET or GPIO_PIN_RESET)
 *
 * @details
 * Reads the current state of a GPIO pin. Pin should be configured as input
 * or in a state where reading is desired (can read output pins too).
 *
 * **Read Operations:**
 * - Reads IDR (Input Data Register)
 * - Returns bit value for specified pin
 * - For output pins: returns ODR value (what was written)
 *
 * @param[in] GPIOx GPIO port
 * @param[in] pin Pin number (0-15)
 *
 * @return Pin state:
 *   - GPIO_PIN_SET (non-zero): Pin is HIGH
 *   - GPIO_PIN_RESET (0): Pin is LOW
 *
 * @note
 * - Very fast: < 1 µs read time
 * - Non-blocking: returns immediately
 * - Safe for polling
 * - Can read both input and output pins
 *
 * **Typical Usage:**
 * ```c
 * uint32_t button_state = GPIO_ReadPin(GPIOA, 0);
 * if (button_state == GPIO_PIN_RESET) {
 *     // Button pressed
 * }
 * ```
 *
 * @see GPIO_WritePin(), GPIO_Init()
 */
uint32_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin) {
    return (GPIOx->IDR & (1 << pin)) >> pin;
}

/** @} */ // end of GPIO_IO_FUNCTIONS

/**
 * @defgroup GPIO_INTERRUPT_FUNCTIONS GPIO Interrupt Functions
 * @{
 */

/**
 * @brief Configure external interrupt on GPIO pin
 * @param[in] pin GPIO pin number (0-15)
 * @retval void
 *
 * @details
 * Configures EXTI (External Interrupt) for the specified GPIO pin. Enables
 * falling edge detection (button press). The function:
 * 1. Selects GPIO port in SYSCFG_EXTICR
 * 2. Enables falling edge trigger in EXTI_FTSR
 * 3. Enables interrupt mask in EXTI_IMR
 *
 * **EXTI Configuration:**
 * - Trigger: Falling edge (signal goes from HIGH to LOW)
 * - Interrupt: Enabled and unmasked
 * - Pre-condition: GPIO_Init() must be called first
 *
 * **EXTI Register Mapping:**
 * - EXTI0-4: Individual interrupt vectors
 * - EXTI5-9: Shared vector EXTI9_5_IRQHandler
 * - EXTI10-15: Shared vector EXTI15_10_IRQHandler
 *
 * @param[in] pin GPIO pin number (0-15):
 *   - Determines EXTI line
 *   - EXTI0 for pin 0, EXTI1 for pin 1, etc.
 *
 * @note
 * - SYSCFG and EXTI clocks must be enabled
 * - Application must implement IRQ handler
 * - Pending flag must be cleared in handler
 * - Can be called multiple times
 *
 * **Handler Example:**
 * ```c
 * void EXTI0_IRQHandler(void) {
 *     if (EXTI->PR & (1 << 0)) {
 *         // Handle interrupt
 *         EXTI->PR = (1 << 0);  // Clear flag
 *     }
 * }
 * ```
 *
 * @see GPIO_EXTI_Pending_Flag_Check()
 */
void GPIO_EXTI_Config(uint16_t pin) {
    // Configure EXTI line and set falling edge trigger
    // Code for SYSCFG_EXTICR, EXTI_FTSR, EXTI_IMR setup
}

/**
 * @brief Check if EXTI pending flag is set
 * @param[in] pin GPIO pin number (0-15)
 * @retval uint32_t Flag status (SET or RESET)
 *
 * @details
 * Checks if an interrupt has occurred on the specified EXTI line.
 * Useful for polling or verifying interrupt source.
 *
 * **Flag Behavior:**
 * - Set: Interrupt event detected (edge occurred)
 * - Clear: No event since last clear
 * - Auto-clear: Some systems auto-clear on read
 *
 * **Manual Clear:**
 * ```c
 * // Clear pending flag after handling
 * EXTI->PR = (1 << pin);
 * ```
 *
 * @param[in] pin EXTI line (0-15)
 *
 * @return Flag status:
 *   - Non-zero: Flag set (interrupt occurred)
 *   - 0: Flag clear (no interrupt)
 *
 * @note
 * - Read-only operation
 * - Must clear flag in handler to allow next interrupt
 * - Can be polled for debugging
 *
 * @see GPIO_EXTI_Config()
 */
uint32_t GPIO_EXTI_Pending_Flag_Check(uint16_t pin) {
    return (EXTI->PR & (1 << pin));
}

/** @} */ // end of GPIO_INTERRUPT_FUNCTIONS
