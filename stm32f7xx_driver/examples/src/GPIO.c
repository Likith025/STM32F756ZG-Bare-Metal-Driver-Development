/*
 * button_led.c
 *
 *  Created on: 31-May-2026
 *      Author: likith
 */


#include "peripheral_config.h"
void button_led(void){
	GPIO_handler_t Button;
	GPIO_handler_t Led;

	led_setup_blue(&Led);
	button_setup(&Button);

	int value=0;
	while(1){
		value=GPIO_ReadPin(GPIO_C, 13);
		GPIO_WritePin(GPIO_B, 7, value);
	}
}

void led_blink(void){
	GPIO_handler_t LED;
	led_setup_blue(&LED);
	while(1){
		GPIO_TogglePin(GPIO_B, 7);
		for(volatile int i=0;i<100000;i++);

	}
}

void button_led_inttrupt(void){
	GPIO_handler_t Button;
	GPIO_handler_t LED;
	led_setup_red(&LED);
	button_interrupt_setup(&Button);
	IntrruptConfig(IRQ_NO_EXTI3, 10, ENABLE);
}
void EXTI15_10_IRQHandler(void){

	GPIO_IntrruptHandler(13);
	GPIO_TogglePin(GPIO_B, 14);
  }

