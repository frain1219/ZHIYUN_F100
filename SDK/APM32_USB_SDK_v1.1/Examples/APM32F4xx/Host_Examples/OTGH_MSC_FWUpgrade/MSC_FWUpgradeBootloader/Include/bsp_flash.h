/*!
 * @file        bsp_flash.h
 *
 * @brief       Header for bsp_flash.c module
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

/* Includes */
#include "apm32f4xx.h"
#include "apm32f4xx_fmc.h"

/** @addtogroup Examples
  * @brief OTG MSC Upgrade Bootloader examples
  @{
  */

/** @addtogroup OTG_MSC_FWUpgradeBootloader
  @{
  */

/** @defgroup OTG_MSC_FWUpgradeBootloader_Macros Macros
  @{
*/

/* flash memory start address */
#define USER_FLASH_START_ADDRESS    ((uint32_t)0x08000000)

/* user application address.
   Note: the 1st sector 0x08000000-0x0800BFFF is reserved for the Firmware upgrade bootloader code */
#define APPLICATION_ADDRESS         ((uint32_t)0x08010000)

/* flash memory end address */
#define USER_FLASH_END_ADDRESS      ((uint32_t)0x080FFFFF)

/* user application size */
#define USER_FLASH_SIZE             (USER_FLASH_END_ADDRESS - USER_FLASH_START_ADDRESS + 1)

/* Base address of the Flash sectors */
#define FLASH_SECTOR_0_ADDR     ((uint32_t)0x08000000) /*!< Base @ of Sector 0, 16 Kbyte */
#define FLASH_SECTOR_1_ADDR     ((uint32_t)0x08004000) /*!< Base @ of Sector 1, 16 Kbyte */
#define FLASH_SECTOR_2_ADDR     ((uint32_t)0x08008000) /*!< Base @ of Sector 2, 16 Kbyte */
#define FLASH_SECTOR_3_ADDR     ((uint32_t)0x0800C000) /*!< Base @ of Sector 3, 16 Kbyte */
#define FLASH_SECTOR_4_ADDR     ((uint32_t)0x08010000) /*!< Base @ of Sector 4, 64 Kbyte */
#define FLASH_SECTOR_5_ADDR     ((uint32_t)0x08020000) /*!< Base @ of Sector 5, 128 Kbyte */
#define FLASH_SECTOR_6_ADDR     ((uint32_t)0x08040000) /*!< Base @ of Sector 6, 128 Kbyte */
#define FLASH_SECTOR_7_ADDR     ((uint32_t)0x08060000) /*!< Base @ of Sector 7, 128 Kbyte */
#define FLASH_SECTOR_8_ADDR     ((uint32_t)0x08080000) /*!< Base @ of Sector 8, 128 Kbyte */
#define FLASH_SECTOR_9_ADDR     ((uint32_t)0x080A0000) /*!< Base @ of Sector 9, 128 Kbyte */
#define FLASH_SECTOR_10_ADDR    ((uint32_t)0x080C0000) /*!< Base @ of Sector 10, 128 Kbyte */
#define FLASH_SECTOR_11_ADDR    ((uint32_t)0x080E0000) /*!< Base @ of Sector 11, 128 Kbyte */

/**@} end of group OTG_MSC_FWUpgradeBootloader_Macros*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Enumerates Enumerates
  @{
  */

/*!
 * @brief    FMC operation type define
 */
typedef enum
{
    FMC_OP_OK,      /*!< Flash operation success */
    FMC_OP_ERROR    /*!< Flash operation error */
} FMC_OP_T;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Enumerates*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

void FLASH_FlashInit(void);
uint8_t FLASH_ReadOutProtectionStatus(void);
uint8_t FLASH_EraseSectors(uint32_t address);
FMC_STATUS_T FLASH_ProgramWord(uint32_t address, uint32_t data);

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */

#endif
