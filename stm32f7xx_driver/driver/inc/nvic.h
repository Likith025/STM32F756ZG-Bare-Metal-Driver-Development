/*
 * nvic.h
 *
 *  Created on: Mar 22, 2026
 *      Author: likith
 */

#ifndef INC_NVIC_H_
#define INC_NVIC_H_

#include <stm32f756zg_reg.h>
#include<stdint.h>

void IntrruptConfig(uint8_t IRQNumber,uint32_t IRQPriority, uint8_t enable);
void IRQ_IntrruptConfig(uint8_t IRQ_Number,uint8_t status);
void IRQ_PriorityConfig(uint8_t IRQ_Number,uint32_t IRQ_Priority);


#endif /* INC_NVIC_H_ */
