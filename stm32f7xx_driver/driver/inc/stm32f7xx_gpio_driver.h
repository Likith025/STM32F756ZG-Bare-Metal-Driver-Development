/*
 * stm32_gpio_driver.h
 *
 *  Created on: Feb 17, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_GPIO_DRIVER_H_
#define INC_STM32F7XX_GPIO_DRIVER_H_

#include "stm32f7xx_driver_custom.h"
#include<stdint.h>

typedef enum{
	GPIO_MODE_INPUT=0,
	GPIO_MODE_OUTPUT,
	GPIO_MODE_ALTERNATE_FUN,
	GPIO_MODE_ANALOG,

}GPIO_MODE_t;

typedef enum{
	GPIO_OPTYPE_PUSH_PULL=0,
	GPIO_OPTYPE_OPEN_DRAIN
}GPIO_OPTYPE_t;

typedef enum{
	GPIO_OPSPEED_LOW=0,
	GPIO_OPSPEED_MEDIUM,
	GPIO_OPSPEED_HIGH,
	GPIO_OPSPEED_VERY_HIGH,
}GPIO_OPSPEED_t;

typedef enum{
	GPIO_PUPD_NO=0,
	GPIO_PUPD_PULL_UP,
	GPIO_PUPD_PUSH_DOWN,
	GPIO_PUPD_RESERVED

}GPIO_PUPD_t;



typedef struct{
	uint8_t GPIO_PinNumber;  //to select pin number of given port 0-15
	GPIO_MODE_t	GPIO_PinMode;	// to select pin mode (input,output,alternate function, analog)
	GPIO_OPTYPE_t GPIO_PinOutType;	// to select output pin mode (push-pull,open drain)
	GPIO_OPSPEED_t GPIO_PinOutSpeed;	// to select output pin speed (low,medium,high,very high speed)
	GPIO_PUPD_t GPIO_PinPushPullResistor; // to select input pin type( no push-up/pull-down,pull-up,push-down,reserved)
	uint8_t GPIO_PinAltFun;		//to config alternate mode
}GPIO_config_t;


typedef struct{
	GPIO_RegDef_t *pGPIOx;
	GPIO_config_t GPIO_pin_config;


}GPIO_handler_t;


//list of GPIO API's
int8_t GPIO_clk_init(const GPIO_RegDef_t* pGPIO_x,uint8_t enable);

int8_t GPIO_init(const GPIO_handler_t* GPIO_xHandler);
int8_t GPIO_deinit(GPIO_RegDef_t* pGPIO_x);

uint8_t GPIO_ReadPin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber);
uint16_t GPIO_ReadPort(GPIO_RegDef_t* pGPIO_x);

int8_t GPIO_WritePin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber,uint8_t value);
int8_t GPIO_WritePort(GPIO_RegDef_t* pGPIO_x,uint16_t value);
int8_t GPIO_TogglePin(GPIO_RegDef_t* pGPIO_x,uint8_t PinNumber);

void GPIO_IntrruptConfig(uint8_t IRQNumber,uint8_t IRQPriority, uint8_t enable);
void GPIO_IntrruptHandler(uint8_t PinNumber);


#endif /* INC_STM32F7XX_GPIO_DRIVER_H_ */
