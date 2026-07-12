/*
 * stm32f7xx_gpio_driver.c
 *
 *  Created on: Feb 17, 2026
 *      Author: likith
 */


#include "stm32f7xx_gpio_driver.h"

/**
 * @brief Initialze clk for corresponding GPIO port.
 *
* Reset and Clock control(RCC) has Register to enable or disable clock based on bus to which the peripheral is connected to
*GPIO's are connected to AHB1 bus, so ENR register of AHB 1 bus is used to enable or disable clock to GPIO Ports
 *
 * @param[in]  pGPIO_x  Pointer pointing to the particular GPIO Port
 * @param[in] enable  denoting whether clock to be enabled or disabled.
 *
 *
 * @return  0 on success, -1 on error.
 *
 * @warning assumed that ports are continous in memory.
 */

int8_t GPIO_clk_init(const GPIO_RegDef_t* pGPIO_x,const uint8_t enable){
	uint8_t position;
	if(pGPIO_x==NULL){
		return -1;
	}
	 position = ((uint32_t)pGPIO_x - GPIOA_BASE_ADDR) / 0x400;

	 if ((uint32_t)pGPIO_x < GPIOA_BASE_ADDR ||(uint32_t)pGPIO_x > GPIOK_BASE_ADDR)
	 {
	        return -1;
	 }
	if(enable){
		RCC->AHB1ENR|=(0x1<<position);
	}
	else{
		RCC->AHB1ENR&=(~(0x1<<position));
	}
	return 0;
	}

/**
 * @brief Initializes and configures a GPIO pin.
 *
 * Configures the selected GPIO pin according to the settings provided in
 * the GPIO handler structure. Depending on the selected mode, this function
 * performs the following operations:
 *  - Enables the GPIO peripheral clock.
 *  - Configures the pin mode (Input, Output, Alternate Function, or Analog).
 *  - Configures output type (Push-Pull/Open-Drain) for output modes.
 *  - Configures output speed for Output and Alternate Function modes.
 *  - Configures internal Pull-Up/Pull-Down resistors.
 *  - Configures the Alternate Function selection (if Alternate Function mode is selected).
 *  - Configures EXTI, SYSCFG mapping, and interrupt trigger settings
 *    (if an interrupt mode is selected).
 *
 * @param[in] GPIO_xHandler
 * Pointer to the GPIO handler containing the GPIO port and pin configuration.
 *
 * @return
 *  - 0  : GPIO successfully initialized.
 *  - -1 : Invalid handler, GPIO port, or pin number.
 *
 * @note
 * This function automatically enables the peripheral clock for the selected
 * GPIO port before configuring the registers.
 *
 * @warning
 * For interrupt modes, this function only configures the GPIO, EXTI, and
 * SYSCFG registers. The corresponding NVIC interrupt must be enabled
 * separately using the interrupt configuration APIs.
 */

int8_t GPIO_init(const GPIO_handler_t* GPIO_xHandler){
	uint32_t temp=0;
	uint32_t pin=0;
	GPIO_MODE_t mode=0;

	if(GPIO_xHandler==NULL || GPIO_xHandler->pGPIOx==NULL){
		return -1;
	}
	GPIO_clk_init(GPIO_xHandler->pGPIOx, ENABLE);
	mode=GPIO_xHandler->GPIO_pin_config.GPIO_PinMode;
	pin=GPIO_xHandler->GPIO_pin_config.GPIO_PinNumber;

	if(pin>15){
		return -1;
	}

	if(mode<=GPIO_MODE_ANALOG){   // not interrupt mode
		temp=(mode)<<(2*pin);
		GPIO_xHandler->pGPIOx->MODER&=~(0x3U<<(0x2U*pin));
		GPIO_xHandler->pGPIOx->MODER|=temp;
	}
	else{
		GPIO_xHandler->pGPIOx->MODER&=~(0x3U<<(0x2U*pin)); // in interrupt mode, moder reg must be in input mode

		if(mode==GPIO_MODE_IT_FALLING_RAISING){
			EXTI->EXTI_RTSR|=(1U<<pin);
			EXTI->EXTI_FTSR|=(1U<<pin);
		}
		else if(mode==GPIO_MODE_IT_FALLING){
			EXTI->EXTI_FTSR|=(1U<<pin);
			EXTI->EXTI_RTSR&=~(1U<<pin);
		}
		else if(mode==GPIO_MODE_IT_RAISING){
			EXTI->EXTI_RTSR|=(1U<<pin);
			EXTI->EXTI_FTSR&=~(1U<<pin);
		}

		SYSCFG_CLK_Enable();
		GPIO_SYSCFG_SetUp(GPIO_xHandler);
		EXTI->EXTI_IMR|=(1U<<pin);
	}


	if(mode==GPIO_MODE_OUTPUT||mode==GPIO_MODE_ALTERNATE_FUN){

		GPIO_xHandler->pGPIOx->OTYPER&=~(0x1U<<pin);
		GPIO_xHandler->pGPIOx->OTYPER|=(GPIO_xHandler->GPIO_pin_config.GPIO_PinOutType<<pin);

		temp=(GPIO_xHandler->GPIO_pin_config.GPIO_PinOutSpeed<<(0x2U*pin));
		GPIO_xHandler->pGPIOx->OSPEEDR&=~(0x3U<<(0x2U*pin));
		GPIO_xHandler->pGPIOx->OSPEEDR|=temp;


	}
	if(mode!=GPIO_MODE_ANALOG){
		temp=(GPIO_xHandler->GPIO_pin_config.GPIO_PinPushPullResistor)<<(0x2U*pin);
		GPIO_xHandler->pGPIOx->PUPDR&=~(0x3U<<(0x2U*pin));
		GPIO_xHandler->pGPIOx->PUPDR|=temp;

	}
	else{
		GPIO_xHandler->pGPIOx->PUPDR&=~(0x3U<<(0x2U*pin));
	}

	if(mode==GPIO_MODE_ALTERNATE_FUN){
		if(pin<=7){
			temp=(GPIO_xHandler->GPIO_pin_config.GPIO_PinAltFun)<<(0x4*pin);
				GPIO_xHandler->pGPIOx->AFR[0]&=~(0xf<<(0x4*pin));
				GPIO_xHandler->pGPIOx->AFR[0]|=temp;
		}
		else{
			temp=(GPIO_xHandler->GPIO_pin_config.GPIO_PinAltFun)<<(0x4*(pin-8));
			GPIO_xHandler->pGPIOx->AFR[1]&=~(0xf<<(4*(pin-8)));
			GPIO_xHandler->pGPIOx->AFR[1]|=temp;

		}
	}
	return 0;

}
/**
 * @brief Deinitializes a GPIO peripheral.
 *
 * Resets the selected GPIO port by asserting and releasing its reset bit in
 * the RCC AHB1 Peripheral Reset Register (AHB1RSTR). This restores all GPIO
 * registers of the selected port to their default reset values.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral to be deinitialized.
 *
 * @return
 *  - 0  : GPIO successfully reset.
 *  - -1 : Invalid GPIO pointer.
 *
 * @note
 * This function uses the RCC peripheral reset mechanism instead of manually
 * clearing individual GPIO configuration registers.
 *
 * @warning
 * Assumes that GPIO ports are contiguous in memory and separated by 0x400 bytes.
 */
int8_t GPIO_deinit(GPIO_RegDef_t* pGPIO_x){
	uint16_t position=0;
	if(pGPIO_x==NULL){
		return -1;
	}
	position= (((uint32_t)pGPIO_x-GPIOA_BASE_ADDR)/0x400);
	RCC->AHB1RSTR|=(1<<position);
	RCC->AHB1RSTR&=~(1<<position);
	return 0;
}
/**
 * @brief Writes a logic level to the specified GPIO pin.
 *
 * Sets or clears the corresponding bit in the Output Data Register (ODR)
 * to drive the selected GPIO pin HIGH or LOW.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral.
 *
 * @param[in] PinNumber
 * GPIO pin number (0–15).
 *
 * @param[in] value
 * Logic level to be written to the pin.
 * - ENABLE  : Sets the pin HIGH.
 * - DISABLE : Sets the pin LOW.
 *
 * @return
 *  - 0  : Pin written successfully.
 *  - -1 : Invalid GPIO pointer or pin number.
 *
 * @note
 * This function performs a read-modify-write operation on the ODR register.
 *
 * @warning
 * Since ODR is modified using a read-modify-write sequence, the operation is
 * not atomic. For interrupt-safe GPIO updates, consider using the BSRR register.
 */
int8_t GPIO_WritePin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber,uint8_t value){
	if(PinNumber>15){
		return -1;
	}
	if(pGPIO_x==NULL){
		return -1;
	}
	if(value==ENABLE){
	pGPIO_x->ODR|=(1<<PinNumber);
	}
	else{
		pGPIO_x->ODR&=~(1<<(PinNumber));
	}
	return 0;
}
/**
 * @brief Writes a 16-bit value to the GPIO output port.
 *
 * Updates the Output Data Register (ODR) of the selected GPIO port,
 * thereby setting the logic level of all 16 GPIO pins simultaneously.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral.
 *
 * @param[in] value
 * 16-bit value to be written to the Output Data Register (ODR).
 *
 * @return
 *  - 0  : Port written successfully.
 *  - -1 : Invalid GPIO pointer.
 *
 * @note
 * Each bit of the value corresponds to the output state of the
 * respective GPIO pin (Pin 0 to Pin 15).
 *
 * @warning
 * This function overwrites the entire ODR register. Pins configured as
 * outputs will immediately reflect the new values. Use with caution if
 * only specific pins need to be modified.
 */
int8_t GPIO_WritePort(GPIO_RegDef_t* pGPIO_x,uint16_t value){

	if(pGPIO_x==NULL){

		return -1;
	}

	pGPIO_x->ODR=value;
	return 0;
}
/**
 * @brief Toggles the output state of the specified GPIO pin.
 *
 * Inverts the current logic level of the selected GPIO pin by toggling
 * its corresponding bit in the Output Data Register (ODR). A HIGH output
 * becomes LOW, and a LOW output becomes HIGH.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral.
 *
 * @param[in] PinNumber
 * GPIO pin number (0–15).
 *
 * @return
 *  - 0  : Pin toggled successfully.
 *  - -1 : Invalid GPIO pointer or pin number.
 *
 * @note
 * This function performs a read-modify-write operation on the ODR register.
 *
 * @warning
 * The toggle operation is not atomic and may be affected if the ODR register
 * is modified concurrently by an interrupt or another execution context.
 */
int8_t GPIO_TogglePin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber){
	if(PinNumber>15){
		return -1;
	}
	if(pGPIO_x==NULL){

		return -1;
	}

	pGPIO_x->ODR^=(1U<<PinNumber);
	return 0;
}

/**
 * @brief Reads the logic level of the specified GPIO pin.
 *
 * Reads the corresponding bit from the Input Data Register (IDR) and
 * returns the current logic level of the selected GPIO pin.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral.
 *
 * @param[in] PinNumber
 * GPIO pin number (0–15).
 *
 * @return
 * Current logic level of the selected pin.
 * - 0 : Logic LOW
 * - 1 : Logic HIGH
 *
 * @note
 * The pin state is read from the Input Data Register (IDR), irrespective
 * of whether the pin is configured as an input or output.
 *
 * @warning
 * This function does not validate the GPIO pointer or pin number. Passing
 * an invalid pointer or pin number results in undefined behavior.
 */
uint8_t GPIO_ReadPin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber){
	uint8_t value=0;
	value=(((pGPIO_x->IDR)>>PinNumber)&1U);
	return value;
}
/**
 * @brief Reads the current state of all GPIO pins in a port.
 *
 * Returns the 16-bit value of the Input Data Register (IDR), where each bit
 * represents the logic level of the corresponding GPIO pin.
 *
 * @param[in] pGPIO_x
 * Pointer to the GPIO peripheral.
 *
 * @return
 * 16-bit value representing the current input state of GPIO pins
 * (Pin 0 to Pin 15).
 *
 * @note
 * Bit 0 corresponds to Pin 0 and Bit 15 corresponds to Pin 15.
 *
 * @warning
 * This function does not validate the GPIO pointer. Passing an invalid
 * pointer results in undefined behavior.
 */

uint16_t GPIO_ReadPort(GPIO_RegDef_t* pGPIO_x){
	return (pGPIO_x->IDR&0xffff);

}
/**
 * @brief Configures the SYSCFG EXTI mapping for a GPIO pin.
 *
 * Maps the selected GPIO port to the corresponding EXTI line by updating
 * the appropriate field in the SYSCFG External Interrupt Configuration
 * Register (EXTICR). This enables the EXTI controller to associate the
 * selected GPIO pin with its interrupt line.
 *
 * @param[in] GPIO_xHandler
 * Pointer to the GPIO handler containing the GPIO port and pin configuration.
 *
 * @note
 * Each EXTI line (0–15) can be connected to only one GPIO port at a time.
 * This function is intended for GPIO interrupt configuration.
 *
 * @warning
 * The SYSCFG peripheral clock must be enabled before calling this function.
 * Assumes that GPIO ports are contiguous in memory and separated by 0x400 bytes.
 */
void GPIO_SYSCFG_SetUp(const GPIO_handler_t* GPIO_xHandler){

	uint8_t exti_index,exti_position=0;
	uint8_t port_code=0;
	if(GPIO_xHandler==NULL || GPIO_xHandler->pGPIOx==NULL){
		return;
	}
	port_code = ((uint32_t)GPIO_xHandler->pGPIOx - GPIOA_BASE_ADDR) / 0x400;
	exti_index=GPIO_xHandler->GPIO_pin_config.GPIO_PinNumber/4;
	exti_position=GPIO_xHandler->GPIO_pin_config.GPIO_PinNumber%4;
	SYSCFG->SYSCFG_EXTICR[exti_index]&=~(0xf<<(exti_position*4));
	SYSCFG->SYSCFG_EXTICR[exti_index]|=(port_code<<(exti_position*4));
}
/**
 * @brief Enables the clock for the SYSCFG peripheral.
 *
 * Enables the SYSCFG peripheral by setting the corresponding bit in the
 * RCC APB2 Peripheral Clock Enable Register (APB2ENR). The SYSCFG clock
 * must be enabled before accessing its registers, such as the EXTI
 * configuration registers (EXTICR).
 *
 * @note
 * This function is typically called before configuring GPIO interrupt
 * mappings using the SYSCFG peripheral.
 */
void SYSCFG_CLK_Enable(){
	RCC->APB2ENR|=(1<<14);
}

/**
 * @brief Clears the pending interrupt flag of an EXTI line.
 *
 * Clears the pending status of the specified EXTI line by writing a logic
 * '1' to the corresponding bit in the EXTI Pending Register (PR).
 *
 * @param[in] pinNumber
 * EXTI line number (0–15) whose pending flag is to be cleared.
 *
 * @note
 * EXTI pending bits are cleared by writing a '1' to the corresponding bit.
 * Writing '0' has no effect.
 *
 * @warning
 * This function only clears the EXTI pending flag. It does not clear any
 * pending interrupt state in the NVIC.
 */

void GPIO_ClearPendingFlag(uint8_t pinNumber){
	if(pinNumber < 16U){
		/* EXTI pending bits are cleared by writing 1 to the target line */
		EXTI->EXTI_PR = (1U << pinNumber);
	}

}
/**
 * @brief Handles a GPIO interrupt by clearing its EXTI pending flag.
 *
 * Invokes the EXTI pending flag clear routine for the specified GPIO pin.
 * This function is intended to be called from the corresponding EXTI
 * interrupt service routine (ISR).
 *
 * @param[in] PinNumber
 * GPIO pin number (EXTI line number) whose interrupt has been serviced.
 *
 * @note
 * User-specific interrupt processing (e.g., callbacks or event handling)
 * should be performed before or after invoking this function, as required
 * by the application.
 */
void GPIO_IntrruptHandler(uint8_t PinNumber){
	GPIO_ClearPendingFlag(PinNumber);
}



