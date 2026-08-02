/*
 * button_led.c
 *
 *  Created on: 31-May-2026
 *      Author: likith
 */


#include "peripheral_config.h"
#include "GPIO.h"

/**
 * @brief Controls the blue LED using the user push button.
 *
 * Initializes the blue LED and push button GPIOs. The button state is
 * continuously read, and the blue LED output is updated to match the
 * current button state.
 *
 * @note
 * This function runs indefinitely and is intended as a basic GPIO input/output
 * demonstration.
 */
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

/**
 * @brief Blinks the blue LED continuously.
 *
 * Initializes the blue LED GPIO and repeatedly toggles its state with
 * a software delay to produce a visible blinking effect.
 *
 * @note
 * The delay is generated using a busy-wait loop and is dependent on the
 * processor clock frequency.
 */
void led_blink(void){
	GPIO_handler_t LED;
	led_setup_blue(&LED);
	while(1){
		GPIO_TogglePin(GPIO_B, 7);
		for(volatile int i=0;i<100000;i++);

	}
}

/**
 * @brief Demonstrates GPIO interrupt handling using the user push button.
 *
 * Configures the user button as an external interrupt source and initializes
 * the red LED. When the button interrupt occurs, the corresponding ISR
 * toggles the red LED.
 *
 * @note
 * This function performs only the peripheral initialization. The interrupt
 * handling is implemented in the EXTI15_10 interrupt service routine.
 */
void button_led_interrupt(void){
	GPIO_handler_t Button;
	GPIO_handler_t LED;
	led_setup_red(&LED);
	button_interrupt_setup(&Button);
	IntrruptConfig(IRQ_NO_EXTI3, 10, ENABLE);
}

/**
 * @brief Interrupt Service Routine for EXTI lines 10 to 15.
 *
 * Services GPIO interrupts generated on EXTI lines 10 through 15 by
 * clearing the pending interrupt flag and toggling the red LED.
 *
 * @note
 * On the STM32F756, EXTI lines 10 to 15 share a common NVIC interrupt.
 * The application should determine the active EXTI line before servicing
 * multiple interrupt sources if more than one line is enabled.
 */
void EXTI15_10_IRQHandler(void){

	GPIO_IntrruptHandler(13);
	GPIO_TogglePin(GPIO_B, 14);
  }

