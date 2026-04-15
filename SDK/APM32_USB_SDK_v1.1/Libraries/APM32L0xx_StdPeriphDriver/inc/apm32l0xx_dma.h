/*!
 * @file        apm32l0xx_dma.h
 *
 * @brief       This file contains all the functions prototypes for the DMA firmware
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

#ifndef __APM32L0xx_DMA_H
#define __APM32L0xx_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup DMA_Driver DMA Driver
  @{
*/

/** @addtogroup DMA_Enumerations Enumerations
  @{
*/

/**
 * @brief   DMA data transfer direction
 */
typedef enum
{
    DMA_DIR_PERIPHERAL,
    DMA_DIR_MEMORY    ,
} DMA_DIR_T;

/**
 * @brief   DMA peripheral increment mode
 */
typedef enum
{
    DMA_PERIPHERAL_INC_DISABLE,
    DMA_PERIPHERAL_INC_ENABLE ,
} DMA_PERIPHERAL_INC_T;

/**
 * @brief   DMA memory increment mode
 */
typedef enum
{
    DMA_MEMORY_INC_DISABLE,
    DMA_MEMORY_INC_ENABLE ,
} DMA_MEMORY_INC_T;

/**
 * @brief   DMA peripheral data size
 */
typedef enum
{
    DMA_PERIPHERAL_DATASIZE_BYTE    ,
    DMA_PERIPHERAL_DATASIZE_HALFWORD,
    DMA_PERIPHERAL_DATASIZE_WORD    ,
} DMA_PERIPHERAL_DATASIZE_T;

/**
 * @brief   DMA memory data size
 */
typedef enum
{
    DMA_MEMORY_DATASIZE_BYTE    ,
    DMA_MEMORY_DATASIZE_HALFWORD,
    DMA_MEMORY_DATASIZE_WORD    ,
} DMA_MEMORY_DATASIZE_T;

/**
 * @brief   DMA circular mode
 */
typedef enum
{
    DMA_CIRCULAR_DISABLE,
    DMA_CIRCULAR_ENABLE ,
} DMA_CIRCULAR_T;

/**
 * @brief   DMA priority level
 */
typedef enum
{
    DMA_PRIORITY_LEVEL_LOW     ,
    DMA_PRIORITY_LEVEL_MEDIUM  ,
    DMA_PRIORITY_LEVEL_HIGHT   ,
    DMA_PRIORITY_LEVEL_VERYHIGH,
} DMA_PRIORITY_LEVEL_T;

/**
 * @brief   DMA memory to memory
 */
typedef enum
{
    DMA_M2M_DISABLE,
    DMA_M2M_ENABLE ,
} DMA_M2M_T;

/**
 * @brief    DMA channels remapping definition
 */
typedef enum
{
    DMA1_CHANNEL1_ADC               = (uint32_t)0x00000000,
    DMA1_CHANNEL1_TMR2_CH3          = (uint32_t)0x00000008,
    DMA1_CHANNEL1_AES_IN            = (uint32_t)0x0000000B,
    DMA1_CHANNEL1_NONE              = (uint32_t)0x0000000F,

    DMA1_CHANNEL2_ADC               = (uint32_t)0x10000000,
    DMA1_CHANNEL2_SPI1_RX           = (uint32_t)0x10000010,
    DMA1_CHANNEL2_USAERT1_TX        = (uint32_t)0x10000030,
    DMA1_CHANNEL2_LPUART1_TX        = (uint32_t)0x10000050,
    DMA1_CHANNEL2_I2C1_TX           = (uint32_t)0x10000060,
    DMA1_CHANNEL2_TMR2_UP           = (uint32_t)0x10000080,
    DMA1_CHANNEL2_TMR6_UP_DAC_CH1   = (uint32_t)0x10000090,
    DMA1_CHANNEL2_TMR3_CH3          = (uint32_t)0x100000A0,
    DMA1_CHANNEL2_AES_OUT           = (uint32_t)0x100000B0,
    DMA1_CHANNEL2_USART4_TX         = (uint32_t)0x100000C0,
    DMA1_CHANNEL2_USART5_TX         = (uint32_t)0x100000D0,
    DMA1_CHANNEL2_I2C3_TX           = (uint32_t)0x100000E0,
    DMA1_CHANNEL2_NONE              = (uint32_t)0x10000F00,

    DMA1_CHANNEL3_SPI1_TX           = (uint32_t)0x20000000,
    DMA1_CHANNEL3_USART1_RX         = (uint32_t)0x20000300,
    DMA1_CHANNEL3_LPUART1_RX        = (uint32_t)0x20000500,
    DMA1_CHANNEL3_I2C1_RX           = (uint32_t)0x20000600,
    DMA1_CHANNEL3_TMR2_CH2          = (uint32_t)0x20000800,
    DMA1_CHANNEL3_TMR4_CH4_TMR4_UP  = (uint32_t)0x20000A00,
    DMA1_CHANNEL3_AES_OUT           = (uint32_t)0x20000B00,
    DMA1_CHANNEL3_USART4_TX         = (uint32_t)0x20000C00,
    DMA1_CHANNEL3_I2C3_RX           = (uint32_t)0x20000E00,

    DMA1_CHANNEL4_SPI2_RX           = (uint32_t)0x30002000,
    DMA1_CHANNEL4_USART1_TX         = (uint32_t)0x30003000,
    DMA1_CHANNEL4_USART2_TX         = (uint32_t)0x30004000,
    DMA1_CHANNEL4_I2C2_TX           = (uint32_t)0x30007000,
    DMA1_CHANNEL4_TMR2_CH4          = (uint32_t)0x30008000,
    DMA1_CHANNEL4_I2C3_TX           = (uint32_t)0x3000E000,
    DMA1_CHANNEL4_TMR7_UP_DAC_CH2   = (uint32_t)0x3000F000,
    DMA1_CHANNEL4_NONE              = (uint32_t)0x30000000,

    DMA1_CHANNEL5_SPI2_TX           = (uint32_t)0x40020000,
    DMA1_CHANNEL5_USART1_RX         = (uint32_t)0x40030000,
    DMA1_CHANNEL5_USART2_RX         = (uint32_t)0x40040000,
    DMA1_CHANNEL5_I2C2_RX           = (uint32_t)0x40070000,
    DMA1_CHANNEL5_TMR2_CH1          = (uint32_t)0x40080000,
    DMA1_CHANNEL5_TMR3_CH1          = (uint32_t)0x400A0000,
    DMA1_CHANNEL5_AES_IN            = (uint32_t)0x400B0000,
    DMA1_CHANNEL5_I2C3_RX           = (uint32_t)0x400E0000,
    DMA1_CHANNEL5_NONE              = (uint32_t)0x400F0000,

    DMA1_CHANNEL6_SPI2_RX           = (uint32_t)0x50200000,
    DMA1_CHANNEL6_USART2_RX         = (uint32_t)0x50400000,
    DMA1_CHANNEL6_LPUART1_RX        = (uint32_t)0x50500000,
    DMA1_CHANNEL6_I2C1_TX           = (uint32_t)0x50600000,
    DMA1_CHANNEL6_TMR3_TRIG         = (uint32_t)0x50900000,
    DMA1_CHANNEL6_USART4_RX         = (uint32_t)0x50C00000,
    DMA1_CHANNEL6_USART5_RX         = (uint32_t)0x50D00000,
    DMA1_CHANNEL6_NONE              = (uint32_t)0x50F00000,

    DMA1_CHANNEL7_SPI2_TX           = (uint32_t)0x62000000,
    DMA1_CHANNEL7_USART2_TX         = (uint32_t)0x64000000,
    DMA1_CHANNEL7_LPUART1_TX        = (uint32_t)0x65000000,
    DMA1_CHANNEL7_I2C1_RX           = (uint32_t)0x66000000,
    DMA1_CHANNEL7_TMR2_CH2_TMR2_CH4 = (uint32_t)0x68000000,
    DMA1_CHANNEL7_USART4_TX         = (uint32_t)0x6C000000,
    DMA1_CHANNEL7_USART5_TX         = (uint32_t)0x6D000000,
    DMA1_CHANNEL7_NONE              = (uint32_t)0x6F000000,
} DMA_CHANNEL_REMAP_T;

/**
 * @brief    DMA flag definition
 */
typedef enum
{
    DMA1_FLAG_AL1    = BIT0 ,  //!< Channel 1 All flag
    DMA1_FLAG_TF1    = BIT1 ,  //!< Channel 1 Transfer Complete flag
    DMA1_FLAG_HT1    = BIT2 ,  //!< Channel 1 Half Transfer Complete flag
    DMA1_FLAG_TE1    = BIT3 ,  //!< Channel 1 Transfer Error flag
    DMA1_FLAG_AL2    = BIT4 ,  //!< Channel 2 All flag
    DMA1_FLAG_TF2    = BIT5 ,  //!< Channel 2 Transfer Complete flag
    DMA1_FLAG_HT2    = BIT6 ,  //!< Channel 2 Half Transfer Complete flag
    DMA1_FLAG_TE2    = BIT7 ,  //!< Channel 2 Transfer Error flag
    DMA1_FLAG_AL3    = BIT8 ,  //!< Channel 3 All flag
    DMA1_FLAG_TF3    = BIT9 ,  //!< Channel 3 Transfer Complete flag
    DMA1_FLAG_HT3    = BIT10,  //!< Channel 3 Half Transfer Complete flag
    DMA1_FLAG_TE3    = BIT11,  //!< Channel 3 Transfer Error flag
    DMA1_FLAG_AL4    = BIT12,  //!< Channel 4 All flag
    DMA1_FLAG_TF4    = BIT13,  //!< Channel 4 Transfer Complete flag
    DMA1_FLAG_HT4    = BIT14,  //!< Channel 4 Half Transfer Complete flag
    DMA1_FLAG_TE4    = BIT15,  //!< Channel 4 Transfer Error flag
    DMA1_FLAG_AL5    = BIT16,  //!< Channel 5 All flag
    DMA1_FLAG_TF5    = BIT17,  //!< Channel 5 Transfer Complete flag
    DMA1_FLAG_HT5    = BIT18,  //!< Channel 5 Half Transfer Complete flag
    DMA1_FLAG_TE5    = BIT19,  //!< Channel 5 Transfer Error flag
    DMA1_FLAG_AL6    = BIT20,  //!< Channel 6 All flag
    DMA1_FLAG_TF6    = BIT21,  //!< Channel 6 Transfer Complete flag
    DMA1_FLAG_HT6    = BIT22,  //!< Channel 6 Half Transfer Complete flag
    DMA1_FLAG_TE6    = BIT23,  //!< Channel 6 Transfer Error flag
    DMA1_FLAG_AL7    = BIT24,  //!< Channel 7 All flag
    DMA1_FLAG_TF7    = BIT25,  //!< Channel 7 Transfer Complete flag
    DMA1_FLAG_HT7    = BIT26,  //!< Channel 7 Half Transfer Complete flag
    DMA1_FLAG_TE7    = BIT27,  //!< Channel 7 Transfer Error flag
} DMA_FLAG_T;

/**
 * @brief    DMA interrupt flag definition
 */
typedef enum
{
    DMA1_INT_FLAG_AL1 = BIT0 ,  //!< Channel 1 All interrupt flag
    DMA1_INT_FLAG_TF1 = BIT1 ,  //!< Channel 1 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT1 = BIT2 ,  //!< Channel 1 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE1 = BIT3 ,  //!< Channel 1 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL2 = BIT4 ,  //!< Channel 2 All interrupt flag
    DMA1_INT_FLAG_TF2 = BIT5 ,  //!< Channel 2 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT2 = BIT6 ,  //!< Channel 2 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE2 = BIT7 ,  //!< Channel 2 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL3 = BIT8 ,  //!< Channel 3 All interrupt flag
    DMA1_INT_FLAG_TF3 = BIT9 ,  //!< Channel 3 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT3 = BIT10,  //!< Channel 3 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE3 = BIT11,  //!< Channel 3 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL4 = BIT12,  //!< Channel 4 All interrupt flag
    DMA1_INT_FLAG_TF4 = BIT13,  //!< Channel 4 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT4 = BIT14,  //!< Channel 4 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE4 = BIT15,  //!< Channel 4 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL5 = BIT16,  //!< Channel 5 All interrupt flag
    DMA1_INT_FLAG_TF5 = BIT17,  //!< Channel 5 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT5 = BIT18,  //!< Channel 5 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE5 = BIT19,  //!< Channel 5 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL6 = BIT20,  //!< Channel 6 All interrupt flag
    DMA1_INT_FLAG_TF6 = BIT21,  //!< Channel 6 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT6 = BIT22,  //!< Channel 6 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE6 = BIT23,  //!< Channel 6 Transfer Error interrupt flag
    DMA1_INT_FLAG_AL7 = BIT24,  //!< Channel 7 All interrupt flag
    DMA1_INT_FLAG_TF7 = BIT25,  //!< Channel 7 Transfer Complete interrupt flag
    DMA1_INT_FLAG_HT7 = BIT26,  //!< Channel 7 Half Transfer Complete interrupt flag
    DMA1_INT_FLAG_TE7 = BIT27,  //!< Channel 7 Transfer Error interrupt flag
} DMA_INT_FLAG_T;

/**
 * @brief   DMA interrupt source
 */
typedef enum
{
    DMA_INT_TFIE    = BIT1,  //!< Transfer complete interrupt enable
    DMA_INT_HTIE    = BIT2,  //!< Half Transfer interrupt enable
    DMA_INT_TEIE    = BIT3,  //!< Transfer error interrupt enable
} DMA_INT_T;

/**@} end of group DMA_Enumerations*/


/** @addtogroup DMA_Structure Data Structure
  @{
*/

/**
 * @brief   DMA Config struct definition
 */
typedef struct
{
    uint32_t                    memoryAddress;        //!< Specifies the DMA memory base address
    uint32_t                    peripheralAddress;    //!< Specifies the DMA peripheral base address
    DMA_DIR_T                   direction;            //!< Specifies the DMA data transfer direction
    uint32_t                    bufferSize;           //!< Specifies the DMA buffer size
    DMA_MEMORY_DATASIZE_T       memoryDataSize;       //!< Specifies the DMA memory data size
    DMA_PERIPHERAL_DATASIZE_T   peripheralDataSize;   //!< Specifies the DMA peripheral data size
    DMA_MEMORY_INC_T            memoryInc;            //!< Specifies the DMA memory address increment
    DMA_PERIPHERAL_INC_T        peripheralInc;        //!< Specifies the DMA peripheral address increment
    DMA_CIRCULAR_T              circular;             //!< Specifies the DMA circular mode
    DMA_PRIORITY_LEVEL_T        priority;             //!< Specifies the DMA software priority
    DMA_M2M_T                   memoryTomemory;       //!< Specifies the DMA memory-to-memory transfer
} DMA_Config_T;

/**@} end of group DMA_Structure*/


/** @addtogroup DMA_Fuctions Fuctions
  @{
*/

/** Reset and configuration */
void DMA_Reset(DMA_CHANNEL_T* channel);
void DMA_Config(DMA_CHANNEL_T* channel, DMA_Config_T* dmaConfig);
void DMA_ConfigStructInit(DMA_Config_T* dmaConfig);
void DMA_Enable(DMA_CHANNEL_T* channel);
void DMA_Disable(DMA_CHANNEL_T* channel);

/** Data number */
void DMA_SetDataNumber(DMA_CHANNEL_T* channel, uint32_t dataNumber);
uint32_t DMA_ReadDataNumber(DMA_CHANNEL_T* channel);

/** Channel remap */
void DMA_ConfigRemap(DMA_CHANNEL_REMAP_T remap);

/** interrupt */
void DMA_EnableInterrupt(DMA_CHANNEL_T* channel, uint32_t interrupt);
void DMA_DisableInterrupt(DMA_CHANNEL_T* channel, uint32_t interrupt);

/** Flag */
uint8_t DMA_ReadStatusFlag(DMA_FLAG_T flag);
void DMA_ClearStatusFlag(uint32_t flag);
uint8_t DMA_ReadIntFlag(DMA_INT_FLAG_T flag);
void DMA_ClearIntFlag(uint32_t flag);

/**@} end of group DMA_Fuctions*/
/**@} end of group DMA_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_DMA_H */
