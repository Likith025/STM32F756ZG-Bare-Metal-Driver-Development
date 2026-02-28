/*
 * stm32f7xx_uart_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: likith
 */


#include "stm32f7xx_uart_driver.h"


int8_t USART_init(USART_handler_t *usart_handle,uint8_t enable){
	uint8_t position=0;
	USART_MODE_t mode;
	USART_WordLen_t len=0;
	USART_Parity_t parity=0;
	len=usart_handle->USART_config.USART_WordLength;
	mode=usart_handle->USART_config.USART_Mode;
	parity=usart_handle->USART_config.USART_ParityControl;

/* Setting up RCC clock to enable USART*/
	USART_RegDef_t *USART=usart_handle->pUSART;
	if((USART==NULL)||((uint32_t)USART<USART2_BASE_ADDR)||((uint32_t)USART>UART5_BASE_ADDR)){
		return -1;
	}
	position=(((uint32_t)USART-USART2_BASE_ADDR)/0x400);
	if(enable){
		RCC->APB1ENR|=(1<<(position+17));
	}
	else{
		RCC->APB1ENR&=~(1<<(position+17));
	}
/*disabling USART before setting up*/
	usart_handle->pUSART->USART_CR1&=~(1<<0);


/*Setting UP direction(Tx,Rx,both)*/
	usart_handle->pUSART->USART_CR1&=~(3<<2);
	usart_handle->pUSART->USART_CR1|=mode;

	if(len==USART_WordLen_7bits){
	usart_handle->pUSART->USART_CR1|=(1<<28);
	usart_handle->pUSART->USART_CR1&=~(1<<12);
	}
	else if(len==USART_WordLen_8bits){
		usart_handle->pUSART->USART_CR1&=~(1<<12);
		usart_handle->pUSART->USART_CR1&=~(1<<28);
	}
	else if(len==USART_WordLen_9bits){
		usart_handle->pUSART->USART_CR1&=~(1<<28);
		usart_handle->pUSART->USART_CR1|=(1<<12);
	}
	else{
		return -1;
	}

/*parity setup*/
	if(parity==USART_Parity_None){
		usart_handle->pUSART->USART_CR1&=~(1<<10);
	}
	else{
		usart_handle->pUSART->USART_CR1|=(1<<10);
		if(parity==USART_Parity_Even){
			usart_handle->pUSART->USART_CR1&=~(1<<9);
		}
		else if(parity==USART_Parity_Odd){
			usart_handle->pUSART->USART_CR1|=(1<<9);
		}
		else{
			return -1;
		}

	}
	/* setting over sampling*/
	if(usart_handle->USART_config.USART_OverSmapling==USART_OverSampling_8){
	usart_handle->pUSART->USART_CR1|=(1<<15);
	}
	else if(usart_handle->USART_config.USART_OverSmapling==USART_OverSampling_16){
		usart_handle->pUSART->USART_CR1&=~(1<<15);
	}
	else{
		return -1;
	}


	/*hardware control setup*/
	if(usart_handle->USART_config.USART_HWflowControl==USART_HW_FlowCtrl_None){
		usart_handle->pUSART->USART_CR3&=~(1<<9);
		usart_handle->pUSART->USART_CR3&=~(1<<8);
	}
	else if(usart_handle->USART_config.USART_HWflowControl==USART_HW_FlowCtrl_CTS){
		usart_handle->pUSART->USART_CR3|=(1<<9);
	}
	else if(usart_handle->USART_config.USART_HWflowControl==USART_HW_FlowCtrl_RTS){
		usart_handle->pUSART->USART_CR3|=(1<<8);
	}
	else if(usart_handle->USART_config.USART_HWflowControl==USART_HW_FlowCtrl_CTS_RTS){
		usart_handle->pUSART->USART_CR3|=(1<<8);
		usart_handle->pUSART->USART_CR3|=(1<<9);
	}





return 0;
}
