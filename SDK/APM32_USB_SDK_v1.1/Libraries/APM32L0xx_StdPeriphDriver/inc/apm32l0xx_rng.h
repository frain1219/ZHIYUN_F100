/*!
 * @file        apm32l0xx_rng.h
 *
 * @brief       This file contains all the functions prototypes for the Random Number Generator(RNG) firmware library.
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

#ifndef __APM32L0XX_RNG_H
#define __APM32L0XX_RNG_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup RNG_Driver RNG Driver
  @{
*/

/** @addtogroup RNG_Enumerations Enumerations
  @{
*/

/**
 * @brief RNG flags definition
 */
typedef enum
{
    RNG_FLAG_DATARDY    = (uint8_t)BIT0, //!< Data ready flag
    RNG_FLAG_CLKERCSTS  = (uint8_t)BIT1, //!< RNG clock error flag
    RNG_FLAG_FSCSTS     = (uint8_t)BIT2, //!< Faulty sequence flag
}RNG_FLAG_T;

/**
 * @brief RNG interrupts definition
 */
 typedef enum
{
    RNG_INT_FLAG_CLKERINT  = (uint8_t)BIT5, //!< RNG clock Error interrupt
    RNG_INT_FLAG_FSINT     = (uint8_t)BIT6, //!< Faulty Sequence Interrupt
}RNG_INT_FLAG_T;

/**@} end of group RNG_Enumerations*/

/** @addtogroup RNG_Fuctions Fuctions
  @{
*/

/** RNG Reset and Configuration */
void RNG_Reset(void);
void RNG_Enable(void);
void RNG_Disable(void);

/** Get 32 bit Random number */
uint32_t RNG_ReadRandomNumber(void);

/** Interrupts and flags */
void RNG_EnableInterrupt(void);
void RNG_DisableInterrupt(void);
uint8_t RNG_ReadStatusFlag(RNG_FLAG_T flag);
void RNG_ClearStatusFlag(uint8_t flag);
uint8_t RNG_ReadIntFlag(RNG_INT_FLAG_T flag);
void RNG_ClearIntFlag(uint8_t flag);

/** Clock Error Detection */
void RNG_EnableClockErrorDetection(void);
void RNG_DisableClockErrorDetection(void);

/**@} end of group RNG_Fuctions*/
/**@} end of group RNG_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /*__APM32L0XX_RNG_H */
