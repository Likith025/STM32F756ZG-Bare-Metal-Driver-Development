/*
 * stm32f7xx_timer_driver.h
 *
 *  Created on: Mar 29, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_TIMER_DRIVER_H_
#define INC_STM32F7XX_TIMER_DRIVER_H_

#include <stm32f756zg_reg.h>

//struct defines
typedef struct{
	uint16_t PreScaler;
	uint16_t Period;
	uint8_t direction;

}TIMER_config_t;


typedef struct{
	TIMER_RegDef_t *pTimer;
	TIMER_config_t TimerConfig;
}TIMER_handler_t;


//API prototypes
void TimerInit(TIMER_handler_t* timer_handle);
void TimerControl(TIMER_handler_t* timer_handle,uint16_t state);
uint32_t TimerGetCount(TIMER_handler_t* timer_handle);
#endif /* INC_STM32F7XX_TIMER_DRIVER_H_ */
