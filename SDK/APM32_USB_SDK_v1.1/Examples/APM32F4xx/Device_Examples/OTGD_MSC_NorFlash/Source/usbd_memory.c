/*!
 * @file        usbd_memory.c
 *
 * @brief       USB device memory management program body
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

/* Includes */
#include "usbd_memory.h"
#include "bsp_w25qxx.h"

/** @addtogroup Examples
  * @brief OTGD MSC NOR Flash examples
  @{
  */

/** @addtogroup OTGD_MSC_NorFlash
  @{
  */

/** @defgroup OTGD_MSC_NorFlash_Macros Macros
  @{
*/
#define MEMORY_LUN_NUM              1
#define MEMORY_BLOCK_SIZE           4096
#define MEMORY_BLOCK_NUM            1024 * 1024 * 2 / STORAGE_BLK_SIZ;

/**@} end of group OTGD_MSC_NorFlash_Macros*/

/** @defgroup OTGD_MSC_NorFlash_Variables Variables
  @{
  */

/* Use 40K RAM as the memory unit of USB disk */
//uint8_t usbDiskData[MEMORY_BLOCK_NUM][MEMORY_BLOCK_SIZE] = {0};

/* USB Mass storage Standard Inquiry Data */
const uint8_t memoryInquiryData[] =
{
    /* lun 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (USBD_LEN_STD_INQUIRY - 5),
    0x00,
    0x00,
    0x00,
    /* Manufacturer : 8 bytes */
    'G', 'e', 'e', 'h', 'y', ' ', ' ', ' ',
    /* Product : 16 Bytes */
    'S', 't', 'o', 'r', 'a', 'g', 'e', ' ',
    'D', 'i', 's', 'k', ' ', ' ', ' ', ' ',
    /* Version : 4 Bytes */
    '1', '.', '0', '0',
};

/**@} end of group OTGD_MSC_NorFlash_Variables*/

/** @defgroup OTGD_MSC_NorFlash_Structures Structures
  @{
  */

/* USB FS MSC memory management handler */
USBD_MSC_MEMORY_T USBD_MEMORY_INTERFACE_FS =
{
    "MSC Memory FS",
    (uint8_t*)memoryInquiryData,
    USBD_FS_MemoryReadMaxLun,
    USBD_FS_MemoryInit,
    USBD_FS_MemoryReadCapacity,
    USBD_FS_MemoryCheckReady,
    USBD_FS_MemoryCheckWPR,
    USBD_FS_MemoryReadData,
    USBD_FS_MemoryWriteData,
};

/**@} end of group OTGD_MSC_NorFlash_Structures*/

/** @defgroup OTGD_MSC_NorFlash_Functions Functions
  @{
  */

/*!
 * @brief       USB device MSC memory unit init handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryInit(uint8_t lun)
{
    USBD_STA_T usbStatus = USBD_OK;

    W25QXX_Init(W25Q16_FLASH);
    
    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit read capacity handler
 *
 * @param       lun: lun number
 *
 * @param       blockNum: block number
 *
 * @param       blockSize: block size
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryReadCapacity(uint8_t lun, uint32_t* blockNum, \
                                      uint16_t* blockSize)
{
    USBD_STA_T usbStatus = USBD_OK;

    *blockNum = w25qxxInfo.sectorNum;
    *blockSize = w25qxxInfo.sectorSize;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit check read status handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryCheckReady(uint8_t lun)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit check write protected status handler
 *
 * @param       lun: lun number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryCheckWPR(uint8_t lun)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory read max LUN handler
 *
 * @param       None
 *
 * @retval      Max LUN number
 */
uint8_t USBD_FS_MemoryReadMaxLun(void)
{
    return (MEMORY_LUN_NUM - 1);
}

/*!
 * @brief       USB device MSC memory unit read data handler
 *
 * @param       lun: lun number
 *
 * @param       buffer: data buffer
 *
 * @param       blockAddr: block address
 *
 * @param       blockLength: block number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryReadData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                  uint16_t blockLength)
{
    USBD_STA_T usbStatus = USBD_OK;

    W25QXX_ReadData(buffer, blockAddr * w25qxxInfo.sectorSize, blockLength * w25qxxInfo.sectorSize);

    return usbStatus;
}

/*!
 * @brief       USB device MSC memory unit write data handler
 *
 * @param       lun: lun number
 *
 * @param       buffer: data buffer
 *
 * @param       blockAddr: block address
 *
 * @param       blockLength: block number
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_MemoryWriteData(uint8_t lun, uint8_t* buffer, uint32_t blockAddr, \
                                   uint16_t blockLength)
{
    USBD_STA_T usbStatus = USBD_OK;

    W25QXX_WriteData(buffer, blockAddr * w25qxxInfo.sectorSize, blockLength * w25qxxInfo.sectorSize);

    return usbStatus;
}

/**@} end of group OTGD_MSC_NorFlash_Functions */
/**@} end of group OTGD_MSC_NorFlash */
/**@} end of group Examples */
