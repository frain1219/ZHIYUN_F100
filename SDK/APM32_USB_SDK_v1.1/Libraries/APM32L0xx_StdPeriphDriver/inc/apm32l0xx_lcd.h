/*!
 * @file        apm32l0xx_lcd.h
 *
 * @brief       This file contains all the functions prototypes for the LCD firmware library
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

#ifndef __APM32L0XX_LCD_H
#define __APM32L0XX_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LCD_Driver LCD Driver
  @{
*/

/** @addtogroup LCD_Enumerations Enumerations
  @{
*/

/**
 * @brief LCD Flag
 */
typedef enum
{
    LCD_FLAG_ENS        = ((uint8_t)0x01),
    LCD_FLAG_SOF        = ((uint8_t)0x02),
    LCD_FLAG_UDR        = ((uint8_t)0x04),
    LCD_FLAG_UDD        = ((uint8_t)0x08),
    LCD_FLAG_RDY        = ((uint8_t)0x10),
    LCD_FLAG_FCRSF      = ((uint8_t)0x20)
} LCD_FLAG_T;

/**
 * @brief LCD Prescaler enum definition
 */
typedef enum
{
    LCD_PRESCALER_1    ,
    LCD_PRESCALER_2    ,
    LCD_PRESCALER_4    ,
    LCD_PRESCALER_8    ,
    LCD_PRESCALER_16   ,
    LCD_PRESCALER_32   ,
    LCD_PRESCALER_64   ,
    LCD_PRESCALER_128  ,
    LCD_PRESCALER_256  ,
    LCD_PRESCALER_512  ,
    LCD_PRESCALER_1024 ,
    LCD_PRESCALER_2048 ,
    LCD_PRESCALER_4096 ,
    LCD_PRESCALER_8192 ,
    LCD_PRESCALER_16384,
    LCD_PRESCALER_32768
} LCD_PRESCALER_T;

/**
 * @brief LCD Divider enum definition
 */
typedef enum
{
    LCD_DIVIDER_16,
    LCD_DIVIDER_17,
    LCD_DIVIDER_18,
    LCD_DIVIDER_19,
    LCD_DIVIDER_20,
    LCD_DIVIDER_21,
    LCD_DIVIDER_22,
    LCD_DIVIDER_23,
    LCD_DIVIDER_24,
    LCD_DIVIDER_25,
    LCD_DIVIDER_26,
    LCD_DIVIDER_27,
    LCD_DIVIDER_28,
    LCD_DIVIDER_29,
    LCD_DIVIDER_30,
    LCD_DIVIDER_31
} LCD_DIVIDER_T;

/**
 * @brief LCD Duty enum definition
 */
typedef enum
{
    LCD_DUTY_STATIC,
    LCD_DUTY_1_2   ,
    LCD_DUTY_1_3   ,
    LCD_DUTY_1_4   ,
    LCD_DUTY_1_8
} LCD_DUTY_T;

/**
 * @brief LCD Bias enum definition
 */
typedef enum
{
    LCD_BIAS_1_4,
    LCD_BIAS_1_2,
    LCD_BIAS_1_3
} LCD_BIAS_T;

/**
 * @brief LCD VoltageSource enum definition
 */
typedef enum
{
    LCD_VOLTAGESOURCE_INTERNAL,
    LCD_VOLTAGESOURCE_EXTERNAL
} LCD_VOLTAGESOURCE_T;

/**
 * @brief LCD Contrast enum definition
 */
typedef enum
{
    LCD_CONTRASTLEVEL_0,
    LCD_CONTRASTLEVEL_1,
    LCD_CONTRASTLEVEL_2,
    LCD_CONTRASTLEVEL_3,
    LCD_CONTRASTLEVEL_4,
    LCD_CONTRASTLEVEL_5,
    LCD_CONTRASTLEVEL_6,
    LCD_CONTRASTLEVEL_7
} LCD_CONTRASTLEVEL_T;

/**
 * @brief LCD DeadTime enum definition
 */
typedef enum
{
    LCD_DEADTIME_0,
    LCD_DEADTIME_1,
    LCD_DEADTIME_2,
    LCD_DEADTIME_3,
    LCD_DEADTIME_4,
    LCD_DEADTIME_5,
    LCD_DEADTIME_6,
    LCD_DEADTIME_7
} LCD_DEADTIME_T;

/**
 * @brief LCD PulseOnDuration enum definition
 */
typedef enum
{
    LCD_PULSEONDURATION_0,
    LCD_PULSEONDURATION_1,
    LCD_PULSEONDURATION_2,
    LCD_PULSEONDURATION_3,
    LCD_PULSEONDURATION_4,
    LCD_PULSEONDURATION_5,
    LCD_PULSEONDURATION_6,
    LCD_PULSEONDURATION_7
} LCD_PULSEONDURATION_T;

/**
 * @brief LCD HighDrive enum definition
 */
typedef enum
{
    LCD_HIGHDRIVE_0,
    LCD_HIGHDRIVE_1
} LCD_HIGHDRIVE_T;

/**
 * @brief LCD BlinkMode enum definition
 */
typedef enum
{
    LCD_BLINKMODE_OFF          ,
    LCD_BLINKMODE_SEG0_COM0    ,
    LCD_BLINKMODE_SEG0_ALLCOM  ,
    LCD_BLINKMODE_ALLSEG_ALLCOM
} LCD_BLINKMODE_T;

/**
 * @brief LCD BlinkFrequency enum definition
 */
typedef enum
{
    LCD_BLINKFREQUENCY_DIV8   ,
    LCD_BLINKFREQUENCY_DIV16  ,
    LCD_BLINKFREQUENCY_DIV32  ,
    LCD_BLINKFREQUENCY_DIV64  ,
    LCD_BLINKFREQUENCY_DIV128 ,
    LCD_BLINKFREQUENCY_DIV256 ,
    LCD_BLINKFREQUENCY_DIV512 ,
    LCD_BLINKFREQUENCY_DIV1024
} LCD_BLINKFREQUENCY_T;

/**
 * @brief LCD MuxSegment enum definition
 */
typedef enum
{
    LCD_MUXSEGMENT_DISABLE,
    LCD_MUXSEGMENT_ENABLE
} LCD_MUXSEGMENT_T;

/**@} end of group LCD_Enumerations*/

/** @addtogroup LCD_Macros Macros
  @{
*/

/** Macros description */
/** Timeout definition */
#define LCD_TIMEOUT_VALUE             1000
/**@} end of group LCD_Macros*/

/** @addtogroup LCD_Structure Data Structure
  @{
*/

/**
 * @brief LCD Config struct definition
 */
typedef struct
{
    LCD_PRESCALER_T        prescaler;
    LCD_DIVIDER_T          divider;
    LCD_DUTY_T             duty;
    LCD_BIAS_T             bias;
    LCD_VOLTAGESOURCE_T    voltageSource;
    LCD_CONTRASTLEVEL_T    contrast;
    LCD_DEADTIME_T         deadTime;
    LCD_PULSEONDURATION_T  pulseOnDuration;
    LCD_HIGHDRIVE_T        highDrive;
    LCD_BLINKMODE_T        blinkMode;
    LCD_BLINKFREQUENCY_T   blinkFrequency;
    LCD_MUXSEGMENT_T       muxSegment;
} LCD_Config_T;

/**@} end of group LCD_Structure*/

/** @addtogroup LCD_Fuctions Fuctions
  @{
*/

/** LCD reset and configuration */
void LCD_Reset(void);
void LCD_Config(LCD_Config_T* lcdConfig);
void LCD_ConfigStructInit(LCD_Config_T* tscConfig);

/** IO operation methods */
void LCD_Write(uint16_t com, uint32_t data);
void LCD_Clear(uint16_t com);
void LCD_EnableUpdateDisplayRequest(void);

/** LCD read and clear status flag */
uint8_t LCD_ReadStatusFlag(LCD_FLAG_T flag);
void LCD_ClearStatusFlag(uint32_t flag);

/** enable and disables the LCD */
void LCD_Enable(void);
void LCD_Disable(void);

/** enable and disables the high divider */
void LCD_EnableHighDriver(void);
void LCD_DisableHighDriver(void);

/**@} end of group LCD_Fuctions*/
/**@} end of group LCD_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0XX_LCD_H */
