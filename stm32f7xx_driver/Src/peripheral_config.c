/*
 * peripheral_config.c
 *
 *  Created on: Mar 1, 2026
 *      Author: likith
 */
#include "stm32f7xx_gpio_driver.h"
#include "stm32f7xx_uart_driver.h"
#include "peripheral_config.h"


USART_handler_t g_usart3 = {0};
GPIO_handler_t g_led3 = {0};
GPIO_handler_t g_button = {0};
GPIO_handler_t g_usart3_tx = {0};
GPIO_handler_t g_usart3_rx = {0};


void board_init(){
	user_led3_init(&g_led3);
	user_button_init(&g_button);
	usart3_tx(&g_usart3_tx);
	usart3_rx(&g_usart3_rx);
	usart3_init(&g_usart3);
}

void user_led3_init(GPIO_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure LED3 on PB14
    handler->pGPIOx = GPIO_B;
    handler->GPIO_pin_config.GPIO_PinMode = GPIO_MODE_OUTPUT;
    handler->GPIO_pin_config.GPIO_PinNumber = 14;
    handler->GPIO_pin_config.GPIO_PinOutType = GPIO_OPTYPE_PUSH_PULL;
    handler->GPIO_pin_config.GPIO_PinOutSpeed = GPIO_OPSPEED_LOW;
    handler->GPIO_pin_config.GPIO_PinPushPullResistor = GPIO_PUPD_NO;

    GPIO_clk_init(GPIO_B, ENABLE);
    GPIO_init(handler);
}

void user_button_init(GPIO_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure button on PC13
    handler->pGPIOx = GPIO_C;
    handler->GPIO_pin_config.GPIO_PinMode = GPIO_MODE_INPUT;
    handler->GPIO_pin_config.GPIO_PinNumber = 13;
    handler->GPIO_pin_config.GPIO_PinPushPullResistor = GPIO_PUPD_NO;

    GPIO_clk_init(GPIO_C, ENABLE);
    GPIO_init(handler);
}

void usart3_init(USART_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure USART3
    handler->pUSART = USART_3;
    handler->USART_config.USART_Mode = USART_MODE_TXRX;
    handler->USART_config.USART_WordLength = USART_WordLen_8bits;
    handler->USART_config.USART_OverSmapling = USART_OverSampling_16;
    handler->USART_config.USART_StopBits = USART_StopBits_1;
    handler->USART_config.USART_ParityControl = USART_Parity_None;
    handler->USART_config.USART_HWflowControl = USART_HW_FlowCtrl_None;

    USART_init(handler, ENABLE);
}

void usart3_tx(GPIO_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure TX pin on PD8
    handler->pGPIOx = GPIO_D;
    handler->GPIO_pin_config.GPIO_PinMode = GPIO_MODE_ALTERNATE_FUN;
    handler->GPIO_pin_config.GPIO_PinNumber = 8;
    handler->GPIO_pin_config.GPIO_PinOutType = GPIO_OPTYPE_PUSH_PULL;
    handler->GPIO_pin_config.GPIO_PinOutSpeed = GPIO_OPSPEED_LOW;
    handler->GPIO_pin_config.GPIO_PinPushPullResistor = GPIO_PUPD_NO;
    handler->GPIO_pin_config.GPIO_PinAltFun = 7;

    GPIO_clk_init(GPIO_D, ENABLE);
    GPIO_init(handler);
}

void usart3_rx(GPIO_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure RX pin on PD9
    handler->pGPIOx = GPIO_D;
    handler->GPIO_pin_config.GPIO_PinMode = GPIO_MODE_ALTERNATE_FUN;
    handler->GPIO_pin_config.GPIO_PinNumber = 9;
    handler->GPIO_pin_config.GPIO_PinOutType = GPIO_OPTYPE_PUSH_PULL;
    handler->GPIO_pin_config.GPIO_PinOutSpeed = GPIO_OPSPEED_LOW;
    handler->GPIO_pin_config.GPIO_PinPushPullResistor = GPIO_PUPD_NO;
    handler->GPIO_pin_config.GPIO_PinAltFun = 7;

    GPIO_clk_init(GPIO_D, ENABLE);
    GPIO_init(handler);
}
