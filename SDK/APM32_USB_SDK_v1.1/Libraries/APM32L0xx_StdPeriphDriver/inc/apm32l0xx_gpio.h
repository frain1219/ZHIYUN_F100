/*!
 * @file        apm32l0xx_gpio.h
 *
 * @brief       This file contains all the functions for the GPIO peripheral
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

#ifndef __APM32L0xx_GPIO_H
#define __APM32L0xx_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup GPIO_Enumerations Enumerations
  @{
*/

/**
 * @brief   Configuration Mode enumeration
 */
typedef enum
{
    GPIO_MODE_IN,         //!< GPIO Input Mode
    GPIO_MODE_OUT,        //!< GPIO Output Mode
    GPIO_MODE_AF,         //!< GPIO Alternate function Mode
    GPIO_MODE_AN          //!< GPIO Analog In/Out Mode
} GPIO_MODE_T;

/**
 * @brief   Output_type_enumeration
 */
typedef enum
{
    GPIO_OUT_TYPE_PP,      //!< General purpose output push-pull
    GPIO_OUT_TYPE_OD       //!< General purpose output Open-drain
} GPIO_OUT_TYPE_T;

/**
 * @brief   GPIO Output Maximum frequency selection
 */
typedef enum
{
    GPIO_SPEED_LOW,
    GPIO_SPEED_MEDIUM,
    GPIO_SPEED_HIGH,
    GPIO_SPEED_VERY_HIGH
} GPIO_SPEED_T;

/**
 * @brief   Configuration_Pull-Up_Pull-Down_enumeration
 */
typedef enum
{
    GPIO_PUPD_NO,
    GPIO_PUPD_PU,          //!< Input with pull-up
    GPIO_PUPD_PD           //!< Input with pull-down
} GPIO_PUPD_T;

/**
 * @brief   Bit_SET and BIT_RESET_enumeration
 */
typedef enum
{
    GPIO_BIT_RESET,
    GPIO_BIT_SET
} GPIO_BSRET_T;

/**
 * @brief    Definition of the GPIO pins
 */
typedef enum
{
    GPIO_PIN_0  = ((uint16_t)BIT0),
    GPIO_PIN_1  = ((uint16_t)BIT1),
    GPIO_PIN_2  = ((uint16_t)BIT2),
    GPIO_PIN_3  = ((uint16_t)BIT3),
    GPIO_PIN_4  = ((uint16_t)BIT4),
    GPIO_PIN_5  = ((uint16_t)BIT5),
    GPIO_PIN_6  = ((uint16_t)BIT6),
    GPIO_PIN_7  = ((uint16_t)BIT7),
    GPIO_PIN_8  = ((uint16_t)BIT8),
    GPIO_PIN_9  = ((uint16_t)BIT9),
    GPIO_PIN_10 = ((uint16_t)BIT10),
    GPIO_PIN_11 = ((uint16_t)BIT11),
    GPIO_PIN_12 = ((uint16_t)BIT12),
    GPIO_PIN_13 = ((uint16_t)BIT13),
    GPIO_PIN_14 = ((uint16_t)BIT14),
    GPIO_PIN_15 = ((uint16_t)BIT15),
    GPIO_PIN_ALL = ((uint32_t)0XFFFF),
} GPIO_PIN_T;

/**
 * @brief   gpio pin source define
 */
typedef enum
{
    GPIO_PIN_SOURCE_0 ,
    GPIO_PIN_SOURCE_1 ,
    GPIO_PIN_SOURCE_2 ,
    GPIO_PIN_SOURCE_3 ,
    GPIO_PIN_SOURCE_4 ,
    GPIO_PIN_SOURCE_5 ,
    GPIO_PIN_SOURCE_6 ,
    GPIO_PIN_SOURCE_7 ,
    GPIO_PIN_SOURCE_8 ,
    GPIO_PIN_SOURCE_9 ,
    GPIO_PIN_SOURCE_10,
    GPIO_PIN_SOURCE_11,
    GPIO_PIN_SOURCE_12,
    GPIO_PIN_SOURCE_13,
    GPIO_PIN_SOURCE_14,
    GPIO_PIN_SOURCE_15,
} GPIO_PIN_SOURCE_T;

/**
 * @brief   gpio alternate_function define
 */
typedef enum
{
    GPIO_AF_PIN0,
    GPIO_AF_PIN1,
    GPIO_AF_PIN2,
    GPIO_AF_PIN3,
    GPIO_AF_PIN4,
    GPIO_AF_PIN5,
    GPIO_AF_PIN6,
    GPIO_AF_PIN7,
} GPIO_AF_T;

/**@} end of group GPIO_Enumerations*/

/** @addtogroup GPIO_Structure Data Structure
  @{
*/

/**
 * @brief   GPIO Config structure definition
 */
typedef struct
{
    uint16_t         pin;
    GPIO_MODE_T      mode;
    GPIO_OUT_TYPE_T  outtype;
    GPIO_SPEED_T     speed;
    GPIO_PUPD_T      pupd;
} GPIO_Config_T;

/**@} end of group GPIO_Structure*/

/** @addtogroup GPIO_Fuctions Fuctions
  @{
*/

/** Reset and Configuration */
void GPIO_Reset(GPIO_T* port);
void GPIO_Config(GPIO_T* port, GPIO_Config_T* gpioConfig);
void GPIO_ConfigStructInit(GPIO_Config_T* gpioConfig);

/** Lock */
void GPIO_ConfigPinLock(GPIO_T* port, GPIO_PIN_T pin);

/** Read */
uint8_t GPIO_ReadInputBit(GPIO_T* port, uint16_t pin);
uint16_t GPIO_ReadInputPort(GPIO_T* port);
uint8_t GPIO_ReadOutputBit(GPIO_T* port, uint16_t pin);
uint16_t GPIO_ReadOutputPort(GPIO_T* port);

/** Write */
void GPIO_SetBit(GPIO_T* port, uint16_t pin);
void GPIO_ClearBit(GPIO_T* port, uint16_t pin);
void GPIO_WriteBitValue(GPIO_T* port, GPIO_PIN_T pin, GPIO_BSRET_T bitVal);
void GPIO_WriteOutputPort(GPIO_T* port, uint16_t portValue);

/** Alternate function */
void GPIO_ConfigPinAF(GPIO_T* port, GPIO_PIN_SOURCE_T pinSource, GPIO_AF_T afPin);

/** Toggle function */
void GPIO_TogglePin(GPIO_T* port, GPIO_PIN_T pin);

/**@} end of group GPIO_Fuctions*/
/**@} end of group GPIO_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_GPIO_H */
