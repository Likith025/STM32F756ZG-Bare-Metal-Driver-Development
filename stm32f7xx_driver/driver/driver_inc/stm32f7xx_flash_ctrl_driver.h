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





uint8_t Flash_IsBusy(void);
Flash_StatusType_e Flash_Unlock(void);
uint8_t Flash_IsLocked(void);
Flash_StatusType_e Flash_Lock(void);
#endif /* DRIVER_INC_STM32F7XX_FLASH_CTRL_DRIVER_H_ */
