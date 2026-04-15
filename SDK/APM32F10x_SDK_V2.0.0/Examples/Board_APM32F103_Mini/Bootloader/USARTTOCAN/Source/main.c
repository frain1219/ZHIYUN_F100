/*!
 * @file        mian.c
 *
 * @brief       Implement Serial Port to CAN Communication
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
#include "config.h"
#include <string.h>
#include "can.h"
#include "uart.h"
#include "main.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOCAN
  @{
*/

/** @defgroup USARTTOCAN_Macros Macros
  @{
*/
#define LED2_OFF GPIOE->ODR |= 0x00000040;
#define LED3_OFF GPIOE->ODR |= 0x00000020;

#define LED2_ON GPIOE->ODR &= ~0x00000040;
#define LED3_ON GPIOE->ODR &= ~0x00000020;

#define LED2_Toggle GPIOE->ODR ^= 0x00000040;
#define LED3_Toggle GPIOE->ODR ^= 0x00000020;

#define APM32F072_OPTION_ADDR         (0x1FFFF800)
#define APM32F407_OPTION_ADDR         (0x1FFFC000)
#define APM32F403_OPTION_ADDR         (0x1FFFC000)
#define OPTION_ADDR          APM32F403_OPTION_ADDR

/**@} end of group USARTTOCAN_Macros */

/** @defgroup USARTTOCAN_Variables Variables
  @{
*/
extern uint8_t  g_uartRxBuf[512];
extern uint16_t g_uartRxBufWpt;
extern uint16_t g_uartRxBufRpt;

extern CAN_RxMessage_T g_canBuf[CAN_BUF_CNT];
extern uint8_t g_canBufRpt;
extern uint8_t g_canBufWpt;

CAN_TxMessage_T canTxMsg;
CAN_RxMessage_T canRxMsg;

uint8_t g_dataBuf[512];

/**@} end of group USARTTOCAN_Variables */

/** @defgroup USARTTOCAN_Functions Functions
  @{
*/

void Can_Protocol(void);

void Delay(uint32_t dy);
uint8_t checksum(uint8_t* dat, uint16_t len);

int main(void)
{
    /* System clocks configuration */
    SystemClockConfig();

    USART_Init();
    Can_Config();
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);

    BOARD_LEDOff(LED2);
    BOARD_LEDOn(LED3);

    for (uint8_t i = 0; i < 6; i++)
    {
        BOARD_LEDToggle(LED2);
        BOARD_LEDToggle(LED3);
        Delay(0x3FFFF);
    }
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);
    Wait7F();
    canTxMsg.data[0] = 0X79;
    canTxMsg.dataLengthCode = 0;
    canTxMsg.extID = 0;
    canTxMsg.typeID = CAN_TYPEID_STD;
    canTxMsg.remoteTxReq = CAN_RTXR_DATA;
    canTxMsg.stdID = 0x79;

    CAN_TxMessage(CAN1, &canTxMsg);
    Delay(5000);

    while (!Can_Read(&canRxMsg))
    {
        if ((canRxMsg.stdID == 0x79) && (canRxMsg.data[0] == 0x79))
        {
            g_canBufRpt = 0;
            g_canBufWpt = 0;
            break;
        }
    }

    while (1)
    {
        Can_Protocol();
    }
}

/*!
 * @brief   USART to CAN Protocol
 *
 * @param   None
 *
 * @retval  None
 *
 */
void Can_Protocol(void)
{
    uint16_t i;
    uint16_t j;
    uint16_t len;
    uint32_t add;
    
    USART_Read(g_dataBuf, 1);

    if (g_dataBuf[0] == 0x7f)
    {
        g_dataBuf[0] = 0x79;
        USART_Write(g_dataBuf, 1);

        canTxMsg.data[0] = 0X79;
        canTxMsg.dataLengthCode = 0;
        canTxMsg.extID = 0;
        canTxMsg.typeID = CAN_TYPEID_STD;
        canTxMsg.remoteTxReq = CAN_RTXR_DATA;
        canTxMsg.stdID = 0x79;

        CAN_TxMessage(CAN1, &canTxMsg);
        Delay(5000);

        while (!Can_Read(&canRxMsg))
        {
            if ((canRxMsg.stdID == 0x79) && (canRxMsg.data[0] == 0x79))
            {
                g_canBufRpt = 0;
                g_canBufWpt = 0;
                break;
            }
        }

        return;
    }

    USART_Read(&g_dataBuf[1], 1);

    switch (g_dataBuf[0])
    {
        case 0X7F:
            USART_SendByte(0x79);
            break;

        case 0x03:
            Can_Write(0x03, g_dataBuf, 0);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }

            g_dataBuf[0] = 0x01;
            Can_Write(0x03, g_dataBuf, 1);
            Can_ReadSlaveAck(&canRxMsg);

        case 0:
            Can_Write(0, g_dataBuf, 0);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }

            Can_ReadDataFromSlave(g_dataBuf, 1);
            Can_ReadDataFromSlave(&g_dataBuf[1], g_dataBuf[0] + 2);
            USART_SendByte(g_dataBuf[0] - 1);
            USART_Write(&g_dataBuf[1], 4);
            USART_Write(&g_dataBuf[6], 9);
            break;

        case 1:
            Can_Write(1, g_dataBuf, 0);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }
            Can_ReadDataFromSlave(g_dataBuf, 4);
            USART_Write(&g_dataBuf[0], 4);
            break;

        case 2:
            Can_Write(2, g_dataBuf, 0);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }

            Can_ReadDataFromSlave(g_dataBuf, 3);
            USART_SendByte(0X01);
            USART_Write(g_dataBuf, 3);
            break;

        case 0x11:
            USART_SendByte(0x79);

            USART_Read(g_dataBuf, 5);

            USART_SendByte(0x79);

            USART_Read(&g_dataBuf[4], 2);
            len = g_dataBuf[4] + 1;
            Can_Write(0x11, g_dataBuf, 5);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                USART_SendByte(0x1f);
                break;
            }

            Can_ReadDataFromSlave(g_dataBuf, len);

            while (!Can_Read(&canRxMsg));
            USART_Write(g_dataBuf, len);
            break;

        case 0x21:
            USART_SendByte(0x79);

            USART_Read(g_dataBuf, 5);
            USART_SendByte(0x79);

            Can_Write(0x21, g_dataBuf, 4);
            Can_ReadSlaveAck(&canRxMsg);
            Delay(1000);
            NVIC_SystemReset();
            break;

        case 0x31:
            USART_SendByte(0x79);

            USART_Read(g_dataBuf, 5);

            add = (g_dataBuf[0] << 24) | (g_dataBuf[1] << 16) | (g_dataBuf[2] << 8) | g_dataBuf[3];

            USART_SendByte(0x79);

            USART_Read(&g_dataBuf[4], 1);
            len = g_dataBuf[4] + 1;
            Can_Write(0x31, g_dataBuf, 5);

            while (!Can_Read(&canRxMsg));

            if (canRxMsg.data[0] != 0x79)
            {
                USART_SendByte(0x1f);
                break;
            }

            USART_Read(g_dataBuf, len + 1);

            for (i = 0; i < len; i += j)
            {
                j = len - i;

                if (j >= 8)
                {
                    j = 8;
                }

                Can_Write(0x31, &g_dataBuf[i], j);

                while (!Can_Read(&canRxMsg));

                if (canRxMsg.data[0] != 0x79)
                {
                    break;
                }
            }

            if (i < len)
            {
                USART_SendByte(0x1f);
            }
            else
            {
                while (!Can_Read(&canRxMsg));

                if (canRxMsg.data[0] != 0x79)
                {
                    USART_SendByte(0x1f);
                    break;
                }

                USART_SendByte(0x79);
                if (add == OPTION_ADDR)
                {
                    for (uint32_t delay = 0; delay < 0x5FFFF; delay++);
                    NVIC_SystemReset();
                }

            }

            break;

        case 0x43:
            USART_SendByte(0x79);

            USART_Read(g_dataBuf, 1);

            if (g_dataBuf[0] == 0xff)
            {
                USART_Read(&g_dataBuf[1], 1);
                Can_Write(0x43, g_dataBuf, 1);

                /* two ACK */
                while (!Can_Read(&canRxMsg));

                if (canRxMsg.data[0] == 0x1F)
                {
                    USART_SendByte(0x1f);
                }

                if (Can_ReadSlaveAck(&canRxMsg))
                {
                    break;
                }
            }
            else
            {
                Can_Write(0x43, g_dataBuf, 1);
                len = g_dataBuf[0] + 1;

                while (!Can_Read(&canRxMsg));

                USART_Read(g_dataBuf, len + 1);

                for (i = 0; i < len; i += j)
                {
                    j = len - i;

                    if (j >= 8)
                    {
                        j = 8;
                    }

                    Can_Write(0x43, &g_dataBuf[i], j);

                    while (!Can_Read(&canRxMsg));

                    if (canRxMsg.data[0] != 0x79)
                    {
                        break;
                    }
                }

                if (i < len)
                {
                    USART_SendByte(0x1f);
                }
                else
                {
                    while (!Can_Read(&canRxMsg));

                    if (canRxMsg.data[0] != 0x79)
                    {
                        USART_SendByte(0x1f);
                    }

                    USART_SendByte(0x79);
                }
            }

            break;

        case 0x63:
            USART_SendByte(0x79);

            USART_Read(g_dataBuf, 1);
            len = g_dataBuf[0] + 1;
            Can_Write(0x63, g_dataBuf, 1);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }

            USART_Read(g_dataBuf, len +1);

            for (i = 0; i < len; i += j)
            {
                j = len - i;

                if (j >= 8)
                {
                    j = 8;
                }

                Can_Write(0x63, &g_dataBuf[i], j);

                while (!Can_Read(&canRxMsg));

                if (canRxMsg.data[0] != 0x79)
                {
                    break;
                }
            }

            if (i < len)
            {
                USART_SendByte(0x1f);
                break;
            }
            Can_ReadSlaveAck(&canRxMsg);
            USART_SendByte(0X79);
            Delay(1000);
            NVIC_SystemReset();
            break;

        case 0x73:
        case 0x82:
        case 0x92:
            Can_Write(g_dataBuf[0], g_dataBuf, 1);

            if (!Can_ReadSlaveAck(&canRxMsg))
            {
                break;
            }

            Can_ReadSlaveAck(&canRxMsg);

            for (uint32_t delay = 0; delay < 0x3FFFF; delay++);
            NVIC_SystemReset();

            break;

        default:
            break;
    }

}

/*!
 * @brief   Delay Function
 *
 * @param   dy: Decrease in a loop five times
 *
 * @retval  None
 *
 */
void Delay(uint32_t dy)
{
    uint8_t i;
    uint32_t delay;

    for (i = 0; i < 5; i++)
    {
        delay = dy;

        while (delay--);
    }
}

/*!
 * @brief   Calculate XOR Sum
 *
 * @param   dat: The array involved in the calculation
 *
 * @param   len: The array length
 *
 * @retval  None
 *
 */
uint8_t checksum(uint8_t* dat, uint16_t len)
{
    uint16_t i;
    uint8_t checksum = 0;

    for (i = 0; i < len; i++)
    {
        checksum ^= dat[i];
    }

    return checksum;
}

/**@} end of group USARTTOCAN_Functions */
/**@} end of group USARTTOCAN */
/**@} end of group Examples */
