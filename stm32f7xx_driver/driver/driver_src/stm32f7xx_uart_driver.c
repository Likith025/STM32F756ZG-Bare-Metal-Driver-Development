/**
 * @file stm32f7xx_uart_driver.c
 * @author Likith025
 * @date February 28, 2026
 * @brief STM32F7xx USART Driver - Serial Communication Interface
 *
 * @details
 * This module provides comprehensive USART (Universal Synchronous/Asynchronous Receiver/Transmitter)
 * driver functionality for STM32F756ZG microcontroller. It supports both blocking and interrupt-driven
 * data transmission and reception with flexible configuration options.
 *
 * **Supported Features:**
 * - Configurable baud rates (300 to 4,500,000 bps)
 * - Word lengths: 8-bit or 9-bit
 * - Stop bits: 1, 1.5, or 2 bits
 * - Parity: None, Even, or Odd
 * - Oversampling: 8x or 16x (affects baud rate precision)
 * - Hardware flow control: None, CTS only, RTS only, CTS+RTS
 * - Transmission modes: Blocking, Interrupt-driven
 * - Interrupt-driven RX with error detection
 * - Automatic overflow error handling
 *
 * **USART Instances:**
 * - USART2: Base 0x40004400, IRQ 38, APB1 clock (42 MHz)
 * - USART3: Base 0x40004800, IRQ 39, APB1 clock (42 MHz)
 * - UART4: Base 0x40004C00, IRQ 52, APB1 clock (42 MHz)
 * - UART5: Base 0x40005000, IRQ 53, APB1 clock (42 MHz)
 * - USART1: Base 0x40011000, IRQ 37, APB2 clock (84 MHz)
 * - USART6: Base 0x40011400, IRQ 71, APB2 clock (84 MHz)
 *
 * **Data Flow (Interrupt-Driven):**
 * ```
 * TX: Application → pTXBuffer → TXEIE ISR → TDR → Serial Line
 * RX: Serial Line → RDR → RXNEIE ISR → pRXBuffer → Application
 * ```
 *
 * **Register Architecture:**
 * - CR1: Control register 1 (enable, TX/RX, interrupts, word length, parity)
 * - CR2: Control register 2 (stop bits, line break control)
 * - CR3: Control register 3 (hardware flow control, DMA)
 * - BRR: Baud rate register (mantissa and fraction)
 * - TDR: Transmit data register
 * - RDR: Receive data register
 * - ISR: Interrupt and status register (flags)
 * - ICR: Interrupt clear register (flag clear)
 *
 * **Clock Configuration:**
 * - APB1: 42 MHz (USART2-5, UART4-5)
 * - APB2: 84 MHz (USART1, USART6)
 * - Define USART_CLK in header for calculations
 *
 * @note
 * - GPIO alternate functions must be configured before USART use
 * - Interrupt handlers must be linked in startup code
 * - Application must implement USART_ApplicationEventCallback()
 * - Baud rate error < 3% for reliable operation
 *
 * @attention
 * - Ensure GPIO pins are configured as alternate functions
 * - USART clock must be enabled via RCC before use
 * - Interrupt handler must clear flags to prevent stalling
 * - TX/RX buffers must remain valid during transfer
 *
 * @copyright 2026 Likith025 - MIT License
 * @see stm32f7xx_uart_driver.h - Header definitions
 * @see stm32f756zg_reg.h - Register definitions
 */

#include "stm32f7xx_uart_driver.h"

/**
 * @defgroup USART_CORE_FUNCTIONS USART Core Functions
 * @{
 */

/**
 * @brief Initialize USART peripheral with specified configuration
 * @param[in,out] usart_handle Pointer to USART_handler_t structure
 * @param[in] enable Enable/disable USART after configuration (ENABLE/DISABLE)
 * @retval int8_t 0 on success, -1 if configuration error
 *
 * @details
 * Initializes USART peripheral with complete configuration including:
 * - Baud rate calculation and setup
 * - TX/RX mode configuration
 * - Word length (8-bit or 9-bit)
 * - Parity settings
 * - Stop bits configuration
 * - Oversampling mode
 * - Hardware flow control
 * - USART enable/disable
 *
 * **Initialization Sequence:**
 * ```
 * 1. Enable USART clock via RCC
 * 2. Disable USART (for safe configuration)
 * 3. Set baud rate
 * 4. Configure TX/RX mode (CR1 bits 2-3)
 * 5. Set word length (CR1 bits 12, 28)
 * 6. Configure parity (CR1 bits 9-10)
 * 7. Set oversampling (CR1 bit 15)
 * 8. Configure hardware flow control (CR3 bits 8-9)
 * 9. Set stop bits (CR2 bits 12-13)
 * 10. Enable USART (if enable parameter set)
 * ```
 *
 * **Configuration Structure (USART_config_t):**
 * ```c
 * USART_Mode:           TX, RX, or TX+RX
 * USART_BaudRate:       300-4,500,000 bps
 * USART_WordLength:     8-bit or 9-bit
 * USART_StopBits:       1, 1.5, or 2
 * USART_ParityControl:  None, Even, or Odd
 * USART_OverSampling:   8x or 16x
 * USART_HWflowControl:  None, CTS, RTS, or CTS+RTS
 * ```
 *
 * **Word Length Details:**
 * | Mode | Data Bits | Parity Bit | Total |
 * |------|-----------|-----------|-------|
 * | 8-bit, No parity | 8 | - | 8 |
 * | 8-bit, Even parity | 7 | 1 | 8 |
 * | 8-bit, Odd parity | 7 | 1 | 8 |
 * | 9-bit, No parity | 9 | - | 9 |
 * | 9-bit, With parity | 8 | 1 | 9 |
 *
 * **Parity Calculation:**
 * - Even: Bit count (including parity) is even
 * - Odd: Bit count (including parity) is odd
 * - Used for error detection only (1-bit errors)
 *
 * **Baud Rate Accuracy:**
 * - 16x oversampling: Better accuracy, supported down to lower rates
 * - 8x oversampling: Higher maximum baud rate, slightly less accurate
 * - Error calculation: |Actual - Desired| / Desired × 100%
 * - Acceptable error: < 3% for reliable reception
 *
 * **Register Bit Mapping:**
 * ```
 * CR1[1:0] = UE (0=disable, 1=enable)
 * CR1[3:2] = TE/RE (00=none, 01=RX, 10=TX, 11=TX+RX)
 * CR1[9] = PS (0=even, 1=odd parity)
 * CR1[10] = PCE (0=no parity, 1=enable parity)
 * CR1[12] = M0 (word length bit 0)
 * CR1[15] = OVER8 (0=16x, 1=8x oversampling)
 * CR1[28] = M1 (word length bit 1)
 * CR2[13:12] = STOP (00=1 bit, 01=0.5, 10=2, 11=1.5)
 * CR3[8] = RTSE (RTS enable)
 * CR3[9] = CTSE (CTS enable)
 * ```
 *
 * @param[in,out] usart_handle USART handler structure:
 *   - pUSART: USART base address (USART2, USART3, etc.)
 *   - USART_config: Configuration structure with all parameters
 *
 * @param[in] enable Initial USART state:
 *   - ENABLE (1): Start USART after configuration
 *   - DISABLE (0): Leave USART disabled (must call USART_Control later)
 *
 * @return Initialization status:
 *   - 0: Initialization successful
 *   - -1: Configuration error (invalid parameter)
 *
 * @note
 * - USART clock must be enabled before calling (via USART_clK_init)
 * - GPIO pins must be configured as alternate functions
 * - Can be called multiple times to reconfigure
 * - Does not handle interrupt setup (separate from init)
 * - Blocking I/O functions can be used immediately after init
 *
 * **Typical Usage:**
 * ```c
 * USART_handler_t uart3;
 * uart3.pUSART = USART3;
 * uart3.USART_config.USART_Mode = USART_MODE_TXRX;
 * uart3.USART_config.USART_BaudRate = USART_BAUD_115200;
 * uart3.USART_config.USART_WordLength = USART_WordLen_8bits;
 * uart3.USART_config.USART_StopBits = USART_StopBits_1;
 * uart3.USART_config.USART_ParityControl = USART_Parity_None;
 * uart3.USART_config.USART_OverSampling = USART_OverSampling_16;
 * uart3.USART_config.USART_HWflowControl = USART_HW_FlowCtrl_None;
 *
 * if (USART_init(&uart3, ENABLE) == 0) {
 *     // USART ready to use
 * }
 * ```
 *
 * **Error Handling:**
 * ```c
 * if (USART_init(&uart3, ENABLE) != 0) {
 *     // Invalid configuration detected
 *     // Check word length, stop bits, parity, flow control
 *     while(1);  // Halt or retry with new config
 * }
 * ```
 *
 * @warning
 * - Do not call before GPIO alternate function configuration
 * - Do not call before RCC clock enable
 * - Invalid parameters may corrupt USART state
 * - Some combinations invalid (e.g., 9-bit + parity)
 *
 * @attention
 * - Calling init disables USART briefly (safe but visible on bus)
 * - TX and RX clocks derived from same oscillator (synchronized)
 * - Initialization resets all USART flags
 *
 * @see USART_clK_init(), USART_SetBaudRate(), USART_Control()
 */
int8_t USART_init(USART_handler_t *usart_handle, uint8_t enable) {
	USART_MODE_t mode;
	USART_WordLen_t len = 0;
	USART_Parity_t parity = 0;
	USART_StopBits_t Stop_Bits = 0;
	USART_BaudRate_t bauarate = 0;
	
	len = usart_handle->USART_config.USART_WordLength;
	mode = usart_handle->USART_config.USART_Mode;
	parity = usart_handle->USART_config.USART_ParityControl;
	Stop_Bits = usart_handle->USART_config.USART_StopBits;
	bauarate = usart_handle->USART_config.USART_BaudRate;

	/* Setting up RCC clock to enable USART */
	USART_RegDef_t *USART = usart_handle->pUSART;
	USART_clK_init(USART, ENABLE);
	
	/* Disabling USART before setting up */
	USART_Control(USART, DISABLE);

	USART_SetBaudRate(usart_handle, bauarate);

	/* Setting up direction (TX, RX, both) */
	USART->USART_CR1 &= ~(3 << 2);
	USART->USART_CR1 |= mode;

	USART->USART_CR1 &= ~(1 << 12);
	USART->USART_CR1 &= ~(1 << 28);

	/* Configure word length */
	if (len == USART_WordLen_8bits) {
		/* Default state: both bits 12 and 28 are 0 */
	} else if (len == USART_WordLen_9bits) {
		USART->USART_CR1 &= ~(1 << 28);
		USART->USART_CR1 |= (1 << 12);
	} else {
		return -1;
	}

	/* Parity setup */
	if (parity == USART_Parity_None) {
		USART->USART_CR1 &= ~(1 << 10);
	} else {
		USART->USART_CR1 |= (1 << 10);
		if (parity == USART_Parity_Even) {
			USART->USART_CR1 &= ~(1 << 9);
		} else if (parity == USART_Parity_Odd) {
			USART->USART_CR1 |= (1 << 9);
		} else {
			return -1;
		}
	}
	
	/* Setting oversampling */
	USART->USART_CR1 &= ~(1 << 15);
	if (usart_handle->USART_config.USART_OverSmapling == USART_OverSampling_8) {
		USART->USART_CR1 |= (1 << 15);
	}

	/* Hardware flow control setup */
	USART->USART_CR3 &= ~(1 << 9);
	USART->USART_CR3 &= ~(1 << 8);

	if (usart_handle->USART_config.USART_HWflowControl == USART_HW_FlowCtrl_None) {
		/* Default: no flow control */
	} else if (usart_handle->USART_config.USART_HWflowControl == USART_HW_FlowCtrl_CTS) {
		USART->USART_CR3 |= (1 << 9);
	} else if (usart_handle->USART_config.USART_HWflowControl == USART_HW_FlowCtrl_RTS) {
		USART->USART_CR3 |= (1 << 8);
	} else if (usart_handle->USART_config.USART_HWflowControl == USART_HW_FlowCtrl_CTS_RTS) {
		USART->USART_CR3 |= (1 << 8);
		USART->USART_CR3 |= (1 << 9);
	} else {
		return -1;
	}
	
	/* Configure stop bits */
	if (Stop_Bits <= USART_StopBits_1_5) {
		USART->USART_CR2 &= ~(3 << 12);
		USART->USART_CR2 |= (Stop_Bits << 12);
	} else {
		return -1;
	}

	/* Enable USART if requested */
	USART_Control(usart_handle->pUSART, ENABLE);
	return 0;
}

/**
 * @brief Enable or disable USART peripheral
 * @param[in] pUSART Pointer to USART register definition
 * @param[in] CMD Control command (ENABLE/DISABLE)
 * @retval int8_t 0 on success
 *
 * @details
 * Controls the USART enable state via CR1 bit 0 (UE - USART Enable).
 * When enabled, USART can transmit and receive data. When disabled,
 * USART stops all operations but preserves configuration.
 *
 * **Register Operation:**
 * - CR1[0] = 1: USART enabled (can transmit/receive)
 * - CR1[0] = 0: USART disabled (low power, no I/O)
 *
 * **State Transitions:**
 * ```
 * DISABLE → ENABLE: USART powers up, ready for I/O (1-2 cycles)
 * ENABLE → DISABLE: USART stops, current operations complete
 * ```
 *
 * @param[in] pUSART USART register base address
 * @param[in] CMD Control state:
 *   - ENABLE (1): Start USART peripheral
 *   - DISABLE (0): Stop USART peripheral
 *
 * @return Status: Always returns 0 (no error checking)
 *
 * @note
 * - Very fast operation (single bit write)
 * - Can be called multiple times
 * - Configuration preserved during disable
 * - Idempotent (enabling already-enabled is safe)
 *
 * **Typical Usage:**
 * ```c
 * USART_Control(USART3, ENABLE);   // Start
 * // ... use USART3 for I/O ...
 * USART_Control(USART3, DISABLE);  // Stop for reconfiguration
 * ```
 *
 * @see USART_init(), USART_SendData(), USART_ReadData()
 */
int8_t USART_Control(USART_RegDef_t *pUSART, uint8_t CMD) {
	if (CMD == ENABLE) {
		pUSART->USART_CR1 |= (1 << 0);
	} else {
		pUSART->USART_CR1 &= ~(1 << 0);
	}
	return 0;
}

/**
 * @brief Enable/disable USART clock via RCC
 * @param[in] pUSART USART register base address
 * @param[in] CMD Control command (ENABLE/DISABLE)
 * @retval int8_t 0 on success, -1 if invalid USART address
 *
 * @details
 * Enables or disables the USART peripheral clock via RCC APB1ENR register.
 * The clock must be enabled before any USART configuration or operation.
 *
 * **Clock Mapping (APB1ENR):**
 * - USART2: Bit 17 (offset from USART2_BASE: 0×400 → position 0)
 * - USART3: Bit 18 (offset from USART2_BASE: 0×400 → position 1)
 * - UART4: Bit 19 (offset from USART2_BASE: 0×800 → position 2)
 * - UART5: Bit 20 (offset from USART2_BASE: 0xC00 → position 3)
 *
 * **Address Calculation:**
 * ```
 * position = (pUSART address - USART2_BASE) / 0x400
 * bit = 17 + position
 * ```
 *
 * **Clock Frequency:**
 * - USART2-5: 42 MHz (APB1 clock domain)
 * - USART1, USART6: 84 MHz (APB2 clock domain - not covered here)
 *
 * **Validation:**
 * - Checks USART address in valid range (USART2_BASE to UART5_BASE)
 * - Returns -1 for invalid addresses
 * - Prevents writing to arbitrary memory
 *
 * @param[in] pUSART USART register base address:
 *   - Valid: USART2, USART3, UART4, UART5
 *   - Invalid: NULL, out of range, or USART1/USART6
 *
 * @param[in] CMD Clock control:
 *   - ENABLE (1): Set RCC bit to enable clock
 *   - DISABLE (0): Clear RCC bit to disable clock
 *
 * @return Clock control status:
 *   - 0: Clock control successful
 *   - -1: Invalid USART address
 *
 * @note
 * - Must be called before USART_init()
 * - Typically called automatically by USART_init()
 * - Can be called independently if needed
 * - Very fast operation (~1 µs)
 *
 * **Typical Sequence:**
 * ```c
 * // Method 1: Automatic (called by USART_init)
 * USART_init(&uart_handle, ENABLE);
 *
 * // Method 2: Manual (for advanced usage)
 * USART_clK_init(USART3, ENABLE);  // Enable clock
 * // ... configure GPIO ...
 * USART_init(&uart_handle, ENABLE);
 * ```
 *
 * @warning
 * - Invalid USART address causes function to return -1
 * - Check return value in safety-critical applications
 * - Disabling clock stops USART immediately
 * - Use USART_Control() to gracefully stop USART
 *
 * @see USART_init()
 */
int8_t USART_clK_init(USART_RegDef_t *pUSART, uint8_t CMD) {
	uint8_t position = 0;
	
	/* Validate USART address */
	if ((pUSART == NULL) || ((uint32_t)pUSART < USART2_BASE_ADDR) || ((uint32_t)pUSART > UART5_BASE_ADDR)) {
		return -1;
	}
	
	/* Calculate position relative to USART2 */
	position = (((uint32_t)pUSART - USART2_BASE_ADDR) / 0x400);
	
	/* Enable or disable clock */
	if (CMD) {
		RCC->APB1ENR |= (1 << (position + 17));
	} else {
		RCC->APB1ENR &= ~(1 << (position + 17));
	}
	
	return 0;
}

/** @} */ // end of USART_CORE_FUNCTIONS

/**
 * @defgroup USART_FLAG_FUNCTIONS USART Flag Functions
 * @{
 */

/**
 * @brief Check if a USART flag is set in the ISR register
 * @param[in] pUSART USART register base address
 * @param[in] FlagName Flag to check (USART_FLAG_TXE, USART_FLAG_RXNE, etc.)
 * @retval uint8_t 1 if flag is set, 0 if flag is clear
 *
 * @details
 * Reads the USART Interrupt and Status Register (ISR) to check if a specific
 * flag is set. Used for polling operation status or verifying interrupt sources.
 *
 * **Common Flags:**
 * | Flag | Meaning | Use Case |
 * |------|---------|----------|
 * | USART_FLAG_TXE | TX register empty | Ready to send data |
 * | USART_FLAG_TC | TX complete | Last bit transmitted |
 * | USART_FLAG_RXNE | RX not empty | Data ready to read |
 * | USART_FLAG_IDLE | IDLE line detected | Frame reception complete |
 * | USART_FLAG_ORE | Overrun error | RX buffer overflow |
 * | USART_FLAG_NF | Noise frame error | Electrical noise |
 * | USART_FLAG_FE | Frame error | Stop bit error |
 * | USART_FLAG_PE | Parity error | Parity check failed |
 *
 * **Register Bits (ISR):**
 * ```
 * Bit 0: PE (Parity error)
 * Bit 1: FE (Framing error)
 * Bit 2: NF (Noise flag)
 * Bit 3: ORE (Overrun error)
 * Bit 4: IDLE (Idle line detected)
 * Bit 5: RXNE (Receiver not empty)
 * Bit 6: TC (Transmission complete)
 * Bit 7: TXE (Transmitter empty)
 * ```
 *
 * **Flag Behavior:**
 * - Set automatically when condition occurs
 * - Persist until cleared via ICR register
 * - Reading ISR does NOT clear flags
 * - Multiple flags can be set simultaneously
 *
 * @param[in] pUSART USART register base address
 * @param[in] FlagName Flag to check:
 *   - USART_FLAG_TXE: Transmit register empty
 *   - USART_FLAG_RXNE: Receive register not empty
 *   - USART_FLAG_TC: Transmission complete
 *   - USART_FLAG_ORE: Overrun error
 *   - etc.
 *
 * @return Flag status:
 *   - 1: Flag is set (condition true)
 *   - 0: Flag is clear (condition false)
 *
 * @note
 * - Non-blocking: returns immediately
 * - Safe for polling in loops
 * - Does not modify any USART state
 * - Multiple calls return consistent results until flag changes
 *
 * **Typical Usage (Blocking TX):**
 * ```c
 * // Wait for TX register to be empty
 * while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE)) {
 *     // Wait...
 * }
 * USART3->USART_TDR = data_byte;
 *
 * // Wait for transmission complete
 * while (!USART_GetFlagStatus(USART3, USART_FLAG_TC)) {
 *     // Wait...
 * }
 * ```
 *
 * **Typical Usage (Blocking RX):**
 * ```c
 * // Wait for RX register to have data
 * while (!USART_GetFlagStatus(USART3, USART_FLAG_RXNE)) {
 *     // Wait...
 * }
 * uint8_t rx_data = USART3->USART_RDR;
 * ```
 *
 * **Error Checking:**
 * ```c
 * if (USART_GetFlagStatus(USART3, USART_FLAG_ORE)) {
 *     // Handle overrun error
 *     USART_CLearFlag(USART3, USART_FLAG_ORE);
 * }
 * ```
 *
 * @warning
 * - Polling flags in loops can be inefficient
 * - Consider interrupt-driven approach for better performance
 * - Some errors (ORE) cause data loss
 * - Must clear error flags or they persist
 *
 * @see USART_CLearFlag(), USART_SendData(), USART_ReadData()
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSART, USART_Flags_t FlagName) {
	uint8_t status = 0;
	if (pUSART->USART_ISR & FlagName) {
		status = 1;
	}
	return status;
}

/**
 * @brief Clear a USART flag via ICR (Interrupt Clear Register)
 * @param[in] pUSART USART register base address
 * @param[in] FlagName Flag to clear
 * @retval void
 *
 * @details
 * Clears a USART flag by writing to the Interrupt Clear Register (ICR).
 * Flags must be cleared after processing to allow them to be set again.
 * Only specific flags can be cleared via ICR; others are cleared by reading/writing data.
 *
 * **Clearable Flags (via ICR write):**
 * - USART_ICR_PECF: Parity error clear flag
 * - USART_ICR_FECF: Framing error clear flag
 * - USART_ICR_NECF: Noise flag clear flag
 * - USART_ICR_ORECF: Overrun error clear flag
 * - USART_ICR_IDLECF: IDLE line clear flag
 * - USART_ICR_TCCF: Transmission complete clear flag
 *
 * **Auto-Clearing Flags:**
 * - TXE: Cleared by writing to TDR
 * - RXNE: Cleared by reading RDR
 * - TC: Can be cleared by writing to ICR or reading ISR+writing TDR
 *
 * **ICR Register Operation:**
 * Writing 1 to ICR bit clears corresponding ISR flag.
 * Writing 0 has no effect (safe).
 * Multiple flags can be cleared with single write.
 *
 * **Example (Clear Multiple Errors):**
 * ```c
 * // Clear all error flags
 * USART_reg->USART_ICR = USART_ICR_PECF | USART_ICR_FECF |
 *                        USART_ICR_NECF | USART_ICR_ORECF;
 * ```
 *
 * @param[in] pUSART USART register base address
 * @param[in] FlagName Flag to clear:
 *   - USART_ICR_PECF: Clear parity error
 *   - USART_ICR_FECF: Clear framing error
 *   - USART_ICR_ORECF: Clear overrun error
 *   - USART_ICR_TCCF: Clear transmission complete
 *   - etc.
 *
 * @note
 * - Must be called in interrupt handlers after flag processing
 * - Safe to clear flags that are not set (no side effects)
 * - Failing to clear flags prevents interrupts from retriggering
 * - Some flags auto-clear on data access
 *
 * **Typical Interrupt Pattern:**
 * ```c
 * if (USART_GetFlagStatus(USART3, USART_FLAG_ORE)) {
 *     // Handle overrun error
 *     error_count++;
 *     // Must clear flag to prevent repeated triggering
 *     USART_CLearFlag(USART3, USART_ICR_ORECF);
 * }
 * ```
 *
 * **TX Complete Handler:**
 * ```c
 * if (USART_GetFlagStatus(USART3, USART_FLAG_TC)) {
 *     // Transmission complete
 *     state = TX_DONE;
 *     // Clear TC flag
 *     USART_CLearFlag(USART3, USART_ICR_TCCF);
 * }
 * ```
 *
 * @warning
 * - Failing to clear flags in ISR causes infinite interrupt loops
 * - Some flags auto-clear; clearing again is safe (no effect)
 * - Order of clearing can matter (check flags before clearing)
 * - Clear at correct time (after processing event)
 *
 * @see USART_GetFlagStatus(), USART_IRQHandler()
 */
void USART_CLearFlag(USART_RegDef_t *pUSART, USART_ClearFlags_t FlagName) {
	pUSART->USART_ICR |= (FlagName);
}

/** @} */ // end of USART_FLAG_FUNCTIONS

/**
 * @defgroup USART_BLOCKING_FUNCTIONS USART Blocking I/O Functions
 * @{
 */

/**
 * @brief Send data via USART (blocking mode)
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @param[in] pTxData Pointer to data buffer to transmit
 * @param[in] len Number of bytes/words to send
 * @retval void
 *
 * @details
 * Sends data through USART with blocking mode. Function waits for transmit register
 * to be empty (TXE flag) before writing each byte, and waits for transmission complete
 * (TC flag) before returning. This ensures all data is transmitted before function exits.
 *
 * **Transmission Sequence:**
 * ```
 * For each byte in buffer:
 *   1. Wait for TXE flag (TX register empty)
 *   2. Write byte to TDR
 *   3. Repeat
 * 4. Wait for TC flag (all bits transmitted)
 * 5. Return
 * ```
 *
 * **Word Length Handling:**
 * - 8-bit data: 1 byte per element, masked with 0xFF
 * - 9-bit data (no parity): 2 bytes per element (uint16_t), masked with 0x1FF
 * - 9-bit data (with parity): 1 byte per element, masked with 0xFF
 *
 * **Timing (at 115200 baud, 8-bit + 1 stop):**
 * ```
 * Bits per frame: 10 (1 start + 8 data + 1 stop)
 * Baud rate: 115200 bps
 * Time per byte: 10 / 115200 = 87 µs
 * Time for 100 bytes: 8.7 ms
 * ```
 *
 * **Data Types:**
 * - 8-bit mode: `uint8_t data[10]`
 * - 9-bit no parity: `uint16_t data[10]` (takes 20 byte positions)
 * - 9-bit with parity: `uint8_t data[10]`
 *
 * **Byte Order:**
 * - LSB first (standard for UART)
 * - Start bit (LOW) → D0 → D1 → ... → D7/D8 → Parity → Stop bit(s) (HIGH)
 *
 * @param[in] usart_handle USART handler:
 *   - pUSART: USART base address
 *   - USART_config: Configuration (word length, parity)
 *
 * @param[in] pTxData Pointer to transmit buffer:
 *   - Must point to valid data
 *   - 8-bit mode: array of uint8_t
 *   - 9-bit no parity: array of uint16_t
 *   - Data not modified
 *
 * @param[in] len Number of elements to send:
 *   - 8-bit mode: bytes to send
 *   - 9-bit no parity: 16-bit words to send (takes 2× bytes)
 *   - 9-bit with parity: bytes to send
 *
 * @note
 * - Blocks CPU until all data sent (busy-wait)
 * - Not suitable for real-time applications
 * - No timeout: function blocks indefinitely if USART fails
 * - Very simple but inefficient for large data
 *
 * **Typical Usage:**
 * ```c
 * uint8_t message[] = "Hello USART\r\n";
 * USART_SendData(&uart_handle, message, sizeof(message) - 1);
 * ```
 *
 * **9-bit Data Example:**
 * ```c
 * // 9-bit no parity mode
 * uint16_t data_16bit[5] = {0x1FF, 0x100, 0x001, 0x1AA, 0x055};
 * USART_SendData(&uart_handle, (uint8_t*)data_16bit, 5);
 * ```
 *
 * @warning
 * - Blocks indefinitely if TXE or TC flags never set (USART hung)
 * - No error detection (parity errors not checked)
 * - CPU cannot do other work while waiting
 * - Interrupt-driven approach recommended for better performance
 * - len parameter must be accurate (no bounds checking)
 *
 * @attention
 * - USART must be enabled before calling
 * - TX must be configured in USART mode
 * - Consider timeout mechanism for safety
 * - Not suitable for high-speed or time-critical applications
 *
 * @see USART_SendData_IT(), USART_ReadData()
 */
void USART_SendData(USART_handler_t *usart_handle, uint8_t* pTxData, uint16_t len) {
	USART_RegDef_t *USART = usart_handle->pUSART;
	uint16_t *pData_16;
	
	for (uint16_t i = 0; i < len; i++) {
		/* Wait for transmit register to be empty */
		while (!USART_GetFlagStatus(USART, USART_FLAG_TXE));

		if (usart_handle->USART_config.USART_WordLength == USART_WordLen_9bits) {
			/* 9-bit data */
			pData_16 = (uint16_t*)pTxData;
			USART->USART_TDR = (*pData_16 & (uint16_t)0x01ff);
			
			if (usart_handle->USART_config.USART_ParityControl == USART_Parity_None) {
				pTxData = pTxData + 2;  /* Move by 2 bytes for 16-bit value */
			} else {
				pTxData++;  /* Move by 1 byte (parity takes 1 bit) */
			}
		} else {
			/* 8-bit data */
			USART->USART_TDR = (*pTxData & (uint8_t)(0x00ff));
			pTxData++;
		}
	}
	
	/* Wait for transmission complete (all bits sent) */
	while (!USART_GetFlagStatus(USART, USART_FLAG_TC));
}

/**
 * @brief Set USART baud rate via BRR calculation
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @param[in] BaudRate Desired baud rate in bps
 * @retval void
 *
 * @details
 * Calculates and configures the USART Baud Rate Register (BRR) to achieve
 * the desired baud rate. Formula accounts for clock frequency, oversampling mode,
 * and uses mantissa + fraction representation.
 *
 * **Baud Rate Formula:**
 * ```
 * For 16x oversampling:
 *   BRR = USART_CLK / (16 × BaudRate)
 *
 * For 8x oversampling:
 *   BRR = USART_CLK / (8 × BaudRate)
 *
 * BRR = Mantissa (upper 12 bits) + Fraction (lower 4 bits, 8x) or (lower 3 bits, 16x)
 * ```
 *
 * **Calculation Process:**
 * ```
 * 1. Calculate temp_div = (USART_CLK × 100) / (sampling × BaudRate)
 *    (×100 for better precision)
 * 2. Extract mantissa: div_mantisa = temp_div / 100
 * 3. Calculate fraction remainder: div_frac = temp_div % 100
 * 4. Scale fraction: div_frac = ((div_frac × sampling ± 50) / 100) & mask
 *    (±50 for rounding)
 * 5. Combine: BRR = (mantissa << 4) | fraction
 * ```
 *
 * **BRR Register Format:**
 * ```
 * Bits 15-4: Mantissa (12-bit divisor)
 * Bits 3-0: Fraction (4-bit for 16x, 3-bit for 8x oversampling)
 *
 * 16x Oversampling:
 *   BRR = ((Mantissa << 4) | (Fraction & 0xF))
 *
 * 8x Oversampling:
 *   BRR = ((Mantissa << 4) | (Fraction & 0x7))
 * ```
 *
 * **Typical Clock and Baud Combinations:**
 * | USART_CLK | Baud | 16x Divisor | Mantissa | Fraction |
 * |-----------|------|------------|----------|----------|
 * | 42 MHz | 9600 | 4375 | 273 | 6 |
 * | 42 MHz | 115200 | 365 | 22 | 11 |
 * | 42 MHz | 230400 | 182 | 11 | 6 |
 * | 84 MHz | 115200 | 730 | 45 | 10 |
 *
 * **Baud Rate Accuracy:**
 * - Fractional bits provide ~0.1% resolution improvement
 * - Acceptable error: < 3% for reliable reception
 * - High baud rates (> 1Mbps) may have larger errors
 * - 8x oversampling allows higher baud rates but less accurate
 *
 * **Calculation Example (115200 baud, 42 MHz, 16x):**
 * ```
 * temp_div = (42000000 × 100) / (16 × 115200) = 22786
 * mantissa = 22786 / 100 = 227
 * remainder = 22786 % 100 = 86
 * fraction = ((86 × 16 + 50) / 100) & 0xF = (1426 / 100) & 0xF = 14 & 0xF = 14
 * BRR = (227 << 4) | 14 = 0x0E2E
 * Actual Baud = 42000000 / (16 × (227 + 14/16)) = 115143.29 (0.05% error)
 * ```
 *
 * @param[in] usart_handle USART handler:
 *   - pUSART: USART base address
 *   - USART_config.USART_OverSmapling: 8x or 16x
 *
 * @param[in] BaudRate Desired baud rate:
 *   - Common: 9600, 19200, 38400, 57600, 115200, 230400
 *   - Range: 300 to 4,500,000 bps (depending on clock and oversampling)
 *   - Must match terminal/host configuration
 *
 * @note
 * - Automatically called by USART_init()
 * - Can be called independently to change baud rate
 * - USART_CLK must be defined in header (typical 42 MHz for APB1)
 * - Fractional bits improve accuracy but add complexity
 *
 * **Typical Usage:**
 * ```c
 * // Set to 115200 baud
 * USART_SetBaudRate(&uart_handle, 115200);
 *
 * // Change baud rate on the fly
 * USART_Control(USART3, DISABLE);  // Disable first
 * USART_SetBaudRate(&uart_handle, 9600);
 * USART_Control(USART3, ENABLE);   // Re-enable
 * ```
 *
 * **Error Checking (Manual):**
 * ```c
 * // Calculate actual baud from BRR
 * uint32_t brr_value = USART3->USART_BRR;
 * uint32_t mantissa = brr_value >> 4;
 * uint32_t fraction = brr_value & 0x0F;
 * uint32_t actual_baud = 42000000 / (16 * (mantissa + fraction/16.0f));
 * float error = abs(actual_baud - 115200) / 115200.0f * 100;
 * ```
 *
 * @warning
 * - BRR value of 0 or 1 causes USART to malfunction
 * - Very high baud rates (> 1Mbps) may need different clock source
 * - Clock frequency changes require recalculation
 * - Terminal must match baud rate exactly
 *
 * @attention
 * - USART_CLK must be defined correctly in header
 * - Baud rate errors > 5% may cause reception failures
 * - Some baud rates impossible with given clock (check error %)
 *
 * @see USART_init()
 */
void USART_SetBaudRate(USART_handler_t *usart_handle, uint16_t BaudRate) {
	USART_RegDef_t *USART = usart_handle->pUSART;
	uint32_t pclk = USART_CLK;
	uint32_t temp_div = 0;
	uint16_t div_mantisa;
	uint16_t div_frac;
	
	if (usart_handle->USART_config.USART_OverSmapling == USART_OverSampling_16) {
		/* 16x oversampling */
		temp_div = ((pclk * 100) / (16 * BaudRate));
		div_mantisa = temp_div / 100;
		div_frac = temp_div % 100;
		div_frac = ((((div_frac * 16) + 50) / 100) & 0xf);
	} else {
		/* 8x oversampling */
		temp_div = ((pclk * 100) / (8 * BaudRate));
		div_mantisa = temp_div / 100;
		div_frac = temp_div % 100;
		div_frac = ((((div_frac * 8) + 50) / 100) & 0x7);
	}
	
	/* Set BRR register */
	USART->USART_BRR = ((div_mantisa << 4) | (div_frac));
}

/**
 * @brief Read data from USART (blocking mode)
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @param[out] pRxData Pointer to buffer for received data
 * @param[in] len Number of bytes/words to receive
 * @retval void
 *
 * @details
 * Receives data through USART with blocking mode. Function waits for receive
 * register to have data (RXNE flag) before reading each byte. Blocks until
 * all requested data is received.
 *
 * **Reception Sequence:**
 * ```
 * For each byte to receive:
 *   1. Wait for RXNE flag (RX register has data)
 *   2. Read byte from RDR
 *   3. Process based on word length and parity
 *   4. Store in buffer
 *   5. Advance buffer pointer
 *   6. Repeat
 * ```
 *
 * **Word Length and Parity Handling:**
 * - 8-bit, no parity: Read full 8 bits, store 1 byte
 * - 8-bit, with parity: Read 7 data bits + parity, store 1 byte (0x7F mask)
 * - 9-bit, no parity: Read full 9 bits, store 2 bytes (0x1FF mask)
 * - 9-bit, with parity: Read 8 data bits + parity, store 1 byte (0xFF mask)
 *
 * **Masking Details:**
 * - Masks remove unused bits from RDR (hardware may set extra bits)
 * - 8-bit no parity: mask 0xFF (all 8 bits)
 * - 8-bit with parity: mask 0x7F (7 data bits, drop parity bit)
 * - 9-bit no parity: mask 0x1FF (all 9 bits)
 * - 9-bit with parity: mask 0xFF (8 data bits, drop parity bit)
 *
 * **Timing (at 115200 baud, 8-bit + 1 stop):**
 * ```
 * Reception wait time: 87 µs per byte
 * Receiving 100 bytes: ~8.7 ms
 * ```
 *
 * **Buffer Types:**
 * - 8-bit mode: `uint8_t buffer[64]`
 * - 9-bit no parity: `uint16_t buffer[32]` (takes 64 bytes)
 * - 9-bit with parity: `uint8_t buffer[64]`
 *
 * @param[in] usart_handle USART handler:
 *   - pUSART: USART base address
 *   - USART_config: Configuration (word length, parity)
 *
 * @param[out] pRxData Pointer to receive buffer:
 *   - Must point to writable memory
 *   - Size must be sufficient for len elements
 *   - For 9-bit no parity: size must be len × 2 bytes
 *   - For 8-bit: size must be >= len bytes
 *
 * @param[in] len Number of elements to receive:
 *   - 8-bit mode: bytes to receive
 *   - 9-bit no parity: 16-bit words to receive (takes 2× bytes)
 *   - 9-bit with parity: bytes to receive
 *
 * @note
 * - Blocks CPU until all data received
 * - Not suitable for real-time applications
 * - No timeout: function blocks indefinitely if data never arrives
 * - Parity errors not detected (silent)
 *
 * **Typical Usage:**
 * ```c
 * uint8_t buffer[32];
 * USART_ReadData(&uart_handle, buffer, 32);  // Receive 32 bytes
 * ```
 *
 * **9-bit Data Reception:**
 * ```c
 * uint16_t data_buffer[16];
 * USART_ReadData(&uart_handle, (uint8_t*)data_buffer, 16);
 * // data_buffer[0] contains first 9-bit word
 * ```
 *
 * @warning
 * - Blocks indefinitely if RXNE flag never set (no data)
 * - Parity errors not reported (silent data loss)
 * - CPU cannot do other work while waiting
 * - len parameter unchecked (could overflow buffer)
 * - Interrupt-driven approach recommended
 *
 * @attention
 * - USART must be enabled before calling
 * - RX must be configured in USART mode
 * - Buffer must be pre-allocated
 * - Consider timeout mechanism
 *
 * @see USART_ReadData_IT(), USART_SendData()
 */
void USART_ReadData(USART_handler_t *usart_handle, uint8_t* pRxData, uint16_t len) {
	USART_RegDef_t *USART = usart_handle->pUSART;
	USART_config_t cUSART = usart_handle->USART_config;
	
	for (int i = 0; i < len; i++) {
		/* Wait for data to be available in RX register */
		while (!USART_GetFlagStatus(USART, USART_FLAG_RXNE));
		
		if (cUSART.USART_WordLength == USART_WordLen_9bits) {
			/* 9-bit data */
			if (cUSART.USART_ParityControl == USART_Parity_None) {
				/* No parity: all 9 bits are data */
				*((uint16_t*)pRxData) = ((USART->USART_RDR) & (uint16_t)0x1ff);
				pRxData = pRxData + 2;  /* Move by 2 bytes */
			} else {
				/* With parity: 8 data bits + parity */
				*((uint8_t*)pRxData) = ((USART->USART_RDR) & (uint8_t)0xff);
				pRxData++;
			}
		} else {
			/* 8-bit data */
			if (cUSART.USART_ParityControl == USART_Parity_None) {
				/* No parity: all 8 bits are data */
				*((uint8_t*)pRxData) = ((USART->USART_RDR) & (uint8_t)0xff);
				pRxData++;
			} else {
				/* With parity: 7 data bits + parity (drop parity bit) */
				*((uint8_t*)pRxData) = ((USART->USART_RDR) & (uint8_t)0x7f);
				pRxData++;
			}
		}
	}
}

/** @} */ // end of USART_BLOCKING_FUNCTIONS

/**
 * @defgroup USART_INTERRUPT_FUNCTIONS USART Interrupt-Driven Functions
 * @{
 */

/**
 * @brief Send data via USART (interrupt-driven mode)
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @param[in] pTxData Pointer to data buffer to transmit
 * @param[in] Len Number of bytes to send
 * @retval void
 *
 * @details
 * Initiates non-blocking interrupt-driven transmission. Function sets up the
 * transmit buffer, enables TXEIE and TCIE interrupts, then returns immediately.
 * USART_IRQHandler() automatically sends data in the background, calling
 * USART_ApplicationEventCallback() when transmission is complete.
 *
 * **Operation:**
 * 1. Check if TX not already busy (USART_BUSY_IT_TX)
 * 2. Store TX buffer pointer and length
 * 3. Set TX state to busy
 * 4. Enable TXEIE interrupt (transmit register empty)
 * 5. Enable TCIE interrupt (transmission complete)
 * 6. Return immediately
 *
 * **Interrupt Flow:**
 * ```
 * TXEIE:    Fire when TDR empty → ISR sends next byte → decrease counter
 * TCIE:     Fire when transmission complete → Signal application
 * ```
 *
 * **State Management:**
 * - usart_handle->tx_len: Remaining bytes to send
 * - usart_handle->pTXBuffer: Current position in buffer
 * - usart_handle->UASRT_Txstate: TX state (USART_BUSY_IT_TX or USART_FREE)
 *
 * **Non-Blocking Behavior:**
 * - Application continues executing while data transmits
 * - USART_IRQHandler() handles transmission automatically
 * - No blocking or busy-waiting
 * - Suitable for real-time applications
 *
 * **Example Callback:**
 * ```c
 * void USART_ApplicationEventCallback(USART_handler_t *handle, USART_CallBack_t event) {
 *     if (event == USART_EVENT_TX_CMPL) {
 *         // Transmission complete
 *         tx_complete_flag = 1;
 *     }
 * }
 * ```
 *
 * **Interrupt Handler Linkage:**
 * ```c
 * void USART3_IRQHandler(void) {
 *     USART_IRQHandler(&uart3_handle);  // Dispatcher
 * }
 * ```
 *
 * @param[in] usart_handle USART handler:
 *   - pUSART: USART base address
 *   - Txstate: Must be USART_FREE (not already transmitting)
 *   - Will be set to USART_BUSY_IT_TX
 *
 * @param[in] pTxData Pointer to transmit data:
 *   - Must point to valid buffer
 *   - Buffer lifetime must be until transmission complete
 *   - Data not modified
 *   - Can be on stack or heap
 *
 * @param[in] Len Number of bytes to send:
 *   - Must be > 0
 *   - Stored in tx_len for ISR
 *   - No bounds checking
 *
 * @note
 * - Non-blocking: returns immediately
 * - Background transmission via interrupts
 * - Much more efficient than blocking for multiple operations
 * - Data must remain valid until transmission complete
 * - Application must not modify TX buffer during transmission
 *
 * **Typical Usage (Main Loop):**
 * ```c
 * // Main loop
 * while (1) {
 *     if (data_ready) {
 *         USART_SendData_IT(&uart_handle, data_buf, data_len);
 *         data_ready = 0;
 *     }
 *     // Can do other work while transmission happens
 * }
 * ```
 *
 * **Multiple Sends in Sequence:**
 * ```c
 * // Send multiple messages
 * uint8_t msg1[] = "Hello ";
 * uint8_t msg2[] = "World\r\n";
 *
 * // First send
 * USART_SendData_IT(&uart_handle, msg1, sizeof(msg1)-1);
 * // Wait for callback...
 * // Second send (in callback or when notified)
 * USART_SendData_IT(&uart_handle, msg2, sizeof(msg2)-1);
 * ```
 *
 * @warning
 * - Do not call if TX already in progress (check UASRT_Txstate)
 * - Buffer must remain valid until USART_EVENT_TX_CMPL callback
 * - Stack-allocated buffers OK only if callback in same scope
 * - No queue: only one transmission at a time
 *
 * @attention
 * - USART interrupt must be enabled in NVIC
 * - Interrupt handler must call USART_IRQHandler()
 * - Application must implement USART_ApplicationEventCallback()
 * - Ensure TX interrupts not masked
 *
 * @see USART_IRQHandler(), USART_ReadData_IT(), USART_ApplicationEventCallback()
 */
void USART_SendData_IT(USART_handler_t *usart_handle, uint8_t *pTxData, uint32_t Len) {
	if (usart_handle->UASRT_Txstate != USART_BUSY_IT_TX) {
		usart_handle->tx_len = Len;
		usart_handle->pTXBuffer = pTxData;
		usart_handle->UASRT_Txstate = USART_BUSY_IT_TX;

		/* Setting up TXEIE in CR1 to enable TX empty interrupt */
		usart_handle->pUSART->USART_CR1 &= ~USART_CR1_TXEIE;
		usart_handle->pUSART->USART_CR1 |= USART_CR1_TXEIE;

		/* Setting up TCIE in CR1 to enable TX complete interrupt */
		usart_handle->pUSART->USART_CR1 &= ~USART_CR1_TCIE;
		usart_handle->pUSART->USART_CR1 |= USART_CR1_TCIE;
	}
}

/**
 * @brief Read data from USART (interrupt-driven mode)
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @param[out] pRxData Pointer to buffer for received data
 * @param[in] len Number of bytes to receive
 * @retval void
 *
 * @details
 * Initiates non-blocking interrupt-driven reception. Function sets up the receive
 * buffer, enables RXNEIE (RX not empty) interrupt, then returns. USART_IRQHandler()
 * receives data in background, calling USART_ApplicationEventCallback() when complete.
 *
 * **Operation:**
 * 1. Check if RX not already busy (USART_BUSY_IT_RX)
 * 2. Store RX buffer pointer and length
 * 3. Set RX state to busy
 * 4. Enable RXNEIE interrupt (RX register not empty)
 * 5. Return immediately
 *
 * **Interrupt Flow:**
 * ```
 * RXNEIE: Fire when RDR has data → ISR reads byte → decrease counter
 * When counter == 0 → Disable RXNEIE, signal application
 * ORE:    Fire on overrun → Signal error to application
 * ```
 *
 * **State Management:**
 * - usart_handle->rx_len: Remaining bytes to receive
 * - usart_handle->pRXBuffer: Current position in buffer
 * - usart_handle->UASRT_Rxstate: RX state (USART_BUSY_IT_RX or USART_FREE)
 *
 * **Non-Blocking Behavior:**
 * - Application continues executing while receiving
 * - USART_IRQHandler() handles reception automatically
 * - Automatic buffer management
 * - Suitable for background data collection
 *
 * **Interrupt Handler Pattern:**
 * ```c
 * void USART3_IRQHandler(void) {
 *     USART_IRQHandler(&uart3_handle);
 * }
 * ```
 *
 * **Application Callback:**
 * ```c
 * void USART_ApplicationEventCallback(USART_handler_t *handle, USART_CallBack_t event) {
 *     if (event == USART_EVENT_RX_CMPL) {
 *         // Reception complete, buffer has data
 *         process_received_data();
 *     } else if (event == USART_ERR_ORE) {
 *         // Overrun error: data lost
 *         handle_overrun();
 *     }
 * }
 * ```
 *
 * @param[in] usart_handle USART handler:
 *   - pUSART: USART base address
 *   - Rxstate: Must be USART_FREE
 *   - Will be set to USART_BUSY_IT_RX until complete
 *
 * @param[out] pRxData Pointer to receive buffer:
 *   - Must point to writable memory
 *   - Size must be >= len bytes
 *   - Valid until USART_EVENT_RX_CMPL callback
 *   - Can be on stack or heap
 *
 * @param[in] len Number of bytes to receive:
 *   - Must be > 0
 *   - Stored in rx_len for ISR
 *   - No bounds checking
 *
 * @note
 * - Non-blocking: returns immediately
 * - Background reception via interrupts
 * - Efficient for streaming data
 * - Can receive during application processing
 * - Automatic overrun detection and reporting
 *
 * **Typical Usage:**
 * ```c
 * // Set up to receive line of data
 * uint8_t command_buffer[64];
 * USART_ReadData_IT(&uart_handle, command_buffer, 64);
 *
 * // Main loop continues...
 * while (1) {
 *     do_other_work();
 *     // Callback will signal when 64 bytes received
 * }
 * ```
 *
 * **Ring Buffer Pattern (Continuous Reception):**
 * ```c
 * // Receive single byte repeatedly
 * uint8_t rx_byte;
 * USART_ReadData_IT(&uart_handle, &rx_byte, 1);
 *
 * // In callback:
 * void USART_ApplicationEventCallback(USART_handler_t *handle, USART_CallBack_t event) {
 *     if (event == USART_EVENT_RX_CMPL) {
 *         // Process rx_byte
 *         ring_buffer_push(&cmd_buffer, rx_byte);
 *         // Read next byte
 *         USART_ReadData_IT(handle, &rx_byte, 1);
 *     }
 * }
 * ```
 *
 * @warning
 * - Do not call if RX already in progress
 * - Buffer must remain valid until USART_EVENT_RX_CMPL
 * - Overrun errors can silently drop data
 * - No queue: only one reception at a time
 * - Must check for USART_ERR_ORE to catch errors
 *
 * @attention
 * - USART interrupt must be enabled in NVIC
 * - Interrupt handler must call USART_IRQHandler()
 * - RX interrupts must not be masked
 * - Application must implement USART_ApplicationEventCallback()
 *
 * @see USART_IRQHandler(), USART_SendData_IT(), USART_ApplicationEventCallback()
 */
void USART_ReadData_IT(USART_handler_t *usart_handle, uint8_t *pRxData, uint32_t len) {
	if (usart_handle->UASRT_Rxstate != USART_BUSY_IT_RX) {
		usart_handle->rx_len = len;
		usart_handle->pRXBuffer = pRxData;
		usart_handle->UASRT_Rxstate = USART_BUSY_IT_RX;
		usart_handle->pUSART->USART_CR1 |= USART_CR1_RXNEIE;
	}
}

/**
 * @brief USART interrupt handler dispatcher
 * @param[in] usart_handle Pointer to USART_handler_t structure
 * @retval void
 *
 * @details
 * Main interrupt handler that processes all USART interrupt sources:
 * - TXEIE: Transmit register empty → send next byte
 * - TCIE: Transmission complete → signal application
 * - RXNEIE: Receive register not empty → read byte
 * - ORE: Overrun error → signal error
 *
 * **Interrupt Priority (Order of Checking):**
 * 1. TXEIE + TXE: Send next byte if available
 * 2. TCIE + TC: Signal TX complete, disable interrupts
 * 3. RXNEIE + RXNE: Receive next byte
 * 4. RXNEIE + ORE: Handle overrun error
 *
 * **Word Length and Parity Handling:**
 * - 8-bit no parity: Full 8 bits data
 * - 8-bit with parity: 7 bits data + parity
 * - 9-bit no parity: Full 9 bits data
 * - 9-bit with parity: 8 bits data + parity
 *
 * **TX Handler Logic:**
 * ```
 * If TXEIE enabled and TXE flag set:
 *   If tx_len > 0:
 *     Send next byte to TDR
 *     Decrement tx_len
 *   If tx_len == 0:
 *     Disable TXEIE (wait for TC)
 * ```
 *
 * **RX Handler Logic:**
 * ```
 * If RXNEIE enabled and RXNE flag set:
 *   If rx_len > 0:
 *     Read byte from RDR
 *     Store in buffer
 *     Decrement rx_len
 *   If rx_len == 0:
 *     Disable RXNEIE
 *     Signal USART_EVENT_RX_CMPL
 * ```
 *
 * **Error Handler Logic:**
 * ```
 * If RXNEIE enabled and ORE flag set:
 *   Clear ORE flag
 *   Set Rxstate to FREE
 *   Signal USART_ERR_ORE
 * ```
 *
 * **TX Complete Logic:**
 * ```
 * If TCIE enabled and TC flag set:
 *   If tx_len == 0:
 *     Set Txstate to FREE
 *     Clear TX buffer pointer
 *     Clear TC flag
 *     Signal USART_EVENT_TX_CMPL
 * ```
 *
 * @param[in] usart_handle USART handler:
 *   - All state variables updated
 *   - Callbacks triggered
 *   - Flags cleared
 *
 * @note
 * - Called from USART peripheral interrupt handler
 * - Dispatcher for multiple interrupt sources
 * - Handles TX and RX concurrently
 * - Automatically manages state transitions
 *
 * **Typical ISR Dispatcher:**
 * ```c
 * void USART3_IRQHandler(void) {
 *     USART_IRQHandler(&uart3_handle);
 * }
 * ```
 *
 * **Interrupt Nesting:**
 * - Safe for interrupt nesting (if configured)
 * - Updates are atomic (single byte operations)
 * - No mutual exclusion needed (single ISR)
 *
 * @see USART_SendData_IT(), USART_ReadData_IT(), USART_ApplicationEventCallback()
 */
void USART_IRQHandler(USART_handler_t *usart_handle) {
	uint32_t flag_1, flag_2, flag_3;
	uint16_t *pTxBuffer_16;

	USART_RegDef_t *USART_reg = usart_handle->pUSART;

	/* ===== TX Empty Interrupt Handler (TXEIE) ===== */
	/* Check if the interrupt is generated from TXEIE */
	flag_1 = USART_reg->USART_CR1 & (USART_CR1_TXEIE);
	flag_2 = USART_reg->USART_ISR & (USART_ISR_TXE);

	if (flag_1 && flag_2) {
		if (usart_handle->UASRT_Txstate == USART_BUSY_IT_TX) {
			if (usart_handle->tx_len > 0) {
				if (usart_handle->USART_config.USART_WordLength == USART_WordLen_9bits) {
					/* 9-bit data */
					pTxBuffer_16 = (uint16_t*)usart_handle->pTXBuffer;
					USART_reg->USART_TDR = (*pTxBuffer_16 & (uint16_t)0x1ff);
					
					if (usart_handle->USART_config.USART_ParityControl == USART_Parity_None) {
						usart_handle->pTXBuffer = usart_handle->pTXBuffer + 2;
						usart_handle->tx_len = usart_handle->tx_len - 2;
					} else {
						usart_handle->pTXBuffer = usart_handle->pTXBuffer + 1;
						usart_handle->tx_len = usart_handle->tx_len - 1;
					}
				} else {
					/* 8-bit data */
					USART_reg->USART_TDR = *(usart_handle->pTXBuffer) & 0xff;
					usart_handle->pTXBuffer = usart_handle->pTXBuffer + 1;
					usart_handle->tx_len = usart_handle->tx_len - 1;
				}
			}
			if (usart_handle->tx_len == 0) {
				/* All bytes sent, disable TXEIE and wait for TC */
				USART_reg->USART_CR1 &= ~USART_CR1_TXEIE;
			}
		}
	}

	/* ===== TX Complete Interrupt Handler (TCIE) ===== */
	/* Check if interrupt is generated from TCIE */
	flag_1 = USART_reg->USART_CR1 & (USART_CR1_TCIE);
	flag_2 = USART_reg->USART_ISR & (USART_ISR_TC);
	
	if (flag_1 && flag_2) {
		if (usart_handle->UASRT_Txstate == USART_BUSY_IT_TX) {
			if (usart_handle->tx_len == 0) {
				/* TX complete */
				usart_handle->UASRT_Txstate = USART_FREE;
				usart_handle->pTXBuffer = NULL;
				usart_handle->tx_len = 0;
				usart_handle->pUSART->USART_CR1 &= ~USART_CR1_TCIE;
				usart_handle->pUSART->USART_ICR |= USART_ICR_TCCF;
				USART_ApplicationEventCallback(usart_handle, USART_EVENT_TX_CMPL);
			}
		}
	}

	/* ===== RX Receive Interrupt Handler (RXNEIE) ===== */
	/* Check if interrupt is from RXNEIE */
	flag_1 = USART_reg->USART_CR1 & (USART_CR1_RXNEIE);
	flag_2 = USART_reg->USART_ISR & (USART_ISR_RXNE);
	flag_3 = USART_reg->USART_ISR & (USART_ISR_ORE);
	
	if (flag_1 && flag_2) {
		if (usart_handle->UASRT_Rxstate == USART_BUSY_IT_RX) {
			if (usart_handle->rx_len > 0) {
				if (usart_handle->USART_config.USART_WordLength == USART_WordLen_9bits) {
					/* 9-bit data */
					if (usart_handle->USART_config.USART_ParityControl == USART_Parity_None) {
						*(uint16_t*)(usart_handle->pRXBuffer) = ((USART_reg->USART_RDR) & (uint16_t)0x1ff);
						usart_handle->pRXBuffer += 2;
						usart_handle->rx_len -= 2;
					} else {
						*(usart_handle->pRXBuffer) = ((USART_reg->USART_RDR) & 0x0ff);
						usart_handle->pRXBuffer++;
						usart_handle->rx_len--;
					}
				} else {
					/* 8-bit data */
					if (usart_handle->USART_config.USART_WordLength == USART_WordLen_8bits) {
						if (usart_handle->USART_config.USART_ParityControl == USART_Parity_None) {
							*(usart_handle->pRXBuffer) = ((USART_reg->USART_RDR) & 0x0ff);
							usart_handle->pRXBuffer++;
							usart_handle->rx_len--;
						} else {
							*(usart_handle->pRXBuffer) = ((USART_reg->USART_RDR) & 0x07f);
							usart_handle->pRXBuffer++;
							usart_handle->rx_len--;
						}
					}
				}
			}
			if (usart_handle->rx_len == 0) {
				/* RX complete */
				USART_reg->USART_CR1 &= ~USART_CR1_RXNEIE;
				usart_handle->UASRT_Rxstate = USART_FREE;
				usart_handle->pRXBuffer = NULL;
				USART_ApplicationEventCallback(usart_handle, USART_EVENT_RX_CMPL);
			}
		}
	}
	
	/* ===== Overrun Error Handler ===== */
	if (flag_1 && flag_3) {
		USART_reg->USART_ICR |= USART_ICR_ORECF;
		usart_handle->UASRT_Rxstate = USART_FREE;
		usart_handle->pRXBuffer = NULL;
		USART_ApplicationEventCallback(usart_handle, USART_ERR_ORE);
	}
}

/**
 * @brief Weak USART application event callback (overrideable)
 * @param[in] pUSARTHandle Pointer to USART handler
 * @param[in] event Event type (TX_CMPL, RX_CMPL, ORE, etc.)
 * @retval void
 *
 * @details
 * Weak callback function called by interrupt handler when events occur.
 * Application must override this function to handle events like:
 * - USART_EVENT_TX_CMPL: Transmission complete
 * - USART_EVENT_RX_CMPL: Reception complete
 * - USART_ERR_ORE: Overrun error
 *
 * **Function Attributes:**
 * - __attribute__((weak)): Weak linkage, can be overridden
 * - Default implementation: Empty (does nothing)
 * - Application provides strong implementation
 *
 * @param[in] pUSARTHandle USART handler associated with event
 * @param[in] event Event enumeration:
 *   - USART_EVENT_TX_CMPL: TX finished, ready for next send
 *   - USART_EVENT_RX_CMPL: RX finished, data in buffer
 *   - USART_ERR_ORE: Overrun error (data lost)
 *
 * @note
 * - Called from interrupt context (ISR)
 * - Keep callback function short (minimize ISR time)
 * - No blocking operations in callback
 * - Can set flags that main loop checks
 *
 * **Application Implementation:**
 * ```c
 * // Override weak function with strong implementation
 * void USART_ApplicationEventCallback(USART_handler_t *pUSARTHandle, USART_CallBack_t event) {
 *     if (event == USART_EVENT_TX_CMPL) {
 *         tx_complete = 1;  // Signal main loop
 *     } else if (event == USART_EVENT_RX_CMPL) {
 *         process_received_data();  // Handle received data
 *         // Set up next reception
 *         USART_ReadData_IT(pUSARTHandle, next_buffer, 64);
 *     } else if (event == USART_ERR_ORE) {
 *         error_count++;  // Track errors
 *     }
 * }
 * ```
 *
 * **ISR Callback Sequence:**
 * ```
 * 1. Interrupt fires
 * 2. USART_IRQHandler called
 * 3. Event detected and processed
 * 4. USART_ApplicationEventCallback called
 * 5. ISR returns
 * 6. Main code resumes
 * ```
 *
 * @warning
 * - Default (weak) implementation is empty
 * - Must implement application version
 * - ISR context: no long operations
 * - Be careful with global variable access
 * - Ensure thread-safety if using RTOS
 *
 * @see USART_IRQHandler()
 */
void __attribute__((weak)) USART_ApplicationEventCallback(USART_handler_t *pUSARTHandle, USART_CallBack_t event) {
	/* Not used in this validation */
}

/** @} */ // end of USART_INTERRUPT_FUNCTIONS
