/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2025-4-23
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
#include "i2c.h"
#include "uart.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOI2C
  @{
*/

/** @defgroup USARTTOI2C_Functions Functions
  @{
*/

void Delay(uint32_t dy);
uint8_t checksum(uint8_t* dat, uint16_t len);
void ConfigLED(void);

/*!
 * @brief   Main program
 *
 * @param   None
 *
 * @retval  None
 *
 */
int main(void)
{
    uint32_t i;
    uint16_t len;
    uint16_t temp;

    /* System clocks configuration */
    SystemClockConfig();

    USART_Init();
    I2C_Init(400000);

    while(1)
    {
        USART_Read(g_i2cBuf, 1);
        if(g_i2cBuf[0] == 0x7f)
        {
            g_i2cBuf[0] = 0x79;
            USART_Write(g_i2cBuf, 1);
            continue;
        }
        USART_Read(&g_i2cBuf[1], 1);
        if (g_i2cBuf[0] == 0x43)
        {
            g_i2cBuf[0] = 0x44;
            g_i2cBuf[1] = 0xBB;
            I2C_Write(g_i2cBuf, 2);

            g_i2cBuf[0] = 0x43;
            g_i2cBuf[1] = 0xBC;
        }
        else
        {
            I2C_Write(g_i2cBuf, 2);
        }

        Delay(10);

        switch(g_i2cBuf[0])
        {
            case 0:
                I2C_Read(g_i2cBuf, 1);
                Delay(10);
                I2C_Read(&g_i2cBuf[1], 13);
                Delay(10);
                I2C_Read(&g_i2cBuf[14], 1);
                /* Upper Computer I2C Version Number */
                g_i2cBuf[2] = 0x32;
                USART_Write(g_i2cBuf, 15);
                break;
            case 1:
                I2C_Read(g_i2cBuf, 1);
                Delay(10);
                I2C_Read(&g_i2cBuf[1], 1);
                Delay(10);
                I2C_Read(&g_i2cBuf[4], 1);
                g_i2cBuf[2] = 0;
                g_i2cBuf[3] = 0;
                USART_Write(g_i2cBuf, 5);
                break;
            case 2:
                I2C_Read(g_i2cBuf, 1);
                Delay(10);
                I2C_Read(&g_i2cBuf[1], 3);
                Delay(10);
                I2C_Read(&g_i2cBuf[4], 1);
                Delay(10);
                USART_Write(g_i2cBuf, 5);
                break;
            case 0x11:
                ReadACK();

                /* Write Address */
                WriteAddr();

                /* Read length */
                USART_Read(g_i2cBuf, 2);
                len = g_i2cBuf[0] + 1;

                if (len >= 0xFF)
                {
                    /* First stage */
                    g_i2cBuf[0] = 0xF0 -1;
                    g_i2cBuf[1] = ~g_i2cBuf[0];
                    /* Write length */
                    I2C_Write(g_i2cBuf, 2);
                    ReadACK();
                    /* Read data */
                    I2C_Read(&g_i2cBuf[1], 0xF0);

                    /* Second stage */
                    uint8_t g_i2cBuf_wkl[32];
                    g_i2cBuf_wkl[0] = 0x11;
                    g_i2cBuf_wkl[1] = 0xEE;
                    /* Write address */
                    I2C_Write(g_i2cBuf_wkl, 2);
                    /* Read ACK */
                    I2C_Read(g_i2cBuf_wkl, 1);
                    if (g_i2cBuf_wkl[0] == 0x1F)
                    {
                        break;
                    }

                    g_addr += 0xF0;
                    g_i2cBuf_wkl[0] = (g_addr >> 24) & 0xFF;
                    g_i2cBuf_wkl[1] = (g_addr >> 16) & 0xFF;
                    g_i2cBuf_wkl[2] = (g_addr >> 8) & 0xFF;
                    g_i2cBuf_wkl[3] = g_addr & 0xFF;
                    g_i2cBuf_wkl[4] = checksum(g_i2cBuf_wkl, 4);
                    /* Write address */
                    I2C_Write(g_i2cBuf_wkl, 5);
                    /* Read ACK */
                    I2C_Read(g_i2cBuf_wkl, 1);
                    if (g_i2cBuf_wkl[0] == 0x1F)
                    {
                        break;
                    }

                    temp = len - 0xF0;
                    g_i2cBuf_wkl[0] = temp - 1;
                    g_i2cBuf_wkl[1] = ~g_i2cBuf_wkl[0];
                    /* Write length */
                    I2C_Write(g_i2cBuf_wkl, 2);
                    /* Read ACK */
                    I2C_Read(g_i2cBuf_wkl, 1);
                    if (g_i2cBuf_wkl[0] == 0x1F)
                    {
                        break;
                    }
                    I2C_Read(&g_i2cBuf[0xF0 + 1], temp);
                    __NOP();

                }
                else
                {
                    temp = len;
                    I2C_Write(g_i2cBuf, 2);
                    ReadACK();
                    I2C_Read(&g_i2cBuf[1], temp);
                }
                USART_Write(&g_i2cBuf[1], len);
                break;
            case 0x21:
                ReadACK();
                WriteAddr();
                ReadACK();
                break;
            case 0x31:
                ReadACK();
                USART_Read(g_i2cBuf, 5);
                I2C_Write(g_i2cBuf, 5);
                I2C_Read(g_i2cBuf, 1);
                Delay(200);
                USART_Write(g_i2cBuf, 1);
                if(g_i2cBuf[0] == 0x1f)
                {
                    break;
                }
                USART_Read(g_i2cBuf, 1);
                USART_Read(&g_i2cBuf[1], g_i2cBuf[0] + 2);
                I2C_Write(g_i2cBuf, g_i2cBuf[0] + 3);
                Delay(3000);
                ReadACK();
                break;
            case 0x43:
                ReadACK();

                USART_Read(g_i2cBuf, 1);
                len = g_i2cBuf[0];
                if((len & 0xff) == 0xff)
                {
                    /* Read for 0x00 */
                    USART_Read(&g_i2cBuf[1], 1);
                    g_i2cBuf[1] = 0xFF;
                    g_i2cBuf[2] = 0x00;
                    I2C_Write(g_i2cBuf, 3);
                    Delay(200);
                    ReadACK();
                    break;
                }
                else
                {
                    g_i2cBuf[1] = 0;
                    g_i2cBuf[2] = g_i2cBuf[0] ^ g_i2cBuf[1];
                    /* Write Page Number and Checksum */
                    I2C_Write(g_i2cBuf, 3);
                    Delay(200);
                    /* Read ACK */
                    I2C_Read(g_i2cBuf, 1);
                    if(g_i2cBuf[0] == 0x1f)
                    {
                        break;
                    }

                    ++len;

                    for (i = 0; i < len * 2; i+=2)
                    {
                        g_i2cBuf[2 + i] = 0;
                        USART_Read(&g_i2cBuf[2 + i + 1], 1);
                    }
                    /* Page verification of the 0x43 command, received and discarded. */
                    USART_Read(&g_i2cBuf[2 + len * 2], 1);
                    g_i2cBuf[2 + len * 2] = checksum(g_i2cBuf, 2 + len * 2);
                    /* Write Page Number and Checksum */
                    I2C_Write(&g_i2cBuf[2], len * 2 + 1);
                }
                Delay(300);
                ReadACK();
                Delay(300);
                break;
            case 0x44:
                ReadACK();

                USART_Read(g_i2cBuf, 2);
                len = (g_i2cBuf[0] << 8) | g_i2cBuf[1];
                if((len & 0xfff0) == 0xfff0)
                {
                    USART_Read(&g_i2cBuf[2], 1);
                    I2C_Write(g_i2cBuf, 3);
                    Delay(200);
                    ReadACK();
                    break;
                }
                else
                {
                    g_i2cBuf[2] = g_i2cBuf[0] ^ g_i2cBuf[1];
                    /* Write Page Number and Checksum */
                    I2C_Write(g_i2cBuf, 3);
                    Delay(200);
                    /* Read ACK */
                    I2C_Read(g_i2cBuf, 1);
                    if(g_i2cBuf[0] == 0x1f)
                    {
                        break;
                    }
                }
                /* Page Number + Check Code length */
                len = (len + 1) * 2 + 1;
                USART_Read(g_i2cBuf, len);
                g_i2cBuf[len - 1] = checksum(g_i2cBuf, len - 1);
                I2C_Write(g_i2cBuf, len);

                Delay(300);
                ReadACK();
                Delay(300);
                break;
            case 0x63:
                ReadACK();

                USART_Read(g_i2cBuf, 1);
                len = g_i2cBuf[0] + 2;
                g_i2cBuf[1] = ~g_i2cBuf[0];
                I2C_Write(g_i2cBuf, 2);
                I2C_Read(g_i2cBuf, 1);
                if(g_i2cBuf[0] == 0x1f)
                {
                    break;
                }

                USART_Read(g_i2cBuf, len);
                g_i2cBuf[len - 1] = checksum(g_i2cBuf, len-1);
                I2C_Write(g_i2cBuf, len);
                Delay(200);
                ReadACK();
                break;
            case 0x73:
            case 0x82:
            case 0x92:
                ReadACK();
                ReadACK();
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief   Delay function
 *
 * @param   dy: Loop decrement five times
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
 * @brief   Calculate XOR sum
 *
 * @param   dat: The array involved in the calculation;
 *
 * @param   len: The length of the array
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

/**@} end of group USARTTOI2C_Functions */
/**@} end of group USARTTOI2C */
/**@} end of group Examples */
