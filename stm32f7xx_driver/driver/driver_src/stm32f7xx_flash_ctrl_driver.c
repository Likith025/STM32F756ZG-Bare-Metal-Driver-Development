/*
 * stm32f7xx_flash_ctrl_driver.c
 *
 *  Created on: 03-Oct-2026
 *      Author: likith
 */


#include "stm32f756zg_reg.h"
#include "stm32f7xx_flash_ctrl_driver.h"

#define BSY_flag (16U)
#define LOCK_flag	(31U)
#define SECTOR_ERASE_flag	(1U)
#define sector_num_pos		(3u)
#define Start_pos			(16U)

#define Flash_key1	 (0x45670123)
#define Flash_key2	(0xCDEF89AB)

#define ERASE_TIMEOUT	(50000000UL)


Flash_OperationStatus_t Operation_status;

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

