/**
 * @file stm32f7xx_driver_custom.h
 * @brief Register definitions and memory mapping for STM32F756ZG
 *
 * This file contains:
 * - Base address definitions for memory and peripherals
 * - Peripheral register structures
 * - NVIC register mapping
 *
 * This is intended for bare-metal driver development without HAL.
 *
 * @author Likith
 * @date Feb 17, 2026
 */

#ifndef INC_STM32F756ZG_REG_H_
#define INC_STM32F756ZG_REG_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @defgroup NVIC NVIC Register Definitions
 * @{
 */

/**
 * @brief Base address of NVIC registers
 */
#define NVIC_BASE_ADDR   ((volatile uint32_t*)(0xE000E100))

/**
 * @brief NVIC register structure
 */
typedef struct
{
    volatile uint32_t ISER[8];   /**< Interrupt Set Enable Registers */
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];   /**< Interrupt Clear Enable Registers */
    uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];   /**< Interrupt Set Pending Registers */
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];   /**< Interrupt Clear Pending Registers */
    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];   /**< Interrupt Active Bit Registers */
    uint32_t RESERVED4[56];
    volatile uint32_t IPR[60];   /**< Interrupt Priority Registers */

} NVIC_RegDef_t;

/** NVIC peripheral definition */
#define NVIC ((NVIC_RegDef_t*)(NVIC_BASE_ADDR))

/** @} */


/**
 * @defgroup MEMORY_BASE_ADDRESSES Memory Base Addresses
 * @{
 */
#define FLASH_BASE_ADDR     0x08000000UL  /**< Flash memory base address */
#define DTCM_BASE_ADDR      0x20000000UL  /**< DTCM base address */
#define SRAM1_BASE_ADDR     0x20010000UL  /**< SRAM1 base address */
#define SRAM2_BASE_ADDR     0x2004C000UL  /**< SRAM2 base address */

#define SRAM SRAM1_BASE_ADDR /**< Default SRAM */
/** @} */


/**
 * @defgroup BUS_BASE_ADDRESSES Bus Base Addresses
 * @{
 */
#define APB1_PERIPH_BASE_ADDR   0x40000000UL
#define APB2_PERIPH_BASE_ADDR   0x40010000UL
#define AHB1_PERIPH_BASE_ADDR   0x40020000UL
#define AHB2_PERIPH_BASE_ADDR   0x50000000UL
#define AHB3_PERIPH_BASE_ADDR   0x60000000UL
/** @} */


/**
 * @brief Generic enable/disable macros
 */
#define ENABLE  0x1UL
#define DISABLE 0x0UL


/**
 * @defgroup APB1_PERIPHERALS APB1 Peripheral Base Addresses
 * @{
 */
#define TIMMER_2_BASE_ADDR  (APB1_PERIPH_BASE_ADDR+0x0)
#define TIMMER_3_BASE_ADDR  (APB1_PERIPH_BASE_ADDR+0x400)

#define I2C1_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x5400)
#define I2C2_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x5800)
#define I2C3_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x5C00)

#define SPI2_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x3800)
#define SPI3_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x3C00)

#define USART2_BASE_ADDR    (APB1_PERIPH_BASE_ADDR+0x4400)
#define USART3_BASE_ADDR    (APB1_PERIPH_BASE_ADDR+0x4800)

#define UART4_BASE_ADDR     (APB1_PERIPH_BASE_ADDR+0x4C00)
#define UART5_BASE_ADDR     (APB1_PERIPH_BASE_ADDR+0x5000)

#define RTC_BASE_ADDR       (APB1_PERIPH_BASE_ADDR+0x2800)
#define CAN1_BASE_ADDR      (APB1_PERIPH_BASE_ADDR+0x6400)
/** @} */


/**
 * @defgroup APB2_PERIPHERALS APB2 Peripheral Base Addresses
 * @{
 */
#define EXTI_BASE_ADDR      (APB2_PERIPH_BASE_ADDR+0x3C00)
#define SYSCFG_BASE_ADDR    (APB2_PERIPH_BASE_ADDR+0x3800)
/** @} */


/**
 * @defgroup AHB1_PERIPHERALS AHB1 Peripheral Base Addresses
 * @{
 */
#define GPIOA_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x0)
#define GPIOB_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x400)
#define GPIOC_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x800)
#define GPIOD_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0xC00)
#define GPIOE_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x1000)
#define GPIOF_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x1400)
#define GPIOG_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x1800)
#define GPIOH_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x1C00)
#define GPIOI_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x2000)
#define GPIOJ_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x2400)
#define GPIOK_BASE_ADDR     (AHB1_PERIPH_BASE_ADDR+0x2800)

#define RCC_BASE_ADDR       (AHB1_PERIPH_BASE_ADDR+0x3800)
/** @} */


/**
 * @brief GPIO register definition structure
 */
typedef struct{
    volatile uint32_t MODER;     /**< GPIO mode register */
    volatile uint32_t OTYPER;    /**< GPIO output type register */
    volatile uint32_t OSPEEDR;   /**< GPIO speed register */
    volatile uint32_t PUPDR;     /**< GPIO pull-up/pull-down register */
    volatile uint32_t IDR;       /**< GPIO input data register */
    volatile uint32_t ODR;       /**< GPIO output data register */
    volatile uint32_t BSRR;      /**< GPIO bit set/reset register */
    volatile uint32_t LCKR;      /**< GPIO configuration lock register */
    volatile uint32_t AFR[2];    /**< GPIO alternate function registers */
} GPIO_RegDef_t;


/**
 * @brief RCC register definition structure
 */
typedef struct{
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t RESERVED_1;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t RESERVED_2[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t RESERVED_3;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
} RCC_RegDef_t;


/**
 * @brief USART register definition structure
 */
typedef struct{
    volatile uint32_t USART_CR1;
    volatile uint32_t USART_CR2;
    volatile uint32_t USART_CR3;
    volatile uint32_t USART_BRR;
    volatile uint32_t USART_GTPR;
    volatile uint32_t USART_RTOR;
    volatile uint32_t USART_RQR;
    volatile uint32_t USART_ISR;
    volatile uint32_t USART_ICR;
    volatile uint32_t USART_RDR;
    volatile uint32_t USART_TDR;
} USART_RegDef_t;


/**
 * @brief EXTI register definition structure
 */
typedef struct{
    volatile uint32_t EXTI_IMR;
    volatile uint32_t EXTI_EMR;
    volatile uint32_t EXTI_RTSR;
    volatile uint32_t EXTI_FTSR;
    volatile uint32_t EXTI_SWIER;
    volatile uint32_t EXTI_PR;
} EXTI_RegDef_t;


/**
 * @brief SYSCFG register definition structure
 */
typedef struct{
    volatile uint32_t SYSCFG_MEMRMP;
    volatile uint32_t SYSCFG_PMC;
    volatile uint32_t SYSCFG_EXTICR[4];
    uint32_t reserved[2];
    volatile uint32_t SYSCFG_CMPCR;
} SYSCFG_RegDef_t;


/**
 * @brief Timer register definition structure
 */
typedef struct{
    volatile uint32_t TIM_CR1;
    volatile uint32_t TIM_CR2;
    volatile uint32_t TIM_SMCR;
    volatile uint32_t TIM_DIER;
    volatile uint32_t TIM_SR;
    volatile uint32_t TIM_EGR;
    volatile uint32_t TIM_CCMR1;
    volatile uint32_t TIM_CCMR2;
    volatile uint32_t TIM_CCER;
    volatile uint32_t TIM_CNT;
    volatile uint32_t TIM_PSC;
    volatile uint32_t TIM_ARR;
    uint32_t RESERVED_1;
    volatile uint32_t TIM_CCR1;
    volatile uint32_t TIM_CCR2;
    volatile uint32_t TIM_CCR3;
    volatile uint32_t TIM_CCR4;
} TIMER_RegDef_t;

#include "stm32f7xx_gpio_driver.h"

#endif /* INC_STM32F756ZG_REG_H_ */
