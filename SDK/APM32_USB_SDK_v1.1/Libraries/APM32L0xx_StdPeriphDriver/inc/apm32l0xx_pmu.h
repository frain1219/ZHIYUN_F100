/*!
 * @file        apm32l0xx_pmu.h
 *
 * @brief       This file contains all functions prototype and macros for the PMU peripheral
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

#ifndef __APM32L0XX_PMU_H
#define __APM32L0XX_PMU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup PMU_Driver PMU Driver
  @{
*/

/** @addtogroup PMU_Enumerations Enumerations
  @{
*/

/**
  * @brief  PMU bit Status
  */
typedef enum
{
    PMU_OK,
    PMU_ERROR,
    PMU_BUSY,
    PMU_TIME_OUT
} PMU_BITSTATUS_T;

/**
 * @brief   Wake Up PIN
 */
typedef enum
{
    PMU_WAKEUPPIN_1 = ((uint16_t)BIT8 ),
    PMU_WAKEUPPIN_2 = ((uint16_t)BIT9 ),
    PMU_WAKEUPPIN_3 = ((uint16_t)BIT10)
} PMU_WAKEUPPIN_T;

/**
 * @brief   PVD detection level
 */
typedef enum
{
    PMU_PVDLEVEL_0,   //!< 1.9V
    PMU_PVDLEVEL_1,   //!< 2.1V
    PMU_PVDLEVEL_2,   //!< 2.3V
    PMU_PVDLEVEL_3,   //!< 2.5V
    PMU_PVDLEVEL_4,   //!< 2.7V
    PMU_PVDLEVEL_5,   //!< 2.9V
    PMU_PVDLEVEL_6,   //!< 3.1V
    PMU_PVDLEVEL_7    //!< External input analog voltage (Compare internally to V REFINT )
} PMU_PVDLEVEL_T;

/**
 * @brief   Regulator mode state
 */
typedef enum
{
    PMU_REGULATOR_MAIN,
    PMU_REGULATOR_LOWPOWER
} PMU_REGULATOR_T;

/**
 * @brief   Sleep mode entry
 */
typedef enum
{
    PMU_SLEEP_ENTRY_WFI,
    PMU_SLEEP_ENTRY_WFE
} PMU_SLEEP_ENTRY_T;

/**
 * @brief PMU STOP mode entry
 */
typedef enum
{
    PMU_STOP_ENTRY_WFI,
    PMU_STOP_ENTRY_WFE
} PMU_STOP_ENTRY_T;

/**
 * @brief PMU voltage scaling range
 */
typedef enum
{
    PMU_VOLTAGE_RANGE1 = BIT0,
    PMU_VOLTAGE_RANGE2 = BIT1,
    PMU_VOLTAGE_RANGE3 = BIT0|BIT1
} PMU_VOLTAGE_RANGE_T;

/**
 * @brief   PMU Flag
 */
typedef enum
{
    PMU_FLAG_WUEFLG      = BIT0,
    PMU_FLAG_SBFLG       = BIT1,
    PMU_FLAG_PVDOFLG     = BIT2,
    PMU_FLAG_VREFINTRDY  = BIT3,
    PMU_FLAG_VSFLG       = BIT4,
    PMU_FLAG_REGLPFLG    = BIT5
} PMU_FLAG_T;

/**@} end of group PMU_Enumerations*/

/** @addtogroup PMU_Fuctions Fuctions
  @{
*/

/** Function used to set the PMU configuration to the default reset state */
void PMU_Reset(void);

/** Backup Domain Access function */
void PMU_EnableBackupAccess(void);
void PMU_DisableBackupAccess(void);

/** PVD configuration functions */
void PMU_ConfigPVDLevel(PMU_PVDLEVEL_T level);
void PMU_ConfigVoltageRange(PMU_VOLTAGE_RANGE_T range);
void PMU_EnablePVD(void);
void PMU_DisablePVD(void);

/** WakeUp pins configuration functions */
void PMU_EnableWakeUpPin(PMU_WAKEUPPIN_T pin);
void PMU_DisableWakeUpPin(PMU_WAKEUPPIN_T pin);

/** Low Power modes configuration functions */
void PMU_EnterSleepMode(PMU_REGULATOR_T regulator, PMU_SLEEP_ENTRY_T entry);
void PMU_EnterStopMode(PMU_REGULATOR_T regulator, PMU_STOP_ENTRY_T entry);
void PMU_EnterStandbyMode(void);
void PMU_EnableFastWakeUp(void);
void PMU_DisableFastWakeUp(void);
void PMU_EnableUltraLowPower(void);
void PMU_DisableUltraLowPower(void);
void PMU_EnterLowPowerRunMode(void);
PMU_BITSTATUS_T PMU_DisableLowPowerRunMode(void);

/** Sleep on exit functions */
void PMU_EnableSleepOnExit(void);
void PMU_DisableSleepOnExit(void);

/** CORTEX M0+ SEVONPEND bit functions */
void PMU_EnableSEVOnPend(void);
void PMU_DisableSEVOnPend(void);

/** Read Flags status functions  */
PMU_VOLTAGE_RANGE_T PMU_ReadVoltageRange(void);
uint8_t PMU_ReadStatusFlag(PMU_FLAG_T flag);
void PMU_ClearStatusFlag(uint8_t flag);

/**@} end of group PMU_Fuctions*/
/**@} end of group PMU_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0XX_PMU_H */
