/*!
 * @file        usb_iap.h
 *
 * @brief       Header for usb_iap.c module
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
#ifndef __USB_IAP_H
#define __USB_IAP_H

/* Includes */
#include "apm32f4xx.h"
#include "bsp_flash.h"
#include "fatfs.h"

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

#define IAP_UPLOAD_FILENAME         "0:image_upload.bin"
#define IAP_DOWNLOAD_FILENAME       "0:image.bin"
#define IAP_FILE_PATH               "0:/image.bin"
#define IAP_DISK_PATH               "0:"

/* In internal SRAM:
   This value can be equal to (512 * x) according to RAM size availability with x=[1, 128].
   In this project is fixed to 64 => 512 * 64 = 32768bytes = 32 Kbytes.
   You can use External SRAM to store more data for firmware upgrade. */
#define IAP_BUFFER_SIZE             ((uint16_t)512 * 64)

/**@} end of group OTG_MSC_FWUpgradeBootloader_Macros*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Enumerates Enumerates
  @{
  */

/*!
 * @brief    IAP operation type define
 */
typedef enum
{
    IAP_OP_OK,      /*!< IAP operation success */
    IAP_OP_ERROR    /*!< IAP operation error */
} IAP_OP_T;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Enumerates*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Variables Variables
  @{
  */

extern FATFS fatfs;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Variables*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

/*!
 * @brief    Jump function pointer
 */
typedef void (*pFunction)(void);

void USB_IAP_Download(void);
void USB_IAP_Jump2App(void);
void USB_IAP_Upload(void);

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */

#endif
