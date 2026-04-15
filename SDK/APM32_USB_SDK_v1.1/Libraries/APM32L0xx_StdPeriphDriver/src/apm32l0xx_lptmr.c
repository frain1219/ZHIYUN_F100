/*!
 * @file        apm32l0xx_lptmr.c
 *
 * @brief       This file provides all the LPTMR firmware functions.
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

#include "apm32l0xx_lptmr.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LPTMR_Driver  LPTMR Driver
  @{
*/

/** @addtogroup LPTMR_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Deinitializes the LPTMRx peripheral registers to their default reset values.
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 *
 * @note
 */
void LPTMR_Reset(LPTMR_T* lptmr)
{
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_LPTMR1);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_LPTMR1);
}

/*!
 * @brief     Initializes the LPTMR peripheral according to the specified parameters
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     Config: Pointer to a LPTMR_Config_T structure
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_Config(LPTMR_T* lptmr, LPTMR_Config_T* config)
{
    uint32_t tmpcfg = 0;

    tmpcfg = lptmr->CFG;

    tmpcfg &= ((uint32_t) 0xFFCFF1FE);

    tmpcfg |= ((uint32_t)config->clockSource | \
               (uint32_t)config->prescaler | \
               (uint32_t)config->waveform | \
               (uint32_t)config->outputPolarity);

    lptmr->CFG = tmpcfg;
}

/*!
 * @brief     Initialize the LPTMR with its default value.
 *
 * @param     config: pointer to a LPTMR_Config_T structure
 *
 * @retval    None
 */
void LPTMR_ConfigStructInit(LPTMR_Config_T* config)
{
    config->clockSource = LPTMR_CLKSOURSE_APBCLK_LPOSC;
    config->outputPolarity = LPTMR_OUTPUTPOLARITY_HIGH;
    config->prescaler = LPTMR_PRESCALER_DIV1;
    config->waveform = LPTMR_WAVEFORM_PWM_OP;
}

/*!
 * @brief     Enable the specified LPTMR peripheral
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_Enable(LPTMR_T* lptmr)
{
    lptmr->CTRL_B.LPTMREN = ENABLE;
}

/*!
 * @brief     Disable the TMRx update event
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_Disable(LPTMR_T* lptmr)
{
    lptmr->CTRL_B.LPTMREN = DISABLE;
}

/*!
 * @brief     Config the Clock source of the LPTMR counter
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     clockSource: the selected clock source
 *            The parameter can be one of following values:
 *            @arg LPTMR_CLKSOURSE_APBCLK_LPOSC
 *            @arg LPTMR_CLKSOURSE_ULPTIM
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigClockSource(LPTMR_T* lptmr, LPTMR_CLKSOURSE_T clockSource)
{
    lptmr->CFG_B.CLKSEL = BIT_RESET;
    lptmr->CFG_B.CLKSEL = clockSource;
}

/*!
 * @brief     Config the polarity of the edge to be used to count
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     clockPolarity: the selected clock polarity
 *            The parameter can be one of following values:
 *            @arg LPTIM_CLKPOLARITY_RISING
 *            @arg LPTIM_CLKPOLARITY_FALLING
 *            @arg LPTIM_CLKPOLARITY_BOTH
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigClockPolarity(LPTMR_T* lptmr, LPTMR_CLKPOLARITY_T clockPolarity)
{
    lptmr->CFG_B.CLKPOLSEL = BIT_RESET;
    lptmr->CFG_B.CLKPOLSEL = clockPolarity;
}

/*!
 * @brief     Config the Clock Prescaler
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     prescaler: the selected clock prescaler
 *            The parameter can be one of following values:
 *            @arg LPTMR_PRESCALER_DIV1
 *            @arg LPTMR_PRESCALER_DIV2
 *            @arg LPTMR_PRESCALER_DIV4
 *            @arg LPTMR_PRESCALER_DIV8
 *            @arg LPTMR_PRESCALER_DIV16
 *            @arg LPTMR_PRESCALER_DIV32
 *            @arg LPTMR_PRESCALER_DIV64
 *            @arg LPTMR_PRESCALER_DIV128
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigPrescaler(LPTMR_T* lptmr, LPTMR_PRESCALER_T prescaler)
{
    lptmr->CFG_B.CLKPRE = BIT_RESET;
    lptmr->CFG |= prescaler;
}

/*!
 * @brief     Config the trigger source for the counter and its polarity
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     extTRGSource: the selected external trigger
 *            The parameter can be one of following values:
 *            @arg LPTMR_EXTTRGSOURSE_TRIG0
 *            @arg LPTMR_EXTTRGSOURSE_TRIG1
 *            @arg LPTMR_EXTTRGSOURSE_TRIG2
 *            @arg LPTMR_EXTTRGSOURSE_TRIG3
 *            @arg LPTMR_EXTTRGSOURSE_TRIG4
 *            @arg LPTMR_EXTTRGSOURSE_TRIG5
 *            @arg LPTMR_EXTTRGSOURSE_TRIG6
 *            @arg LPTMR_EXTTRGSOURSE_TRIG7
 *
 * @param     extTRGPolarity: the selected external trigger
 *            The parameter can be one of following values:
 *            @arg LPTMR_EXTTRGPOLARITY_RISING
 *            @arg LPTMR_EXTTRGPOLARITY_FALLING
 *            @arg LPTMR_EXTTRGPOLARITY_BOTH
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigExternalTrigger(LPTMR_T* lptmr, LPTMR_EXTTRGSOURSE_T extTRGSource, LPTMR_EXTTRGPOLARITY_T extTRGPolarity)
{
    lptmr->CFG_B.TRGSEL = BIT_RESET;
    lptmr->CFG_B.TRGSEL = extTRGSource;
    lptmr->CFG_B.TRGPOLSEL = BIT_RESET;
    lptmr->CFG_B.TRGPOLSEL = extTRGPolarity;
}

/*!
 * @brief     Config a software start of the counter
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigSoftwareStart(LPTMR_T* lptmr)
{
    lptmr->CFG_B.TRGPOLSEL = BIT_RESET;
}

/*!
 * @brief     Config the Clock Prescaler
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     prescaler: the selected clock prescaler
 *            The parameter can be one of following values:
 *            @arg LPTMR_TRGSAMPLE_DIRECT
 *            @arg LPTMR_TRGSAMPLE_2TRANS
 *            @arg LPTMR_TRGSAMPLE_4TRANS
 *            @arg LPTMR_TRGSAMPLE_8TRANS
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigTriggerGlitchFilter(LPTMR_T* lptmr, LPTMR_TRGSAMPLE_T trigSampleTime)
{
    lptmr->CFG_B.TRGCFG = BIT_RESET;
    lptmr->CFG_B.TRGCFG = trigSampleTime;
}

/*!
 * @brief     Config the Clock Prescaler
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     prescaler: the selected clock prescaler
 *            The parameter can be one of following values:
 *            @arg LPTMR_CLKSAMPLE_DIRECT
 *            @arg LPTMR_CLKSAMPLE_2TRANS
 *            @arg LPTMR_CLKSAMPLE_4TRANS
 *            @arg LPTMR_CLKSAMPLE_8TRANS
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigClockGlitchFilter(LPTMR_T* lptmr, LPTMR_CLKSAMPLE_T clockSampleTime)
{
    lptmr->CFG_B.ECLKCFG = BIT_RESET;
    lptmr->CFG_B.ECLKCFG = clockSampleTime;
}

/*!
 * @brief     Config an operating mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     mode: the selected mode
 *            The parameter can be one of following values:
 *            @arg LPTMR_MODE_CONTINUOUS
 *            @arg LPTMR_MODE_SINGLE
 *
 * @retval    None
 *
 */
void LPTMR_ConfigOperatingMode(LPTMR_T* lptmr, LPTMR_MODE_T mode)
{
    if(mode == LPTMR_MODE_CONTINUOUS)
    {
        lptmr->CTRL_B.STRCON = BIT_SET;
    }
    else
    {
        lptmr->CTRL_B.STRSNG = BIT_SET;
    }
}

/*!
 * @brief     Enable the Timeout function
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_EnableTimeout(LPTMR_T* lptmr)
{
    lptmr->CFG_B.RECNT = BIT_SET;
}

/*!
 * @brief     Disable the Timeout function
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_DisableTimeout(LPTMR_T* lptmr)
{
    lptmr->CFG_B.RECNT = BIT_RESET;
}

/*!
 * @brief     Config the Waveform shape
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     waveform: the waveform
 *            The parameter can be one of following values:
 *            @arg LPTMR_WAVEFORM_PWM_OP
 *            @arg LPTMR_WAVEFORM_SETONCE
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigWaveform(LPTMR_T* lptmr, LPTMR_WAVEFORM_T waveform)
{
    lptmr->CFG_B.SETONEEN = BIT_RESET;
    lptmr->CFG |= waveform;
}

/*!
 * @brief     Config the Autoreload and Compare registers update mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     update: the update mode
 *            The parameter can be one of following values:
 *            @arg LPTMR_UPDATE_IMMEDIATE
 *            @arg LPTMR_UPDATE_ENDOFPERIOD
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_ConfigUpdate(LPTMR_T* lptmr, LPTMR_UPDATE_T update)
{
    lptmr->CFG_B.REGUDSEL = BIT_RESET;
    lptmr->CFG_B.REGUDSEL = update;
}

/*!
 * @brief     Config the passed parameter in the Autoreload register
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     autoreload: the autoreload value
 *            This parameter must be a value between 0x0000 and 0xFFFF
 *
 * @retval    None
 */
void LPTMR_ConfigAutoreloadValue(LPTMR_T* lptmr, uint32_t autoreload)
{
    lptmr->AUTORLD = autoreload;
}

/*!
 * @brief     Config the passed parameter in the Compare register
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     compare: the compare value
 *            This parameter must be a value between 0x0000 and 0xFFFF
 *
 * @retval    None
 */
void LPTMR_ConfigCompareValue(LPTMR_T* lptmr, uint32_t compare)
{
    lptmr->COMPV = compare;
}

/*!
 * @brief     Enable the Counter mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_EnableCounterMode(LPTMR_T* lptmr)
{
    lptmr->CFG_B.CNTINSEL = BIT_SET;
}

/*!
 * @brief     Disable the Counter mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_DisableCounterMode(LPTMR_T* lptmr)
{
    lptmr->CFG_B.CNTINSEL = BIT_RESET;
}

/*!
 * @brief     Enable the Encoder mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_EnableEncoderMode(LPTMR_T* lptmr)
{
    lptmr->CFG_B.ENCEN = BIT_SET;
}

/*!
 * @brief     Disable the Encoder mode
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    None
 */
void LPTMR_DisableEncoderMode(LPTMR_T* lptmr)
{
    lptmr->CFG_B.ENCEN = BIT_RESET;
}

/*!
 * @brief     Read the LPTMRx counter value
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    Counter Register value
 */
uint32_t LPTMR_ReadCounterValue(LPTMR_T* lptmr)
{
    return lptmr->CNT;
}

/*!
 * @brief     Read the LPTMRx Autoreload value
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    Autoreload Register value
 */
uint32_t LPTMR_ReadAutoreloadValue(LPTMR_T* lptmr)
{
    return lptmr->AUTORLD;
}

/*!
 * @brief     Read the LPTMRx Compare value
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @retval    Compare Register value
 */
uint32_t LPTMR_ReadCompareValue(LPTMR_T* lptmr)
{
    return lptmr->COMPV;
}

/*!
 * @brief     Enable the specified LPTMR interrupts
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     interrupt: specifies the LPTMR interrupts sources to be enabled or disabled.
 *            The parameter can be one of following values:
 *            @arg LPTMR_INT_FLAG_DOWN   (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_UP     (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_ARROK
 *            @arg LPTMR_INT_FLAG_CMPOK
 *            @arg LPTMR_INT_FLAG_EXTTRIG
 *            @arg LPTMR_INT_FLAG_ARRM
 *            @arg LPTMR_INT_FLAG_CMPM
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_EnableInterrupt(LPTMR_T* lptmr, uint32_t interrupt)
 {
    lptmr->IEN |= interrupt;
}

/*!
 * @brief     Disable the specified LPTMR interrupts
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     interrupt: specifies the LPTMR interrupts sources to be enabled or disabled.
 *            The parameter can be one of following values:
 *            @arg LPTMR_INT_FLAG_DOWN   (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_UP     (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_ARROK
 *            @arg LPTMR_INT_FLAG_CMPOK
 *            @arg LPTMR_INT_FLAG_EXTTRIG
 *            @arg LPTMR_INT_FLAG_ARRM
 *            @arg LPTMR_INT_FLAG_CMPM
 *
 * @retval    None
 *
 * @note      It is mandatory to disable the peripheral to use this function
 */
void LPTMR_DisableInterrupt(LPTMR_T* lptmr, uint32_t interrupt)
 {
    lptmr->IEN &= ~(interrupt);
}

/*!
 * @brief     Check whether the flag is set or reset
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     flag: specifies the LPTMR Flag sources
 *            The parameter can be any combination of following values:
 *            @arg LPTMR_FLAG_DOWN    (only for LPTMR1)
 *            @arg LPTMR_FLAG_UP      (only for LPTMR1)
 *            @arg LPTMR_FLAG_ARROK
 *            @arg LPTMR_FLAG_CMPOK
 *            @arg LPTMR_FLAG_EXTTRIG
 *            @arg LPTMR_FLAG_ARRM
 *            @arg LPTMR_FLAG_CMPM
 *
 * @retval    SET or RESET
 */
uint16_t LPTMR_ReadStatusFlag(LPTMR_T* lptmr, LPTMR_FLAG_T flag)
{
    if ((lptmr->ISTS & flag) != RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief     Clears the LPTMRx's pending flag
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     flag: specifies the LPTMR Flag sources
 *            The parameter can be any combination of following values:
 *            @arg LPTMR_FLAG_DOWN    (only for LPTMR1)
 *            @arg LPTMR_FLAG_UP      (only for LPTMR1)
 *            @arg LPTMR_FLAG_ARROK
 *            @arg LPTMR_FLAG_CMPOK
 *            @arg LPTMR_FLAG_EXTTRIG
 *            @arg LPTMR_FLAG_ARRM
 *            @arg LPTMR_FLAG_CMPM
 *
 * @retval    SET or RESET
 */
void LPTMR_ClearStatusFlag(LPTMR_T* lptmr, uint32_t flag)
{
    lptmr->INTFCLR |= flag;
}

/*!
 * @brief     Check whether the specified LPTMR interrupt has occurred or not
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     flag: specifies the LPTMR interrupts sources to be enabled or disabled.
 *            The parameter can be one of following values:
 *            @arg LPTMR_INT_FLAG_DOWN   (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_UP     (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_ARROK
 *            @arg LPTMR_INT_FLAG_CMPOK
 *            @arg LPTMR_INT_FLAG_EXTTRIG
 *            @arg LPTMR_INT_FLAG_ARRM
 *            @arg LPTMR_INT_FLAG_CMPM
 *
 * @retval    SET or RESET
 */
uint16_t LPTMR_ReadIntFlag(LPTMR_T* lptmr, LPTMR_INT_FLAG_T flag)
{
    if (((lptmr->ISTS & flag) != RESET ) && ((lptmr->IEN & flag) != RESET))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief     Clears the LPTMRx's interrupt pending bits
 *
 * @param     lptmr: Select LPTMRx peripheral, The x can be 1
 *
 * @param     flag: specifies the LPTMR interrupts sources to be enabled or disabled.
 *            The parameter can be one of following values:
 *            @arg LPTMR_INT_FLAG_DOWN   (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_UP     (only for LPTMR1)
 *            @arg LPTMR_INT_FLAG_ARROK
 *            @arg LPTMR_INT_FLAG_CMPOK
 *            @arg LPTMR_INT_FLAG_EXTTRIG
 *            @arg LPTMR_INT_FLAG_ARRM
 *            @arg LPTMR_INT_FLAG_CMPM
 *
 * @retval    None
 */
void LPTMR_ClearIntFlag(LPTMR_T* lptmr, uint32_t flag)
{
    LPTMR->INTFCLR |= flag;
}

/**@} end of group TMR_Fuctions*/
/**@} end of group TMR_Driver*/
/**@} end of group Peripherals_Library*/
