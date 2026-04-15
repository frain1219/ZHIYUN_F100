/*!
 * @file        main.h
 *
 * @brief       Main program body
 *
 * @version     V1.0.4
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
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

/** @addtogroup SPI_FullDuplex SPI FullDuplex
  @{
*/

/** @defgroup SPI_FullDuplex_Enumerates Enumerates
  @{
*/

enum {FAILED, PASSED};

/**@} end of group SPI_FullDuplex_Enumerates */

/** @defgroup SPI_FullDuplex_Functions Functions
  @{
*/

/* FullDuplex Init*/
void SPI_FullDuplex_Init(void);
void USART_FullDuplex_Init(void);
/* Delay */
void Delay(uint32_t count);
/* SPI Write uint8_t dat */
void SPI_WriteByte(uint8_t dat);
/* compare Buffer */
uint8_t BufferCompare(uint8_t* buf1, uint8_t* buf2, uint8_t size);

#endif

/**@} end of group SPI_FullDuplex_Functions */
/**@} end of group SPI_FullDuplex */
/**@} end of group Examples */
