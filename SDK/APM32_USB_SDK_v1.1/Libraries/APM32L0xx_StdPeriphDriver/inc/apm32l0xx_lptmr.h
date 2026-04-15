/*!
 * @file        apm32l0xx_lptmr.h
 *
 * @brief       This file contains all the functions prototypes for the LPTMR firmware library.
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
#ifndef __APM32L0xx_LPTMR_H
#define __APM32L0xx_LPTMR_H

#include "apm32l0xx.h"

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LPTMR_Driver  LPTMR Driver
  @{
*/

/** @addtogroup LPTMR_Enumerations Enumerations
  @{
*/

/**
 * @brief    LPTMR Clock Source
 */
typedef enum
{
    LPTMR_CLKSOURSE_APBCLK_LPOSC,
    LPTMR_CLKSOURSE_ULPTIM
} LPTMR_CLKSOURSE_T;

/**
 * @brief    LPTMR Clock Prescaler
 */
typedef enum
{
    LPTMR_PRESCALER_DIV1   = 0x00000000,
    LPTMR_PRESCALER_DIV2   = 0x00000200,
    LPTMR_PRESCALER_DIV4   = 0x00000400,
    LPTMR_PRESCALER_DIV8   = 0x00000600,
    LPTMR_PRESCALER_DIV16  = 0x00000800,
    LPTMR_PRESCALER_DIV32  = 0x00000A00,
    LPTMR_PRESCALER_DIV64  = 0x00000C00,
    LPTMR_PRESCALER_DIV128 = 0x00000E00
} LPTMR_PRESCALER_T;

/**
 * @brief    LPTMR Waveform Shape
 */
typedef enum
{
    LPTMR_WAVEFORM_PWM_OP  = 0,
    LPTMR_WAVEFORM_SETONCE = BIT20
} LPTMR_WAVEFORM_T;

/**
 * @brief    LPTMR Output Polarity
 */
typedef enum
{
    LPTMR_OUTPUTPOLARITY_HIGH = 0,
    LPTMR_OUTPUTPOLARITY_LOW  = BIT21
} LPTMR_OUTPUTPOLARITY_T;

/**
 * @brief    LPTMR Config struct definition
 */
typedef struct
{
    LPTMR_CLKSOURSE_T       clockSource;
    LPTMR_PRESCALER_T       prescaler;
    LPTMR_WAVEFORM_T        waveform;
    LPTMR_OUTPUTPOLARITY_T  outputPolarity;
}LPTMR_Config_T;

/**
 * @brief    LPTMR Clock Polarity
 */
typedef enum
{
    LPTMR_CLKPOLARITY_RISING ,
    LPTMR_CLKPOLARITY_FALLING,
    LPTMR_CLKPOLARITY_BOTH
} LPTMR_CLKPOLARITY_T;

/**
 * @brief    LPTMR External Trigger Source
 */
typedef enum
{
    LPTMR_EXTTRGSOURSE_TRIG0,
    LPTMR_EXTTRGSOURSE_TRIG1,
    LPTMR_EXTTRGSOURSE_TRIG2,
    LPTMR_EXTTRGSOURSE_TRIG3,
    LPTMR_EXTTRGSOURSE_TRIG4,
    LPTMR_EXTTRGSOURSE_TRIG5,
    LPTMR_EXTTRGSOURSE_TRIG6,
    LPTMR_EXTTRGSOURSE_TRIG7
} LPTMR_EXTTRGSOURSE_T;

/**
 * @brief    LPTMR External Trigger Polarity
 */
typedef enum
{
    LPTMR_EXTTRGPOLARITY_SOFTWARE,
    LPTMR_EXTTRGPOLARITY_RISING  ,
    LPTMR_EXTTRGPOLARITY_FALLING ,
    LPTMR_EXTTRGPOLARITY_BOTH
} LPTMR_EXTTRGPOLARITY_T;

/**
 * @brief    LPTMR Clock Sample Time
 */
typedef enum
{
    LPTMR_CLKSAMPLE_DIRECT,
    LPTMR_CLKSAMPLE_2TRANS,
    LPTMR_CLKSAMPLE_4TRANS,
    LPTMR_CLKSAMPLE_8TRANS
} LPTMR_CLKSAMPLE_T;

/**
 * @brief    LPTMR Trigger Sample Time
 */
typedef enum
{
    LPTMR_TRGSAMPLE_DIRECT,
    LPTMR_TRGSAMPLE_2TRANS,
    LPTMR_TRGSAMPLE_4TRANS,
    LPTMR_TRGSAMPLE_8TRANS
} LPTMR_TRGSAMPLE_T;

/**
 * @brief    LPTMR Operating Mode
 */
typedef enum
{
    LPTMR_MODE_CONTINUOUS,
    LPTMR_MODE_SINGLE
} LPTMR_MODE_T;

/**
 * @brief    LPTMR Updating Register
 */
typedef enum
{
    LPTMR_UPDATE_IMMEDIATE  ,
    LPTMR_UPDATE_ENDOFPERIOD
} LPTMR_UPDATE_T;

/**
 * @brief    LPTMR Interrupts Definition
 */
typedef enum
{
    LPTMR_INT_FLAG_DOWN    = BIT6,
    LPTMR_INT_FLAG_UP      = BIT5,
    LPTMR_INT_FLAG_ARROK   = BIT4,
    LPTMR_INT_FLAG_CMPOK   = BIT3,
    LPTMR_INT_FLAG_EXTTRIG = BIT2,
    LPTMR_INT_FLAG_ARRM    = BIT1,
    LPTMR_INT_FLAG_CMPM    = BIT0
} LPTMR_INT_FLAG_T;

/**
 * @brief    LPTMR Flag Definition
 */
typedef enum
{
    LPTMR_FLAG_DOWN    = BIT6,
    LPTMR_FLAG_UP      = BIT5,
    LPTMR_FLAG_ARROK   = BIT4,
    LPTMR_FLAG_CMPOK   = BIT3,
    LPTMR_FLAG_EXTTRIG = BIT2,
    LPTMR_FLAG_ARRM    = BIT1,
    LPTMR_FLAG_CMPM    = BIT0
} LPTMR_FLAG_T;

/**@} end of group LPTMR_Enumerations*/

/** @addtogroup  LPTMR_Fuctions Fuctions
  @{
*/

/** Initialization functions */
void LPTMR_Reset(LPTMR_T* lptmr);
void LPTMR_Config(LPTMR_T* lptmr, LPTMR_Config_T* config);
void LPTMR_ConfigStructInit(LPTMR_Config_T* config);

/** Configuration functions */
void LPTMR_Enable(LPTMR_T* lptmr);
void LPTMR_Disable(LPTMR_T* lptmr);
void LPTMR_ConfigClockSource(LPTMR_T* lptmr, LPTMR_CLKSOURSE_T clockSource);
void LPTMR_ConfigClockPolarity(LPTMR_T* lptmr, LPTMR_CLKPOLARITY_T clockPolarity);
void LPTMR_ConfigPrescaler(LPTMR_T* lptmr, LPTMR_PRESCALER_T prescaler);
void LPTMR_ConfigExternalTrigger(LPTMR_T* lptmr, LPTMR_EXTTRGSOURSE_T extTRGSource, LPTMR_EXTTRGPOLARITY_T extTRGPolarity);
void LPTMR_ConfigSoftwareStart(LPTMR_T* lptmr);
void LPTMR_ConfigTriggerGlitchFilter(LPTMR_T* lptmr, LPTMR_TRGSAMPLE_T trigSampleTime);
void LPTMR_ConfigClockGlitchFilter(LPTMR_T* lptmr, LPTMR_CLKSAMPLE_T clockSampleTime);
void LPTMR_ConfigOperatingMode(LPTMR_T* lptmr, LPTMR_MODE_T mode);
void LPTMR_EnableTimeout(LPTMR_T* lptmr);
void LPTMR_DisableTimeout(LPTMR_T* lptmr);
void LPTMR_ConfigWaveform(LPTMR_T* lptmr, LPTMR_WAVEFORM_T waveform);
void LPTMR_ConfigUpdate(LPTMR_T* lptmr, LPTMR_UPDATE_T update);
void LPTMR_ConfigAutoreloadValue(LPTMR_T* lptmr, uint32_t autoreload);
void LPTMR_ConfigCompareValue(LPTMR_T* lptmr, uint32_t compare);
void LPTMR_EnableCounterMode(LPTMR_T* lptmr);
void LPTMR_DisableCounterMode(LPTMR_T* lptmr);
void LPTMR_EnableEncoderMode(LPTMR_T* lptmr);
void LPTMR_DisableEncoderMode(LPTMR_T* lptmr);
uint32_t LPTMR_ReadCounterValue(LPTMR_T* lptmr);
uint32_t LPTMR_ReadAutoreloadValue(LPTMR_T* lptmr);
uint32_t LPTMR_ReadCompareValue(LPTMR_T* lptmr);

/** Interrupts and flags management functions */
void LPTMR_EnableInterrupt(LPTMR_T* lptmr, uint32_t interrupt);
void LPTMR_DisableInterrupt(LPTMR_T* lptmr, uint32_t interrupt);
uint16_t LPTMR_ReadStatusFlag(LPTMR_T* lptmr, LPTMR_FLAG_T flag);
void LPTMR_ClearStatusFlag(LPTMR_T* lptmr, uint32_t flag);
uint16_t LPTMR_ReadIntFlag(LPTMR_T* lptmr, LPTMR_INT_FLAG_T flag);
void LPTMR_ClearIntFlag(LPTMR_T* lptmr, uint32_t flag);

/**@} end of group LPTMR_Fuctions*/
/**@} end of group LPTMR_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /*__APM32L0xx_LPTMR_H */
