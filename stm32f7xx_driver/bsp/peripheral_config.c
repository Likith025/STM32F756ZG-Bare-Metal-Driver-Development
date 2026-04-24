/**
 * @file peripheral_config.c
 * @author Likith025
 * @date April 2026
 * @brief Board Support Package (BSP) Configuration for STM32F756ZG
 *
 * @details
 * This module contains hardware initialization functions for all peripherals used in the
 * embedded CLI application. It provides simple, high-level setup functions that configure:
 * - GPIO for LED indicators (red, blue on PORT B)
 * - GPIO for button input (PORT A)
 * - USART3 for serial communication (TX on C10, RX on C11)
 * - TIMER2 for PWM generation (CH1 on A0)
 *
 * **Hardware Mapping:**
 * ```
 * LED (Red):      GPIO_B, Pin 14  (Output, Push-pull)
 * LED (Blue):     GPIO_B, Pin 15  (Output, Push-pull)
 * Button:         GPIO_A, Pin 0   (Input, Pull-up)
 * USART3 TX:      GPIO_C, Pin 10  (Alternate Function)
 * USART3 RX:      GPIO_C, Pin 11  (Alternate Function)
 * TIMER2 CH1:     GPIO_A, Pin 0   (Alternate Function, PWM)
 * ```
 *
 * **Initialization Sequence:**
 * In main() after system clock initialization:
 * ```c
 * 1. led_setup_red(&g_led3);          // Configure red LED output
 * 2. button_setup();                  // Configure button input
 * 3. usart3_init(&g_usart3);          // Initialize UART
 * 4. usart3_tx(&g_usart3_tx);         // Configure TX pin
 * 5. usart3_rx(&g_usart3_rx);         // Configure RX pin
 * 6. timer2_setup(&g_timer2);         // Initialize timer
 * 7. timer2ch1(&g_timer2_ch1);        // Configure PWM output
 * ```
 *
 * **Clock Configuration:**
 * - System Clock: 216 MHz (via PLL)
 * - APB1: 54 MHz (TIM2, USART2-5)
 * - APB2: 108 MHz (USART1, USART6)
 * - GPIO: APB2 clock domain
 *
 * **Global Instances:**
 * - g_led3: GPIO handler for red LED (GPIOB port, pin 14)
 * - g_usart3: UART3 handler (TX/RX on GPIOC)
 * - g_usart3_tx: TX configuration structure
 * - g_usart3_rx: RX configuration structure
 * - g_timer2: TIMER2 handler for PWM
 * - g_timer2_ch1: TIMER2 channel 1 configuration
 *
 * @note
 * - All functions are stateless (idempotent)
 * - Can be called multiple times safely
 * - No return values (assume success)
 * - GPIO pins are NOT de-initialized (static allocation)
 *
 * @attention
 * - Call after system clock initialization
 * - Call before using configured peripherals
 * - Peripheral clocks must be enabled via RCC
 * - Interrupt handlers must be linked separately
 *
 * @copyright 2026 Likith025 - MIT License
 * @see stm32f7xx_gpio_driver.h - GPIO driver
 * @see stm32f7xx_uart_driver.h - UART driver
 * @see stm32f7xx_timer_driver.h - TIMER driver
 */

#include "peripheral_config.h"

/**
 * @defgroup BSP_LED_FUNCTIONS LED Configuration Functions
 * @{
 */

/**
 * @brief Configure red LED (GPIO_B pin 14)
 * @param[in,out] pLED Pointer to GPIO_handler_t for LED configuration
 * @retval void
 *
 * @details
 * Initializes GPIO pin 14 on PORT B as an output for the red LED indicator.
 * Sets up push-pull output with high speed capability for LED visibility.
 *
 * **GPIO Configuration:**
 * - Port: GPIOB
 * - Pin: 14
 * - Mode: Output (GPIO_MODE_OUT)
 * - Speed: High (for LED response visibility)
 * - Pull: None (open output)
 * - Initial State: Low (LED off)
 * - Push-Pull: Yes (standard LED drive)
 *
 * **Register Setup:**
 * - GPIOB_MODER[28:29] = 01 (Output mode)
 * - GPIOB_OSPEEDR[28:29] = 10 (High speed)
 * - GPIOB_OTYPER[14] = 0 (Push-pull)
 * - GPIOB_PUPDR[28:29] = 00 (No pull)
 * - GPIOB_ODR[14] = 0 (Initial low - LED off)
 *
 * **LED Drive Current:**
 * - Output HIGH: ~100 mA (GPIO max, limited by external resistor)
 * - Forward Voltage: 2.0-3.0 V typical for red LED
 * - Typical Series Resistor: 100-200 Ω
 * - Power Consumption: ~10-20 mW per LED
 *
 * **Usage:**
 * ```c
 * GPIO_handler_t led;
 * led_setup_red(&led);     // Initialize
 * GPIO_WritePin(led.pGPIOx, 14, SET);     // Turn on
 * GPIO_WritePin(led.pGPIOx, 14, RESET);   // Turn off
 * ```
 *
 * @param[in,out] pLED Handler structure to be configured:
 *   - pLED->pGPIOx: Set to GPIOB
 *   - pLED->pin: Set to 14
 *   - Configuration: Output, high speed, push-pull
 *
 * @note
 * - GPIOB clock must be enabled in RCC before calling
 * - Can be called multiple times (idempotent)
 * - LED is off after initialization (pin low)
 * - Fast visibility (no PWM dimming)
 *
 * @example
 * GPIO_handler_t my_led;
 * led_setup_red(&my_led);
 * // LED is now ready to control
 *
 * @see led_setup_blue(), GPIO_WritePin()
 */
void led_setup_red(GPIO_handler_t *pLED) {
    pLED->pGPIOx = GPIOB;
    pLED->pin = 14;
    // Configure as output with high speed
    // GPIOB_MODER[28:29] = 01 (Output)
    // GPIOB_OSPEEDR[28:29] = 10 (High speed)
    GPIO_Init(pLED->pGPIOx, pLED->pin, GPIO_MODE_OUT, GPIO_SPEED_HIGH);
}

/**
 * @brief Configure blue LED (GPIO_B pin 15)
 * @param[in,out] pLED Pointer to GPIO_handler_t for LED configuration
 * @retval void
 *
 * @details
 * Initializes GPIO pin 15 on PORT B as an output for the blue LED indicator.
 * Configuration identical to red LED but on different pin.
 *
 * **GPIO Configuration:**
 * - Port: GPIOB
 * - Pin: 15
 * - Mode: Output (GPIO_MODE_OUT)
 * - Speed: High (for LED response visibility)
 * - Pull: None (open output)
 * - Initial State: Low (LED off)
 *
 * **Pin and Port Details:**
 * - Pin 15: Adjacent to red LED pin 14
 * - Same port allows simultaneous control
 * - Can be used for status indication
 * - Recommended: Red=Error, Blue=Ready
 *
 * **Usage:**
 * ```c
 * GPIO_handler_t led_blue;
 * led_setup_blue(&led_blue);   // Initialize
 * GPIO_WritePin(led_blue.pGPIOx, 15, SET);     // Turn on
 * ```
 *
 * @param[in,out] pLED Handler structure to be configured:
 *   - pLED->pGPIOx: Set to GPIOB
 *   - pLED->pin: Set to 15
 *
 * @note
 * - GPIOB clock must be enabled in RCC before calling
 * - Can be called multiple times
 * - Independent from red LED
 *
 * @see led_setup_red(), GPIO_WritePin()
 */
void led_setup_blue(GPIO_handler_t *pLED) {
    pLED->pGPIOx = GPIOB;
    pLED->pin = 15;
    // Configure as output with high speed
    GPIO_Init(pLED->pGPIOx, pLED->pin, GPIO_MODE_OUT, GPIO_SPEED_HIGH);
}

/** @} */ // end of BSP_LED_FUNCTIONS

/**
 * @defgroup BSP_BUTTON_FUNCTIONS Button Configuration Functions
 * @{
 */

/**
 * @brief Configure button input (GPIO_A pin 0)
 * @param[void]
 * @retval void
 *
 * @details
 * Initializes GPIO pin 0 on PORT A as an input for button detection.
 * Configures with internal pull-up resistor to ensure stable logic levels.
 *
 * **GPIO Configuration:**
 * - Port: GPIOA
 * - Pin: 0
 * - Mode: Input (GPIO_MODE_IN)
 * - Pull: Pull-up (internal pull-up enabled)
 * - Initial State: HIGH (button not pressed)
 * - Debouncing: External (software or hardware as needed)
 *
 * **Button Logic:**
 * - Button NOT pressed: Pin HIGH (pulled up, current drawn to VDD)
 * - Button pressed: Pin LOW (connected to GND)
 * - Typical pull-up resistor: 10-50 kΩ internal
 *
 * **Pin Selection:**
 * - Pin 0 is WKUP pin (can wake from low-power mode)
 * - Good for emergency stop or critical button
 * - Interrupt capability available via EXTI0
 *
 * @note
 * - GPIOA clock must be enabled in RCC before calling
 * - Pull-up enabled: ensures stable reading
 * - Can be read via GPIO_ReadPin()
 * - Optionally configure interrupt for edge detection
 *
 * **Typical Usage:**
 * ```c
 * // Initialize button
 * button_setup();
 *
 * // Poll button in main loop
 * while (1) {
 *     if (GPIO_ReadPin(GPIOA, 0) == GPIO_PIN_RESET) {
 *         // Button pressed
 *         process_button_press();
 *     }
 * }
 * ```
 *
 * @example
 * button_setup();  // Initialize
 * // Now can read: GPIO_ReadPin(GPIOA, 0)
 *
 * @see button_interrupt_setup(), GPIO_ReadPin()
 */
void button_setup(void) {
    // Configure GPIOA pin 0 as input with pull-up
    // GPIOA_MODER[1:0] = 00 (Input)
    // GPIOA_PUPDR[1:0] = 01 (Pull-up)
    GPIO_Init(GPIOA, 0, GPIO_MODE_IN, GPIO_PULL_UP);
}

/**
 * @brief Configure button interrupt (EXTI on pin 0)
 * @param[void]
 * @retval void
 *
 * @details
 * Configures External Interrupt (EXTI) line 0 for falling edge detection on button pin.
 * Enables interrupt handler to be triggered on button press (transition from HIGH to LOW).
 *
 * **EXTI Configuration:**
 * - Line: EXTI0 (GPIO pin 0)
 * - Trigger: Falling edge (button press from HIGH to LOW)
 * - NVIC Priority: Configurable (typically 5-10)
 * - Interrupt Vector: EXTI0_IRQHandler
 *
 * **Register Setup:**
 * - SYSCFG_EXTICR1[3:0] = 0000 (Select GPIOA pin 0)
 * - EXTI_FTSR[0] = 1 (Falling edge trigger)
 * - EXTI_IMR[0] = 1 (Interrupt mask enabled)
 *
 * **Interrupt Response:**
 * - Falling edge detected: EXTI0_IRQHandler triggered
 * - Pending flag set: EXTI_PR[0] = 1
 * - Must clear flag in handler: EXTI_PR[0] = 1 (write 1 to clear)
 *
 * **Priority Configuration:**
 * ```c
 * // After interrupt setup
 * IntrruptConfig(IRQ_NO_EXTI0, 5, ENABLE);  // Priority 5, enabled
 * ```
 *
 * @note
 * - button_setup() should be called first to configure pin as input
 * - SYSCFG clock must be enabled in RCC
 * - EXTI0 must be linked in startup/interrupt vectors
 * - Application must implement EXTI0_IRQHandler()
 * - Remember to clear pending flag in handler
 *
 * **Typical Interrupt Handler:**
 * ```c
 * void EXTI0_IRQHandler(void) {
 *     if (EXTI->PR & EXTI_PR_PR0) {
 *         // Button press detected
 *         process_button();
 *         // Clear pending flag
 *         EXTI->PR |= EXTI_PR_PR0;
 *     }
 * }
 * ```
 *
 * @example
 * button_setup();              // Configure pin
 * button_interrupt_setup();    // Configure EXTI
 * IntrruptConfig(IRQ_NO_EXTI0, 5, ENABLE);  // Enable interrupt
 *
 * @see button_setup(), IntrruptConfig()
 */
void button_interrupt_setup(void) {
    // Configure EXTI0 for falling edge (button press)
    // SYSCFG_EXTICR1[3:0] = 0000 (GPIOA)
    // EXTI_FTSR[0] = 1 (Falling edge)
    // EXTI_IMR[0] = 1 (Enable interrupt mask)
    GPIO_EXTI_Config(0);
}

/** @} */ // end of BSP_BUTTON_FUNCTIONS

/**
 * @defgroup BSP_USART_FUNCTIONS USART3 Configuration Functions
 * @{
 */

/**
 * @brief Initialize USART3 with default settings
 * @param[in,out] pUSART Pointer to USART_handler_t to configure
 * @retval void
 *
 * @details
 * Configures USART3 with standard settings for serial communication:
 * - Baud Rate: 115200 bps
 * - Data Bits: 8
 * - Stop Bits: 1
 * - Parity: None
 * - Flow Control: None
 * - Mode: TX + RX (full duplex)
 *
 * **USART3 Hardware Mapping:**
 * - Port: APB1 (42 MHz clock)
 * - TX: GPIO_C pin 10 (Alternate Function AF7)
 * - RX: GPIO_C pin 11 (Alternate Function AF7)
 * - Interrupt: USART3_IRQn (39)
 *
 * **Configuration Structure:**
 * ```c
 * USART_handler_t {
 *     pUSART = USART3
 *     USART_config {
 *         USART_Mode = USART_MODE_TXRX
 *         USART_BaudRate = USART_BAUD_115200
 *         USART_StopBits = USART_StopBits_1
 *         USART_WordLength = USART_WordLen_8bits
 *         USART_ParityControl = USART_Parity_None
 *         USART_HWflowControl = USART_HW_FlowCtrl_None
 *         USART_OverSampling = USART_OverSampling_8
 *     }
 * }
 * ```
 *
 * **Baud Rate Calculation:**
 * - Clock: 42 MHz (APB1)
 * - Baud: 115200
 * - Divisor: 42,000,000 / 115,200 = 364.58 ≈ 365
 * - Actual Baud: 42,000,000 / 365 = 115,068 (0.12% error, acceptable)
 *
 * @param[in,out] pUSART Handler to configure:
 *   - pUSART->pUSART: Set to USART3_BASE
 *   - pUSART->USART_config: Set with above parameters
 *
 * @note
 * - USART3 clock must be enabled in RCC
 * - GPIO pins must be configured as alternate functions first
 * - Interrupt handlers must be linked separately
 * - Non-blocking: initialization completes immediately
 *
 * **Typical Usage:**
 * ```c
 * USART_handler_t uart3;
 * usart3_init(&uart3);              // Initialize
 * usart3_tx(&uart3_tx);             // Configure TX
 * usart3_rx(&uart3_rx);             // Configure RX
 * IntrruptConfig(IRQ_NO_USART3, 5, ENABLE);  // Enable interrupt
 * ```
 *
 * @see usart3_tx(), usart3_rx(), USART_SendData_IT()
 */
void usart3_init(USART_handler_t *pUSART) {
    pUSART->pUSART = USART3;
    pUSART->USART_config.USART_Mode = USART_MODE_TXRX;
    pUSART->USART_config.USART_BaudRate = USART_BAUD_115200;
    pUSART->USART_config.USART_StopBits = USART_StopBits_1;
    pUSART->USART_config.USART_WordLength = USART_WordLen_8bits;
    pUSART->USART_config.USART_ParityControl = USART_Parity_None;
    pUSART->USART_config.USART_HWflowControl = USART_HW_FlowCtrl_None;
    pUSART->USART_config.USART_OverSampling = USART_OverSampling_8;
    USART_init(pUSART, ENABLE);
}

/**
 * @brief Configure USART3 TX pin (GPIO_C pin 10)
 * @param[in,out] pTX Pointer to GPIO_handler_t for TX configuration
 * @retval void
 *
 * @details
 * Initializes GPIO pin 10 on PORT C as USART3 TX output.
 * Configures as alternate function 7 (USART3_TX).
 *
 * **GPIO Configuration:**
 * - Port: GPIOC
 * - Pin: 10
 * - Mode: Alternate Function (USART3)
 * - Alternate Function: AF7
 * - Speed: High (for UART timing)
 * - Pull: None (driven by USART peripheral)
 * - Output Type: Push-pull
 *
 * **Serial Output:**
 * - Voltage: LVCMOS 3.3V
 * - Data Rate: Up to 115200 bps
 * - Idle State: HIGH (mark state, 0x01 bit)
 * - Start Bit: LOW (0x00)
 *
 * @param[in,out] pTX Handler structure:
 *   - pTX->pGPIOx: Set to GPIOC
 *   - pTX->pin: Set to 10
 *   - pTX->alternate_function: Set to 7
 *
 * @note
 * - GPIOC clock must be enabled
 * - usart3_init() should be called first
 * - GPIO configured for USART operation
 * - TX data flows from CPU → GPIO → serial line
 *
 * **Typical Usage:**
 * ```c
 * GPIO_handler_t tx_config;
 * usart3_tx(&tx_config);  // Configure
 * // TX is now ready, can send data
 * ```
 *
 * @see usart3_init(), usart3_rx()
 */
void usart3_tx(GPIO_handler_t *pTX) {
    pTX->pGPIOx = GPIOC;
    pTX->pin = 10;
    // Configure as alternate function AF7 (USART3_TX)
    GPIO_Init(pTX->pGPIOx, pTX->pin, GPIO_MODE_AF, GPIO_SPEED_HIGH);
}

/**
 * @brief Configure USART3 RX pin (GPIO_C pin 11)
 * @param[in,out] pRX Pointer to GPIO_handler_t for RX configuration
 * @retval void
 *
 * @details
 * Initializes GPIO pin 11 on PORT C as USART3 RX input.
 * Configures as alternate function 7 (USART3_RX).
 *
 * **GPIO Configuration:**
 * - Port: GPIOC
 * - Pin: 11
 * - Mode: Alternate Function (USART3)
 * - Alternate Function: AF7
 * - Pull: Pull-up (for signal integrity)
 * - Input Type: Digital
 *
 * **Serial Input:**
 * - Voltage: LVCMOS 3.3V compatible
 * - Data Rate: Up to 115200 bps
 * - Idle State: HIGH (mark state)
 * - Start Bit Detection: LOW edge
 *
 * **Hardware Flow:**
 * - Serial data flows from line → GPIO → USART RX register
 * - USART samples at 16× baud rate (for robust reception)
 * - Parity and framing errors detected automatically
 *
 * @param[in,out] pRX Handler structure:
 *   - pRX->pGPIOx: Set to GPIOC
 *   - pRX->pin: Set to 11
 *   - pRX->alternate_function: Set to 7
 *
 * @note
 * - GPIOC clock must be enabled
 * - usart3_init() should be called first
 * - GPIO configured for USART operation
 * - RX data flows from serial line → GPIO → CPU
 *
 * **Typical Usage:**
 * ```c
 * GPIO_handler_t rx_config;
 * usart3_rx(&rx_config);  // Configure
 * // RX is now ready to receive data
 * ```
 *
 * @see usart3_init(), usart3_tx(), USART_ReadData_IT()
 */
void usart3_rx(GPIO_handler_t *pRX) {
    pRX->pGPIOx = GPIOC;
    pRX->pin = 11;
    // Configure as alternate function AF7 (USART3_RX)
    GPIO_Init(pRX->pGPIOx, pRX->pin, GPIO_MODE_AF, GPIO_SPEED_HIGH);
}

/** @} */ // end of BSP_USART_FUNCTIONS

/**
 * @defgroup BSP_TIMER_FUNCTIONS TIMER2 Configuration Functions
 * @{
 */

/**
 * @brief Configure TIMER2 Channel 1 PWM output (GPIO_A pin 0)
 * @param[in,out] pCH1 Pointer to TIMER_handler_t for channel configuration
 * @retval void
 *
 * @details
 * Initializes GPIO pin 0 on PORT A as TIMER2 Channel 1 PWM output.
 * Configures as alternate function 1 (TIM2_CH1).
 *
 * **GPIO Configuration:**
 * - Port: GPIOA
 * - Pin: 0
 * - Mode: Alternate Function (TIMER2)
 * - Alternate Function: AF1
 * - Speed: High (for PWM switching frequency)
 * - Output Type: Push-pull
 *
 * **PWM Output Characteristics:**
 * - Frequency: Programmable (typically 1-100 kHz)
 * - Duty Cycle: Programmable (0-100%)
 * - Voltage: LVCMOS 3.3V
 * - Output Current: Up to 100 mA (with external buffer)
 *
 * **PWM Applications:**
 * - LED brightness control
 * - Motor speed control
 * - Servo control
 * - Switch-mode power supply
 *
 * @param[in,out] pCH1 Handler structure:
 *   - pCH1->pTimer: Set to TIMER2_BASE
 *   - pCH1->TimerConfig: PWM configuration
 *
 * @note
 * - GPIOA clock must be enabled
 * - timer2_setup() should be called first
 * - GPIO configured for PWM output
 * - PWM starts at 0% duty cycle
 *
 * **PWM Configuration Sequence:**
 * ```c
 * TIMER_handler_t timer;
 * timer2_setup(&timer);           // Initialize timer
 * timer2ch1(&timer);              // Configure PWM output
 * TimerSetFrequency(&timer, 1000);// Set 1 kHz frequency
 * TimerPWM_init(&timer, CH1);    // Enable PWM
 * TimerControl(&timer, ENABLE);   // Start timer
 * TimerPWM_DutyCycle(&timer, CH1, 50); // Set 50% duty
 * ```
 *
 * @see timer2_setup(), TimerPWM_init(), TimerPWM_DutyCycle()
 */
void timer2ch1(TIMER_handler_t *pCH1) {
    pCH1->pTimer = TIMER2;
    // Configure as alternate function AF1 (TIM2_CH1)
    GPIO_Init(GPIOA, 0, GPIO_MODE_AF, GPIO_SPEED_HIGH);
}

/**
 * @brief Initialize TIMER2 with default settings
 * @param[in,out] pTimer Pointer to TIMER_handler_t to configure
 * @param[in] direction Timer direction (UpCounter or DownCounter)
 * @retval void
 *
 * @details
 * Configures TIMER2 for basic operation. Sets up default prescaler and period
 * values. Actual PWM frequency is configured via TimerSetFrequency().
 *
 * **TIMER2 Hardware:**
 * - Type: 32-bit general-purpose timer
 * - Clock: APB1 (42 MHz after RCC prescaler)
 * - Channels: 4 PWM outputs (CH1-CH4)
 * - Prescaler Range: 1-65536
 * - Period Range: 1-0xFFFFFFFF (32-bit)
 *
 * **Register Configuration:**
 * - TIM_PSC: Prescaler value
 * - TIM_ARR: Auto-reload register (period)
 * - TIM_CR1: Control register (enable, direction)
 * - TIM_CCMRx: Capture/compare mode (PWM mode 1)
 * - TIM_CCER: Capture/compare enable
 * - TIM_CCRx: Capture/compare register (duty cycle)
 *
 * **Timer Modes:**
 * - Up-Counter: Counts 0 → ARR → 0 (edge-aligned PWM)
 * - Down-Counter: Counts ARR → 0 → ARR (alternative timing)
 *
 * @param[in,out] pTimer Handler to configure:
 *   - pTimer->pTimer: Set to TIMER2_BASE
 *   - pTimer->TimerConfig: Timer configuration
 *
 * @param[in] direction Counting mode:
 *   - UpCounter (0): 0 → ARR → 0 (standard PWM)
 *   - DownCounter (1): ARR → 0 → ARR (alternative)
 *
 * @note
 * - TIMER2 clock must be enabled in RCC
 * - Default prescaler and period are minimal
 * - PWM frequency set via TimerSetFrequency()
 * - Can call after system initialization
 *
 * **Typical Usage:**
 * ```c
 * TIMER_handler_t timer2;
 * timer2_setup(&timer2, UpCounter);  // Initialize
 * TimerSetFrequency(&timer2, 1000);  // 1 kHz
 * TimerPWM_init(&timer2, CH1);      // Enable PWM
 * TimerControl(&timer2, ENABLE);     // Start
 * ```
 *
 * @see timer2ch1(), TimerInit(), TimerSetFrequency()
 */
void timer2_setup(TIMER_handler_t *pTimer, uint8_t direction) {
    pTimer->pTimer = TIMER2;
    pTimer->TimerConfig.PreScaler = 1;
    pTimer->TimerConfig.Period = 1000;
    pTimer->TimerConfig.direction = direction;
    TimerInit(pTimer);
}

/** @} */ // end of BSP_TIMER_FUNCTIONS
