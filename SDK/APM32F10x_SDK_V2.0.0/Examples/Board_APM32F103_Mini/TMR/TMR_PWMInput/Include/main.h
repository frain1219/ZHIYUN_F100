/*!
 * @file        main.h
 *
 * @brief       Header for main.c module
 *
 * @version     V1.0.2
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
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
#include "stdio.h"
#include "apm32f10x.h"
#include "apm32f10x_tmr.h"
#include "apm32f10x_usart.h"
#include "apm32f10x_rcm.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_PWMInput
  @{
*/

/** @defgroup TMR_PWMInput_Variables Variables
  @{
*/

extern volatile uint32_t tick;

extern uint8_t  irqFlag;
extern uint32_t IC2Value;
extern uint32_t DutyCycle;
extern uint32_t Frequency;

/**@} end of group TMR_PWMInput_Variables */

/** @defgroup TMR_PWMInput_Functions Functions
  @{
*/

void Delay(void);

/**@} end of group TMR_PWMInput_Functions */
/**@} end of group TMR_PWMInput */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif /*__MAIN_H */
