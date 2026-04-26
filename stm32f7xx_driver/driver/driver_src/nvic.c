/*
 * nvic.c
 *
 *  Created on: Mar 22, 2026
 *      Author: likith
 */
#include<stdint.h>
#include "nvic.h"


void IntrruptConfig(uint8_t IRQNumber,uint32_t IRQPriority, uint8_t enable){
	IRQ_PriorityConfig(IRQNumber, IRQPriority);
	IRQ_IntrruptConfig(IRQNumber, enable);
}

void  IRQ_IntrruptConfig(uint8_t IRQ_Number,uint8_t status){
	if(status==ENABLE){
		NVIC->ISER[IRQ_Number/32]|=(1UL<<(IRQ_Number%32));
	}
	else{
		NVIC->ICER[IRQ_Number/32]|=(1UL<<(IRQ_Number%32));
	}
}

void IRQ_PriorityConfig(uint8_t IRQ_Number, uint32_t IRQ_Priority)
{
	uint8_t ipr_index = IRQ_Number / 4;
	uint8_t ipr_section = IRQ_Number % 4;

	uint8_t shift = (ipr_section * 8) + 4;


	NVIC->IPR[ipr_index] &= ~((uint32_t)0xF << shift);
	NVIC->IPR[ipr_index] |= ((uint32_t)IRQ_Priority << shift);

}
