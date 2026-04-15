/*!
 * @file        apm32l0xx_fwl.h
 *
 * @brief       This file contains all the functions prototypes for the FWL firmware library
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

#ifndef __APM32L0xx_FWL_H
#define __APM32L0xx_FWL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup FWL_Driver FWL Driver
  @{
*/

/** @addtogroup FWL_Enumerations Enumerations
  @{
*/

/**
 * @brief    FWL configuration register flag definition
 */
typedef enum
{
    FWL_FLAG_CODESEL = ((uint8_t)0x01),
    FWL_FLAG_VDSSHR  = ((uint8_t)0x02),
    FWL_FLAG_VDSE    = ((uint8_t)0x04)
} FWL_FLAG_T;

/**@} end of group FWL_Enumerations*/

/** @addtogroup FWL_Structure Data Structure
  @{
*/

/**
 * @brief    FWL Config struct definition
 */
typedef struct
{
    uint32_t      codeStartAddress;
    uint32_t      codeLength;
    uint32_t      nonVDataStartAddress;
    uint32_t      nonVDataLength;
    uint32_t      vDataStartAddress;
    uint32_t      vDataLength;
} FWL_Config_T;

/**@} end of group FWL_Structure*/

/** @addtogroup FWL_Fuctions Fuctions
  @{
*/

/** FWL configuration */
void FWL_Config(FWL_Config_T* fwlConfig);
void FWL_ConfigStructInit(FWL_Config_T* fwlConfig);

void FWL_Enable(void);
void FWL_EnablePreArm(void);
void FWL_DisablePreArm(void);
void FWL_EnableVolatileDataShare(void);
void FWL_DisableVolatileDataShare(void);
void FWL_EnableVolatileDataExecute(void);
void FWL_DisableVolatileDataExecute(void);

uint8_t FWL_ReadPeripheralFlag(void);
void FWL_ClearPeripheralFlag(void);
uint8_t FWL_ReadStatusFlag(FWL_FLAG_T flag);
void FWL_ClearStatusFlag(FWL_FLAG_T flag);

/**@} end of group FWL_Fuctions*/
/**@} end of group FWL_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_FWL_H */
