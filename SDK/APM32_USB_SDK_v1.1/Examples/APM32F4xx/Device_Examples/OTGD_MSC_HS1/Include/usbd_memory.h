/*!
 * @file        usbd_memory.h
 *
 * @brief       usb device memory management header file
 *
 * @version     V1.0.0
 *
 * @date        2023-03-27
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
#ifndef _USBD_MEMORY_H_
#define _USBD_MEMORY_H_

/* Includes */
#include "usbd_msc.h"

/** @addtogroup Examples
  * @brief OTGD MSC HS1 examples
  @{
  */

/** @addtogroup OTGD_MSC_HS1
  @{
  */

/** @defgroup OTGD_MSC_HS1_Variables Variables
  @{
  */

extern USBD_MSC_MEMORY_T USBD_MEMORY_INTERFACE_HS;

/**@} end of group OTGD_MSC_HS1_Variables*/

/** @defgroup OTGD_MSC_HS1_Functions Functions
  @{
  */

uint8_t USBD_HS_MemoryReadMaxLun(void);
USBD_STA_T USBD_HS_MemoryCheckWPR(uint8_t lun);
USBD_STA_T USBD_HS_MemoryCheckReady(uint8_t lun);
USBD_STA_T USBD_HS_MemoryInit(uint8_t lun);
USBD_STA_T USBD_HS_MemoryReadCapacity(uint8_t lun, uint32_t* blockNum, \
                                      uint16_t* blockSize);
USBD_STA_T USBD_HS_MemoryReadData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                  uint16_t blockLength);
USBD_STA_T USBD_HS_MemoryWriteData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                   uint16_t blockLength);

/**@} end of group OTGD_MSC_HS1_Functions */
/**@} end of group OTGD_MSC_HS1 */
/**@} end of group Examples */

#endif
