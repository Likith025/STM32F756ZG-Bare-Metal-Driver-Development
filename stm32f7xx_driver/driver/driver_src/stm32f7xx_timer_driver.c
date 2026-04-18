/**
 * @file stm32f7xx_timer_driver.c
 * @author Likith025
 * @date March 29, 2026
 * @brief STM32F7xx Timer Driver - PWM and Timer Management
 *
 * @details
 * This module provides comprehensive timer control functions for the STM32F756ZG microcontroller.
 * It includes:
 * - Timer initialization with prescaler and period configuration
 * - PWM generation on multiple channels
 * - Frequency setting and duty cycle control
 * - Timer counter management and flag handling
 * - Support for up/down counting modes
 *
 * **Timer Features:**
 * - 16-bit general-purpose timers (TIM2-TIM5, etc.)
 * - 4 PWM output channels per timer
 * - Programmable prescaler (1-65536)
 * - Auto-reload register for period control
 * - Flexible frequency selection
 * - Edge-aligned or center-aligned PWM
 *
 * **Clock Configuration:**
 * - APB1 Timers (TIM2-TIM5): 42 MHz clock
 * - Frequency calculation: F = (Clock / Prescaler) / Period
 *
 * @copyright 2026 Likith025 - MIT License
 * @see stm32f756zg_reg.h - Timer register definitions
 */

#include "stm32f7xx_timer_driver.h"

#define INPUT_CLK_FREQ		(16000000)
#define TARGET_CLK_FREQ		(100000)

/**
 * @defgroup TIMER_FUNCTIONS Timer Driver Functions
 * @{
 */

/**
 * @brief Initialize timer with specified configuration
 * @param[in,out] timer_handle Pointer to TIMER_handler_t structure
 * @retval void
 *
 * @details
 * Initializes a timer peripheral with the configuration stored in the handler structure.
 * Sets up prescaler, period, and counting direction. Enables the timer clock automatically.
 *
 * **Initialization Steps:**
 * 1. Enable timer clock via RCC
 * 2. Set prescaler value (TIM_PSC)
 * 3. Set auto-reload value / period (TIM_ARR)
 * 4. Configure counting direction (up/down)
 * 5. Clear counter and flags
 * 6. Timer is disabled; use TimerControl() to start
 *
 * **Register Mapping:**
 * - TIM_PSC: Prescaler value (divides input clock)
 * - TIM_ARR: Auto-reload register (period - 1)
 * - TIM_CR1: Control register (direction, enable bits)
 * - TIM_CNT: Counter value (current count)
 *
 * @param[in] timer_handle Handler with configuration:
 *   - pTimer: Base address of timer (TIMER2, TIMER3, etc.)
 *   - TimerConfig.PreScaler: Clock divider value (1-65536)
 *   - TimerConfig.Period: Reload value (1-65535)
 *   - TimerConfig.direction: UpCounter (0) or DownCounter (1)
 *
 * @note
 * - Timer clock must be enabled in RCC before calling this function
 * - Handler must be properly initialized with configuration values
 * - After initialization, timer is stopped; call TimerControl(handle, ENABLE) to start
 * - Prescaler acts as (PSC + 1) divisor
 * - Period acts as (ARR + 1) reload count
 *
 * @warning
 * - Zero prescaler/period causes undefined behavior
 * - Invalid timer address causes register corruption
 * - Do not call while timer is running
 *
 * @example
 * // Initialize TIMER2 for 1kHz with 50% PWM
 * TIMER_handler_t timer;
 * timer.pTimer = TIMER2;
 * timer.TimerConfig.PreScaler = 16;    // 1MHz after prescale
 * timer.TimerConfig.Period = 1000;     // 1kHz frequency
 * timer.TimerConfig.direction = UpCounter;
 * TimerInit(&timer);
 * TimerControl(&timer, ENABLE);
 *
 * @see TimerControl(), TimerSetFrequency()
 */
void TimerInit(TIMER_handler_t* timer_handle) {
	RCC->APB1ENR |= (1 << 0);
	timer_handle->pTimer = TIMER2;
	timer_handle->pTimer->TIM_PSC = timer_handle->TimerConfig.PreScaler - 1;
	timer_handle->pTimer->TIM_ARR = timer_handle->TimerConfig.Period - 1;
	
	if (timer_handle->TimerConfig.direction == 1) {
		timer_handle->pTimer->TIM_CR1 |= (1 << 4);
	} else {
		timer_handle->pTimer->TIM_CR1 &= ~(1 << 4);
	}
	
	timer_handle->pTimer->TIM_CNT = 0;
	TimerResetFlag(timer_handle, Timer_Flag_UIF);
}

/**
 * @brief Enable or disable timer operation
 * @param[in] timer_handle Pointer to TIMER_handler_t structure
 * @param[in] state Enable (ENABLE/1) or disable (DISABLE/0)
 * @retval void
 *
 * @details
 * Controls the timer enable state via the CEN (Counter Enable) bit in CR1 register.
 * When enabled, the timer counter starts incrementing/decrementing. When disabled,
 * the counter is frozen but retains its current value.
 *
 * **Register Operation:**
 * - CR1 bit 0 (CEN): Counter Enable
 * - Set to 1: Timer starts counting
 * - Set to 0: Timer stops counting (value preserved)
 *
 * @param[in] timer_handle Timer handler with pointer to timer instance
 * @param[in] state Timer control:
 *   - ENABLE (1): Start the timer counter
 *   - DISABLE (0): Stop the timer counter
 *
 * @note
 * - Does not affect timer configuration (prescaler, period)
 * - Can be called repeatedly without issues
 * - Counter value is preserved when disabled
 * - Safe to disable during running operation
 *
 * @example
 * // Start timer after initialization
 * TimerControl(&timer, ENABLE);
 *
 * // Stop timer temporarily
 * TimerControl(&timer, DISABLE);
 *
 * // Resume timer operation
 * TimerControl(&timer, ENABLE);
 *
 * @see TimerInit(), TimerGetCount()
 */
void TimerControl(TIMER_handler_t* timer_handle, uint16_t state) {
	if (state) {
		timer_handle->pTimer->TIM_CR1 |= (1 << 0);
	} else {
		timer_handle->pTimer->TIM_CR1 &= ~(1 << 0);
	}
}

/**
 * @brief Get current timer counter value
 * @param[in] timer_handle Pointer to TIMER_handler_t structure
 * @retval uint32_t Current counter value (0 to Period)
 *
 * @details
 * Reads the current value of the timer counter. Useful for timing measurements,
 * event detection, or synchronization purposes.
 *
 * **Counter Behavior:**
 * - Up-counting: Increments from 0 to (Period-1), then resets to 0
 * - Down-counting: Decrements from (Period-1) to 0, then resets to (Period-1)
 * - Value range: 0 to (ARR + 1 - 1)
 * - Frequency: F_counter = Timer_Clock / Prescaler
 *
 * @param[in] timer_handle Timer handler with pointer to timer instance
 *
 * @return Current counter value:
 *   - Up-counter: 0 when timer just started, increases to Period
 *   - Down-counter: Period when timer just started, decreases to 0
 *   - Stopped: Last value before timer stopped
 *
 * @note
 * - Non-blocking operation (returns immediately)
 * - Multiple reads may return different values (counter incrementing)
 * - Useful for profiling and timing analysis
 * - Counter wraps around at Period value
 *
 * @example
 * // Measure elapsed time
 * uint32_t start = TimerGetCount(&timer);
 * // ... do something ...
 * uint32_t end = TimerGetCount(&timer);
 * uint32_t elapsed = end - start;  // Time in counter ticks
 *
 * // Calculate actual time in seconds
 * // Assuming 1kHz timer: elapsed / 1000 = seconds
 *
 * @see TimerControl(), TimerSetFrequency()
 */
uint32_t TimerGetCount(TIMER_handler_t* timer_handle) {
	uint32_t timer_count = 0;
	timer_count = timer_handle->pTimer->TIM_CNT;
	return timer_count;
}

/**
 * @brief Check if a specific timer flag is set
 * @param[in] timer_handle Pointer to TIMER_handler_t structure
 * @param[in] Flag Flag to check (Timer_Flag_UIF, Timer_Flag_CC1IF, etc.)
 * @retval uint8_t 1 if flag is set, 0 if flag is clear
 *
 * @details
 * Reads the status of a timer flag. Common flags include:
 * - UIF (Update Interrupt Flag): Set when counter reloads
 * - CCxIF (Capture/Compare Flag): Set when CCR matches counter
 * - TIF (Trigger Interrupt Flag): Set on trigger event
 * - CCxOF (Capture/Compare Overcapture Flag): Multiple captures without reading
 *
 * **Flag Behavior:**
 * - Flag is automatically set when event occurs
 * - Flag is cleared by software (via TimerResetFlag) or by reading ICR
 * - Must be checked before interrupt handler to verify event source
 * - Multiple flags can be set simultaneously
 *
 * @param[in] timer_handle Timer handler with pointer to timer instance
 * @param[in] Flag Flag enumeration:
 *   - Timer_Flag_UIF: Update/reload event
 *   - Timer_Flag_CC1IF: Channel 1 capture/compare
 *   - Timer_Flag_CC2IF: Channel 2 capture/compare
 *   - Timer_Flag_CC3IF: Channel 3 capture/compare
 *   - Timer_Flag_CC4IF: Channel 4 capture/compare
 *   - Timer_Flag_TIF: Trigger event
 *   - Timer_Flag_CC1OF: Channel 1 overcapture
 *   - Timer_Flag_CC2OF: Channel 2 overcapture
 *   - Timer_Flag_CC3OF: Channel 3 overcapture
 *   - Timer_Flag_CC4OF: Channel 4 overcapture
 *
 * @return Flag status:
 *   - 1: Flag is set (event occurred)
 *   - 0: Flag is clear (no event)
 *
 * @note
 * - Non-blocking, immediate return
 * - Does not clear the flag
 * - Typically used in polling loops
 * - Can be called multiple times to check flag state
 *
 * @example
 * // Poll for update event
 * if (TimerGetFlagStatus(&timer, Timer_Flag_UIF)) {
 *     // Counter has reloaded
 *     update_count++;
 *     TimerResetFlag(&timer, Timer_Flag_UIF);
 * }
 *
 * // Check for capture/compare event
 * if (TimerGetFlagStatus(&timer, Timer_Flag_CC1IF)) {
 *     // Channel 1 compare event occurred
 *     process_event();
 *     TimerResetFlag(&timer, Timer_Flag_CC1IF);
 * }
 *
 * @see TimerResetFlag(), TimerGetCount()
 */
uint8_t TimerGetFlagStatus(TIMER_handler_t* timer_handle, Timer_flags_t Flag) {
	uint8_t status = 0;
	if (timer_handle->pTimer->TIM_SR & Flag) {
		status = 1;
	}
	return status;
}

/**
 * @brief Clear a timer flag
 * @param[in] timer_handle Pointer to TIMER_handler_t structure
 * @param[in] Flag Flag to clear
 * @retval void
 *
 * @details
 * Clears a status flag in the timer's Status Register (SR). Must be called after
 * processing an event to allow the flag to be set again. Typically used in interrupt
 * handlers or polling loops after checking TimerGetFlagStatus().
 *
 * **Flag Clearing Mechanism:**
 * - Writing 0 to SR bit clears that flag
 * - Other bits are unaffected
 * - Some flags auto-clear; others require software clearing
 * - Must be called before flag can set again
 *
 * @param[in] timer_handle Timer handler with pointer to timer instance
 * @param[in] Flag Flag to clear (same enumeration as TimerGetFlagStatus)
 *
 * @note
 * - Should be called immediately after handling event
 * - Clears only specified flag; other flags unchanged
 * - Safe to call even if flag is not set
 * - Necessary for re-triggering flag detection
 *
 * @example
 * // Typical interrupt handler pattern
 * void TIMER2_IRQHandler(void) {
 *     if (TimerGetFlagStatus(&timer, Timer_Flag_UIF)) {
 *         // Handle update event
 *         process_timer_tick();
 *         // Clear flag to allow next event
 *         TimerResetFlag(&timer, Timer_Flag_UIF);
 *     }
 * }
 *
 * // Polling pattern
 * while (1) {
 *     if (TimerGetFlagStatus(&timer, Timer_Flag_CC1IF)) {
 *         capture_value = TimerGetCount(&timer);
 *         TimerResetFlag(&timer, Timer_Flag_CC1IF);
 *     }
 * }
 *
 * @see TimerGetFlagStatus(), TimerInit()
 */
void TimerResetFlag(TIMER_handler_t* timer_handle, Timer_flags_t Flag) {
	timer_handle->pTimer->TIM_SR &= ~(Flag);
}

/**
 * @brief Set timer frequency by calculating prescaler and period
 * @param[in,out] handler Pointer to TIMER_handler_t structure
 * @param[in] freq Desired frequency in Hz
 * @retval void
 *
 * @details
 * Calculates and configures prescaler and period registers to achieve the specified
 * output frequency. Automatically selects an ARR value of 1000 for good resolution
 * and calculates the prescaler accordingly.
 *
 * **Frequency Calculation:**
 * ```
 * Timer_Clock = 16 MHz (APB1)
 * Timer_Frequency = Timer_Clock / (Prescaler × Period)
 * Prescaler = Timer_Clock / (Period × Timer_Frequency)
 * ```
 *
 * **Algorithm:**
 * 1. Calculate total divider needed: total_counts = Clock / frequency
 * 2. Set Period (ARR) to fixed value (1000) for good resolution
 * 3. Calculate Prescaler = total_counts / Period
 * 4. Store in handler for later initialization
 *
 * **Example Calculation:**
 * ```
 * Desired: 1000 Hz
 * Clock: 16 MHz
 * total_counts = 16,000,000 / 1000 = 16,000
 * ARR = 1000
 * PSC = 16,000 / 1000 = 16
 * Actual Freq = 16,000,000 / (16 × 1000) = 1000 Hz ✓
 * ```
 *
 * @param[in,out] handler Timer handler to store calculated values:
 *   - handler->TimerConfig.PreScaler: Calculated prescaler value
 *   - handler->TimerConfig.Period: Fixed at 1000 for resolution
 *
 * @param[in] freq Target frequency in Hz:
 *   - Common values: 1000, 10000, 100000 Hz
 *   - Higher frequencies require faster timer clock
 *   - Lower frequencies need larger prescaler
 *   - Range: 1 Hz to (Clock / 2)
 *
 * @note
 * - Automatically selects Period = 1000 for good PWM resolution
 * - Period fixed value; adjust manually for different resolution
 * - Prescaler automatically calculated
 * - Must call TimerInit() after this function to apply values
 * - Clock frequency fixed at 16 MHz (adjust USART_CLK if different)
 *
 * **Typical Usage Sequence:**
 * ```c
 * TimerSetFrequency(&timer, 1000);    // Set to 1 kHz
 * TimerInit(&timer);                   // Apply configuration
 * TimerControl(&timer, ENABLE);        // Start timer
 * ```
 *
 * @example
 * // Generate 1 kHz timer frequency
 * TimerSetFrequency(&timer, 1000);
 *
 * // For other frequencies
 * TimerSetFrequency(&timer, 10000);   // 10 kHz
 * TimerSetFrequency(&timer, 100000);  // 100 kHz
 *
 * @see TimerInit(), TimerControl()
 */
void TimerSetFrequency(TIMER_handler_t* handler, uint32_t freq) {
	uint32_t timer_clk = INPUT_CLK_FREQ;
	uint32_t total_counts = timer_clk / freq;

	// Choose ARR first (good resolution)
	uint32_t arr = 1000;

	// Compute prescaler
	uint32_t prescaler = total_counts / arr;

	handler->TimerConfig.PreScaler = prescaler;
	handler->TimerConfig.Period = arr;
}

/**
 * @brief Initialize PWM on a timer channel
 * @param[in,out] timer_handle Pointer to TIMER_handler_t structure
 * @param[in] Channel PWM output channel (CH1, CH2, CH3, CH4)
 * @retval void
 *
 * @details
 * Configures a timer channel for PWM (Pulse Width Modulation) output. Sets up the
 * capture/compare mode register (CCMR) and enables the channel output.
 *
 * **PWM Configuration:**
 * - Mode: PWM Mode 1 (output high while TIM_CNT < TIM_CCR)
 * - Channel mapping:
 *   - CH1: TIM_CCMR1 bits 4-6
 *   - CH2: TIM_CCMR1 bits 12-14
 *   - CH3: TIM_CCMR2 bits 4-6
 *   - CH4: TIM_CCMR2 bits 12-14
 * - CCR: Compare register for duty cycle control
 *
 * **PWM Output Behavior:**
 * - PWM frequency: Timer frequency
 * - PWM resolution: Based on ARR value (typically 0-1000 for 10-bit resolution)
 * - Duty cycle: (CCR / ARR) × 100%
 * - Output pin: Must be configured via GPIO alternate function
 *
 * @param[in] timer_handle Timer handler with timer base address
 * @param[in] Channel PWM channel selection:
 *   - CH1 (1): Channel 1 output
 *   - CH2 (2): Channel 2 output
 *   - CH3 (3): Channel 3 output
 *   - CH4 (4): Channel 4 output
 *
 * @note
 * - GPIO pin must be configured as alternate function before PWM output
 * - PWM output starts at 0% duty cycle (CCR = 0)
 * - Must call TimerSetFrequency() first to set period
 * - Use TimerPWM_DutyCycle() to change duty cycle
 * - Timer must be disabled during PWM_init()
 *
 * **Register Configuration Steps:**
 * 1. Select CCMR register (1 for CH1-2, 2 for CH3-4)
 * 2. Clear channel output mode bits
 * 3. Clear channel input capture bits
 * 4. Set PWM mode (110 = PWM mode 1)
 * 5. Set preload enable bit
 * 6. Enable channel output via CCER
 * 7. Clear CCR register (0% duty cycle)
 *
 * @example
 * // Initialize PWM on channel 1
 * TimerSetFrequency(&timer, 1000);    // 1 kHz
 * timer2_setup(&timer, UpCounter);    // Configure timer
 * TimerPWM_init(&timer, CH1);         // Enable PWM
 * TimerControl(&timer, ENABLE);       // Start timer
 * TimerPWM_DutyCycle(&timer, CH1, 50); // Set 50% duty
 *
 * // Multiple channels
 * TimerPWM_init(&timer, CH2);
 * TimerPWM_init(&timer, CH3);
 *
 * @see TimerSetFrequency(), TimerPWM_DutyCycle(), TimerControl()
 */
void TimerPWM_init(TIMER_handler_t* timer_handle, Timer_channel_t Channel) {
	volatile uint32_t *ccmr;

	ccmr = (Channel <= 2) ? &timer_handle->pTimer->TIM_CCMR1
	                       : &timer_handle->pTimer->TIM_CCMR2;

	uint8_t shift = ((Channel - 1) % 2) * 8;
	*ccmr &= ~(3 << shift);
	*ccmr &= ~(7 << (shift + 4));
	*ccmr |= (6 << (shift + 4));
	*ccmr |= (1 << (shift + 3));
	timer_handle->pTimer->TIM_CR1 |= (1 << 7);
	timer_handle->pTimer->TIM_CCER |= (1 << (4 * (Channel - 1)));
	volatile uint32_t *ccr = &timer_handle->pTimer->TIM_CCR1 + (Channel - 1);
	*ccr = 0;
}

/**
 * @brief Set PWM duty cycle on a channel
 * @param[in,out] timer_handle Pointer to TIMER_handler_t structure
 * @param[in] Channel PWM output channel (CH1, CH2, CH3, CH4)
 * @param[in] DutyCycle Duty cycle percentage (0-100%)
 * @retval void
 *
 * @details
 * Modifies the Compare Register (CCR) to change PWM duty cycle. The duty cycle
 * determines the fraction of the period where the PWM output is HIGH.
 *
 * **Duty Cycle Calculation:**
 * ```
 * DutyCycle = 0%:   CCR = 0 (always low)
 * DutyCycle = 50%:  CCR = ARR / 2
 * DutyCycle = 100%: CCR = ARR (always high)
 * Formula: CCR = (ARR × DutyCycle) / 100
 * ```
 *
 * **Example (ARR = 1000):**
 * - 0% duty:   CCR = 0
 * - 25% duty:  CCR = 250
 * - 50% duty:  CCR = 500
 * - 75% duty:  CCR = 750
 * - 100% duty: CCR = 1000
 *
 * **PWM Output Timeline:**
 * ```
 * ARR = 1000 (period)
 * CCR = 300 (30% duty)
 *
 * Counter:    0  100  200  300  400  500  ...  1000
 * Output:    |  HIGH    |         LOW           |
 *
 * Output High Time = 300 / 1000 = 30% of period
 * ```
 *
 * @param[in] timer_handle Timer handler with timer base address and ARR value
 * @param[in] Channel PWM channel to modify:
 *   - CH1-CH4: Respective PWM channels
 *
 * @param[in] DutyCycle PWM duty cycle:
 *   - 0: Output always LOW (0% duty)
 *   - 50: Output 50% HIGH, 50% LOW (square wave)
 *   - 100: Output always HIGH (100% duty)
 *   - Values outside 0-100 are clamped to range
 *
 * @note
 * - Can be called while timer is running (changes take effect immediately)
 * - Resolution: (100 / ARR) % per LSB
 *   - With ARR=1000: 0.1% resolution
 *   - With ARR=256: 0.39% resolution
 * - Duty cycle updates are synchronous with PWM period
 * - Very fast operation (no blocking)
 *
 * **Implementation Details:**
 * - Reads current ARR (period) value
 * - Handles edge cases (0%, 100%)
 * - Calculates CCR from percentage
 * - Writes to Compare Register
 *
 * @example
 * // Set various duty cycles
 * TimerPWM_DutyCycle(&timer, CH1, 0);    // Off (always low)
 * TimerPWM_DutyCycle(&timer, CH1, 25);   // 25% duty
 * TimerPWM_DutyCycle(&timer, CH1, 50);   // 50% duty (square wave)
 * TimerPWM_DutyCycle(&timer, CH1, 75);   // 75% duty
 * TimerPWM_DutyCycle(&timer, CH1, 100);  // On (always high)
 *
 * // Dynamic PWM adjustment
 * for (int duty = 0; duty <= 100; duty += 10) {
 *     TimerPWM_DutyCycle(&timer, CH1, duty);
 *     delay(100);  // 100ms between changes
 * }
 *
 * @see TimerPWM_init(), TimerSetFrequency()
 */
void TimerPWM_DutyCycle(TIMER_handler_t* timer_handle, Timer_channel_t Channel, uint8_t DutyCycle) {
	uint32_t arr;
	uint32_t ccr;
	arr = timer_handle->pTimer->TIM_ARR;
	
	if (DutyCycle == 0) {
		ccr = 0;
	} else if (DutyCycle >= 100) {
		ccr = arr;
	} else {
		ccr = ((arr + 1) * DutyCycle) / 100;
	}
	
	volatile uint32_t *ccr_reg = &timer_handle->pTimer->TIM_CCR1 + (Channel - 1);
	*ccr_reg = ccr;
}

/** @} */ // end of TIMER_FUNCTIONS
