/*
 * stm32f7xx_driver_custom.h
 *
 *  Created on: Feb 17, 2026
 *      Author: likith
 */

#ifndef INC_STM32F7XX_DRIVER_CUSTOM_H_
#define INC_STM32F7XX_DRIVER_CUSTOM_H_
#include <stdint.h>
#include <stddef.h>


//Defining BASE address of memory regions
#define FLASH_BASE_ADDR					0x08000000UL  //Base Address of flash memory
#define DTCM_BASE_ADDR                  0x20000000UL	//Base Address of DTCM
#define SRAM1_BASE_ADDR					0x20010000UL //Base Address of SRAM1 memory
#define SRAM2_BASE_ADDR					0x2004C000UL //Base Address of SRAM2 memory

#define SRAM								SRAM1_BASE_ADDR

//Defining BASE address of BUS regions

#define APB1_PERIPH_BASE_ADDR			0x40000000UL	//Base address of APB1 bus peripherals
#define APB2_PERIPH_BASE_ADDR			0x40010000UL	//Base address of APB2 bus peripherals
#define	AHB1_PERIPH_BASE_ADDR	        0x40020000UL	//Base address of AHB1 bus peripherals
#define	AHB2_PERIPH_BASE_ADDR		    0x50000000UL	//Base address of AHB2 bus peripherals
#define AHB3_PERIPH_BASE_ADDR			0x60000000UL	//Base address of AHB3 bus peripherals



//generic macros
#define ENABLE 	0x1UL
#define DISABLE 0x0UL



//Defining BASE address of peripherals of APB1 BUS regions
// TIM2,3, RTC,  I2C1,I2C2,I2C3,  SPI2,SPI3,  USART2,3   UART4,5

#define TIMMER_2_BASE_ADDR		(APB1_PERIPH_BASE_ADDR+0x0)
#define TIMMER_3_BASE_ADDR		(APB1_PERIPH_BASE_ADDR+0x400)

#define	I2C1_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x5400)
#define I2C2_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x5800)
#define I2C3_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x5C00)

#define SPI2_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x3800)
#define SPI3_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x3C00)

#define USART2_BASE_ADDR		(APB1_PERIPH_BASE_ADDR+0x4400)
#define USART3_BASE_ADDR		(APB1_PERIPH_BASE_ADDR+0x4800)

#define UART4_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x4C00)
#define UART5_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x5000)

#define RTC_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x2800)
#define CAN1_BASE_ADDR			(APB1_PERIPH_BASE_ADDR+0x6400)


//Defining BASE address of peripherals of APB2 BUS regions
#define	EXTI_BASE_ADDR		(APB2_PERIPH_BASE_ADDR+0x3C00)
#define SYSCFG_BASE_ADDR	(APB2_PERIPH_BASE_ADDR+0x3800)

//defining base address of peripherals of AHB1 bus
#define GPIOA_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x0)
#define GPIOB_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x400)
#define GPIOC_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x800)
#define GPIOD_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0xC00)
#define GPIOE_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x1000)
#define GPIOF_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x1400)
#define GPIOG_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x1800)
#define GPIOH_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x1C00)
#define GPIOI_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x2000)
#define GPIOJ_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x2400)
#define GPIOK_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x2800)

#define RCC_BASE_ADDR		(AHB1_PERIPH_BASE_ADDR+0x3800)


//Defining Base address of GPIO peripherals
#define GPIO_A	((GPIO_RegDef_t*)GPIOA_BASE_ADDR)
#define GPIO_B	((GPIO_RegDef_t*)GPIOB_BASE_ADDR)
#define GPIO_C	((GPIO_RegDef_t*)GPIOC_BASE_ADDR)
#define GPIO_D	((GPIO_RegDef_t*)GPIOD_BASE_ADDR)
#define GPIO_E	((GPIO_RegDef_t*)GPIOE_BASE_ADDR)
#define GPIO_F	((GPIO_RegDef_t*)GPIOF_BASE_ADDR)
#define GPIO_G	((GPIO_RegDef_t*)GPIOG_BASE_ADDR)
#define GPIO_H	((GPIO_RegDef_t*)GPIOH_BASE_ADDR)
#define GPIO_I	((GPIO_RegDef_t*)GPIOI_BASE_ADDR)
#define GPIO_J	((GPIO_RegDef_t*)GPIOJ_BASE_ADDR)
#define GPIO_K	((GPIO_RegDef_t*)GPIOK_BASE_ADDR)


#define RCC 	((RCC_RegDef_t*)RCC_BASE_ADDR)


//Creating structures for peripheral registers

typedef struct{
	volatile uint32_t MODER; // GPIO port mode register
	volatile uint32_t OTYPER; // GPIO port o/p type register
	volatile uint32_t OSPEEDR; // GPIO port o/p speed register
	volatile uint32_t PUPDR; //GPIO port pull-up/push-down register
	volatile uint32_t IDR;	// GPIO port i/p data register
	volatile uint32_t ODR;	//GPIO port o/p data register
	volatile uint32_t BSRR; //GPIO port set/reset register
	volatile uint32_t LCKR;	//GPIO port config lock register
	volatile uint32_t AFR[2]; // GPIO alternate function register
	}GPIO_RegDef_t;


typedef struct{
	volatile uint32_t CR;			//RCC clock control register
	volatile uint32_t PLLCFGR;		// RCC PLL config register
	volatile uint32_t CFGR;			// RCC clk config register
	volatile uint32_t CIR;			//RCC clk interrupt register
	volatile uint32_t AHB1RSTR;		//RCC AHB1 peripheral reset register
	volatile uint32_t AHB2RSTR;		//RCC AHB2 peripheral reset register
	volatile uint32_t AHB3RSTR;		//RCC AHB3 peripheral reset register
	uint32_t	RESERVED_1;
	volatile uint32_t APB1RSTR;		//RCC APB1 peripheral reset register
	volatile uint32_t APB2RSTR;		//RCC APB2 peripheral reset register
	uint32_t	RESERVED_2[2];
	volatile uint32_t AHB1ENR;		//RCC AHB1 peripheral clock register
	volatile uint32_t AHB2ENR;		//RCC AHB2 peripheral clock register
	volatile uint32_t AHB3ENR;		//RCC AHB3 peripheral clock register
	uint32_t	RESERVED_3;
	volatile uint32_t APB1ENR;		//RCC APB1 peripheral clock enable register
	volatile uint32_t APB2ENR;		//RCC APB1 peripheral clock enable register
	uint32_t	RESERVED_4[2];
	volatile uint32_t AHB1LPENR;	//RCC AHB1 peripheral clock enable in low power mode register
	volatile uint32_t AHB2LPENR;	//RCC AHB2 peripheral clock enable in low power mode register
	volatile uint32_t AHB3LPENR;	//RCC AHB3 peripheral clock enable in low power mode register
	uint32_t	RESERVED_5;
	volatile uint32_t APB1LPENR;	//RCC APB1 peripheral clock enable in low power mode register
	volatile uint32_t APB2LPENR;	//RCC APB1 peripheral clock enable in low power mode register
	uint32_t	RESERVED_6[2];
	volatile uint32_t BDCR;			//RCC Backup domain control register
	volatile uint32_t CSR;			//RCC clock control & status register
	uint32_t	RESERVED_7[2];
	volatile uint32_t SSCGR;		//RCC spread spectrum clock generation register
	volatile uint32_t PLLI2SCFGR;	//RCC PLLI2S configuration register
	volatile uint32_t PLLSAICFGR;	//RCC PLLSAI configuration register
	volatile uint32_t DCKCFGR1;		//RCC Dedicated Clocks configuration register1
	volatile uint32_t DCKCFGR2;		//RCC Dedicated Clocks configuration register2
}RCC_RegDef_t;






#endif /* INC_STM32F7XX_DRIVER_CUSTOM_H_ */
