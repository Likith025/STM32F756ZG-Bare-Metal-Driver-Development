/*
 * stm32f7xx_uart_driver.h
 *
 *  Created on: Feb 21, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_UART_DRIVER_H_
#define INC_STM32F7XX_UART_DRIVER_H_

#include <stm32f756zg_reg.h>

#define USART_ISR_PE     (1U << 0)
#define USART_ISR_FE     (1U << 1)
#define USART_ISR_NF     (1U << 2)
#define USART_ISR_ORE    (1U << 3)
#define USART_ISR_IDLE   (1U << 4)
#define USART_ISR_RXNE   (1U << 5)
#define USART_ISR_TC     (1U << 6)
#define USART_ISR_TXE    (1U << 7)


#define USART_CR1_TXEIE 	(1U << 7)
#define USART_CR1_TCIE 	(1U << 6)
#define USART_CR1_RXNEIE 	(1U << 5)

#define USART_ICR_PECF     		(1U << 0)
#define USART_ICR_FECF     		(1U << 1)
#define USART_ICR_NCF     		(1U << 2)
#define USART_ICR_ORECF    		(1U << 3)
#define USART_ICR_IDLECF   		(1U << 4)
#define USART_ICR_TCCF     		(1U << 6)

#define USART_CLK 			16000000U  //must change to consider prescalar setup too






typedef enum{
    USART_FLAG_PECF    = USART_ICR_PECF,
    USART_FLAG_FECF    = USART_ICR_FECF,
    USART_FLAG_NCF    = USART_ICR_NCF,
    USART_FLAG_ORECF   = USART_ICR_ORECF,
    USART_FLAG_IDLECF  = USART_ICR_IDLECF,
    USART_FLAG_TCCF    =  USART_ICR_TCCF,
} USART_ClearFlags_t;

typedef enum{
    USART_FLAG_PE    = USART_ISR_PE,
    USART_FLAG_FE    = USART_ISR_FE,
    USART_FLAG_NF    = USART_ISR_NF,
    USART_FLAG_ORE   = USART_ISR_ORE,
    USART_FLAG_IDLE  = USART_ISR_IDLE,
    USART_FLAG_RXNE  = USART_ISR_RXNE,
    USART_FLAG_TC    = USART_ISR_TC,
    USART_FLAG_TXE   = USART_ISR_TXE
} USART_Flags_t;



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
    USART_StopBits_1=0,
    USART_StopBits_0_5,
    USART_StopBits_2,
    USART_StopBits_1_5,
}USART_StopBits_t;

typedef enum{
	USART_WordLen_8bits=0,
	USART_WordLen_9bits,
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


typedef enum{
	USART_FREE=0,
	USART_BUSY_IT_TX,
	USART_BUSY_IT_RX,
}USART_State_t;

typedef enum{
	USART_EVENT_TX_CMPL=0,
	USART_EVENT_RX_CMPL,
	USART_ERR_ORE,
}USART_CallBack_t;

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
	uint32_t tx_len;
	uint8_t* pTXBuffer;
	uint32_t rx_len;
	uint8_t* pRXBuffer;
	USART_State_t UASRT_Txstate;
	USART_State_t UASRT_Rxstate;
}USART_handler_t;



//API's//
int8_t USART_init(USART_handler_t *usart_handle,uint8_t enable);
int8_t USART_Control(USART_RegDef_t *pUSART,uint8_t CMD);
int8_t USART_clK_init(USART_RegDef_t *pUSART,uint8_t CMD);
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSART,USART_Flags_t FlagName);
void USART_CLearFlag(USART_RegDef_t *pUSART,USART_ClearFlags_t FlagName);
void USART_SendData(USART_handler_t *usart_handle,uint8_t* pTxData,uint16_t len);
void USART_SetBaudRate(USART_handler_t *usart_handle,uint16_t BaudRate);
void USART_ReadData(USART_handler_t *usart_handle,uint8_t* pRxData,uint16_t len);
void USART_ReadData_IT(USART_handler_t *usart_handle,uint8_t *pRxData,uint32_t len);
void USART_SendData_IT(USART_handler_t *usart_handle,uint8_t *pTxData,uint32_t Len);
void USART_IRQHandler(USART_handler_t *usart_handle);
void USART_ApplicationEventCallback(USART_handler_t *usart_handle, USART_CallBack_t);

#endif /* INC_STM32F7XX_UART_DRIVER_H_ */
