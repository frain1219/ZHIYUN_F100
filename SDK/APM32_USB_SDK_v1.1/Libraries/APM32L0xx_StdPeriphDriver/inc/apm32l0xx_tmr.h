/*!
 * @file        apm32l0xx_tmr.h
 *
 * @brief       This file contains all the functions prototypes for the TMR firmware library.
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

#ifndef __APM32L0xx_TMR_H
#define __APM32L0xx_TMR_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup TMR_Driver  TMR Driver
  @{
*/

/** @addtogroup TMR_Enumerations Enumerations
  @{
*/
     /**
 * @brief    TMR Output Compare and PWM modes
 */
typedef enum
{
    TMR_OC_MODE_TMRING   ,
    TMR_OC_MODE_ACTIVE   ,
    TMR_OC_MODE_INACTIVE ,
    TMR_OC_MODE_TOGGEL   ,
    TMR_OC_MODE_LOWLEVEL ,
    TMR_OC_MODE_HIGHLEVEL,
    TMR_OC_MODE_PWM1     ,
    TMR_OC_MODE_PWM2
} TMR_OC_MODE_T;

/**
 * @brief    TMR Single Pulse Mode
 */
typedef enum
{
    TMR_SPM_REPETITIVE,
    TMR_SPM_SINGLE
} TMR_SPM_T;

/**
 * @brief    TMR Input Capture Init structure definition
 */
typedef enum
{
    TMR_CHANNEL_1 = 0x0000,
    TMR_CHANNEL_2 = 0x0004,
    TMR_CHANNEL_3 = 0x0008,
    TMR_CHANNEL_4 = 0x000C
} TMR_CHANNEL_T;

/**
 * @brief    TMR Clock division
 */

typedef enum
{
    TMR_CLOCK_DIV_1 = 0x0000,
    TMR_CLOCK_DIV_2 = 0x0100,
    TMR_CLOCK_DIV_4 = 0x0200,
} TMR_CLOCK_DIV_T;

/**
 * @brief    TMR Counter Mode
 */
typedef enum
{
    TMR_COUNTER_MODE_UP             = 0x0000,
    TMR_COUNTER_MODE_DOWN           = 0x0010,
    TMR_COUNTER_MODE_CENTERALIGNED1 = 0x0020,
    TMR_COUNTER_MODE_CENTERALIGNED2 = 0x0040,
    TMR_COUNTER_MODE_CENTERALIGNED3 = 0x0060
} TMR_COUNTER_MODE_T;

/**
 * @brief    TMR Output Compare Polarity
 */
typedef enum
{
    TMR_OC_POLARITY_HIGH,
    TMR_OC_POLARITY_LOW
} TMR_OC_POLARITY_T;

/**
 * @brief    TMR Output Compare N Polarity
 */
typedef enum
{
    TMR_OC_NPOLARITY_HIGH,
    TMR_OC_NPOLARITY_LOW
} TMR_OC_NPOLARITY_T;

/**
 * @brief    TMR Output Compare state
 */
typedef enum
{
    TMR_OC_STATE_DISABLE,
    TMR_OC_STATE_ENABLE
} TMR_OC_STATE_T;

/**
 * @brief    TMR Input Capture Polarity
 */
typedef enum
{
    TMR_IC_POLARITY_RISING   = 0x00,
    TMR_IC_POLARITY_FALLING  = 0x02,
    TMR_IC_POLARITY_BOTHEDGE = 0x0A
} TMR_IC_POLARITY_T;

/**
 * @brief    TMR Input Capture Selection
 */
typedef enum
{
    TMR_IC_SELECTION_DIRECT_TI  ,
    TMR_IC_SELECTION_INDIRECT_TI,
    TMR_IC_SELECTION_TRC
} TMR_IC_SELECTION_T;

/**
 * @brief    TMR Input Capture Prescaler
 */
typedef enum
{
    TMR_IC_PSC_1,
    TMR_IC_PSC_2,
    TMR_IC_PSC_4,
    TMR_IC_PSC_8
} TMR_IC_PSC_T;

/**
 * @brief    TMR_interrupt_sources
 */
typedef enum
{
    TMR_INT_UPDATE = 0x0001,
    TMR_INT_CH1    = 0x0002,
    TMR_INT_CH2    = 0x0004,
    TMR_INT_CH3    = 0x0008,
    TMR_INT_CH4    = 0x0010,
    TMR_INT_TRG    = 0x0040
} TMR_INT_T;

/**
 * @brief    TMR DMA Base Address
 */
typedef enum
{
    TMR_DMA_BASE_CTRL1   = 0x0000,
    TMR_DMA_BASE_CTRL2   = 0x0001,
    TMR_DMA_BASE_SMCTRL  = 0x0002,
    TMR_DMA_BASE_DIEN    = 0x0003,
    TMR_DMA_BASE_STS     = 0x0004,
    TMR_DMA_BASE_CEG     = 0x0005,
    TMR_DMA_BASE_CCM1    = 0x0006,
    TMR_DMA_BASE_CCM2    = 0x0007,
    TMR_DMA_BASE_CCEN    = 0x0008,
    TMR_DMA_BASE_CNT     = 0x0009,
    TMR_DMA_BASE_PSC     = 0x000A,
    TMR_DMA_BASE_AUTORLD = 0x000B,
    TMR_DMA_BASE_CC1     = 0x000D,
    TMR_DMA_BASE_CC2     = 0x000E,
    TMR_DMA_BASE_CC3     = 0x000F,
    TMR_DMA_BASE_CC4     = 0x0010,
    TMR_DMA_BASE_DCTRL   = 0x0012,
    TMR_DMA_BASE_DMADDR  = 0x0013,
    TMR_DMA_BASE_OPT     = 0x0014
} TMR_DMA_BASE_T;

/**
 * @brief    TMR DMA Soueces
 */
typedef enum
{
    TMR_DMA_SOURCE_UPDATE    = 0x0100,
    TMR_DMA_SOURCE_CH1       = 0x0200,
    TMR_DMA_SOURCE_CH2       = 0x0400,
    TMR_DMA_SOURCE_CH3       = 0x0800,
    TMR_DMA_SOURCE_CH4       = 0x1000,
    TMR_DMA_SOURCE_TRG       = 0x4000
} TMR_DMA_SOURCE_T;

/**
 * @brief    TMR  The external Trigger Prescaler.
 */
typedef enum
{
    TMR_EXTTRG_PSC_OFF ,
    TMR_EXTTRG_PSC_DIV2,
    TMR_EXTTRG_PSC_DIV4,
    TMR_EXTTRG_PSC_DIV8
} TMR_EXTTRG_PSC_T;

/**
 * @brief    TMR Internal Trigger Selection
 */
typedef enum
{
    TMR_TRIGGER_SOURCE_ITR0     = 0x00,
    TMR_TRIGGER_SOURCE_ITR1     = 0x01,
    TMR_TRIGGER_SOURCE_ITR2     = 0x02,
    TMR_TRIGGER_SOURCE_TI1F_ED  = 0x04,
    TMR_TRIGGER_SOURCE_TI1FP1   = 0x05,
    TMR_TRIGGER_SOURCE_TI2FP2   = 0x06,
    TMR_TRIGGER_SOURCE_ETRF     = 0x07
} TMR_TRIGGER_SOURCE_T;

/**
 * @brief    TMR External Trigger Polarity
 */
typedef enum
{
    TMR_EXTTGR_POL_NONINVERTED,
    TMR_EXTTRG_POL_INVERTED
} TMR_EXTTRG_POL_T;

/**
 * @brief    TMR Prescaler Reload Mode
 */
typedef enum
{
    TMR_PRESCALER_RELOAD_UPDATE,
    TMR_PRESCALER_RELOAD_IMMEDIATE
} TMR_PRESCALER_RELOAD_T;

/**
 * @brief    TMR Forced Action
 */
typedef enum
{
    TMR_FORCED_ACTION_INACTIVE = 0x04,
    TMR_FORCED_ACTION_ACTIVE   = 0x05
} TMR_FORCED_ACTION_T;

/**
 * @brief    TMR Encoder Mode
 */
typedef enum
{
    TMR_ENCODER_MODE_TI1 = 1,
    TMR_ENCODER_MODE_TI2 ,
    TMR_ENCODER_MODE_TI12
} TMR_ENCODER_MODE_T;

/**
 * @brief    TMR event sources
 */
typedef enum
{
    TMR_EVENT_UPDATE = 0x0001,
    TMR_EVENT_CH1    = 0x0002,
    TMR_EVENT_CH2    = 0x0004,
    TMR_EVENT_CH3    = 0x0008,
    TMR_EVENT_CH4    = 0x0010,
    TMR_EVENT_TRG    = 0x0040
} TMR_EVENT_T;

/**
 * @brief    TMR UpdateSource
 */
typedef enum
{
    TMR_UPDATE_SOURCE_GLOBAL,
    TMR_UPDATE_SOURCE_REGULAR
} TMR_UPDATE_SOURCE_T;

/**
 * @brief    TMR Output Compare Preload State
 */
typedef enum
{
    TMR_OC_PRELOAD_DISABLE,
    TMR_OC_PRELOAD_ENABLE
} TMR_OC_PRELOAD_T;

/**
 * @brief    TMR Output Compare Preload State
 */
typedef enum
{
    TMR_OC_FAST_DISABLE,
    TMR_OC_FAST_ENABLE
} TMR_OC_FAST_T;

/**
 * @brief    TMR Output Compare Preload State
 */
typedef enum
{
    TMR_OC_CLEAR_DISABLE,
    TMR_OC_CLEAR_ENABLE
} TMR_OC_CLEAR_T;

/**
 * @brief    TMR Trigger Output Source
 */
typedef enum
{
    TMR_TRGO_SOURCE_RESET ,
    TMR_TRGO_SOURCE_ENABLE,
    TMR_TRGO_SOURCE_UPDATE,
    TMR_TRGO_SOURCE_OC1   ,
    TMR_TRGO_SOURCE_OC1REF,
    TMR_TRGO_SOURCE_OC2REF,
    TMR_TRGO_SOURCE_OC3REF,
    TMR_TRGO_SOURCE_OC4REF
} TMR_TRGO_SOURCE_T;

/**
 * @brief    TMR Slave Mode
 */
typedef enum
{
    TMR_SLAVE_MODE_DISABLE  ,
    TMR_SLAVE_MODE_ECODER1  ,
    TMR_SLAVE_MODE_ECODER2  ,
    TMR_SLAVE_MODE_ECODER3  ,
    TMR_SLAVE_MODE_RESET    ,
    TMR_SLAVE_MODE_GATED    ,
    TMR_SLAVE_MODE_TRIGGER  ,
    TMR_SLAVE_MODE_EXTERNAL1
} TMR_SLAVE_MODE_T;

/**
 * @brief    TMR Remap
 */
typedef enum
{
    TMR2_ETR_GPIO               = 0x0000,
    TMR2_ETR_HSI16              = 0x0003,
    TMR2_ETR_HSI48              = 0x0004,
    TMR2_ETR_LSE                = 0x0005,
    TMR2_ETR_COMP2_OUT          = 0x0006,
    TMR2_ETR_COMP1_OUT          = 0x0007,
    TMR2_TI4_GPIO               = 0x0000,
    TMR2_TI4_COMP2_OUT          = 0x0008,
    TMR2_TI4_COMP1_OUT          = 0x0010,

    TMR3_ETR_PE2_PD2            = 0x0000,
    TMR3_ETR_HSI48DIV6          = 0x0002,
    TMR3_TI1_USB_SOF            = 0x0000,
    TMR3_TI1_PE3_PA6_PC6_PB4    = 0x0004,
    TMR3_TI2_PB5_TMR22_CH2      = 0x0000,
    TMR3_TI2_PB5_TMR3_CH2       = 0x0008,
    TMR3_TI4_PC9_USB_NOE        = 0x0000,
    TMR3_TI4_PC9_TMR3_CH4       = 0x0010,

    TMR21_ETR_GPIO              = 0x0000,
    TMR21_ETR_COMP2_OUT         = 0x0001,
    TMR21_ETR_COMP1_OUT         = 0x0002,
    TMR21_ETR_LSECLK            = 0x0003,
    TMR21_TI1_GPIO              = 0x0000,
    TMR21_TI1_RTC_WKPINT        = 0x0004,
    TMR21_TI1_HSE_RTC           = 0x0008,
    TMR21_TI1_MSICLK            = 0x000C,
    TMR21_TI1_LSECLK            = 0x0010,
    TMR21_TI1_LSICLK            = 0x0014,
    TMR21_TI1_COMP1_OUT         = 0x0018,
    TMR21_TI1_MCOCLK            = 0x001C,
    TMR21_TI2_GPIO              = 0x0000,
    TMR21_TI2_COMP2_OUT         = 0x0020,

    TMR22_ETR_GPIO              = 0x0000,
    TMR22_ETR_COMP2_OUT         = 0x0001,
    TMR22_ETR_COMP1_OUT         = 0x0002,
    TMR22_ETR_LSECLK            = 0x0003,
    TMR22_TI1_GPIO              = 0x0000,
    TMR22_TI1_COMP2_OUT         = 0x0004,
    TMR22_TI1_COMP1_OUT         = 0x0008
} TMR_REMAP_T;

/**
 * @brief    TMR Flag
 */
typedef enum
{
    TMR_FLAG_UPDATE  = BIT0,
    TMR_FLAG_CC1     = BIT1,
    TMR_FLAG_CC2     = BIT2,
    TMR_FLAG_CC3     = BIT3,
    TMR_FLAG_CC4     = BIT4,
    TMR_FLAG_TRG     = BIT6,
    TMR_FLAG_CC1RC   = BIT9,
    TMR_FLAG_CC2RC   = BIT10,
    TMR_FLAG_CC3RC   = BIT11,
    TMR_FLAG_CC4RC   = BIT12
} TMR_FLAG_T;

/**
 * @brief    TMR DMA Burst Length
 */
typedef enum
{
    TMR_DMA_BURSTLENGTH_1TRANSFER   = 0x0000,
    TMR_DMA_BURSTLENGTH_2TRANSFERS  = 0x0100,
    TMR_DMA_BURSTLENGTH_3TRANSFERS  = 0x0200,
    TMR_DMA_BURSTLENGTH_4TRANSFERS  = 0x0300,
    TMR_DMA_BURSTLENGTH_5TRANSFERS  = 0x0400,
    TMR_DMA_BURSTLENGTH_6TRANSFERS  = 0x0500,
    TMR_DMA_BURSTLENGTH_7TRANSFERS  = 0x0600,
    TMR_DMA_BURSTLENGTH_8TRANSFERS  = 0x0700,
    TMR_DMA_BURSTLENGTH_9TRANSFERS  = 0x0800,
    TMR_DMA_BURSTLENGTH_10TRANSFERS = 0x0900,
    TMR_DMA_BURSTLENGTH_11TRANSFERS = 0x0A00,
    TMR_DMA_BURSTLENGTH_12TRANSFERS = 0x0B00,
    TMR_DMA_BURSTLENGTH_13TRANSFERS = 0x0C00,
    TMR_DMA_BURSTLENGTH_14TRANSFERS = 0x0D00,
    TMR_DMA_BURSTLENGTH_15TRANSFERS = 0x0E00,
    TMR_DMA_BURSTLENGTH_16TRANSFERS = 0x0F00,
    TMR_DMA_BURSTLENGTH_17TRANSFERS = 0x1000,
    TMR_DMA_BURSTLENGTH_18TRANSFERS = 0x1100
} TMR_DMA_BURSTLENGTH_T;

/**@} end of group TMR_Enumeration Enumerations*/

/** @addtogroup  TMR_Structure Structures
  @{
*/

/**
  * @brief  TMR Time Base Init structure definition
  * @note   This structure is used with all TMR except for TMR6 and TMR7.
  */
typedef struct
{
    TMR_COUNTER_MODE_T     countMode;
    TMR_CLOCK_DIV_T        clockDivision;
    uint16_t               period;            //!< This must between 0x0000 and 0xFFFF
    uint16_t               division;          //!< This must between 0x0000 and 0xFFFF
} TMR_BaseConfig_T;

/**
 * @brief    TMR Config struct definition
 */
typedef struct
{
    TMR_OC_MODE_T          mode;
    TMR_OC_STATE_T         outputState;
    TMR_OC_POLARITY_T      polarity;
    TMR_OC_NPOLARITY_T     nPolarity;
    uint16_t               pulse;     //!< This must between 0x0000 and 0xFFFF
} TMR_OCConfig_T;

/**
 * @brief    TMR Input Capture Config struct definition
 */
typedef struct
{
    TMR_CHANNEL_T          channel;
    TMR_IC_POLARITY_T      polarity;
    TMR_IC_SELECTION_T     selection;
    TMR_IC_PSC_T           prescaler;
    uint16_t               filter;    //!< This must between 0x00 and 0x0F
} TMR_ICConfig_T;

/**@} end of group TMR_Structure*/

/** @addtogroup  TMR_Fuctions Fuctions
  @{
*/

/** Reset and Configuration */
void TMR_Reset(TMR_T* tmr);
void TMR_ConfigTimeBase(TMR_T* tmr, TMR_BaseConfig_T* baseConfig);
void TMR_ConfigTimeBaseStructInit(TMR_BaseConfig_T* baseConfig);
void TMR_ConfigPrescaler(TMR_T* tmr, uint16_t prescaler, TMR_PRESCALER_RELOAD_T pscReloadMode);
void TMR_ConfigCounterMode(TMR_T* tmr, TMR_COUNTER_MODE_T countMode);
void TMR_ConfigCounter(TMR_T* tmr, uint16_t counter);
void TMR_ConfigAutoreload(TMR_T* tmr, uint16_t autoReload);
uint16_t TMR_ReadCounter(TMR_T* tmr);
uint16_t TMR_ReadPrescaler(TMR_T* tmr);
void TMR_EnableNoUpdate(TMR_T* tmr);
void TMR_DisableNoUpdate(TMR_T* tmr);
void TMR_ConfigUpdateRequest(TMR_T* tmr, TMR_UPDATE_SOURCE_T updateSource);
void TMR_EnableAutoReload(TMR_T* tmr);
void TMR_DisableAutoReload(TMR_T* tmr);
void TMR_SelectSinglePulseMode(TMR_T* tmr, TMR_SPM_T singlePulseMode);
void TMR_ConfigClockDivision(TMR_T* tmr, TMR_CLOCK_DIV_T clockDivision);
void TMR_Enable(TMR_T* tmr);
void TMR_Disable(TMR_T* tmr);

/** Output Compare management */
void TMR_ConfigOC1(TMR_T* tmr, TMR_OCConfig_T* ocConfig);
void TMR_ConfigOC2(TMR_T* tmr, TMR_OCConfig_T* ocConfig);
void TMR_ConfigOC3(TMR_T* tmr, TMR_OCConfig_T* ocConfig);
void TMR_ConfigOC4(TMR_T* tmr, TMR_OCConfig_T* ocConfig);
void TMR_ConfigOCStructInit(TMR_OCConfig_T* ocConfig);
void TMR_SelectOCxMode(TMR_T* tmr, TMR_CHANNEL_T channel, TMR_OC_MODE_T mode);
void TMR_ConfigCompare1(TMR_T* tmr, uint16_t compare1);
void TMR_ConfigCompare2(TMR_T* tmr, uint16_t compare2);
void TMR_ConfigCompare3(TMR_T* tmr, uint16_t compare3);
void TMR_ConfigCompare4(TMR_T* tmr, uint16_t compare4);
void TMR_ConfigForcedOC1(TMR_T* tmr,TMR_FORCED_ACTION_T forcesAction);
void TMR_ConfigForcedOC2(TMR_T* tmr,TMR_FORCED_ACTION_T forcesAction);
void TMR_ConfigForcedOC3(TMR_T* tmr,TMR_FORCED_ACTION_T forcesAction);
void TMR_ConfigForcedOC4(TMR_T* tmr,TMR_FORCED_ACTION_T forcesAction);
void TMR_ConfigOC1Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload);
void TMR_ConfigOC2Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload);
void TMR_ConfigOC3Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload);
void TMR_ConfigOC4Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload);
void TMR_ConfigOC1Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast);
void TMR_ConfigOC2Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast);
void TMR_ConfigOC3Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast);
void TMR_ConfigOC4Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast);
void TMR_ClearOC1Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear);
void TMR_ClearOC2Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear);
void TMR_ClearOC3Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear);
void TMR_ClearOC4Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear);
void TMR_ConfigOC1Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity);
void TMR_ConfigOC1NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity);
void TMR_ConfigOC2Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity);
void TMR_ConfigOC2NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity);
void TMR_ConfigOC3Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity);
void TMR_ConfigOC3NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity);
void TMR_ConfigOC4Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity);
void TMR_ConfigOC4NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity);
void TMR_EnableCCxChannel(TMR_T* tmr,TMR_CHANNEL_T channel);
void TMR_DisableCCxChannel(TMR_T* tmr,TMR_CHANNEL_T channel);

/** Input Capture management */
void TMR_ConfigIC(TMR_T* tmr, TMR_ICConfig_T *icConfig);
void TMR_ConfigICStructInit(TMR_ICConfig_T *icConfig);
void TMR_ConfigPWM(TMR_T* tmr, TMR_ICConfig_T *pwmConfig);
uint16_t TMR_ReadCaputer1(TMR_T* tmr);
uint16_t TMR_ReadCaputer2(TMR_T* tmr);
uint16_t TMR_ReadCaputer3(TMR_T* tmr);
uint16_t TMR_ReadCaputer4(TMR_T* tmr);
void TMR_ConfigIC1Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler);
void TMR_ConfigIC2Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler);
void TMR_ConfigIC3Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler);
void TMR_ConfigIC4Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler);

/** Interrupts, DMA and flags management */
void TMR_EnableInterrupt(TMR_T* tmr, uint16_t interrupt);
void TMR_DisableInterrupt(TMR_T* tmr, uint16_t interrupt);
void TMR_GenerateEvent(TMR_T* tmr, uint16_t eventSources);
uint16_t TMR_ReadStatusFlag(TMR_T* tmr, TMR_FLAG_T flag);
void TMR_ClearStatusFlag(TMR_T* tmr, uint16_t flag);
uint16_t TMR_ReadIntFlag(TMR_T* tmr,  TMR_INT_T flag);
void TMR_ClearIntFlag(TMR_T* tmr,  uint16_t flag);
void TMR_ConfigDMA(TMR_T* tmr, TMR_DMA_BASE_T address, TMR_DMA_BURSTLENGTH_T lenght);
void TMR_EnableDMASoure(TMR_T* tmr, uint16_t dmaSource);
void TMR_DisableDMASoure(TMR_T* tmr, uint16_t dmaSource);
void TMR_EnableCCDMA(TMR_T* tmr);
void TMR_DisableCCDMA(TMR_T* tmr);

/** Clocks management */
void TMR_ConfigInternalClock(TMR_T* tmr);
void TMR_ConfigITRxExternalClock(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource);
void TMR_ConfigTIxExternalClock(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource,
                                TMR_IC_POLARITY_T icPolarity, uint16_t icFilter);
void TMR_ConfigETRClockMode1(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                             TMR_EXTTRG_POL_T polarity, uint16_t filter);
void TMR_ConfigETRClockMode2(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                             TMR_EXTTRG_POL_T polarity, uint16_t filter);

/** Synchronization management */
void TMR_SelectInputTrigger(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource);
void TMR_SelectOutputTrigger(TMR_T* tmr, TMR_TRGO_SOURCE_T trgoSource);
void TMR_SelectSlaveMode(TMR_T* tmr, TMR_SLAVE_MODE_T slaveMode);
void TMR_EnableMasterSlaveMode(TMR_T* tmr);
void TMR_DisableMasterSlaveMode(TMR_T* tmr);
void TMR_ConfigETR(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                   TMR_EXTTRG_POL_T polarity, uint16_t filter);

/** Specific interface management */
void TMR_ConfigEncodeInterface(TMR_T* tmr, TMR_ENCODER_MODE_T encodeMode, TMR_IC_POLARITY_T ic1Polarity,
                               TMR_IC_POLARITY_T ic2Polarity);
void TMR_EnableHallSensor(TMR_T* tmr);
void TMR_DisableHallSensor(TMR_T* tmr);

/** Specific remapping management */
void TMR_ConfigRemap(TMR_T* tmr, uint32_t remap);

/**@} end of group TMR_Fuctions*/
/**@} end of group TMR_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /*__APM32L0xx_TMR_H */
