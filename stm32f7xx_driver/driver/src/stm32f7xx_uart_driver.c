/*
 * stm32f7xx_uart_driver.c
 *
 *  Created on: Feb 28, 2026
 *      Author: likith
 */


#include "stm32f7xx_uart_driver.h"


int8_t USART_init(USART_handler_t *usart_handle,uint8_t enable){
	USART_MODE_t mode;
	USART_WordLen_t len=0;
	USART_Parity_t parity=0;
	USART_StopBits_t Stop_Bits=0;
	len=usart_handle->USART_config.USART_WordLength;
	mode=usart_handle->USART_config.USART_Mode;
	parity=usart_handle->USART_config.USART_ParityControl;
	Stop_Bits=usart_handle->USART_config.USART_StopBits;

/* Setting up RCC clock to enable USART*/
	USART_RegDef_t *USART=usart_handle->pUSART;
	USART_clK_init(USART, ENABLE);
/*disabling USART before setting up*/
	USART_Control(usart_handle->pUSART,DISABLE);

	usart_handle->pUSART->USART_BRR=0x0683;

/*Setting UP direction(Tx,Rx,both)*/
	usart_handle->pUSART->USART_CR1&=~(3<<2);
	usart_handle->pUSART->USART_CR1|=mode;

	usart_handle->pUSART->USART_CR1&=~(1<<12);
	usart_handle->pUSART->USART_CR1&=~(1<<28);


	 if(len==USART_WordLen_8bits){
		 //equivalent of resetting bits 12 and 28 which is default state
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
	usart_handle->pUSART->USART_CR1&=~(1<<15);
	if(usart_handle->USART_config.USART_OverSmapling==USART_OverSampling_8){
	usart_handle->pUSART->USART_CR1|=(1<<15);
	}



	/*hardware control setup*/
	usart_handle->pUSART->USART_CR3&=~(1<<9);
	usart_handle->pUSART->USART_CR3&=~(1<<8);
	if(usart_handle->USART_config.USART_HWflowControl==USART_HW_FlowCtrl_CTS){
		//same as resetting both bits(default case)
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
	else{
		return -1;
	}
	if(Stop_Bits<=USART_StopBits_1_5){
		usart_handle->pUSART->USART_CR2&=~(3<<12);
		usart_handle->pUSART->USART_CR2|=(Stop_Bits<<12);
	}
	else{
		return -1;
	}





	USART_Control(usart_handle->pUSART,ENABLE);
return 0;
}

int8_t USART_Control(USART_RegDef_t *pUSART,uint8_t CMD){
	if(CMD==ENABLE){
		pUSART->USART_CR1|=(1<<0);
	}
	else{
		pUSART->USART_CR1&=~(1<<0);
	}
return 0;
}
int8_t USART_clK_init(USART_RegDef_t *pUSART,uint8_t CMD){
	uint8_t position=0;
	if((pUSART==NULL)||((uint32_t)pUSART<USART2_BASE_ADDR)||((uint32_t)pUSART>UART5_BASE_ADDR)){
		return -1;
	}
	position=(((uint32_t)pUSART-USART2_BASE_ADDR)/0x400);
	if(CMD){
		RCC->APB1ENR|=(1<<(position+17));
	}
	else{
		RCC->APB1ENR&=~(1<<(position+17));

	}
	return 0;
}

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSART,USART_Flags_t FlagName){
	uint8_t status=0;
	 if(pUSART->USART_ISR&FlagName){
		 status=1;
	 }
	 return status;

}

void USART_CLearFlag(USART_RegDef_t *pUSART,USART_ClearFlags_t FlagName){
	pUSART->USART_ICR|=(FlagName);
}

void USART_SendData(USART_handler_t *usart_handle,uint8_t* pTxData,uint16_t len){
	USART_RegDef_t *USART=usart_handle->pUSART;
	uint16_t *pData_16;
	while(len>0){
	while(!USART_GetFlagStatus(USART, USART_FLAG_TXE));

	if(usart_handle->USART_config.USART_WordLength==USART_WordLen_9bits){
		pData_16=(uint16_t*)pTxData;
		USART->USART_TDR=(*pData_16&(uint16_t)0x01ff);
		if(usart_handle->USART_config.USART_ParityControl==USART_Parity_None){
			pTxData=pTxData+2;
			len=len-2;
		}
		else{
			pTxData++;
			len--;
		}

	}
	else{
		USART->USART_TDR=(*pTxData&(uint8_t)(0x00ff));
		pTxData++;
		len--;
	}

	}
	while(!USART_GetFlagStatus(USART, USART_FLAG_TC));
}


