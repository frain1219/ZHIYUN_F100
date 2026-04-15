/*!
 * @file        fatfs.c
 *
 * @brief       Fatfs program body
 *
 * @version     V1.0.0
 *
 * @date        2025-02-15
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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

/* Includes ***************************************************************/
#include "fatfs.h"
#include <string.h>
#include <stdio.h>

/* Private includes *******************************************************/

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* workspace of logic disk */
FATFS* fs[FF_VOLUMES];
/* workspace of logic disk */
char fileScanPath[255] = "0:";

UINT byteReadWriteNum;
/* fatfs read buffer */
BYTE readBuffer[1024] = {0};
/* fatfs write buffer */
BYTE writeBuffer[] = "Hello Geehy";

/* Private function prototypes ********************************************/

/* External variables *****************************************************/

/* External functions *****************************************************/

/*!
 * @brief       Traverse files of FatFs
 *
 * @param       path: init scan path
 *
 * @retval      File function return code
 *
 * @note        This function is assuming non-Unicode cfg
 */
FRESULT FATFS_ScanFiles(char* path)
{
    FRESULT status;
    /* file informations */
    FILINFO fileInfo;
    /* file directory */
    DIR dir;
    /* file name */
    char* fileName;
    uint32_t lengthScanPath;

#if _USE_LFN
    /* Simplified Chinese need two bytes to save a word */
    static char lfn[_MAX_LFN * 2 + 1];
    fileInfo.lfsize = sizeof(lfn);
    fileInfo.lfname = lfn;
#endif

    /* open directory */
    status = f_opendir(&dir, (const TCHAR*)path);
    if (status == FR_OK)
    {
        lengthScanPath = strlen(path);
        while (1)
        {
            /* Read the contents of the directory */
            status = f_readdir(&dir, &fileInfo);

            /* Error or read all items complete */
            if ((status != FR_OK) || (fileInfo.fname[0] == 0))
            {
                break;
            }
#if _USE_LFN
            fileName = *fileInfo.lfname ? fileInfo.lfname : fileInfo.fname;
#else
            fileName = fileInfo.fname;
#endif
            /* Ignore parent directory */
            if (*fileName == '.')
            {
                continue;
            }
            /* It is a directory */
            if (fileInfo.fattrib & AM_DIR)
            {
                sprintf(&path[lengthScanPath], "/%s", fileName);

                /** recursive */
                status = FATFS_ScanFiles(path);
                path[lengthScanPath] = 0;
                if (status != FR_OK)
                {
                    break;
                }
            }
            /* It is a file */
            else
            {
                printf("%s/%s\r\n", path, fileName);
            }
        }
    }

    return status;
}

/*!
* @brief       Write a file to fatfs
*
* @param       file: point to the file.
*
* @retval      None
*
*/
void FATFS_WriteFile(FIL* file)
{
    FRESULT status;

    /* Write file */
    /* Open or create file */
    printf("------ Write File ------\r\n");
    status = f_open(file, "0:HOSTMSC.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    if (status == FR_OK)
    {
        printf(">> Open or create <HOSTMSC.TXT> success.\r\n");
        printf(">> Write data to <HOSTMSC.TXT>\r\n");

        /* Write data */
        status = f_write(file, writeBuffer, strlen((char*)writeBuffer), &byteReadWriteNum);
        if (status == FR_OK)
        {
            printf(">> Write %d bytes data to file.\r\n", byteReadWriteNum);
            printf(">> The data written to the file is \"%s\".\r\n", writeBuffer);
        }
        else
        {
            printf(">> Write data to file fail.\r\n");
        }

        /* Close file */
        f_close(file);
    }
    else
    {
        printf(">> Open or create file fail, status is %d\r\n", status);
    }

    /* Close file */
    f_close(file);
}

/*!
* @brief       Read a file from fatfs
*
* @param       file: point to the file.
*
* @retval      None
*
*/
void FATFS_ReadFile(FIL* file)
{
    FRESULT status;

    /* Read file */
    /* Open file */
    printf("------ Read File ------\r\n");
    status = f_open(file, "0:HOSTMSC.TXT", FA_OPEN_EXISTING | FA_READ);
    if (status == FR_OK)
    {
        printf(">> Open <HOSTMSC.TXT> success.\r\n");
        status = f_read(file, readBuffer, sizeof(readBuffer), &byteReadWriteNum);
        if (status == FR_OK)
        {
            printf(">> Read %d bytes data from file.\r\n", byteReadWriteNum);
            printf(">> The data read from the file is \"%s\".\r\n", readBuffer);
        }
        else
        {
            printf(">> Read data from file fail!\r\n");
        }
    }
    else
    {
        printf(">> Open file fail, status is %d\r\n", status);
    }

    /* Close file */
    f_close(file);
}

DWORD get_fattime(void)
{
    return 0;
}

/**
 * @brief  Get Drive Path
 * 
 * @param  driveNum: Drive number
 *         This parameter can be one of the following values:
 *          @arg USBH_DISK
 *          @arg RAM_DISK
 *          @arg SD_DISK
 *          @arg MMC_DISK
 *          @arg NAND_DISK
 *          @arg USER_DISK
 * 
 * @retval char*: Drive path
 */
char* FATFS_GetDrivePath(BYTE driveNum)
{
    static char path[3];

    if (driveNum < FF_VOLUMES)
    {
        path[0] = '0' + driveNum;
        path[1] = ':';
        path[2] = '\0';
    }
    else
    {
        path[0] = '\0';
    }

    return path;
}
