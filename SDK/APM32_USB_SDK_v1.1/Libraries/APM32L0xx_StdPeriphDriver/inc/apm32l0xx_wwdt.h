/*!
 * @file        apm32l0xx_wwdt.h
 *
 * @brief       This file contains all the functions prototypes for the WWDG firmware library
 *
 * @version     V1.0.0
 *
 * @date        2021-12-25
 *
 * @attention
 *
 *  Copyright (C) 2020-2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#ifndef __APM32L0xx_WWDT_H
#define __APM32L0xx_WWDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup WWDT_Driver WWDT Driver
  @{
*/

/** @addtogroup  WWDT_Enumerations Enumerations
  @{
*/

/**
 * @brief   WWDT Timebase(Prescaler) define
 */
typedef enum
{
    WWDT_DIV_1,
    WWDT_DIV_2,
    WWDT_DIV_4,
    WWDT_DIV_8
} WWDT_DIV_T;

/**@} end of group WWDT_Enumerations*/

/** @addtogroup  WWDT_Fuctions Fuctions
  @{
*/

/** WWDT reset */
void WWDT_Reset(void);

/** Set WWDT Timebase */
void WWDT_SetTimebase(uint32_t div);

/** Set Window Data */
void WWDT_ConfigWindowValue(uint16_t windowValue);

/** Set Couter */
void WWDT_ConfigCounter(uint8_t couter);

/** Enable WWDT and Early Wakeup interrupt */
void WWDT_EnableEWI(void);
void WWDT_Enable(uint8_t count);

/** Read Flag and Clear Flag */
uint8_t WWDT_ReadStatusFlag(void);
void WWDT_ClearStatusFlag(void);

/**@} end of group WWDT_Fuctions*/
/**@} end of group WWDT_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_WWDT_H */

