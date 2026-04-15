/*!
 * @file        apm32l0xx_misc.h
 *
 * @brief       This file contains all the functions prototypes for the miscellaneous
 *              firmware library functions (add-on to CMSIS functions).
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

#ifndef __APM32L0xx_MISC_H
#define __APM32L0xx_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup MISC_Driver MISC Driver
  @{
*/

/** @addtogroup MISC_Enumerations Enumerations
  @{
*/

/**
 * @brief    System low power mode
 */
typedef enum
{
    NVIC_LOWPOWER_SEVONPEND   = 0x10, //!< Low Power SEV on Pend
    NVIC_LOWPOWER_SLEEPDEEP   = 0x04, //!< Low Power DEEPSLEEP request
    NVIC_LOWPOWER_SLEEPONEXIT = 0x02  //!< Low Power Sleep on Exit
} NVIC_LOWPOWER_T;

/**
 * @brief    MISC SysTick clock source
 */
typedef enum
{
    SYSTICK_CLKSOURCE_HCLK_DIV8 = 0x00, 
    SYSTICK_CLKSOURCE_HCLK      = 0x04
} SYSTICK_CLKSOURCE_T;

/**@} end of group MISC_Enumerations*/

/** @addtogroup MISC_Fuctions Fuctions
  @{
*/

/** NVIC */
void NVIC_EnableIRQRequest(IRQn_Type irq, uint8_t priority);
void NVIC_DisableIRQRequest(IRQn_Type irq);

/** Low Power */
void NVIC_EnableSystemLowPower(uint8_t lowPowerMode);
void NVIC_DisableSystemLowPower(uint8_t lowPowerMode);

/** SysTick */
void SysTick_ConfigClockSource(uint32_t sysTickCLKSource);

/** PMU */
void PMU_EnterWaitMode(void);
void PMU_EnterHaltModeWFI(void);
void PMU_EnterHaltModeWFE(void);

/**@} end of group MISC_Fuctions*/
/**@} end of group MISC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_MISC_H */
