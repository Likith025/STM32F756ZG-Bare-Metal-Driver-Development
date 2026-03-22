/*
 * peripheral_config.h
 *
 *  Created on: Mar 1, 2026
 *      Author: likith
 */

#ifndef PERIPHERAL_CONFIG_H_
#define PERIPHERAL_CONFIG_H_

#include "stm32f7xx_uart_driver.h"
#include "stm32f7xx_gpio_driver.h"
#include "nvic.h"


extern USART_handler_t g_usart3;
extern GPIO_handler_t g_led3;
extern GPIO_handler_t g_button;
extern GPIO_handler_t g_usart3_tx;
extern GPIO_handler_t g_usart3_rx;


// Initialization functions - accept handler pointers
void led_setup_blue(GPIO_handler_t *LED);
void led_setup_red(GPIO_handler_t *LED);
void button_setup(GPIO_handler_t *BUTTON);
void button_interrupt_setup(GPIO_handler_t *BUTTON);
void usart3_init(USART_handler_t *handler);
void usart3_tx(GPIO_handler_t *handler);
void usart3_rx(GPIO_handler_t *handler);


#endif /* PERIPHERAL_CONFIG_H_ */
