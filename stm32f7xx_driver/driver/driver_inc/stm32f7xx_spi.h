/*
 * stm32f7xx_spi.h
 *
 *  Created on: 26-Apr-2026
 *      Author: likith
 */

#ifndef DRIVER_INC_STM32F7XX_SPI_H_
#define DRIVER_INC_STM32F7XX_SPI_H_

#include "stm32f756zg_reg.h"

#define SPI_CR1_CPHA_Pos      0U
#define SPI_CR1_CPOL_Pos      1U
#define SPI_CR1_MSTR_Pos      2U
#define SPI_CR1_BR_Pos        3U
#define SPI_CR1_SPE_Pos		  6U
#define SPI_CR1_LSBFIRST_Pos  7U
#define SPI_CR1_SSI_Pos       8U
#define SPI_CR1_SSM_Pos       9U
#define SPI_CR1_RXONLY_Pos	  10U
#define SPI_CR1_BIDI_Pos	  15U

#define SPI_CR2_DS_Pos       8U



typedef enum{
	Slave_Mode=0,
	Master_Mode,
}SPI_DeviceMode_t;

typedef enum{
//	SPI_BUS_Simplex_TXonly, -> equivalent to removing mosi pin
	SPI_BUS_Simplex_RXonly,
	SPI_BUS_Half_Duplex,
	SPI_BUS_Full_Duplex,
}SPI_BusMode_t;

typedef enum{
	SPI_Hardware_SlaveSelect=0,
	SPI_Software_SlaveSelect,
}SPI_SlaveSelectMode_t;

typedef enum{
	SPI_CLK_DIV2=0,
	SPI_CLK_DIV4,
	SPI_CLK_DIV8,
	SPI_CLK_DIV16,
	SPI_CLK_DIV32,
	SPI_CLK_DIV64,
	SPI_CLK_DIV128,
	SPI_CLK_DIV256,
}SPI_ClkPrescaler_t;

typedef enum{
	SPI_MSB_First=0,
	SPI_LSB_First,
}SPI_ByteOrder_t;

typedef enum{
	SPI_CaptureFirstEdge=0,
	SPI_CaptureSecondEdge,
}SPI_ClockPhase_t;

typedef enum{
	SPI_ClkLow=0,
	SPI_ClkHigh,
}SPI_ClockPolarity_t;


typedef struct{
	SPI_DeviceMode_t SPI_DeviceMode;
	SPI_BusMode_t SPI_BusConfig;
	SPI_ClkPrescaler_t SPI_ClkSpeed;
	uint8_t SPI_FrameSize;
	SPI_ClockPolarity_t SPI_CPOL;
	SPI_ClockPhase_t SPI_CPHA;
	SPI_SlaveSelectMode_t SPI_SSM;
	SPI_ByteOrder_t	SPI_ByteOrder;

}SPI_config_t;


typedef struct{
	SPI_RegDef_t *pspi;
	SPI_config_t SPIConfig;

}spi_handler_t;

Status_t SPI_init(spi_handler_t* spi_handle);
Status_t SPI_clk_init(const SPI_RegDef_t* spi_ptr,const uint8_t enable);
void SPI_Start(SPI_RegDef_t* pSPI,uint8_t enable);
#endif /* DRIVER_INC_STM32F7XX_SPI_H_ */
