/*!
 * @file        apm32l0xx_syscfg.h
 *
 * @brief       This file contains all the functions prototypes for the SYSCFG firmware
 *              library.
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

#ifndef __APM32L0xx_SYSCFG_H
#define __APM32L0xx_SYSCFG_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library
  @{
*/

/** @addtogroup SYSCFG_Driver
  @{
*/

/** @addtogroup SYSCFG_Enumerations Enumerations
  @{
*/

typedef enum
{
    SYSCFG_PORT_GPIOA,  //!< Port Source GPIOA
    SYSCFG_PORT_GPIOB,  //!< Port Source GPIOB
    SYSCFG_PORT_GPIOC,  //!< Port Source GPIOC
    SYSCFG_PORT_GPIOD,  //!< Port Source GPIOD
    SYSCFG_PORT_GPIOE,  //!< Port Source GPIOE
    SYSCFG_PORT_GPIOH   //!< Port Source GPIOH
} SYSCFG_PORT_T;

/**
 * @brief   SYSCFG EINT Pin Sources
 */
typedef enum
{
    SYSCFG_PIN_0 ,      //!< Pin Source 0
    SYSCFG_PIN_1 ,      //!< Pin Source 1
    SYSCFG_PIN_2 ,      //!< Pin Source 2
    SYSCFG_PIN_3 ,      //!< Pin Source 3
    SYSCFG_PIN_4 ,      //!< Pin Source 4
    SYSCFG_PIN_5 ,      //!< Pin Source 5
    SYSCFG_PIN_6 ,      //!< Pin Source 6
    SYSCFG_PIN_7 ,      //!< Pin Source 7
    SYSCFG_PIN_8 ,      //!< Pin Source 8
    SYSCFG_PIN_9 ,      //!< Pin Source 9
    SYSCFG_PIN_10,      //!< Pin Source 10
    SYSCFG_PIN_11,      //!< Pin Source 11
    SYSCFG_PIN_12,      //!< Pin Source 12
    SYSCFG_PIN_13,      //!< Pin Source 13
    SYSCFG_PIN_14,      //!< Pin Source 14
    SYSCFG_PIN_15       //!< Pin Source 15
} SYSCFG_PIN_T;

/**
 * @brief   SYSCFG Memory Remap Config
 */
typedef enum
{
    SYSCFG_MEMORY_REMAP_FMC     = ((uint8_t)0x00),         //!< SYSCFG MemoryRemap Flash
    SYSCFG_MEMORY_REMAP_SYSTEM  = ((uint8_t)0x01),         //!< SYSCFG MemoryRemap SystemMemory
    SYSCFG_MEMORY_REMAP_SMC     = ((uint8_t)0x02),         //!< SYSCFG MemoryRemap SMC Bank
    SYSCFG_MEMORY_REMAP_SRAM    = ((uint8_t)0x03)          //!< SYSCFG MemoryRemap SRAM
} SYSCFG_MEMORY_REMAP_T;

/**
 * @brief   SYSCFG DMA Remap Config
 */
typedef enum
{
    SYSCFG_GPIO_FMP_PB6       = BIT8 ,   //!< PB6 Fast mode plus
    SYSCFG_GPIO_FMP_PB7       = BIT9 ,   //!< PB7 Fast mode plus
    SYSCFG_GPIO_FMP_PB8       = BIT10,   //!< PB8 Fast mode plus
    SYSCFG_GPIO_FMP_PB9       = BIT11    //!< PB9 Fast mode plus
} SYSCFG_GPIO_FMP_T;

/**
 * @brief   SYSCFG I2C FastModePlus Config
 */
typedef enum
{
    SYSCFG_I2C_FMP_I2C1      = BIT12,     //!< Enable Fast Mode Plus on I2C1 pins
    SYSCFG_I2C_FMP_I2C2      = BIT13,     //!< Enable Fast Mode Plus on I2C2 pins
    SYSCFG_I2C_FMP_I2C3      = BIT14      //!< Enable Fast Mode Plus on I2C3 pins
} SYSCFG_I2C_FMP_T;

/**
 * @brief   SYSCFG Lock Config
 */
typedef enum
{
    SYSCFG_MODULE_ADC     = BIT8 ,       //!< Buffer for ADC
    SYSCFG_MODULE_TS      = BIT9 ,       //!< Buffer for Temperature Sensor
    SYSCFG_MODULE_COMP2   = BIT12,       //!< Buffer for COMP2
    SYSCFG_MODULE_HSI48   = BIT13        //!< Buffer for HSICLK48
} SYSCFG_MODULE_T;

/**
 * @brief   IRDA ENV SOURCE
 */

/**
 * @brief   SYSCFG flag
 */
typedef enum
{
    SYSCFG_CFG2_SRAMPEF     = BIT8,      //!< SRAM Parity error flag
} SYSCFG_FLAG_T;

/**@} end of group SYSCFG_Enumerations*/


/** @addtogroup SYSCFG_Macros Macros
  @{
*/

/** Macros description */
#define SYSCFG_CFG1_MEMMODE      ((uint32_t)0x00000000);   //!< SYSCFG_Memory Remap Config

/**@} end of group SYSCFG_Macros*/


/** @addtogroup SYSCFG_Fuctions Fuctions
  @{
*/
/** Reset */
void SYSCFG_Reset(void);

/** Memory Remap selects */
void SYSCFG_MemoryRemapSelect(uint8_t memory);

/** I2C Fast Mode Plus */
void SYSCFG_EnableI2CFastModePlus(uint32_t pin);
void SYSCFG_DisableI2CFastModePlus(uint32_t pin);

/** Eint Line */
void SYSCFG_EINTLine(SYSCFG_PORT_T port, SYSCFG_PIN_T pin);
void SYSCFG_EnableGPIOFastModePlus(uint32_t pin);
void SYSCFG_DisableGPIOFastModePlus(uint32_t pin);
void SYSCFG_EnableModuleBuffer(uint32_t module);
void SYSCFG_DisableModuleBuffer(uint32_t module);

/**@} end of group SYSCFG_Fuctions*/
/**@} end of group SYSCFG_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_SYSCFG_H */
