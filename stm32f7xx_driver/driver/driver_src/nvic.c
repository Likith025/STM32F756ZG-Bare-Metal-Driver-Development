/**
 * @file nvic.c
 * @author Likith025
 * @date March 22, 2026
 * @brief Nested Vectored Interrupt Controller (NVIC) driver for STM32F756ZG
 *
 * @details
 * This driver provides low-level interrupt management functions for the ARM Cortex-M7
 * Nested Vectored Interrupt Controller (NVIC). It handles:
 * - Interrupt enable/disable
 * - Priority configuration
 * - IRQ management for all STM32F756ZG peripherals
 *
 * @copyright 2026 Likith025 - MIT License
 * @see stm32f756zg_reg.h - NVIC register definitions
 */

#include<stdint.h>
#include "nvic.h"

/**
 * @defgroup NVIC_FUNCTIONS NVIC Driver Functions
 * @{
 */

/**
 * @brief Configure interrupt priority and enable/disable
 * @param[in] IRQNumber IRQ number (0-239 for STM32F756ZG peripherals)
 * @param[in] IRQPriority Priority level (0-15, where 0 is highest priority)
 * @param[in] enable Enable or disable the interrupt (ENABLE/DISABLE)
 * @retval void
 *
 * @details
 * Wrapper function that configures both the priority and enable state of an interrupt.
 * This is the typical entry point for interrupt configuration in applications.
 *
 * **Priority Values:**
 * - 0: Highest priority (executed immediately)
 * - 1-14: Intermediate priorities
 * - 15: Lowest priority (executed after higher priority interrupts)
 *
 * @param[in] IRQNumber Valid IRQ numbers for STM32F756ZG:
 *   - 0-15: System exceptions (handled by Cortex-M7)
 *   - 16+: Peripheral interrupts (USART, GPIO, TIMER, etc.)
 *   - Examples:
 *     - IRQ_NO_USART2 = 38
 *     - IRQ_NO_USART3 = 39
 *     - IRQ_NO_EXTI10_15 = 40
 *
 * @param[in] IRQPriority Priority level assignment:
 *   | Value | Priority Level |
 *   |-------|----------------|
 *   | 0 | Highest (preempts all) |
 *   | 5 | Mid-range (typical for most peripherals) |
 *   | 15 | Lowest (preempted by all) |
 *
 * @param[in] enable Interrupt state:
 *   - ENABLE (1): Enable the interrupt in NVIC
 *   - DISABLE (0): Disable the interrupt in NVIC
 *
 * @note
 * - This is the preferred high-level interface for interrupt configuration
 * - Both priority and enable state are configured atomically
 * - Typical priority assignments:
 *   - Time-critical interrupts: Priority 0-5
 *   - General peripherals: Priority 5-10
 *   - Background tasks: Priority 10-15
 *
 * @example
 * // Configure USART3 interrupt (priority 5)
 * IntrruptConfig(IRQ_NO_USART3, 5, ENABLE);
 *
 * // Set high-priority GPIO interrupt
 * IntrruptConfig(IRQ_NO_EXTI0, 0, ENABLE);  // Highest priority
 *
 * // Disable EXTI interrupt temporarily
 * IntrruptConfig(IRQ_NO_EXTI10_15, 10, DISABLE);
 *
 * @see IRQ_PriorityConfig(), IRQ_IntrruptConfig()
 */
void IntrruptConfig(uint8_t IRQNumber, uint32_t IRQPriority, uint8_t enable) {
	IRQ_PriorityConfig(IRQNumber, IRQPriority);
	IRQ_IntrruptConfig(IRQNumber, enable);
}

/**
 * @brief Enable or disable an interrupt in NVIC
 * @param[in] IRQ_Number IRQ number to enable/disable (0-239)
 * @param[in] status Enable or disable state (ENABLE/DISABLE)
 * @retval void
 *
 * @details
 * Low-level function to set or clear the enable bit for an interrupt in the NVIC.
 * - Setting enable bit in ISER (Interrupt Set Enable Register) enables the interrupt
 * - Setting enable bit in ICER (Interrupt Clear Enable Register) disables the interrupt
 *
 * **NVIC Register Architecture:**
 * ```
 * ISER (Interrupt Set Enable Registers):
 *   ISER[0]: Bits 31-0 for IRQs 0-31
 *   ISER[1]: Bits 63-32 for IRQs 32-63
 *   ...
 *   ISER[7]: Bits 239-224 for IRQs 224-239
 *
 * ICER (Interrupt Clear Enable Registers): Identical structure
 * ```
 *
 * **Register Calculation:**
 * - ISER/ICER index = IRQNumber / 32 (which register)
 * - Bit position = IRQNumber % 32 (which bit in register)
 *
 * @param[in] IRQ_Number IRQ number (0-239 for STM32F756ZG):
 *   - Valid range: 0 to 239
 *   - Common peripheral IRQs:
 *     - USART2: 38, USART3: 39, USART4: 52, USART5: 53
 *     - GPIO/EXTI: 6-10, 23, 40
 *     - TIMER: 28-30, 44-50
 *
 * @param[in] status Interrupt state control:
 *   - ENABLE (non-zero): Write to ISER (set enable bit)
 *   - DISABLE (0): Write to ICER (clear enable bit)
 *
 * @note
 * - Operation is atomic (register write is atomic operation)
 * - Multiple interrupts can be enabled/disabled without conflicts
 * - Enabling already-enabled interrupt has no effect
 * - Disabling already-disabled interrupt has no effect
 * - No return value indicates success/failure
 *
 * @warning
 * - IRQ_Number > 239 causes invalid register access
 * - Enabling interrupt without proper handler causes Hard Fault
 * - Disabling interrupt during its execution may cause lockup
 * - Interrupt flag must be cleared at peripheral before re-enabling
 *
 * @example
 * // Enable USART3 interrupt
 * IRQ_IntrruptConfig(IRQ_NO_USART3, ENABLE);
 *
 * // Disable EXTI interrupt
 * IRQ_IntrruptConfig(IRQ_NO_EXTI10_15, DISABLE);
 *
 * // Example showing register calculation for IRQ 39 (USART3):
 * // ISER index = 39 / 32 = 1 (use ISER[1])
 * // Bit position = 39 % 32 = 7 (set bit 7 in ISER[1])
 * NVIC->ISER[1] |= (1UL << 7);  // Same as IRQ_IntrruptConfig(39, ENABLE)
 *
 * @see IntrruptConfig(), IRQ_PriorityConfig()
 */
void IRQ_IntrruptConfig(uint8_t IRQ_Number, uint8_t status) {
	if (status == ENABLE) {
		// Enable: Write 1 to ISER register
		NVIC->ISER[IRQ_Number / 32] |= (1UL << (IRQ_Number % 32));
	} else {
		// Disable: Write 1 to ICER register
		NVIC->ICER[IRQ_Number / 32] |= (1UL << (IRQ_Number % 32));
	}
}

/**
 * @brief Set the priority level for an interrupt
 * @param[in] IRQ_Number IRQ number (0-239)
 * @param[in] IRQ_Priority Priority level (0-15, or 0-255 if custom grouping used)
 * @retval void
 *
 * @details
 * Configures the priority level for a given interrupt. Priority levels control
 * interrupt preemption behavior:
 * - Lower priority values preempt higher priority values
 * - Interrupts with same priority use sub-priority for ordering (if configured)
 * - Priority 0 is highest (preempts all others)
 * - Priority 15 is lowest (preempted by all others)
 *
 * **NVIC Priority Register Architecture:**
 * ```
 * IPR (Interrupt Priority Registers):
 *   Each register holds 4 IRQ priorities (8 bits per priority)
 *   IPR[0] contains priorities for IRQs 0-3
 *   IPR[1] contains priorities for IRQs 4-7
 *   ...
 *   IPR[59] contains priorities for IRQs 236-239
 *
 * For STM32F756ZG (4-bit priority):
 *   - Only upper 4 bits of 8-bit field are used (bits 7-4)
 *   - Lower 4 bits (3-0) are always 0
 *   - This allows for 16 priority levels (0-15)
 * ```
 *
 * **Priority Field Calculation:**
 * - IPR Index = IRQ_Number / 4 (which register)
 * - Section = IRQ_Number % 4 (which byte in register, 0-3)
 * - Bit Shift = (section × 8) + 4 (shift for upper 4 bits)
 *
 * **Example for IRQ 39 (USART3):**
 * ```
 * IPR Index = 39 / 4 = 9 (use IPR[9])
 * Section = 39 % 4 = 3 (use byte 3 of IPR[9])
 * Bit Shift = (3 × 8) + 4 = 28 (bits 31-28)
 * ```
 *
 * @param[in] IRQ_Number IRQ number (0-239)
 *   - Must be valid; invalid numbers cause undefined behavior
 *   - Examples:
 *     - USART2: 38, USART3: 39
 *     - EXTI10_15: 40
 *     - GPIO interrupts: 6-10, 23
 *
 * @param[in] IRQ_Priority Priority level to assign:
 *   - Value 0: Highest priority (preempts all)
 *   - Value 1-14: Intermediate priorities
 *   - Value 15: Lowest priority (preempted by all)
 *   - Recommended values:
 *     - 0-3: Time-critical operations
 *     - 5-7: High-priority peripherals
 *     - 10-12: General-purpose peripherals
 *     - 13-15: Low-priority background tasks
 *
 * @note
 * - Function is atomic (single register operation)
 * - All 4 priority bits are cleared before writing new value
 * - Used internally by IntrruptConfig() wrapper
 * - Priority change takes effect immediately
 * - Changing priority of running interrupt is generally safe
 *
 * @example
 * // Set USART3 interrupt to priority 5
 * IRQ_PriorityConfig(IRQ_NO_USART3, 5);
 *
 * // Set high-priority GPIO interrupt
 * IRQ_PriorityConfig(IRQ_NO_EXTI0, 0);  // Highest priority
 *
 * // Set low-priority background task
 * IRQ_PriorityConfig(IRQ_NO_TIMER2, 15); // Lowest priority
 *
 * // Detailed register operation for IRQ 39 (USART3):
 * uint8_t ipr_index = 39 / 4;        // = 9
 * uint8_t ipr_section = 39 % 4;      // = 3
 * uint8_t shift = (3 * 8) + 4;       // = 28
 * NVIC->IPR[9] &= ~((uint32_t)0xF << 28);  // Clear bits 31-28
 * NVIC->IPR[9] |= ((uint32_t)5 << 28);     // Set priority to 5
 *
 * @see IntrruptConfig(), IRQ_IntrruptConfig()
 */
void IRQ_PriorityConfig(uint8_t IRQ_Number, uint32_t IRQ_Priority) {
	uint8_t ipr_index = IRQ_Number / 4;
	uint8_t ipr_section = IRQ_Number % 4;

	// Calculate bit shift position (upper 4 bits only)
	uint8_t shift = (ipr_section * 8) + 4;

	// Clear and set priority bits
	NVIC->IPR[ipr_index] &= ~((uint32_t)0xF << shift);
	NVIC->IPR[ipr_index] |= ((uint32_t)IRQ_Priority << shift);
}

/** @} */ // end of NVIC_FUNCTIONS
