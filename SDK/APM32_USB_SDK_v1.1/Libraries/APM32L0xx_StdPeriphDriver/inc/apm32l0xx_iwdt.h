/*!
 * @file        apm32l0xx_iwdt.h
 *
 * @brief       This file contains all the functions prototypes for the IWDT firmware library
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

#ifndef __APM32L0XX_IWDT_H
#define __APM32L0XX_IWDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup IWDT_Driver IWDT Driver
  @{
*/

/** @addtogroup IWDT_Enumerations Enumerations
  @{
*/

/**
 * @brief   IWDT key definition
 */
typedef enum
{
    IWDT_KEY_REFRESH = ((uint16_t)0xAAAA),
    IWDT_KEY_ENABLE  = ((uint16_t)0xCCCC),
    IWDT_KEY_ACCESS  = ((uint16_t)0x5555),
} IWDT_KEY_T;

/**
 * @brief   IWDT divider
 */
typedef enum
{
    IWDT_DIV_4  ,
    IWDT_DIV_8  ,
    IWDT_DIV_16 ,
    IWDT_DIV_32 ,
    IWDT_DIV_64 ,
    IWDT_DIV_128,
    IWDT_DIV_256,
} IWDT_DIV_T;

/**
 * @brief   IWDT flag definition
 */
typedef enum
{
    IWDT_FLAG_PSCUFLG  = BIT0,
    IWDT_FLAG_CNTUFLG  = BIT1,
    IWDT_FLAG_CNTWUFLG = BIT2,
} IWDT_FLAG_T;

/**@} end of group IWDT_Enumerations*/


/** @addtogroup IWDT_Fuctions Fuctions
  @{
*/

/** Enable IWDT */
void IWDT_Enable(void);

/** Refresh IWDT */
void IWDT_Refresh(void);

/** Window Value */
void IWDT_ConfigWindowValue(uint16_t windowValue);

/** Set Counter reload */
void IWDT_ConfigReload(uint16_t reload);

/** Write Access */
void IWDT_EnableWriteAccess(void);
void IWDT_DisableWriteAccess(void);

/** divider */
void IWDT_ConfigDivider(IWDT_DIV_T div);

/** flag */
uint8_t IWDT_ReadStatusFlag(IWDT_FLAG_T flag);

/**@} end of group IWDT_Fuctions*/
/**@} end of group IWDT_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0XX_IWDT_H */
