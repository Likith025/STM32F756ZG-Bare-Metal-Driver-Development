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

Flash_StatusType_e Flash_EraseSector(uint8_t sector_num)
{
	Flash_StatusType_e sector_erase_sts=Status_OK;
	if(Flash_IsBusy()==0)
	{

		if(sector_num>=8){
			sector_erase_sts=Status_ERROR;
		}
		else
		{
			FLASH->FLASH_CR|=(1<<SECTOR_ERASE_flag);
			FLASH->FLASH_CR&=~(0xfu<sector_num_pos);
			FLASH->FLASH_CR|=(sector_num<<sector_num_pos);
			FLASH->FLASH_CR|=(1<<Start_pos);
			while(Flash_IsBusy()!=0);
			//add get status and error
		}
	}
	else
	{
		sector_erase_sts=Status_BUSY;
	}
	return sector_erase_sts;
}
