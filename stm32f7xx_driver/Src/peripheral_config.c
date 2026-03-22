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




void led_setup_blue(GPIO_handler_t *LED){
	LED->pGPIOx=GPIO_B;
	LED->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_OUTPUT;
	LED->GPIO_pin_config.GPIO_PinNumber=7;
	LED->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
	LED->GPIO_pin_config.GPIO_PinOutType=GPIO_OPTYPE_PUSH_PULL;
	GPIO_init(LED);

}

void led_setup_red(GPIO_handler_t *LED){
	LED->pGPIOx=GPIO_B;
	LED->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_OUTPUT;
	LED->GPIO_pin_config.GPIO_PinNumber=14;
	LED->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
	LED->GPIO_pin_config.GPIO_PinOutType=GPIO_OPTYPE_PUSH_PULL;
	GPIO_init(LED);

}

void button_setup(GPIO_handler_t *BUTTON){
	BUTTON->pGPIOx=GPIO_C;
	BUTTON->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_INPUT;
	BUTTON->GPIO_pin_config.GPIO_PinNumber=13;
	BUTTON->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
	GPIO_init(BUTTON);
}

void button_interrupt_setup(GPIO_handler_t *BUTTON){
	BUTTON->pGPIOx=GPIO_C;
	BUTTON->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_IT_FALLING;
	BUTTON->GPIO_pin_config.GPIO_PinNumber=13;
	BUTTON->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
	GPIO_init(BUTTON);

	/* Clear stale EXTI pending flag before unmasking NVIC */
	GPIO_ClearPendingFlag(13);
	GPIO_IntrruptConfig(IRQ_NO_EXTI10_15,15,ENABLE);
}

void usart3_init(USART_handler_t *handler){
    if(handler == NULL) return;
    
    // Configure USART3
    handler->pUSART = USART_3;
    handler->USART_config.USART_Mode = USART_MODE_TXRX;
    handler->USART_config.USART_WordLength = USART_WordLen_8bits;
    handler->USART_config.USART_OverSmapling = USART_OverSampling_8;
    handler->USART_config.USART_StopBits = USART_StopBits_1;
    handler->USART_config.USART_ParityControl = USART_Parity_None;
    handler->USART_config.USART_HWflowControl = USART_HW_FlowCtrl_None;
    handler->USART_config.USART_BaudRate=USART_BAUD_9600;
    handler->UASRT_Txstate=USART_FREE;
    handler->UASRT_Rxstate=USART_FREE;

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
    handler->GPIO_pin_config.GPIO_PinOutSpeed = GPIO_OPSPEED_LOW;
    handler->GPIO_pin_config.GPIO_PinPushPullResistor = GPIO_PUPD_NO;
    handler->GPIO_pin_config.GPIO_PinAltFun = 7;

    GPIO_clk_init(GPIO_D, ENABLE);
    GPIO_init(handler);
}
