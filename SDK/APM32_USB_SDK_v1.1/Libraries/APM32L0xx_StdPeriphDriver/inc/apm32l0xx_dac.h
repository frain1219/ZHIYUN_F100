/*!
 * @file        apm32l0xx_dac.h
 *
 * @brief       This file contains all the functions prototypes for the DAC firmware library
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

#ifndef __APM32L0xx_DAC_H
#define __APM32L0xx_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup DAC_Driver DAC Driver
  @{
*/

/** @addtogroup DAC_Enumerations Enumerations
  @{
*/

/**
 * @brief    DAC_Trigger
 */
typedef enum
{
    DAC_TRIGGER_T6_TRGO,            //!< Timer 6 TRGO event
    DAC_TRIGGER_T3_TRGO,            //!< Timer 3 TRGO event
    DAC_TRIGGER_T3_CH3,             //!< Timer 3 CH3 event
    DAC_TRIGGER_T21_TRGO,           //!< Timer 21 TRGO event
    DAC_TRIGGER_T2_TRGO,            //!< Timer 2 TRGO event
    DAC_TRIGGER_T7_TRGO,            //!< Timer 7 TRGO event
    DAC_TRIGGER_EINT_IT9,           //!< EINT line9
    DAC_TRIGGER_SOFTWARE            //!< Software trigger
}DAC_TRIGGER_T;

/**
 * @brief    DAC_wave_generation
 */

typedef enum
{
    DAC_WAVE_GENERATION_NONE,       //!< Wave generation disabled
    DAC_WAVE_GENERATION_NOISE,      //!< Noise wave generation enabled
    DAC_WAVE_GENERATION_TRIANGLE    //!< Triangle wave generation enabled
}DAC_WAVE_GENERATION_T;

/**
 * @brief    DAC channelx mask/amplitude selector
 */
typedef enum
{
    DAC_LFSRUNAMASK_BIT0,           //!< Unmask bit0 of LFSR
    DAC_LFSRUNAMASK_BITS1_0,        //!< Unmask bits[1:0] of LFSR to 3
    DAC_LFSRUNAMASK_BITS2_0,        //!< Unmask bits[2:0] of LFSR to 7
    DAC_LFSRUNAMASK_BITS3_0,        //!< Unmask bits[3:0] of LFSR to 15
    DAC_LFSRUNAMASK_BITS4_0,        //!< Unmask bits[4:0] of LFSR to 31
    DAC_LFSRUNAMASK_BITS5_0,        //!< Unmask bits[5:0] of LFSR to 63
    DAC_LFSRUNAMASK_BITS6_0,        //!< Unmask bits[6:0] of LFSR to 127
    DAC_LFSRUNAMASK_BITS7_0,        //!< Unmask bits[7:0] of LFSR to 255
    DAC_LFSRUNAMASK_BITS8_0,        //!< Unmask bits[8:0] of LFSR to 511
    DAC_LFSRUNAMASK_BITS9_0,        //!< Unmask bits[9:0] of LFSR to 1023
    DAC_LFSRUNAMASK_BITS10_0,       //!< Unmask bits[10:0] of LFS to 2047
    DAC_LFSRUNAMASK_BITS11_0,       //!< Unmask bits[11:0] of LFS to 4095
    DAC_TRIANGLEAMPLITUDE_1 = 0,    //!< Select max triangle amplitude of 1
    DAC_TRIANGLEAMPLITUDE_3,        //!< Select max triangle amplitude of 3
    DAC_TRIANGLEAMPLITUDE_7,        //!< Select max triangle amplitude of 7
    DAC_TRIANGLEAMPLITUDE_15,       //!< Select max triangle amplitude of 15
    DAC_TRIANGLEAMPLITUDE_31,       //!< Select max triangle amplitude of 31
    DAC_TRIANGLEAMPLITUDE_63,       //!< Select max triangle amplitude of 63
    DAC_TRIANGLEAMPLITUDE_127,      //!< Select max triangle amplitude of 127
    DAC_TRIANGLEAMPLITUDE_255,      //!< Select max triangle amplitude of 255
    DAC_TRIANGLEAMPLITUDE_511,      //!< Select max triangle amplitude of 511
    DAC_TRIANGLEAMPLITUDE_1023,     //!< Select max triangle amplitude of 1023
    DAC_TRIANGLEAMPLITUDE_2047,     //!< Select max triangle amplitude of 2047
    DAC_TRIANGLEAMPLITUDE_4095      //!< Select max triangle amplitude of 4095
}DAC_MASK_AMPLITUDE_SEL_T;

/**
 * @brief    DAC_OutputBuffer
 */
typedef enum
{
    DAC_OUTPUTBUFF_ENABLE,          //!< DAC channel1 output buffer enabled
    DAC_OUTPUTBUFF_DISABLE          //!< DAC channel1 output buffer disabled
}DAC_OUTPUTBUFF_T;

/**
 * @brief    DAC_Channel_selection
 */
typedef enum
{
    DAC_CHANNEL_1  =  0,           //!< DAC channel1
    DAC_CHANNEL_2  =  BIT4         //!< DAC channel2
}DAC_CHANNEL_T;

/**
 * @brief    DAC_data_alignment
 */
typedef enum
{
    DAC_ALIGN_12B_R = 0,           //!< DAC 12-bit right-aligned data
    DAC_ALIGN_12B_L = BIT2,        //!< DAC 12-bit left-aligned data
    DAC_ALIGN_8B_R  = BIT3         //!< DAC 8-bit right-aligned data
}DAC_DATA_ALIGN_T;

/**
 * @brief    DAC_interrupts_definition
 */
typedef enum
{
    DAC_INT_CH1_DMAUDR = BIT13,     //!< DAC channel1 DMA Underrun Interrupt
    DAC_INT_CH2_DMAUDR = BIT29      //!< DAC channel2 DMA Underrun Interrupt
}DAC_INT_T;

/**
 * @brief    DAC_flags_definition
 */
typedef enum
{
    DAC_FLAG_CH1_DMAUDR =  BIT13,    //!< DAC channel1 DMA Underrun Flag
    DAC_FLAG_CH2_DMAUDR =  BIT29     //!< DAC channel2 DMA Underrun Flag
}DAC_FLAG_T;

/**@} end of group DAC_Enumerations*/


/** @addtogroup DAC_Macros Macros
  @{
*/

/** Macros description */
#define CTRL_CLEAR_MASK         ((uint32_t)0x00000FFE)
#define DUAL_SWTRIG_SET         ((uint32_t)0x00000003)
#define DUAL_SWTRIG_RESET       ((uint32_t)0xFFFFFFFC)
#define DH12RCH1_OFFSET         ((uint32_t)0x00000008)
#define DH12RCH2_OFFSET         ((uint32_t)0x00000014)
#define DH12RD_OFFSET           ((uint32_t)0x00000020)
#define DATOUT_OFFSET           ((uint32_t)0x0000002C)

/**@} end of group DAC_Macros*/


/** @addtogroup DAC_Structure Data Structure
  @{
*/

/**
 * @brief    DAC Config structure definition
 */
typedef struct
{
    DAC_TRIGGER_T             trigger;             //!< DAC trigger selection
    DAC_OUTPUTBUFF_T          outputBuff;          //!< DAC output buffer disable
    DAC_WAVE_GENERATION_T     waveGeneration;      //!< DAC noise/triangle wave generation selection
    DAC_MASK_AMPLITUDE_SEL_T  maskAmplitudeSelect; //!< DAC mask/amplitude selector
}DAC_Config_T;

/**@} end of group DAC_Structure*/


/** @addtogroup DAC_Fuctions Fuctions
  @{
*/

/** DAC reset and configuration */
void DAC_Reset(void);
void DAC_Config(DAC_CHANNEL_T, DAC_Config_T* dacConfig);
void DAC_ConfigStructInit(DAC_Config_T* dacConfig);
void DAC_Enable(DAC_CHANNEL_T channel);
void DAC_Disable(DAC_CHANNEL_T channel);
void DAC_EnableSoftwareTrigger(DAC_CHANNEL_T channel);
void DAC_DisableSoftwareTrigger(DAC_CHANNEL_T channel);
void DAC_EnableDualSoftwareTrigger(void);
void DAC_DisableDualSoftwareTrigger(void);
void DAC_EnableWaveGeneration(DAC_CHANNEL_T channel, DAC_WAVE_GENERATION_T wave);
void DAC_DisableWaveGeneration(DAC_CHANNEL_T channel, DAC_WAVE_GENERATION_T wave);

/** Read data */
void DAC_ConfigChannel1Data(DAC_DATA_ALIGN_T dataAlign, uint16_t data);
void DAC_ConfigChannel2Data(DAC_DATA_ALIGN_T dataAlign, uint16_t data);
void DAC_ConfigDualChannelData(DAC_DATA_ALIGN_T dataAlign, uint16_t data2, uint16_t data1);
uint16_t DAC_ReadDataOutputValue(DAC_CHANNEL_T channel);

/** DMA */
void DAC_EnableDMA(DAC_CHANNEL_T channel);
void DAC_DisableDMA(DAC_CHANNEL_T channel);

/** Interrupt and flag */
void DAC_EnableInterrupt(DAC_CHANNEL_T channel);
void DAC_DisableInterrupt(DAC_CHANNEL_T channel);
uint8_t DAC_ReadStatusFlag(DAC_FLAG_T flag);
void DAC_ClearStatusFlag(DAC_FLAG_T flag);
uint8_t DAC_ReadIntFlag(DAC_INT_T intFlag);
void DAC_ClearIntFlag(DAC_INT_T intFlag);

/**@} end of group DAC_Fuctions*/
/**@} end of group DAC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_DAC_H */
