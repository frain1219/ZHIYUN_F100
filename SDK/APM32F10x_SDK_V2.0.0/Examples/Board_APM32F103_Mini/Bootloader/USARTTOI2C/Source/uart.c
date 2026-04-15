/*!
 * @file        uart.h
 *
 * @brief       uart driver
 *
 * @version     V1.0.0
 *
 * @date        2025-04-23
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

/* Includes */
#include "uart.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOI2C
  @{
*/

/** @defgroup USARTTOI2C_Variables Variables
  @{
*/

uint8_t  g_uartRxBuf[512];
uint16_t g_uartRxBufWpt = 0;
uint16_t g_uartRxBufRpt = 0;

/**@} end of group USARTTOI2C_Variables */

/** @defgroup USARTTOI2C_Functions Functions
  @{
*/

/*!
 * @brief   USART Init
 *
 * @param   None
 *
 * @retval  None
 *
 */
void USART_Init(void)
{
    GPIO_Config_T GPIO_configStruct;
    USART_Config_T USART_ConfigStruct;

    RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)(RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_USART1));

    /* Configure USART Tx as alternate function push-pull */
    GPIO_configStruct.mode = GPIO_MODE_AF_PP;
    GPIO_configStruct.pin = GPIO_PIN_9;
    GPIO_configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_configStruct);

    /* Configure USART Rx as input floating */
    GPIO_configStruct.mode = GPIO_MODE_IN_FLOATING;
    GPIO_configStruct.pin = GPIO_PIN_10;
    GPIO_Config(GPIOA, &GPIO_configStruct);

    /* USART Configuration */
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_EVEN;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_9B;
    USART_Config(USART1, &USART_ConfigStruct);

    NVIC_EnableIRQRequest(USART1_IRQn, 0, 0);

    USART_EnableInterrupt(USART1, USART_INT_RXBNE);

    USART_Enable(USART1);
}

/*!
 * @brief   USART Send Byte
 *
 * @param   dat: send byte
 *
 * @retval  None
 *
 */
void USART_SendByte(uint8_t dat)
{
    while(USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
    USART_TxData(USART1, dat);
}

/*!
 * @brief   USART write data
 *
 * @param   dat: the address of data
 *
 * @param   len: the length of data
 *
 * @retval  None
 *
 */
void USART_Write(uint8_t *dat, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        USART_SendByte(dat[i]);
    }
}

/*!
 * @brief   USART read data
 *
 * @param   dat: the address of data
 *
 * @param   len: the length of data
 *
 * @retval  None
 *
 */
void USART_Read(uint8_t *dat, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        while(g_uartRxBufRpt == g_uartRxBufWpt);
        dat[i] = g_uartRxBuf[g_uartRxBufRpt++];
        if(g_uartRxBufRpt == sizeof(g_uartRxBuf))
        {
            g_uartRxBufRpt = 0;
        }
    }
}

/*!
 * @brief   USART Send Byte
 *
 * @param   None
 *
 * @retval  read byte
 *
 */
uint8_t USART_ReadByte(void)
{
    uint8_t dat;

    while(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE) == RESET);

    dat = USART_RxData(USART1);

    return dat;
}

/*!
 * @brief   USART Interrupt Service Function
 *
 * @param   None
 *
 * @retval  None
 *
 */
void USART1_IRQHandler(void)
{
    if(USART_ReadStatusFlag(USART1, USART_FLAG_RXBNE))
    {
        g_uartRxBuf[g_uartRxBufWpt++] = USART_RxData(USART1);
        if(g_uartRxBufWpt == sizeof(g_uartRxBuf))
        {
            g_uartRxBufWpt = 0;
        }
    }
}

/**@} end of group USARTTOI2C_Functions */
/**@} end of group USARTTOI2C */
/**@} end of group Examples */
