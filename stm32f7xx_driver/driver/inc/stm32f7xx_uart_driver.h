/*
 * stm32f7xx_uart_driver.h
 *
 *  Created on: Feb 21, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_UART_DRIVER_H_
#define INC_STM32F7XX_UART_DRIVER_H_

#include "stm32f7xx_driver_custom.h"

typedef enum
{
    USART_BAUD_1200   = 1200,
    USART_BAUD_2400   = 2400,
    USART_BAUD_9600   = 9600,
    USART_BAUD_19200  = 19200,
    USART_BAUD_38400  = 38400,
    USART_BAUD_57600  = 57600,
    USART_BAUD_115200 = 115200
} USART_BaudRate_t;


typedef enum{
	USART_MODE_RX=(1<<2),
	USART_MODE_TX=(1<<3),
	USART_MODE_TXRX=((1<<2)|(1<<3)),

}USART_MODE_t;

typedef enum{
	USART_Parity_None=0,
	USART_Parity_Odd,
	USART_Parity_Even,
}USART_Parity_t;

typedef enum{
    USART_StopBits_0_5=0,
    USART_StopBits_1,
    USART_StopBits_1_5,
    USART_StopBits_2,
}USART_StopBits_t;

typedef enum{
	USART_WordLen_8bits=0,
	USART_WordLen_9bits,
	USART_WordLen_7bits,
}USART_WordLen_t;

typedef enum{
	USART_HW_FlowCtrl_None=0,
	USART_HW_FlowCtrl_CTS,
	USART_HW_FlowCtrl_RTS,
	USART_HW_FlowCtrl_CTS_RTS,
}USART_HW_FlowCtrl_t;

typedef enum{
	USART_OverSampling_16=0,
	USART_OverSampling_8,
}USART_OverSampling_t;

typedef struct{
	USART_MODE_t USART_Mode;
	USART_OverSampling_t USART_OverSmapling ;
	USART_BaudRate_t USART_BaudRate;
	USART_StopBits_t	USART_StopBits;
	USART_WordLen_t USART_WordLength;
	USART_Parity_t USART_ParityControl;
	USART_HW_FlowCtrl_t USART_HWflowControl;
}USART_config_t;

typedef struct{
	USART_RegDef_t *pUSART;
	USART_config_t USART_config;
}USART_handler_t;



//API's//
int8_t USART_init(USART_handler_t *usart_handle,uint8_t enable);

#endif /* INC_STM32F7XX_UART_DRIVER_H_ */
