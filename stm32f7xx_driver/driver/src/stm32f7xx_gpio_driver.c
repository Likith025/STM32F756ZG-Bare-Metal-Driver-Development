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

int8_t GPIO_clk_init(const GPIO_RegDef_t* pGPIO_x,const uint8_t enable){
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

	if(mode<=GPIO_MODE_ANALOG){
		temp=(mode)<<(2*pin);
		GPIO_xHandler->pGPIOx->MODER&=~(0x3U<<(0x2U*pin));
		GPIO_xHandler->pGPIOx->MODER|=temp;
	}
	else{
		GPIO_xHandler->pGPIOx->MODER&=~(0x3U<<(0x2U*pin)); // input mode for EXTI

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

int8_t GPIO_WritePort(GPIO_RegDef_t* pGPIO_x,uint16_t value){

	if(pGPIO_x==NULL){

		return -1;
	}

	pGPIO_x->ODR=value;
	return 0;
}

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

uint8_t GPIO_ReadPin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber){
	uint8_t value=0;
	value=(((pGPIO_x->IDR)>>PinNumber)&1U);
	return value;
}


uint16_t GPIO_ReadPort(GPIO_RegDef_t* pGPIO_x){
	return (pGPIO_x->IDR&0xffff);

}

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

void SYSCFG_CLK_Enable(){
	RCC->APB2ENR|=(1<<14);
}

void  GPIO_IRQ_IntrruptConfig(uint8_t IRQ_Number,uint8_t status){
	if(status==ENABLE){
		NVIC->ISER[IRQ_Number/32]|=(1<<(IRQ_Number%32));
	}
	else{
		NVIC->ICER[IRQ_Number/32]|=(1<<(IRQ_Number%32));
	}
}

void GPIO_IRQ_PriorityConfig(uint8_t IRQ_Number, uint8_t IRQ_Priority)
{
	uint8_t ipr_index = IRQ_Number / 4;
	uint8_t ipr_section = IRQ_Number % 4;

	uint8_t shift = (ipr_section * 8) + 4;

	/* Clear previous priority */
	NVIC->IPR[ipr_index] &= ~(0xF << shift);

	/* Set new priority */
	NVIC->IPR[ipr_index] |= (IRQ_Priority << shift);
}


void GPIO_IRQ_Handler(uint8_t pinNumber){
	if(pinNumber < 16U){
		/* EXTI pending bits are cleared by writing 1 to the target line */
		EXTI->EXTI_PR = (1U << pinNumber);
	}

}

void GPIO_IntrruptConfig(uint8_t IRQNumber,uint8_t IRQPriority, uint8_t enable){
	GPIO_IRQ_PriorityConfig(IRQNumber, IRQPriority);
	GPIO_IRQ_IntrruptConfig(IRQNumber, enable);
}

void GPIO_IntrruptHandler(uint8_t PinNumber){
	GPIO_IRQ_Handler(PinNumber);
}



