/*!
 * @file        apm32l0xx_crs.h
 *
 * @brief       This file contains all the functions prototypes for the CRS firmware library
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

#ifndef __APM32L0xx_CRS_H
#define __APM32L0xx_CRS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup CRS_Driver CRS Driver
  @{
*/

/** @addtogroup CRS_Enumerations Enumerations
  @{
*/

/**
 * @brief    CRS_Interrupt_Sources
 */
typedef enum
{
    CRS_INT_SNFLG    = BIT0,  //!<SYNC event OK interrupt
    CRS_INT_SWFLG    = BIT1,  //!<SYNC warning interrupt
    CRS_INT_EFLG     = BIT2,  //!<Synchronization or trimming error interrupt
    CRS_INT_ESFLG    = BIT3,  //!<Expected SYNC interrupt
    CRS_INT_ERRORFLG = BIT8,  //!<SYNC error
    CRS_INT_SYNCMISS = BIT9,  //!<SYNC missed
    CRS_INT_TRMFLG   = BIT10, //!<Trimming overflow or underflow
}CRS_INT_T;

/**
 * @brief    CRS_Flags
 */
typedef enum
{
    CRS_FLAG_SNFLG    =  BIT0,  //!<SYNC event OK flag
    CRS_FLAG_SWFLG    =  BIT1,  //!<SYNC warning flag
    CRS_FLAG_EFLG     =  BIT2,  //!<Synchronization or trimming error flag
    CRS_FLAG_ESFLG    =  BIT3,  //!<Expected SYNC flag
    CRS_FLAG_ERRORFLG =  BIT8,  //!<SYNC error flag
    CRS_FLAG_SYNCMISS =  BIT9,  //!<SYNC missed flag
    CRS_FLAG_TRMFLG   =  BIT10, //!<Trimming overflow or underflow falg
}CRS_FLAG_T;

/**
 * @brief    CRS_Synchro_Source
 */
typedef enum
{
    CRS_SYNC_SOURCE_GPIO,   //!<GPIO selected as SYNC signal source
    CRS_SYNC_SOURCE_LSE,    //!<LSE selected as SYNC signal source
    CRS_SYNC_SOURCE_USB,    //!<USB SNFLG selected as SYNC signal source (default).
}CRS_SYNC_SOURCE_T;

/**
 * @brief    CRS_Synchro_Source
 */
typedef enum
{
    CRS_SYNC_DIV1,   //!<Synchro Signal not divided (default)
    CRS_SYNC_DIV2,   //!<Synchro Signal divided by 2
    CRS_SYNC_DIV4,   //!<Synchro Signal divided by 4
    CRS_SYNC_DIV8,   //!<Synchro Signal divided by 8
    CRS_SYNC_DIV16,  //!<Synchro Signal divided by 16
    CRS_SYNC_DIV32,  //!<Synchro Signal divided by 32
    CRS_SYNC_DIV64,  //!<Synchro Signal divided by 64
    CRS_SYNC_DIV128, //!<Synchro Signal divided by 128
} CRS_SYNC_DIV_T;

/**
 * @brief    CRS_SynchroPolarity
 */
typedef enum
{
    CRS_SYNC_POL_RISING, //!<Synchro active on rising edge
    CRS_SYNC_POL_FALLING //!<Synchro active on falling edge
}CRS_SYNC_POL_T;
/**@} end of group CRS_Enumerations*/

/** @addtogroup CRS_Structure Data Structure
  @{
*/

/**@} end of group CRS_Structure*/


/** @addtogroup CRS_Fuctions Fuctions
  @{
*/

/** Configuration of the CRS **/
void CRS_Reset(void);
void CRS_AdjustHSI48CalibrationValue(uint8_t calibrationVal);
void CRS_EnableFrequencyErrorCounter(void);
void CRS_DisableFrequencyErrorCounter(void);
void CRS_EnableAutomaticCalibration(void);
void CRS_DisableAutomaticCalibration(void);
void CRS_GenerateSoftwareSynchronization(void);
void CRS_FrequencyErrorCounterReloadValue(uint16_t reloadVal);
void CRS_ConfigFrequencyErrorLimit(uint8_t errLimitVal);
void CRS_ConfigSynchronizationPrescaler(CRS_SYNC_DIV_T div);
void CRS_ConfigSynchronizationSource(CRS_SYNC_SOURCE_T source);
void CRS_ConfigSynchronizationPolarity(CRS_SYNC_POL_T polarity);
uint32_t CRS_ReadReloadValue(void);
uint32_t CRS_ReadHSI48CalibrationValue(void);
uint32_t CRS_ReadFrequencyErrorValue(void);
uint32_t CRS_ReadFrequencyErrorDirection(void);

/** Interrupts and flags management functions **/
void CRS_EnableInterrupt(CRS_INT_T interrupt);
void CRS_DisableInterrupt(CRS_INT_T interrupt);
uint8_t CRS_ReadStatusFlag(CRS_FLAG_T flag);
void CRS_ClearStatusFlag(CRS_FLAG_T flag);
uint8_t CRS_ReadIntFlag(CRS_INT_T flag);
void CRS_ClearIntFlag(CRS_INT_T flag);

/**@} end of group CRS_Fuctions*/
/**@} end of group CRS_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_CRS_H */
