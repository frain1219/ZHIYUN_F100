/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "main.h"
#include "board.h"
#include "bsp_delay.h"
#include "bsp_flash.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "usb_host_user.h"
#include "usbh_msc.h"
#include "usb_iap.h"
#include "fatfs.h"
#include <stdio.h>

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

#define DEBUG_USART    USART1

/**@} end of group OTG_MSC_FWUpgradeBootloader_Macros*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Variables Variables
  @{
  */

/* user application error table */
const char* userAppErrorTable[] =
{
    "USER_APP_OK",
    "USER_APP_ERR_FATFS",
    "USER_APP_ERR_DISK_WP",
    "USER_APP_ERR_FSIZE_OVER",
    "USER_APP_ERR_FMC_ERASE",
    "USER_APP_ERR_FMC_PG",
    "USER_APP_ERR_FMC_WP",
    "USER_APP_ERR_DISK_DISCONNECTED",
};

static uint8_t userAppState = USER_APP_JUMP;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Variables*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      int
 */
int main(void)
{
    uint32_t timeout = 0;

    /* Set the Vector Table base address at 0x08000000 */
    NVIC_ConfigVectorTable(NVIC_VECT_TAB_FLASH, 0x0000);

    USART_Config_T usartConfigStruct;

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate      = 115200;
    usartConfigStruct.mode          = USART_MODE_TX_RX;
    usartConfigStruct.parity        = USART_PARITY_NONE;
    usartConfigStruct.stopBits      = USART_STOP_BIT_1;
    usartConfigStruct.wordLength    = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow  = USART_HARDWARE_FLOW_NONE;

    /* COM1 init*/
    APM_MINI_COMInit(COM1, &usartConfigStruct);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    APM_MINI_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Init USB host */
    USB_HostInit();

    /* Flash unlock */
    FLASH_FlashInit();

    printf(">> USB FW Upgrade Bootloader\r\n");
    printf(">> Waitting for the U disk to be inserted for firmware upgrade\r\n");
    printf(">> Please press KEY1 to enter firmware upgrade mode\r\n");

    while (1)
    {
        if (!APM_MINI_PBGetState(BUTTON_KEY1))
        {
            APM_DelayMs(10);
            if (!APM_MINI_PBGetState(BUTTON_KEY1))
            {
                userAppState = USER_APP_INIT;

                while (!APM_MINI_PBGetState(BUTTON_KEY1));
                printf("\r\nEnter firmware upgrade mode\r\n");
                break;
            }
        }
        APM_DelayMs(10);
        timeout++;
        if ((timeout % 10)  == 0)
        {
            printf(">");
        }

        if (timeout >= 200)
        {
            printf("\r\n");
            timeout = 0;
            USB_IAP_Jump2App();
            break;
        }
    }

    while (1)
    {
        USB_HostProcess();
        USB_HostUserApplication();
    }
}

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostUserApplication(void)
{
    uint8_t status = gUsbHostAppStatus;

    switch (status)
    {
        case USBH_APP_CONNECTION:
            break;

        case USBH_APP_DISCONNECTION:
            break;

        case USBH_APP_READY:
            switch (userAppState)
            {
                case USER_APP_INIT:
                    printf(">> USER_APP_FS_INIT\r\n");

                    /* Initialises the File System*/
                    if (f_mount(&fatfs, IAP_DISK_PATH, 0) != FR_OK)
                    {
                        USER_ErrorHandler(USER_APP_ERR_FATFS);
                    }

                    /* Flash Disk is write protected */
                    if (USBH_MSC_ReadDevWP(&gUsbHostFS, 0) == ENABLE)
                    {
                        USER_ErrorHandler(USER_APP_ERR_DISK_WP);
                    }

                    /* Go to IAP menu */
                    userAppState = USER_APP_RUN;
                    break;

                case USER_APP_RUN:
                    printf(">> USER_APP_IAP\r\n");
                    printf(">> Please do not unplug the U disk\r\n");
                    /* Writes Flash memory */
                    USB_IAP_Download();

                    /* Reads all flash memory */
                    USB_IAP_Upload();

                    /* Go to JUMP menu */
                    userAppState = USER_APP_JUMP;
                    break;

                case USER_APP_JUMP:
                    printf(">> USER_APP_JUMP\r\n");
                    /* Jumps to user application code located in the internal Flash memory */
                    USB_IAP_Jump2App();
                    break;
            }
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB error handler
 *
 * @param       errCode: error code
 *
 * @retval      None
 */
__weak void USER_ErrorHandler(uint8_t errCode)
{
    while (1)
    {
        printf("%s\r\n", userAppErrorTable[errCode]);
        APM_MINI_LEDToggle(LED2);
        APM_DelayMs(500);
    }
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */
