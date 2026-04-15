/*!
 * @file        apm32l0xx_dbgmcu.h
 *
 * @brief       This file contains all the functions prototypes for the DBGMCU firmware library
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

#ifndef __APM32L0xx_DBGMCU_H
#define __APM32L0xx_DBGMCU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup DBGMCU_Driver DBGMCU Driver
  @{
*/

/** @addtogroup DBGMCU_Enumerations Enumerations
  @{
*/

/**
 * @brief   MCU Debug mode in the low power mode behavior
 */
typedef enum
{
    DBGMCU_MODE_SLEEP   = BIT0,
    DBGMCU_MODE_STOP    = BIT1,
    DBGMCU_MODE_STANDBY = BIT2,
} DBGMCU_MODE_T;

/**
 * @brief   MCU Debug mode in the APB1 peripheral behavior
 */
typedef enum
{
    DBGMCU_APB1_PER_TMR2_STOP   = BIT0,
    DBGMCU_APB1_PER_TMR3_STOP   = BIT1,
    DBGMCU_APB1_PER_TMR6_STOP   = BIT4,
    DBGMCU_APB1_PER_TMR7_STOP   = BIT5,
    DBGMCU_APB1_PER_RTC_STOP    = BIT10,
    DBGMCU_APB1_PER_WWDT_STOP   = BIT11,
    DBGMCU_APB1_PER_IWDT_STOP   = BIT12,
    DBGMCU_APB1_PER_I2C1_SMBUS_TIMEOUT_STOP = BIT21,
    DBGMCU_APB1_PER_I2C3_SMBUS_TIMEOUT_STOP = BIT30,
    DBGMCU_APB1_PER_LPTMR1_STOP = (int32_t)BIT31
} DBGMCU_APB1_PER_T;

/**
 * @brief   MCU Debug mode in the APB2 peripheral behavior
 */
typedef enum
{
    DBGMCU_APB2_PER_TMR21_STOP   = BIT2,
    DBGMCU_APB2_PER_TMR22_STOP   = BIT5
} DBGMCU_APB2_PER_T;

/**@} end of group DBGMCU_Enumerations*/

/** @addtogroup DBGMCU_Fuctions Fuctions
  @{
*/

/** Read MCU ID Code  */
uint32_t DBGMCU_ReadDevId(void);
uint32_t DBGMCU_ReadRevId(void);

/** Debug Mode */
void DBGMCU_EnableDebugMode(uint32_t mode);
void DBGMCU_DisableDebugMode(uint32_t mode);

/** APB1 peripheral */
void DBGMCU_EnableAPB1Periph(uint32_t peripheral);
void DBGMCU_DisableAPB1Periph(uint32_t peripheral);

/** APB2 peripheral */
void DBGMCU_EnableAPB2Periph(uint32_t peripheral);
void DBGMCU_DisableAPB2Periph(uint32_t peripheral);

/**@} end of group DBGMCU_Fuctions*/
/**@} end of group DBGMCU_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_DBGMCU_H */
