/*
 * stm32f7xx_flash_ctrl_driver.c
 *
 *  Created on: 03-Oct-2026
 *      Author: likith
 */


#include "stm32f756zg_reg.h"
#include "stm32f7xx_flash_ctrl_driver.h"


#define LOCK_flag	(31U)
#define SECTOR_ERASE_flag	(1U)
#define sector_num_pos		(3u)
#define Start_pos			(16U)

#define EOP_flag				(0U) //END OF OPERATION
#define OPERR_flag				(1U) // OPERATION ERROR
#define WRPERR_flag				(4U)//WRITE PROTECTION ERROR
#define PGAERR_flag				(5U)//PROGRAMMING ALIGNMENT ERROR
#define PGPERR_flag				(6U)//PROGRAMMING PARALLELISM ERROR
#define ERSERR_flag 			(7U)//ERASE SEQ ERROR
#define BSY_flag 				(16U)// BUSY FLAG

#define FLASH_CLEAR_FLAGS_MASK		((1U<<EOP_flag)|(1U<<OPERR_flag)|(1U<<WRPERR_flag)|(1U<<PGAERR_flag)|(1U<<PGPERR_flag)|(1U<<ERSERR_flag))

#define Flash_key1	 (0x45670123)
#define Flash_key2	(0xCDEF89AB)

#define ERASE_TIMEOUT	(50000000UL)


Flash_OperationStatus_t Operation_status;


static uint8_t Flash_IsAddressValid(uint32_t address);
static Flash_StatusType_e Flash_Write_Byte(uint32_t address, uint8_t data);
static Flash_StatusType_e Flash_Write_HalfWord(uint32_t address, uint16_t data);
static Flash_StatusType_e Flash_Write_Word(uint32_t address,uint32_t data);

void FLash_init()
{
	//setting up default values
}


uint8_t Flash_IsBusy(void)
{
	uint8_t	reval=0;

	reval=((FLASH->FLASH_SR>>BSY_flag)&1U);

	return reval;
}


Flash_StatusType_e Flash_Unlock(void)
{
	Flash_StatusType_e unlock_sts=Status_OK;
	if(Flash_IsBusy()==0)
	{
		FLASH->FLASH_KEYR=(Flash_key1);
		FLASH->FLASH_KEYR=(Flash_key2);

		if((FLASH->FLASH_CR>>LOCK_flag)&1U)
			{
				unlock_sts=Status_ERROR;
			}
	}
	else{
		unlock_sts=Status_BUSY;
	}

return unlock_sts;
}


uint8_t Flash_IsLocked()
{
	uint8_t locked_sts=0;
	locked_sts=((FLASH->FLASH_CR>>LOCK_flag)&1U);
	return locked_sts;
}


Flash_StatusType_e Flash_Lock(void)
{
	Flash_StatusType_e lock_sts=Status_OK;
	FLASH->FLASH_CR |= (1UL << LOCK_flag);
	if(Flash_IsLocked()==0)
	{
		lock_sts=Status_ERROR;
	}
	return lock_sts;
}


Flash_StatusType_e Flash_WaitReady(uint32_t timeout)
{
	Flash_StatusType_e timeout_flag=Status_OK;
	while((Flash_IsBusy()!=0U)&&(timeout_flag!=Status_TIMEOUT))
	{

		if(timeout<=0U)
		{
			timeout_flag=Status_TIMEOUT;
		}
		else
		{
			timeout--;
		}

	}
	return timeout_flag;
}

Flash_StatusType_e Flash_GetError(void)
{
	Flash_StatusType_e status=Status_OK;

	Operation_status.Erase_seq_error=((FLASH->FLASH_SR>>7U)&1U);
	Operation_status.Opertaion_error=((FLASH->FLASH_SR>>1U)&1U);
	Operation_status.Pro_align_error=((FLASH->FLASH_SR>>5U)&1U);
	Operation_status.Pro_parallel_error=((FLASH->FLASH_SR>>6U)&1U);
	Operation_status.Write_protection_error=((FLASH->FLASH_SR>>4U)&1U);
	Operation_status.End_of_op=((FLASH->FLASH_SR>>0U)&1U);

	 if ((Operation_status.Erase_seq_error != 0U) ||
	        (Operation_status.Opertaion_error != 0U) ||
	        (Operation_status.Pro_align_error != 0U) ||
	        (Operation_status.Pro_parallel_error != 0U) ||
	        (Operation_status.Write_protection_error != 0U))
	    {
	        status = Status_ERROR;
	    }
return status;
}

Flash_StatusType_e Flash_ClearErrorFlags(void)
{
	Flash_StatusType_e status;
	if (Flash_IsBusy()!=0U)
	{
		status= Status_BUSY;
	}
	else
	{
		FLASH->FLASH_SR=FLASH_CLEAR_FLAGS_MASK;
		status=Status_OK;
	}
	return status;
}


Flash_StatusType_e Flash_EraseSector(uint8_t sector_num)
{
    Flash_StatusType_e sector_erase_sts = Status_OK;

    if (Flash_IsLocked() == 1U)
    {
        sector_erase_sts = Status_ERROR;
    }
    else if (sector_num >= 8U)
    {
        sector_erase_sts = Status_ERROR;
    }
    else
    {
        sector_erase_sts = Flash_WaitReady(ERASE_TIMEOUT);
    }
    if( sector_erase_sts==Status_OK)
    {
    	sector_erase_sts=Flash_ClearErrorFlags();
    }

    if (sector_erase_sts == Status_OK)
    {
        FLASH->FLASH_CR |= (1UL << SECTOR_ERASE_flag);
        FLASH->FLASH_CR &= ~(0xFUL << sector_num_pos);
        FLASH->FLASH_CR |= ((uint32_t)sector_num << sector_num_pos);
        FLASH->FLASH_CR |= (1UL << Start_pos);
        sector_erase_sts = Flash_WaitReady(ERASE_TIMEOUT);
        if (sector_erase_sts == Status_OK)
        {
            sector_erase_sts = Flash_GetError();
        }

        if (Flash_IsBusy() == 0U)
        {
            FLASH->FLASH_CR &= ~(1UL << SECTOR_ERASE_flag);
        }
    }

    return sector_erase_sts;
}


Flash_StatusType_e Flash_Write(Flash_Write_Width_e width,uint32_t address,uint64_t data)
{
	Flash_StatusType_e status=Status_OK;
	if((Flash_IsAddressValid(address)==0)||(Flash_IsLocked()!=0))
	{
		status=Status_ERROR;
	}
	else if(Flash_IsBusy()==1)
	{
		status=Status_BUSY;
	}
	else
	{
		switch(width)
		{
			case FLASH_TYPEPROGRAM_BYTE:
			{
				status=Flash_Write_Byte(address, (uint8_t)data);
				break;
			}
			case FLASH_TYPEPROGRAM_HALFWORD:
			{
				status=Flash_Write_HalfWord(address, (uint16_t)data);
				break;
			}
			case FLASH_TYPEPROGRAM_WORD:
			{
				status=Flash_Write_Word(address,(uint32_t) data);
				break;
			}

			default:
			{
				status=Status_ERROR;
				break;
			}
		}
	}
	return status;

}

static uint8_t Flash_IsAddressValid(uint32_t address)
{
	uint8_t reval=0;

	if(address>=FLASH_BASE_ADDR&&address<=(FLASH_END_ADDR-0x04))
	{
		reval=1;
	}
	return reval;
}


static Flash_StatusType_e Flash_Write_Word(uint32_t address, uint32_t data)
{
    uint32_t read_back = 0U;
    Flash_StatusType_e status = Status_OK;

    /* Check 4-byte alignment */
    if ((address % 4U) == 0U)
    {
        /* Clear previous error flags */
        status = Flash_ClearErrorFlags();

        if (status == Status_OK)
        {
            /* Configure PSIZE for 32-bit programming */
            FLASH->FLASH_CR &= ~(3U << 8U);
            FLASH->FLASH_CR |=  (2U << 8U);

            /* Enable programming */
            FLASH->FLASH_CR |= (1U << 0U);

            /* Write 32-bit data */
            *(volatile uint32_t *)address =(uint32_t) data;

            /* Wait for programming completion */
            status = Flash_WaitReady(1000U);

            if (status == Status_OK)
            {
                /* Check hardware error flags */
                status = Flash_GetError();
            }

            /* Disable programming only when Flash is ready */
            if (Flash_IsBusy() == 0U)
            {
                FLASH->FLASH_CR &= ~(1U << 0U);
            }

            /* Verify programmed data only after successful operation */
            if ((status == Status_OK) && (Flash_IsBusy() == 0U))
            {
                read_back = *(volatile uint32_t *)address;

                if (read_back != (uint32_t)data)
                {
                    status = Status_ERROR;
                }
            }
        }
    }
    else
    {
        status = Status_ERROR;
    }

    return status;
}

static Flash_StatusType_e Flash_Write_Byte(uint32_t address, uint8_t data)
{
    uint8_t read_back = 0U;
    Flash_StatusType_e status = Status_OK;

    /* Clear previous error flags */
    status = Flash_ClearErrorFlags();

    if (status == Status_OK)
    {
        /* Configure PSIZE for 8-bit programming */
        FLASH->FLASH_CR &= ~(3U << 8U);

        /* Enable programming */
        FLASH->FLASH_CR |= (1U << 0U);

        /* Write 8-bit data */
        *(volatile uint8_t *)address = data;

        /* Wait for programming completion */
        status = Flash_WaitReady(1000U);

        if (status == Status_OK)
        {
            /* Check hardware errors */
            status = Flash_GetError();
        }

        /* Disable programming when Flash is ready */
        if (Flash_IsBusy() == 0U)
        {
            FLASH->FLASH_CR &= ~(1U << 0U);
        }

        /* Verify programmed data */
        if ((status == Status_OK) && (Flash_IsBusy() == 0U))
        {
            read_back = *(volatile uint8_t *)address;

            if (read_back != data)
            {
                status = Status_ERROR;
            }
        }
    }

    return status;
}


static Flash_StatusType_e Flash_Write_HalfWord(uint32_t address,
                                               uint16_t data)
{
    uint16_t read_back = 0U;
    Flash_StatusType_e status = Status_OK;

    if ((address % 2U) == 0U)
    {
        /* Clear previous error flags */
        status = Flash_ClearErrorFlags();

        if (status == Status_OK)
        {
            /* Configure PSIZE for 16-bit programming */
            FLASH->FLASH_CR &= ~(3U << 8U);
            FLASH->FLASH_CR |=  (1U << 8U);

            /* Enable programming */
            FLASH->FLASH_CR |= (1U << 0U);

            /* Write 16-bit data */
            *(volatile uint16_t *)address = data;

            /* Wait for programming completion */
            status = Flash_WaitReady(1000U);

            if (status == Status_OK)
            {
                /* Check hardware errors */
                status = Flash_GetError();
            }

            /* Disable programming when Flash is ready */
            if (Flash_IsBusy() == 0U)
            {
                FLASH->FLASH_CR &= ~(1U << 0U);
            }

            /* Verify programmed data */
            if ((status == Status_OK) && (Flash_IsBusy() == 0U))
            {
                read_back = *(volatile uint16_t *)address;

                if (read_back != data)
                {
                    status = Status_ERROR;
                }
            }
        }
    }
    else
    {
        status = Status_ERROR;
    }

    return status;
}

