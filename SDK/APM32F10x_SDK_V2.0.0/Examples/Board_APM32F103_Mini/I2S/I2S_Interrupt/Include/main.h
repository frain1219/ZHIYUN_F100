/*!
 * @file        main.h
 *
 * @brief       Main program body
 *
 * @version     V1.0.2
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
#include <string.h>
#include "Board.h"
#include "apm32f10x_fmc.h"
#include "apm32f10x_spi.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup I2S_Interrupt
  @{
  */

/** @defgroup I2S_Interrupt_Variables Variables
  @{
*/

extern const uint16_t I2S3_Buffer_Tx[32];
extern uint16_t I2S2_Buffer_Rx[32];
extern volatile uint32_t txCnt;
extern volatile uint32_t rxCnt;

/**@} end of group I2S_Interrupt_Variables */

/** @defgroup I2S_Interrupt_Functions Functions
  @{
  */

void RCM_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
uint8_t Buffercmp24bits(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

#endif

/**@} end of group I2S_Interrupt_Functions */
/**@} end of group I2S_Interrupt */
/**@} end of group Examples */
