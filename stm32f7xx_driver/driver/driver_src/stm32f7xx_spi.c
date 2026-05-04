/*
 * stm32f7xx_spi.c
 *
 *  Created on: 26-Apr-2026
 *      Author: likith
 */

#include "stm32f7xx_spi.h"

Status_t SPI_init(spi_handler_t* spi_handle)
{
	Status_t state=STATUS_OK;
	if(spi_handle==NULL)
	{
		state=STATUS_ERROR;
	}
	else{
		SPI_config_t* spi_config=&spi_handle->SPIConfig;
		SPI_RegDef_t* spi_ptr=spi_handle->pspi;
		state=SPI_clk_init(spi_ptr,ENABLE);
		uint32_t temp_reg=0;
		//setting up clk phase
		temp_reg|=(spi_config->SPI_CPHA<<SPI_CR1_CPHA_Pos);
		//setting up clk polarity
		temp_reg|=(spi_config->SPI_CPOL<<SPI_CR1_CPOL_Pos);
		//setting up device mode
		temp_reg|=(spi_config->SPI_DeviceMode<<SPI_CR1_MSTR_Pos);
		//setting up clk speed
		temp_reg|=(spi_config->SPI_ClkSpeed<<SPI_CR1_BR_Pos);
		//setting up byte order
		temp_reg|=(spi_config->SPI_ByteOrder<<SPI_CR1_LSBFIRST_Pos);
		//setting up slave select
		temp_reg|=(spi_config->SPI_SSM<<SPI_CR1_SSM_Pos);
		if(spi_config->SPI_SSM==SPI_Software_SlaveSelect){
			temp_reg|=(1<<SPI_CR1_SSI_Pos);
		}

		//setting up bus mode
		if(spi_config->SPI_BusConfig==SPI_BUS_Full_Duplex)
		{
			temp_reg&=~(1<<SPI_CR1_BIDI_Pos);
			temp_reg&=~(1<<SPI_CR1_RXONLY_Pos);

		}
		else if(spi_config->SPI_BusConfig==SPI_BUS_Half_Duplex)
		{
			temp_reg|=(1<<SPI_CR1_BIDI_Pos);
		}
		else
		{
			temp_reg&=~(1<<SPI_CR1_BIDI_Pos);
			temp_reg|=(1<<SPI_CR1_RXONLY_Pos);
		}
		spi_ptr->SPI_CR1=temp_reg;
		if(spi_config->SPI_FrameSize>15U||spi_config->SPI_FrameSize<4)
		{
			spi_ptr->SPI_CR2=(7<<SPI_CR2_DS_Pos);
		}
		else
		{
			spi_ptr->SPI_CR2=(spi_config->SPI_FrameSize<<SPI_CR2_DS_Pos);
		}
	}
	return state;
}


Status_t SPI_clk_init(const SPI_RegDef_t* spi_ptr,const uint8_t enable)
{
	Status_t state=STATUS_OK;
	if(spi_ptr==NULL){
		state=STATUS_ERROR;
	}
	else
	{
		if(enable==ENABLE)
		{
			if(SPI1==spi_ptr)
			{
				RCC->APB2ENR|=(1<<12);
			}
			else if(SPI2==spi_ptr)
			{
				RCC->APB1ENR|=(1<<14);
			}
			else if(SPI3==spi_ptr)
			{
				RCC->APB1ENR|=(1<<15);
			}
			else if(SPI4==spi_ptr)
			{
				RCC->APB2ENR|=(1<<13);
			}
			else if(SPI5==spi_ptr)
			{
				RCC->APB2ENR|=(1<<20);
			}
			else if(SPI6==spi_ptr)
			{
				RCC->APB2ENR|=(1<<21);
			}
			else
			{
				state=STATUS_INVALID_PARAM;
			}
		}
		else
		{
			if(SPI1==spi_ptr)
			{
				RCC->APB2ENR&=~(1<<12);
			}
			else if(SPI2==spi_ptr)
			{
				RCC->APB1ENR&=~(1<<14);
			}
			else if(SPI3==spi_ptr)
			{
				RCC->APB1ENR&=~(1<<15);
			}
			else if(SPI4==spi_ptr)
			{
				RCC->APB2ENR&=~(1<<13);
			}
			else if(SPI5==spi_ptr)
			{
				RCC->APB2ENR&=~(1<<20);
			}
			else if(SPI6==spi_ptr)
			{
				RCC->APB2ENR&=~(1<<21);
			}
			else
			{
				state=STATUS_INVALID_PARAM;
			}
		}

	}
	return state;
}

Status_t SPI_Deinit(const SPI_RegDef_t* spi_ptr)
{
	Status_t state=STATUS_OK;
	if(SPI1==spi_ptr)
	{
		RCC->APB2RSTR|=(1<<12);
		RCC->APB2RSTR&=~(1<<12);
	}
	else if(SPI2==spi_ptr)
	{
		RCC->APB1RSTR|=(1<<14);
		RCC->APB1RSTR&=~(1<<14);
	}
	else if(SPI3==spi_ptr)
	{
		RCC->APB1RSTR|=(1<<15);
		RCC->APB1RSTR&=~(1<<15);
	}
	else if(SPI4==spi_ptr)
	{
		RCC->APB2RSTR|=(1<<13);
		RCC->APB2RSTR&=~(1<<13);
	}
	else if(SPI5==spi_ptr)
	{
		RCC->APB2RSTR|=(1<<20);
		RCC->APB2RSTR&=~(1<<20);
	}
	else if(SPI6==spi_ptr)
	{
		RCC->APB2RSTR|=(1<<21);
		RCC->APB2RSTR&=~(1<<21);
	}
	else
	{
		state=STATUS_INVALID_PARAM;
	}
	return state;

}

void SPI_Start(SPI_RegDef_t* pSPI,uint8_t enable){
	if(enable==ENABLE){
		pSPI->SPI_CR1|=(1<<SPI_CR1_SPE_Pos);
	}
	else{
		pSPI->SPI_CR1&=~(1<<SPI_CR1_SPE_Pos);

	}
}
//Status_t SPI_SendData()
