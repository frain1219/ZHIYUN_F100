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

/* Define to prevent recursive inclusion */
#ifndef __UART_H
#define __UART_H

/* Includes */
#include "config.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOSPI
  @{
*/

/** @defgroup USARTTOSPI_Macros Macros
  @{
*/

#define USART_ACK()    USART_SendByte(0x79)

/**@} end of group USARTTOSPI_Macros */

/** @defgroup USARTTOSPI_Functions Functions
  @{
*/

void USART_Init(void);
void USART_SendByte(uint8_t dat);
void USART_Write(uint8_t *dat, uint16_t len);
void USART_Read(uint8_t *dat, uint16_t len);
void USART_Wait7F(void);
uint8_t Utility_Checksum(uint8_t *dat, uint16_t len);
uint8_t USART_ReadByte(void);

/**@} end of group USARTTOSPI_Functions */
/**@} end of group USARTTOSPI */
/**@} end of group Examples */

#endif

