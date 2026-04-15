/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2026-03-01
 *
 * @attention
 *
 *  Copyright (C) 2023-2026 Geehy Semiconductor
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
#include "Board.h"

/* Private includes *******************************************************/
#include "board_delay.h"
#include "apm32f10x_misc.h"
#include "apm32f10x_usart.h"
#include "usb_device_user.h"
#include "usbd_composite.h"
#include "usbd_composite_cdc_vcp.h"
#include "usbd_composite_winusb_itf.h"
#include <stdio.h>
#include <string.h>

/* Private macro **********************************************************/
#define DEBUG_USART    USART1

/* Private typedef ********************************************************/
/**
 * @brief User application state
 */
typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
} USER_APP_STATE_T;

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/
extern USBD_INFO_T gUsbDeviceFS;

extern USBD_CDC_VCP_INFO_T gCDCVCP;
extern uint8_t cdcTxBuffer[USBD_CDC_TX_BUF_LEN];
extern uint8_t cdcRxBuffer[USBD_CDC_RX_BUF_LEN];

extern USBD_WINUSB_ITF_INFO_T gWinusbItf;
extern uint8_t winusbTxBuffer[USBD_WINUSB_TX_BUF_LEN];
extern uint8_t winusbRxBuffer[USBD_WINUSB_RX_BUF_LEN];

/* External functions *****************************************************/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      int
 */
int main(void)
{
    /* System clocks configuration */
    SystemClockConfig();

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
    BOARD_COMInit(COM1, &usartConfigStruct);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Init USB device */
    USB_DeviceInit();

    printf("USB Composite Device Application\n");

    while (1)
    {
        USB_DevUserApplication();
    }
}

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_DevUserApplication(void)
{
    static uint8_t userAppState = USER_APP_INIT;
    static uint8_t interval = 0;
    
    switch (userAppState)
    {
        case USER_APP_INIT:
            interval = USBD_Composite_CDC_ReadInterval(&gUsbDeviceFS);
            userAppState = USER_APP_RUN;
            break;

        case USER_APP_RUN:
            if(gUsbDeviceFS.devState == USBD_DEV_CONFIGURE)
            {
                /* Loopback */
                if(gCDCVCP.state == USBD_CDC_VCP_REV_UPDATE)
                {
                    gCDCVCP.state = USBD_CDC_VCP_IDLE;
                    
                    USBD_FS_CDC_ItfSend((uint8_t *)cdcRxBuffer, (uint16_t)gCDCVCP.rxUpdateLen);
                    
                    USBD_FS_CDC_ItfSend((uint8_t *)"\r\n", strlen("\r\n"));
                }

                if(gWinusbItf.state == USBD_WINUSB_ITF_REV_UPDATE)
                {
                    gWinusbItf.state = USBD_WINUSB_ITF_IDLE;
                    
                    USBD_FS_WINUSB_ItfSend((uint8_t *)winusbRxBuffer, (uint16_t)gWinusbItf.rxUpdateLen);
                    
                    USBD_FS_WINUSB_ItfSend((uint8_t *)"\r\n", strlen("\r\n"));
                }

                BOARD_DelayMs(interval);
            }
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

    UNUSED(file);

    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif
