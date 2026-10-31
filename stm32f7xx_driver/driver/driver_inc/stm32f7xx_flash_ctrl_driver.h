/*
 * stm32f7xx_flash_ctrl_driver.h
 *
 *  Created on: 03-Oct-2026
 *      Author: likith
 */

#ifndef DRIVER_INC_STM32F7XX_FLASH_CTRL_DRIVER_H_
#define DRIVER_INC_STM32F7XX_FLASH_CTRL_DRIVER_H_

typedef enum
{
  Status_OK       = 0x00U,
  Status_ERROR    = 0x01U,
  Status_BUSY     = 0x02U,
  Status_TIMEOUT  = 0x03U
} Flash_StatusType_e;

typedef struct flash_status_tag
{
	uint8_t Erase_seq_error;
	uint8_t Pro_parallel_error;
	uint8_t Pro_align_error;
	uint8_t Write_protection_error;
	uint8_t	Opertaion_error;
	uint8_t End_of_op;
}Flash_OperationStatus_t;



uint8_t Flash_IsBusy(void);
Flash_StatusType_e Flash_Unlock(void);
uint8_t Flash_IsLocked(void);
Flash_StatusType_e Flash_Lock(void);
Flash_StatusType_e Flash_EraseSector(uint8_t sector_num);
Flash_StatusType_e Flash_GetError(void);
Flash_StatusType_e Flash_WaitReady(uint32_t timeout);
#endif /* DRIVER_INC_STM32F7XX_FLASH_CTRL_DRIVER_H_ */
