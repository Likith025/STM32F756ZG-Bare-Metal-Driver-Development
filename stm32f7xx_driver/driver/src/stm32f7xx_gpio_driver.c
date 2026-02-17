/*
 * stm32f7xx_gpio_driver.c
 *
 *  Created on: Feb 17, 2026
 *      Author: likith
 */


#include "stm32f7xx_gpio_driver.h"

/*
 * function_name: GPIO_clk_init
 * input arguments: 1) pointer to base address of GPIO port
 * 					2) enable
 * return type: -1 on unsuccessful initialization
 * 				0 on successful initialization
 * details: To set or reset the peripheral clock to the give gpio port
 */

int8_t GPIO_clk_init(GPIO_RegDef_t* pGPIO_x,uint8_t enable){
	uint8_t position;
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





