/*!
 * @file        i2c.h
 *
 * @brief       i2c driver
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

/* Define to prevent recursive inclusion */
#ifndef __I2C_H
#define __I2C_H

/* Includes */
#include "config.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOI2C
  @{
*/

/** @defgroup USARTTOI2C_Macros Macros
  @{
*/

#define ReadACK()\
{\
    Delay(10);\
    I2C_Read(g_i2cBuf, 1);\
    USART_Write(g_i2cBuf, 1);\
    if(g_i2cBuf[0] == 0x1f)\
    {\
        break;\
    }\
}

#define WriteAddr()\
{\
    USART_Read(g_i2cBuf, 5);\
    g_addr = (g_i2cBuf[0] << 24) | (g_i2cBuf[1] << 16) | (g_i2cBuf[2] << 8) | (g_i2cBuf[3]);\
    I2C_Write(g_i2cBuf, 5);\
    I2C_Read(g_i2cBuf, 1);\
    USART_Write(g_i2cBuf, 1);\
    if(g_i2cBuf[0] == 0x1f)\
    {\
        break;\
    }\
}\

/**@} end of group USARTTOI2C_Macros */

/** @defgroup USARTTOI2C_Variables Variables
  @{
*/

extern uint8_t g_i2cBuf[512];
extern uint32_t g_addr;

/**@} end of group USARTTOI2C_Variables */

/** @defgroup USARTTOI2C_Functions Functions
  @{
*/

void I2C_Init(uint32_t speed);
void I2C_Write(uint8_t *dat, uint16_t len);
void I2C_Read(uint8_t *dat, uint16_t len);

/**@} end of group USARTTOI2C_Functions */
/**@} end of group USARTTOI2C */
/**@} end of group Examples */

#endif
