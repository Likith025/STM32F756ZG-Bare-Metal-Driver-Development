/*
 * stm32f7xx_timer_driver.c
 *
 *  Created on: Mar 29, 2026
 *      Author: likith
 */
#include "stm32f7xx_timer_driver.h"

#define INPUT_CLK_FREQ		(16000000);
#define TARGET_CLK_FREQ		(100000);

void TimerInit(TIMER_handler_t* timer_handle){
	RCC->APB1ENR|=(1<<0);
	timer_handle->pTimer=TIMER2;
	timer_handle->pTimer->TIM_PSC=timer_handle->TimerConfig.PreScaler-1;
	timer_handle->pTimer->TIM_ARR=timer_handle->TimerConfig.Period-1;
	if(timer_handle->TimerConfig.direction==1)
	{
		timer_handle->pTimer->TIM_CR1|=(1<<4);
	}
	else{
		timer_handle->pTimer->TIM_CR1&=~(1<<4);
	}
	timer_handle->pTimer->TIM_CNT=0;
	TimerResetFlag(timer_handle, Timer_Flag_UIF);

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
void TimerSetFrequency(TIMER_handler_t* timer_handle,uint32_t frequency){
	uint32_t timer_clk_freq=INPUT_CLK_FREQ;
	uint32_t target_clk_freq=TARGET_CLK_FREQ;

	uint32_t PreScaler=timer_clk_freq/target_clk_freq;
	uint32_t ARR=target_clk_freq/frequency;

	timer_handle->TimerConfig.PreScaler=PreScaler;
	timer_handle->TimerConfig.Period=ARR;
}

void TimerPWM_init(TIMER_handler_t* timer_handle,Timer_channel_t Channel){
    volatile uint32_t *ccmr;

    ccmr = (Channel <= 2) ? &timer_handle->pTimer->TIM_CCMR1
                     : &timer_handle->pTimer->TIM_CCMR2;

    uint8_t shift = ((Channel - 1) % 2) * 8;
    *ccmr &= ~(3 << shift);
    *ccmr &= ~(7 << (shift + 4));
    *ccmr |=  (6 << (shift + 4));
    *ccmr |= (1 << (shift + 3));
    timer_handle->pTimer->TIM_CR1 |= (1 << 7);
    timer_handle->pTimer->TIM_CCER |= (1 << (4 * (Channel - 1)));
    volatile uint32_t *ccr = &timer_handle->pTimer->TIM_CCR1 + (Channel - 1);
    *ccr = 0;

}

void TimerPWM_DutyCycle(TIMER_handler_t* timer_handle,Timer_channel_t Channel,uint8_t DutyCycle){
	uint32_t arr;
	uint32_t ccr;
	arr=timer_handle->pTimer->TIM_ARR;
	if(DutyCycle==0){
		ccr=0;
	}
	else if(DutyCycle>=100){
		ccr=arr;

	}
	else{
		ccr=((arr+1)*DutyCycle)/100;
	}
    volatile uint32_t *ccr_reg = &timer_handle->pTimer->TIM_CCR1 + (Channel - 1);

    *ccr_reg = ccr;
}
