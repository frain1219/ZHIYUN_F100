/*!
 * @file        apm32l0xx_eint.h
 *
 * @brief       This file contains all the functions prototypes for the EINT firmware library
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

#ifndef __APM32L0xx_EINT_H
#define __APM32L0xx_EINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup EINT_Driver EINT Driver
  @{
*/

/** @addtogroup EINT_Enumerations Enumerations
  @{
*/

/**
 * @brief   EINT mode enumeration
 */
typedef enum
{
    EINT_MODE_INTERRUPT,
    EINT_MODE_EVENT    ,
} EINT_MODE_T;

/**
 * @brief   EINT Trigger enumeration
 */
typedef enum
{
    EINT_TRIGGER_RISING ,
    EINT_TRIGGER_FALLING,
    EINT_TRIGGER_ALL    ,
} EINT_TRIGGER_T;

/**
 * @brief   EINT line enumeration
 */
typedef enum
{
    EINT_LINE0   = BIT0 ,  //!< External interrupt line 0
    EINT_LINE1   = BIT1 ,  //!< External interrupt line 1
    EINT_LINE2   = BIT2 ,  //!< External interrupt line 2
    EINT_LINE3   = BIT3 ,  //!< External interrupt line 3
    EINT_LINE4   = BIT4 ,  //!< External interrupt line 4
    EINT_LINE5   = BIT5 ,  //!< External interrupt line 5
    EINT_LINE6   = BIT6 ,  //!< External interrupt line 6
    EINT_LINE7   = BIT7 ,  //!< External interrupt line 7
    EINT_LINE8   = BIT8 ,  //!< External interrupt line 8
    EINT_LINE9   = BIT9 ,  //!< External interrupt line 9
    EINT_LINE10  = BIT10,  //!< External interrupt line 10
    EINT_LINE11  = BIT11,  //!< External interrupt line 11
    EINT_LINE12  = BIT12,  //!< External interrupt line 12
    EINT_LINE13  = BIT13,  //!< External interrupt line 13
    EINT_LINE14  = BIT14,  //!< External interrupt line 14
    EINT_LINE15  = BIT15,  //!< External interrupt line 15
    EINT_LINE16  = BIT16,  //!< External interrupt line 16
    EINT_LINE17  = BIT17,  //!< External interrupt line 17
    EINT_LINE19  = BIT19,  //!< External interrupt line 19
    EINT_LINE20  = BIT20,  //!< External interrupt line 20
    EINT_LINE21  = BIT21,  //!< External interrupt line 21
    EINT_LINE22  = BIT22,  //!< External interrupt line 22
} EINT_LINE_T;

/**@} end of group EINT_Enumerations*/


/** @addtogroup EINT_Macros Macros
  @{
*/

/** No interrupt selected */
#define EINT_LINENONE                  ((uint32_t)0x00000)

/** EINT register reset value */
#define EINT_IMASK_RESET_VALUE         ((uint32_t)(0x3F840000))
#define EINT_EMASK_RESET_VALUE         ((uint32_t)(0x00000000))
#define EINT_RTEN_RESET_VALUE          ((uint32_t)(0x00000000))
#define EINT_FTEN_RESET_VALUE          ((uint32_t)(0x00000000))
#define EINT_SWINTE_RESET_VALUE        ((uint32_t)(0x00000000))
#define EINT_IPEND_RESET_VALUE         ((uint32_t)(0x007BFFFF))

/**@} end of group EINT_Macros*/


/** @addtogroup EINT_Structure Data Structure
  @{
*/

/**
 * @brief   EINT Config struct definition
 */
typedef struct
{
    uint32_t        line;        //!< Specifies the new state of the selected EINT lines.
    EINT_MODE_T     mode;        //!< Specifies the mode for the EINT lines.
    EINT_TRIGGER_T  trigger;     //!< Specifies the trigger signal active edge for the EINT lines.
    uint8_t         lineCmd;     //!< Specifies the EINT lines to be enabled or disabled.
} EINT_Config_T;

/**@} end of group EINT_Structure*/


/** @addtogroup EINT_Fuctions Fuctions
  @{
*/

/** Reset and configuration */
void EINT_Reset(void);
void EINT_Config(EINT_Config_T* eintConfig);
void EINT_ConfigStructInit(EINT_Config_T* eintConfig);

/** interrupt */
void EINT_SelectSWInterrupt(uint32_t line);

/** Flag */
uint8_t EINT_ReadStatusFlag(uint32_t line);
void EINT_ClearStatusFlag(uint32_t line);
uint8_t EINT_ReadIntFlag(uint32_t line);
void EINT_ClearIntFlag(uint32_t line);

/**@} end of group EINT_Fuctions*/
/**@} end of group EINT_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_EINT_H */
