/*
 * stm32f7xx_timer_driver.c
 *
 *  Created on: Mar 29, 2026
 *      Author: likith
 */
#include "stm32f7xx_timer_driver.h"



void TimerInit(TIMER_handler_t* timer_handle){
	RCC->APB1ENR&=~(1<<0);// using only timer2
	RCC->APB1ENR|=(1<<0);
	timer_handle->pTimer=TIMER2;
	timer_handle->pTimer->TIM_PSC=timer_handle->TimerConfig.PreScaler;
	timer_handle->pTimer->TIM_ARR=timer_handle->TimerConfig.Period;
	if(timer_handle->TimerConfig.direction==1){
		timer_handle->pTimer->TIM_CR1|=(1<<4);
	}
	else{
		timer_handle->pTimer->TIM_CR1&=~(1<<4);
	}
	timer_handle->pTimer->TIM_CNT=0;
	timer_handle->pTimer->TIM_SR &= ~(1<<0);

}

void TimerControl(TIMER_handler_t* timer_handle,uint16_t state){
	//timer_handle->pTimer=TIMER2;
	if(state)
	{
		timer_handle->pTimer->TIM_CR1|=(1<<0);
	}
	else{
		timer_handle->pTimer->TIM_CR1&=~(1<<0);
	}
}

uint32_t TimerGetCount(TIMER_handler_t* timer_handle){
	 uint32_t timer_count=0;
		//timer_handle->pTimer=TIMER2;
	timer_count=timer_handle->pTimer->TIM_CNT;
	return timer_count;
}

uint8_t TimerGetFlagStatus(TIMER_handler_t* timer_handle,Timer_flags_t Flag){
	uint8_t status=0;
	if(timer_handle->pTimer->TIM_SR&Flag){
		status=1;
	}
	return status;
}

void TimerResetFlag(TIMER_handler_t* timer_handle,Timer_flags_t Flag){
	timer_handle->pTimer->TIM_SR&=~(Flag);
}
