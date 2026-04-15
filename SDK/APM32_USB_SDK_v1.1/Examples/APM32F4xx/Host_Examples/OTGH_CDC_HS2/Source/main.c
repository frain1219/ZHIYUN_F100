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
#include "usbh_cdc.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup Examples
  * @brief OTGH CDC examples
  @{
  */

/** @addtogroup OTGH_CDC
  @{
  */

/** @defgroup OTGH_CDC_Macros Macros
  @{
*/
#define DEBUG_USART    USART1

/**@} end of group OTGH_CDC_Macros*/

/** @defgroup OTGH_CDC_Enumerates Enumerates
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

/**
 * @brief User application CDC state
 */
typedef enum
{
    USER_CDC_IDLE,
    USER_CDC_SEND_DATA,
    USER_CDC_REV_DATA,
} USER_CDC_STATE_T;

/**@} end of group OTGH_CDC_Enumerates*/

/** @defgroup OTGH_CDC_Variables Variables
  @{
  */

uint8_t usb_cdc_rxBuffer[10];

uint8_t userCDCDataState = USER_CDC_IDLE;

/**@} end of group OTGH_CDC_Variables*/

/** @defgroup OTGH_CDC_Functions Functions
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
    APM_EVAL_COMInit(COM1, &usartConfigStruct);
    APM_EVAL_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    APM_EVAL_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Init USB host */
    USB_HostInit();

    printf("USB Host CDC HS2 Application\r\n");
    printf("Please insert the CDC device\r\n");

    while (1)
    {
        USB_HostProcess();
        USB_HostUserApplication();
    }
}

/*!
 * @brief       USB host CDC send data finish callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
void USBH_CDC_XferEndCallback(USBH_INFO_T* usbInfo)
{
    userCDCDataState = USER_CDC_REV_DATA;
}

/*!
 * @brief       USB host CDC receive data finish
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
void USBH_CDC_RevEndCallback(USBH_INFO_T* usbInfo)
{
    uint32_t length;
    uint32_t i;

    length = USBH_CDC_ReadRevDataSize(usbInfo);

    if (length > 0)
    {
        for (i = 0; i < length; i++)
        {
            USBH_USR_LOG("%X ", usb_cdc_rxBuffer[i]);
        }
    }

    userCDCDataState = USER_CDC_IDLE;
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
    static uint8_t preCDCStatus = USER_CDC_IDLE;
    static uint8_t temp = 0;

    switch (status)
    {
        case USBH_APP_CONNECTION:
            break;

        case USBH_APP_DISCONNECTION:
            preCDCStatus = USER_CDC_IDLE;
            break;

        case USBH_APP_READY:

            switch (userAppState)
            {
                case USER_APP_INIT:
                    userAppState = USER_APP_RUN;
                    break;

                case USER_APP_RUN:

                    if (preCDCStatus != userCDCDataState)
                    {
                        switch (userCDCDataState)
                        {
                            case USER_CDC_IDLE:
                                break;

                            case USER_CDC_SEND_DATA:
                                USBH_CDC_SendData(&gUsbHostHS, &temp, 1);
                                temp++;
                                break;

                            case USER_CDC_REV_DATA:
                                USBH_CDC_ReceiveData(&gUsbHostHS, usb_cdc_rxBuffer, 1);
                                break;
                        }

                        preCDCStatus = userCDCDataState;
                    }

                    if (!APM_EVAL_PBGetState(BUTTON_KEY1))
                    {
                        APM_DelayMs(10);
                        if (!APM_EVAL_PBGetState(BUTTON_KEY1))
                        {
                            if (gUsbHostHS.devInfo.connectedStatus == ENABLE)
                            {
                                userCDCDataState = USER_CDC_SEND_DATA;
                            }

                            while (!APM_EVAL_PBGetState(BUTTON_KEY1));
                        }
                    }

                    if (!APM_EVAL_PBGetState(BUTTON_KEY2))
                    {
                        APM_DelayMs(10);
                        if (!APM_EVAL_PBGetState(BUTTON_KEY2))
                        {
                            if (gUsbHostHS.devInfo.connectedStatus == ENABLE)
                            {

                            }

                            while (!APM_EVAL_PBGetState(BUTTON_KEY2));
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

/**@} end of group OTGH_CDC_Functions */
/**@} end of group OTGH_CDC */
/**@} end of group Examples */
