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
#include "board.h"
#include "bsp_delay.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "usb_host_user.h"
#include "fatfs.h"
#include <stdio.h>

/** @addtogroup Examples
  * @brief OTG MSC Upgrade App examples
  @{
  */

/** @addtogroup OTG_MSC_FWUpgradeApp
  @{
  */

/** @defgroup OTG_MSC_FWUpgradeApp_Macros Macros
  @{
*/

#define DEBUG_USART    USART1

/**@} end of group OTG_MSC_FWUpgradeApp_Macros*/

/** @defgroup OTG_MSC_FWUpgradeApp_Variables Variables
  @{
  */

FATFS fatfs;
FIL file;
FIL fileR;
DIR dir;
FILINFO fno;

/**@} end of group OTG_MSC_FWUpgradeApp_Variables*/

/** @defgroup OTG_MSC_FWUpgradeApp_Enumerates Enumerates
  @{
  */

/**
 * @brief User application state
 */
typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
} USER_APP_STATE_T;

/**@} end of group OTG_MSC_FWUpgradeApp_Enumerates*/

/** @defgroup OTG_MSC_FWUpgradeApp_Functions Functions
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
    /* Set the Vector Table base address at 0x0800C000 */
    NVIC_ConfigVectorTable(NVIC_VECT_TAB_FLASH, 0xC000);

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

    printf("USB Host MSC Application\r\n");
    printf("Please insert the U disk for read and write file\r\n");

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
    static uint8_t userAppState = USER_APP_INIT;

    switch (status)
    {
        case USBH_APP_CONNECTION:
            break;

        case USBH_APP_DISCONNECTION:
            userAppState = USER_APP_INIT;
            break;

        case USBH_APP_READY:
            switch (userAppState)
            {
                case USER_APP_INIT:
                    printf("\r\n***********OP Menu************\r\n");
                    printf("1 - Press KEY 1 to scan the files on the USB drive\r\n");
                    printf("2 - Press KEY 2 to write and read string into file on the USB drive\r\n");
                    printf("***********OP Menu************\r\n");

                    userAppState = USER_APP_RUN;
                    break;

                case USER_APP_RUN:
                    if (!APM_MINI_PBGetState(BUTTON_KEY1))
                    {
                        APM_DelayMs(10);
                        if (!APM_MINI_PBGetState(BUTTON_KEY1))
                        {
                            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ Write files to U disk ------ \r\n");

                                if (f_mount(&fatfs, "0:", 0) == FR_OK)
                                {
                                    FATFS_WriteFile(&file);
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!APM_MINI_PBGetState(BUTTON_KEY1));
                        }
                    }

                    if (!APM_MINI_PBGetState(BUTTON_KEY2))
                    {
                        APM_DelayMs(10);
                        if (!APM_MINI_PBGetState(BUTTON_KEY2))
                        {
                            if (gUsbHostFS.devInfo.connectedStatus == ENABLE)
                            {
                                printf("------ read U disk file ------\r\n");

                                if (f_mount(&fatfs, "0:", 0) == FR_OK)
                                {
                                    FATFS_ReadFile(&file);
                                }
                                else
                                {
                                    printf("fatFs mount fail!\r\n");
                                }
                            }

                            while (!APM_MINI_PBGetState(BUTTON_KEY2));
                        }
                    }
                    break;
            }
            break;

        default:
            break;
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

/**@} end of group OTG_MSC_FWUpgradeApp_Functions */
/**@} end of group OTG_MSC_FWUpgradeApp */
/**@} end of group Examples */
