/*
 * stm32f7xx_timer_driver.h
 *
 *  Created on: Mar 29, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_TIMER_DRIVER_H_
#define INC_STM32F7XX_TIMER_DRIVER_H_

#include <stm32f756zg_reg.h>

#define		TIMER_SR_UIF	(1<<0)
#define		TIMER_SR_CC1IF	(1<<1)
#define		TIMER_SR_CC2IF	(1<<2)
#define		TIMER_SR_CC3IF	(1<<3)
#define		TIMER_SR_CC4IF	(1<<4)
#define		TIMER_SR_TIF	(1<<6)
#define		TIMER_SR_CC1OF	(1<<9)
#define		TIMER_SR_CC2OF	(1<<10)
#define		TIMER_SR_CC3OF	(1<<11)
#define		TIMER_SR_CC4OF	(1<<12)

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

typedef enum{
	UpCounter=0,
	DownCounter,

}Timer_direction_t;

typedef enum{
	Timer_Flag_UIF=TIMER_SR_UIF,
	Timer_Flag_CC1IF=TIMER_SR_CC1IF,
	Timer_Flag_CC2IF=TIMER_SR_CC2IF,
	Timer_Flag_CC3IF=TIMER_SR_CC3IF,
	Timer_Flag_CC4IF=TIMER_SR_CC4IF,
	Timer_Flag_TIF=TIMER_SR_TIF,
	Timer_Flag_CC1OF=TIMER_SR_CC1OF,
	Timer_Flag_CC2OF=TIMER_SR_CC2OF,
	Timer_Flag_CC3OF=TIMER_SR_CC3OF,
	Timer_Flag_CC4OF=TIMER_SR_CC4OF,
}Timer_flags_t;

typedef enum{
	CH1=1,
	CH2,
	CH3,
	CH4,
}Timer_channel_t;


//API prototypes
void TimerInit(TIMER_handler_t* timer_handle);
void TimerControl(TIMER_handler_t* timer_handle,uint16_t state);
uint32_t TimerGetCount(TIMER_handler_t* timer_handle);
uint8_t TimerGetFlagStatus(TIMER_handler_t* timer_handle,Timer_flags_t Flag);
void TimerResetFlag(TIMER_handler_t* timer_handle,Timer_flags_t Flag);
void TimerSetFrequency(TIMER_handler_t* timer_handle,uint32_t frequency);
void TimerPWM_init(TIMER_handler_t* timer_handle,Timer_channel_t Channel);
void TimerPWM_DutyCycle(TIMER_handler_t* timer_handle,Timer_channel_t Channel,uint8_t DutyCycle);
#endif /* INC_STM32F7XX_TIMER_DRIVER_H_ */
