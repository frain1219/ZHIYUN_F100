/*!
 * @file        usb_iap.c
 *
 * @brief       Include USB IAP operation
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

/* Includes */
#include "usb_iap.h"
#include "main.h"
#include "usb_host_user.h"
#include <stdio.h>
#include <stdbool.h>

/** @addtogroup Examples
  * @brief OTG MSC Upgrade Bootloader examples
  @{
  */

/** @addtogroup OTG_MSC_FWUpgradeBootloader
  @{
  */

/** @defgroup OTG_MSC_FWUpgradeBootloader_Variables Variables
  @{
  */

static uint8_t iapBuffer[IAP_BUFFER_SIZE] = { 0x00 };

FATFS fatfs;
FIL file;
FIL fileR;
DIR dir;
FILINFO fno;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Variables*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

/*!
 * @brief       IAP upload flash data to file
 *
 * @param       None
 *
 * @retval      None
 *
 */
void USB_IAP_Upload(void)
{
    uint8_t status = SET;
    uint16_t bytesWritten;

    __IO uint32_t address = APPLICATION_ADDRESS;
    __IO uint32_t readIndex = 0x00;
    uint32_t temp = 0x00;
    uint32_t indexOffset = 0x00;

    printf(">> Please wait for upload finish...\r\n");

    /* Read the read out protection status */
    status = FLASH_ReadOutProtectionStatus();

    if (status == RESET)
    {
        /* Remove UPLOAD file if exist on U disk */
        f_unlink(IAP_UPLOAD_FILENAME);

        indexOffset = (APPLICATION_ADDRESS - USER_FLASH_START_ADDRESS);

        /* Open UPLOAD file to write on it */
        if ((gUsbHostFS.devInfo.connectedStatus == ENABLE) && (f_open(&file, IAP_UPLOAD_FILENAME, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK))
        {
            /* Read flash memory */
            while ((indexOffset != USER_FLASH_SIZE) && (gUsbHostFS.devInfo.connectedStatus == ENABLE))
            {
                for (readIndex = 0; readIndex < IAP_BUFFER_SIZE; readIndex++)
                {
                    /* Check the flash data */
                    if (indexOffset + readIndex != USER_FLASH_SIZE)
                    {
                        temp = readIndex;
                        iapBuffer[temp] = (*(uint8_t*)(address++));
                    }
                    else
                    {
                        break;
                    }
                }

                /* Write buffer to file */
                f_write(&file, iapBuffer, IAP_BUFFER_SIZE, (void*)&bytesWritten);

                /* Count number of byte written  */
                indexOffset = indexOffset + readIndex;
            }

            /* Close file and U file system */
            f_close(&file);
            f_mount(NULL, IAP_DISK_PATH, 0);

            printf(">> IAP upload done\r\n");
        }
    }
    else
    {
        USER_ErrorHandler(USER_APP_ERR_FMC_WP);
    }
}

/*!
 * @brief       IAP program flash memory
 *
 * @param       None
 *
 * @retval      IAP operation status
 *
 */
IAP_OP_T USB_IAP_ProgramFlashMemory(void)
{
    IAP_OP_T errSta = IAP_OP_OK;

    __IO uint32_t programIndex = 0x00;
    uint8_t readFlag = 1;
    uint16_t bytesRead;
    uint16_t cntPG = 0;

    uint32_t programCntTemp = 0x00;
    uint32_t readSizeTemp   = 0x00;
    uint32_t bufferAddr     = 0x00;
    uint32_t lastPGAddr     = APPLICATION_ADDRESS;

    /* Init buffer address */
    bufferAddr = (uint32_t) & iapBuffer;

    /* Init erase address */
    lastPGAddr = APPLICATION_ADDRESS;

    /* While file still contain data */
    while ((readFlag == 1) && (gUsbHostFS.devInfo.connectedStatus == ENABLE))
    {
        printf(">> Reading binary file data into RAM\r\n");
        /* Read maximum 512 Kbyte(Based on the size of iapBuffer) from the selected file */
        f_read(&fileR, iapBuffer, IAP_BUFFER_SIZE, (void*)&bytesRead);

        readSizeTemp = bytesRead;

        /* The read data < "IAP_BUFFER_SIZE" Kbyte */
        if (readSizeTemp < IAP_BUFFER_SIZE)
        {
            printf("<< last PG time:%d,addr:%08lX\r\n", ++cntPG, lastPGAddr);
            readFlag = 0;
        }
        else
        {
            cntPG++;
            printf("<< PG time:%d,addr:%08lX\r\n", cntPG, lastPGAddr);
        }

        /* Program flash memory */
        for (programIndex = readSizeTemp; programIndex != 0; programIndex -= 4)
        {
            programCntTemp = programIndex;

            /* Write word into flash memory */
            if (FLASH_ProgramWord((lastPGAddr - programCntTemp + readSizeTemp), \
                                  * (__IO uint32_t*)(bufferAddr - programIndex + readSizeTemp)) != FMC_COMPLETE)
            {
                /* Flash programming error */
                USER_ErrorHandler(USER_APP_ERR_FMC_PG);
            }
        }

        /* Update last PG address value */
        lastPGAddr = lastPGAddr + readSizeTemp;

        if (gUsbHostFS.devInfo.connectedStatus != ENABLE)
        {
            errSta = IAP_OP_ERROR;
        }
    }

    return errSta;
}

/*!
 * @brief       USB IAP download
 *
 * @param       None
 *
 * @retval      None
 */
void USB_IAP_Download(void)
{
    /* Open the binary file to be downloaded */
    if (f_open(&fileR, IAP_DOWNLOAD_FILENAME, FA_READ) == FR_OK)
    {
        f_stat((const TCHAR*)IAP_FILE_PATH, &fno);
        printf(">> file size of %s:%d\r\n", IAP_DOWNLOAD_FILENAME, (int)fno.fsize);

        printf(">> Please wait for download finish...\r\n");
        if (fno.fsize > USER_FLASH_SIZE)
        {
            USER_ErrorHandler(USER_APP_ERR_FSIZE_OVER);
        }
        else
        {
            /* Erase FLASH sectors to download image */
            if (FLASH_EraseSectors(APPLICATION_ADDRESS) != 0x00)
            {
                USER_ErrorHandler(USER_APP_ERR_FMC_ERASE);
            }
            /* Program flash memory */
            if (USB_IAP_ProgramFlashMemory() != IAP_OP_OK)
            {
                USER_ErrorHandler(USER_APP_ERR_DISK_DISCONNECTED);
            }
            else
            {
                printf(">> IAP download done\r\n");
                /* Close file and filesystem */
                f_close(&fileR);
            }
        }
    }
    else
    {
        USER_ErrorHandler(USER_APP_ERR_FATFS);
    }
}

/*!
 * @brief       USB IAP jump to application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_IAP_Jump2App(void)
{
    pFunction JumpToApplication;
    uint32_t JumpAddress;

    printf(">> IAP prepare to jump to application.\r\n\r\n");

    FMC_Lock();

    RCM->AHB1RST = 0xFFFFFFFFU;
    RCM->APB1RST = 0xFFFFFFFFU;
    RCM->APB2RST = 0xFFFFFFFFU;
    RCM->AHB2RST = 0xFFFFFFFFU;

    RCM->AHB1RST = 0x00000000U;
    RCM->APB1RST = 0x00000000U;
    RCM->APB2RST = 0x00000000U;
    RCM->AHB2RST = 0x00000000U;

    RCM->AHB1CLKEN = 0x00000000U;
    RCM->AHB2CLKEN = 0x00000000U;
    RCM->APB1CLKEN = 0x00000000U;
    RCM->APB2CLKEN = 0x00000000U;

    /* Check vector table */
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)
    {
        /* Jump to user application */
        JumpAddress = *(__IO uint32_t*)(APPLICATION_ADDRESS + 4);
        JumpToApplication = (pFunction) JumpAddress;

        /* Initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
        JumpToApplication();
    }
    else
    {
        /* Software reset */
        NVIC_SystemReset();
    }
}

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */
