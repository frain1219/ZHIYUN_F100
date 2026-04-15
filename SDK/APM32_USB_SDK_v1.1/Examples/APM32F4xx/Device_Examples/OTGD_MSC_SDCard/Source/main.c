/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "board.h"
#include "bsp_delay.h"
#include "bsp_sdcard.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usart.h"
#include "usb_device_user.h"
#include <stdio.h>

/** @addtogroup Examples
  * @brief OTGD MSC SD Card examples
  @{
  */

/** @addtogroup OTGH_MSC_SDCard
  @{
  */

/** @defgroup OTGH_MSC_SDCard_Macros Macros
  @{
*/

#define DEBUG_USART    USART1

/**@} end of group OTGH_MSC_SDCard_Macros*/

/** @defgroup OTGH_MSC_SDCard_Enumerates Enumerates
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

/**@} end of group OTGH_MSC_SDCard_Enumerates*/

/** @defgroup OTGH_MSC_SDCard_Functions Functions
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
    APM_DelayInit();
    
    /* Wait board and module power on */
    SD_ResetSDIOReg();
    APM_DelayMs(200);
    
    printf("USB Device MSC SD Card Application\r\n");

    if(SD_Init(&sdmmcInfo) == SD_OK)
    {
        SD_ShowCardInfo(&sdmmcInfo);

        /* Init USB device */
        USB_DeviceInit();

        while (1)
        {
            USB_DevUserApplication();
        }
    }
    else
    {
        printf("SD Card Init Fail. Please check the SD card and reset and try again.");
        while(1);
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

/**@} end of group OTGH_MSC_SDCard_Functions */
/**@} end of group OTGH_MSC_SDCard */
/**@} end of group Examples */
