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

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "Board.h"
#include "apm32f10x_can.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup CAN_LoopBack
  @{
  */

/** @defgroup CAN_LoopBack_Variables Variables
  @{
  */

extern uint8_t intFlag;

/**@} end of group CAN_LoopBack_Variables*/

/** @defgroup CAN_LoopBack_Enumerates Enumerates
  @{
  */

enum {FAILED = 0, PASSED = 1};

/**@} end of group CAN_LoopBack_Enumerates*/

/** @defgroup CAN_LoopBack_Functions Functions
  @{
  */

void NVIC_Configuration(void);
uint8_t CAN_Polling(void);
uint8_t CAN_Interrupt(void);

/**@} end of group CAN_LoopBack_Functions */
/**@} end of group CAN_LoopBack */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif
