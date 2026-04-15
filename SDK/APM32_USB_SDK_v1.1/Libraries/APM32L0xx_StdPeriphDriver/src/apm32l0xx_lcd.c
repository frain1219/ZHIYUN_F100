/*!
 * @file        apm32l0xx_lcd.c
 *
 * @brief       This file contains all the functions for the LCD peripheral
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

#include "apm32l0xx_lcd.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LCD_Driver LCD Driver
  @{
*/

/** @addtogroup LCD_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Set the LCD peripheral registers to their default reset values
 *
 * @param     None
 *
 * @retval    None
 */
void LCD_Reset(void)
{
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_LCD);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_LCD);
}

/*!
 * @brief       Config the LCD peripheral according to the specified parameters in the tscConfig
 *
 * @param       lcdConfig:  Pointer to a LCD_Config_T structure that
 *                          contains the configuration information for the LCD peripheral
 *
 * @retval      None
 */
void LCD_Config(LCD_Config_T* lcdConfig)
{
    /** Set all functions */
    LCD->FCTRL_B.PRESEL    = lcdConfig->prescaler;
    LCD->FCTRL_B.CLKDIVSEL = lcdConfig->divider;
    LCD->FCTRL_B.BMSEL     = lcdConfig->blinkMode;
    LCD->FCTRL_B.BFSEL     = lcdConfig->blinkFrequency;
    LCD->FCTRL_B.DEADCFG   = lcdConfig->deadTime;
    LCD->FCTRL_B.PONCFG    = lcdConfig->pulseOnDuration;
    LCD->FCTRL_B.VLCDCFG   = lcdConfig->contrast;
    LCD->FCTRL_B.HDEN      = lcdConfig->highDrive;
    LCD->CTRL_B.DUTYCFG    = lcdConfig->duty;
    LCD->CTRL_B.BIASCFG    = lcdConfig->bias;
    LCD->CTRL_B.VSCFG      = lcdConfig->voltageSource;
    LCD->CTRL_B.MUXSEGEN   = lcdConfig->muxSegment;
}

/*!
 * @brief       Fills each lcdConfig member with its default value
 *
 * @param       lcdConfig:  Pointer to a LCD_Config_T structure which will be initialized
 *
 * @retval      None
 */
void LCD_ConfigStructInit(LCD_Config_T* lcdConfig)
{
    lcdConfig->prescaler       = LCD_PRESCALER_16;
    lcdConfig->divider         = LCD_DIVIDER_16;
    lcdConfig->blinkMode       = LCD_BLINKMODE_OFF;
    lcdConfig->blinkFrequency  = LCD_BLINKFREQUENCY_DIV8;
    lcdConfig->deadTime        = LCD_DEADTIME_0;
    lcdConfig->pulseOnDuration = LCD_PULSEONDURATION_0;
    lcdConfig->contrast        = LCD_CONTRASTLEVEL_0;
    lcdConfig->highDrive       = LCD_HIGHDRIVE_0;
    lcdConfig->duty            = LCD_DUTY_STATIC;
    lcdConfig->bias            = LCD_BIAS_1_4;
    lcdConfig->voltageSource   = LCD_VOLTAGESOURCE_INTERNAL;
    lcdConfig->muxSegment      = LCD_MUXSEGMENT_DISABLE;
}

/*!
 * @brief  Writes a word in the specific LCD DM
 *
 * @param  com: Display memory index from 1 to 15
 *
 *         data: Each bit corresponds to one pixel of the LCD display
 *
 * @retval None
 */
void LCD_Write(uint16_t com, uint32_t data)
{
    LCD->DM[com] = data;
}

/*!
 * @brief  Clears the LCD DM registers
 *
 * @param  com: Display memory index from 1 to 15
 *
 * @retval None
 */
void LCD_Clear(uint16_t com)
{
    LCD->DM[com] = RESET;
}

/*!
 * @brief  Enables the Update Display Request
 *
 * @param  None
 *
 * @retval None
 */
void LCD_EnableUpdateDisplayRequest(void)
{
    LCD->STS_B.UDRFLG = SET;
}

/*!
 * @brief       Reads the Specifies LCD flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                          @arg LCD_FLAG_ENS
 *                          @arg LCD_FLAG_SOF
 *                          @arg LCD_FLAG_UDR
 *                          @arg LCD_FLAG_UDD
 *                          @arg LCD_FLAG_RDY
 *                          @arg LCD_FLAG_FCRSF
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t LCD_ReadStatusFlag(LCD_FLAG_T flag)
{
    uint32_t status;

    status = LCD->STS;

    if ((status & flag) != (uint32_t)RESET)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified LCD flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be any combination of the following values:
 *                          @arg LCD_FLAG_SOF
 *                          @arg LCD_FLAG_UDD
 *
 * @retval      None
 */
void LCD_ClearStatusFlag(uint32_t flag)
{
    LCD->CLR = (uint32_t)flag;
}

/*!
 *@brief   macros to enables the LCD
 *
 * @param  None
 *
 * @retval None
 */
void LCD_Enable(void)
{
    LCD->CTRL_B.LCDEN = BIT_SET;
}

/*!
 * @brief  macros to disables the LCD
 *
 * @param  None
 *
 * @retval None
 */
void LCD_Disable(void)
{
    LCD->CTRL_B.LCDEN = BIT_RESET;
}

/*!
 *@brief   Macros to enable the low resistance divider.
 *
 * @param  None
 *
 * @retval None
 */
void LCD_EnableHighDriver(void)
{
    LCD->FCTRL_B.HDEN = BIT_SET;
}

/*!
 *@brief   Macros to enable the low resistance divider.
 *
 * @param  None
 *
 * @retval None
 */
void LCD_DisableHighDriver(void)
{
    LCD->FCTRL_B.HDEN = BIT_RESET;
}

/**@} end of group LCD_Fuctions*/
/**@} end of group LCD_Driver*/
/**@} end of group Peripherals_Library*/
