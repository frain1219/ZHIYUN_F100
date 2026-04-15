/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes */
#include <stdio.h>
#include <stdarg.h>
#include "Board.h"
#include "apm32f10x_spi.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOSPI
  @{
*/

/** @defgroup USARTTOSPI_Enumerates Enumerates
  @{
*/

enum {FAILED, PASSED};

/**@} end of group USARTTOSPI_Enumerates */

/** @defgroup USARTTOSPI_Functions Functions
  @{
*/

/* FullDuplex Init*/
void USARTTOSPI_Init(void);
void USART_FullDuplex_Init(void);
/* Delay */
void Delay(uint32_t count);
/* SPI Write uint8_t dat */
void SPI_WriteByte(uint8_t dat);
/* compare Buffer */
uint8_t BufferCompare(uint8_t* buf1, uint8_t* buf2, uint8_t size);

#endif

/**@} end of group USARTTOSPI_Functions */
/**@} end of group USARTTOSPI */
/**@} end of group Examples */
