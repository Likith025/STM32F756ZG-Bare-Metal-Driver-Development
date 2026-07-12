	/*
 * peripheral_config.c
 *
 *  Created on: Mar 1, 2026
 *      Author: likith
 */
#include "stm32f7xx_gpio_driver.h"
#include "stm32f7xx_uart_driver.h"
#include "stm32f7xx_timer_driver.h"
#include "peripheral_config.h"
#include "stm32f7xx_spi.h"


USART_handler_t g_usart3 = {0};
GPIO_handler_t g_led3 = {0};
GPIO_handler_t g_button = {0};
GPIO_handler_t g_usart3_tx = {0};
GPIO_handler_t g_usart3_rx = {0};
GPIO_handler_t g_timer2_ch1={0};
TIMER_handler_t g_timer2={0};



static void usart3_rx(GPIO_handler_t *handler);
static void usart3_tx(GPIO_handler_t *handler);

/**
 * @brief Initializes the blue user LED.
 *
 * Configures the GPIO pin connected to the blue LED as a low-speed
 * push-pull output and initializes the corresponding GPIO peripheral.
 *
 * @param[out] LED
 * Pointer to the GPIO handler associated with the blue LED.
 */
void led_setup_blue(GPIO_handler_t *LED){
	LED->pGPIOx=GPIO_B;
	LED->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_OUTPUT;
	LED->GPIO_pin_config.GPIO_PinNumber=7;
	LED->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
	LED->GPIO_pin_config.GPIO_PinOutType=GPIO_OPTYPE_PUSH_PULL;
	GPIO_init(LED);

}


/**
 * @brief Initializes the red user LED.
 *
 * Configures the GPIO pin connected to the red LED as a low-speed
 * push-pull output and initializes the corresponding GPIO peripheral.
 *
 * @param[out] LED
 * Pointer to the GPIO handler associated with the red LED.
 */
void led_setup_red(GPIO_handler_t *LED){
	LED->pGPIOx=GPIO_B;
	LED->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_OUTPUT;
	LED->GPIO_pin_config.GPIO_PinNumber=14;
	LED->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
	LED->GPIO_pin_config.GPIO_PinOutType=GPIO_OPTYPE_PUSH_PULL;
	GPIO_init(LED);

}

/**
 * @brief Initializes the user push button as a GPIO input.
 *
 * Configures the button GPIO pin as a digital input with no internal
 * pull-up or pull-down resistor.
 *
 * @param[out] BUTTON
 * Pointer to the GPIO handler associated with the push button.
 */
void button_setup(GPIO_handler_t *BUTTON){
	BUTTON->pGPIOx=GPIO_C;
	BUTTON->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_INPUT;
	BUTTON->GPIO_pin_config.GPIO_PinNumber=13;
	BUTTON->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
	GPIO_init(BUTTON);
}

/**
 * @brief Configures the user push button as an external interrupt source.
 *
 * Initializes the button GPIO pin for falling-edge triggered EXTI operation,
 * clears any pending interrupt flag, and enables the corresponding NVIC
 * interrupt.
 *
 * @param[out] BUTTON
 * Pointer to the GPIO handler associated with the push button.
 *
 * @note
 * This function configures GPIO, EXTI, SYSCFG, and NVIC for button interrupts.
 */
void button_interrupt_setup(GPIO_handler_t *BUTTON){
	BUTTON->pGPIOx=GPIO_C;
	BUTTON->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_IT_FALLING;
	BUTTON->GPIO_pin_config.GPIO_PinNumber=13;
	BUTTON->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
	GPIO_init(BUTTON);

	/* Clear stale EXTI pending flag before unmasking NVIC */
	GPIO_ClearPendingFlag(13);
	IntrruptConfig(IRQ_NO_EXTI10_15,15,ENABLE);
}

/**
 * @brief Initializes USART3 peripheral.
 *
 * Configures USART3 with the desired communication parameters and
 * initializes its associated TX and RX GPIO pins.
 *
 * @param[out] handler
 * Pointer to the USART handler structure.
 *
 * @note
 * USART3 TX and RX pins are configured automatically by this function.
 */
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
    usart3_tx(&g_usart3_tx);
    usart3_rx(&g_usart3_rx);
}

/**
 * @brief Configures the USART3 TX pin.
 *
 * Initializes the GPIO pin connected to the USART3 transmit line in
 * Alternate Function mode.
 *
 * @param[out] handler
 * Pointer to the GPIO handler for the TX pin.
 *
 * @note
 * This function is intended for internal use only.
 */
static void usart3_tx(GPIO_handler_t *handler){
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

/**
 * @brief Configures the USART3 RX pin.
 *
 * Initializes the GPIO pin connected to the USART3 receive line in
 * Alternate Function mode.
 *
 * @param[out] handler
 * Pointer to the GPIO handler for the RX pin.
 *
 * @note
 * This function is intended for internal use only.
 */
static void usart3_rx(GPIO_handler_t *handler){
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

/**
 * @brief Configures TIM2 Channel 1 GPIO pin.
 *
 * Initializes the GPIO pin associated with TIM2 Channel 1 in Alternate
 * Function mode for timer operation.
 *
 * @param[out] handler
 * Pointer to the GPIO handler for TIM2 Channel 1.
 */
void timer2ch1(GPIO_handler_t* handler){
	handler->pGPIOx=GPIO_A;
	handler->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_ALTERNATE_FUN;
	handler->GPIO_pin_config.GPIO_PinNumber=5;
	handler->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
	handler->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
	handler->GPIO_pin_config.GPIO_PinAltFun=1;


    GPIO_clk_init(GPIO_A, ENABLE);
    GPIO_init(handler);
}

/**
 * @brief Initializes TIM2.
 *
 * Configures and initializes Timer 2 with the specified counting direction.
 *
 * @param[out] handler
 * Pointer to the timer handler.
 *
 * @param[in] direction
 * Timer counting direction (up-counting or down-counting).
 */
void timer2_setup(TIMER_handler_t* handler,Timer_direction_t direction){
	handler->TimerConfig.direction=direction;
	TimerInit(handler);
}

//
//void spi2_CS_pin(GPIO_handler_t* handler){
//	handler->pGPIOx=GPIO_B;
//	handler->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_ALTERNATE_FUN;
//	handler->GPIO_pin_config.GPIO_PinNumber=12;
//	handler->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
//	handler->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
//	handler->GPIO_pin_config.GPIO_PinAltFun=5;
//
//
//    GPIO_clk_init(GPIO_B, ENABLE);
//    GPIO_init(handler);
//}
//void spi2_Clk_pin(GPIO_handler_t* handler){
//	handler->pGPIOx=GPIO_B;
//	handler->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_ALTERNATE_FUN;
//	handler->GPIO_pin_config.GPIO_PinNumber=13;
//	handler->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
//	handler->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
//	handler->GPIO_pin_config.GPIO_PinAltFun=5;
//
//
//    GPIO_clk_init(GPIO_B, ENABLE);
//    GPIO_init(handler);
//}
//void spi2_MISO_pin(GPIO_handler_t* handler){
//	handler->pGPIOx=GPIO_B;
//	handler->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_ALTERNATE_FUN;
//	handler->GPIO_pin_config.GPIO_PinNumber=14;
//	handler->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
//	handler->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
//	handler->GPIO_pin_config.GPIO_PinAltFun=5;
//
//
//    GPIO_clk_init(GPIO_B, ENABLE);
//    GPIO_init(handler);
//}
//void spi2_MOSI_pin(GPIO_handler_t* handler){
//	handler->pGPIOx=GPIO_B;
//	handler->GPIO_pin_config.GPIO_PinMode=GPIO_MODE_ALTERNATE_FUN;
//	handler->GPIO_pin_config.GPIO_PinNumber=15;
//	handler->GPIO_pin_config.GPIO_PinOutSpeed=GPIO_OPSPEED_LOW;
//	handler->GPIO_pin_config.GPIO_PinPushPullResistor=GPIO_PUPD_NO;
//	handler->GPIO_pin_config.GPIO_PinAltFun=5;
//
//
//    GPIO_clk_init(GPIO_B, ENABLE);
//    GPIO_init(handler);
//}
void spi2_init(spi_handler_t SPI_Handle){
	SPI_Handle.SPIConfig.SPI_BusConfig=1;
	SPI_Handle.SPIConfig.SPI_CPHA=1;
	SPI_Handle.SPIConfig.SPI_CPOL=1;

}
