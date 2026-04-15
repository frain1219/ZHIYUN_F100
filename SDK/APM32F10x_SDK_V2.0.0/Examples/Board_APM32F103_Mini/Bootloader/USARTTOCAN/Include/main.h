/*!
 * @file        main.h
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

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include <stdio.h>
#include "Board.h"
#include "apm32f10x_can.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup USARTTOCAN
  @{
  */

/** @defgroup USARTTOCAN_Variables Variables
  @{
  */

extern uint8_t intFlag;

/**@} end of group USARTTOCAN_Variables*/

/** @defgroup USARTTOCAN_Functions Functions
  @{
*/

void USART_Init(void);
void CAN_GPIO_Init(void);
void CAN_Config_Init(void);
void CAN_Transmit_Init(void);
void CAN1_RX0_ISR(void);
void CAN2_RX0_ISR(void);
void NVIC_Configuration(void);

/**@} end of group USARTTOCAN_Functions */
/**@} end of group USARTTOCAN */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif
