/*!
 * @file        apm32l0xx_tmr.c
 *
 * @brief       This file provides all the TMR firmware functions.
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

#include "apm32l0xx_tmr.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup TMR_Driver  TMR Driver
  @{
*/

/** @addtogroup  TMR_Fuctions Fuctions
  @{
*/

static void TI1Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter);
static void TI2Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter);
static void TI3Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter);
static void TI4Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter);

/*!
 * @brief     Deinitializes the TMRx peripheral registers to their default reset values.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 *
 * @note
 */
void TMR_Reset(TMR_T* tmr)
{
    if (tmr == TMR21)
    {
    RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_TMR21);
    RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_TMR21);
    }
    else if (tmr == TMR2)
    {
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_TMR2);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_TMR2);
    }
    else if (tmr == TMR3)
    {
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_TMR3);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_TMR3);
    }
    else if (tmr == TMR6)
    {
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_TMR6);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_TMR6);
    }
    else if (tmr == TMR7)
    {
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_TMR7);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_TMR7);
    }
    else if (tmr == TMR22)
    {
    RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_TMR22);
    RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_TMR22);
    }
}

/*!
 * @brief     Initializes the base timer through the structure
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     baseConfig: Pointer to a TMR_BaseConfig_T structure
 *
 * @retval    None
 */
void TMR_ConfigTimeBase(TMR_T* tmr, TMR_BaseConfig_T* baseConfig)
{
    uint16_t temp1 = 0;

    temp1 = tmr->CTRL1;

    temp1 &= 0x038F;
    temp1 |= baseConfig->countMode;

    temp1 &= 0x00FF;
    temp1 |= baseConfig->clockDivision;

    tmr->CTRL1 = temp1;

    tmr->AUTORLD = baseConfig->period ;

    tmr->PSC = baseConfig->division;

    tmr->CEG_B.UEG = TMR_PRESCALER_RELOAD_IMMEDIATE;
}

/*!
 * @brief     Initialize the Base timer with its default value.
 *
 * @param     baseConfig: pointer to a TMR_BaseConfig_T
 *
 * @retval    None
 */
void TMR_ConfigTimeBaseStructInit(TMR_BaseConfig_T* baseConfig)
{
    baseConfig->countMode = TMR_COUNTER_MODE_UP;
    baseConfig->clockDivision = TMR_CLOCK_DIV_1;
    baseConfig->period = 0xFFFF;
    baseConfig->division = 0x0000;
}

/*!
 * @brief     Configures the TMRx Prescaler
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     prescaler: specifies the Prescaler Register value
 *
 * @param     pscReloadMode: specifies the TMR Prescaler Reload mode
 *            The parameter can be one of following values:
 *            @arg TMR_PRESCALER_RELOAD_UPDATE   : The Prescaler is loaded at the update event
 *            @arg TMR_PRESCALER_RELOAD_IMMEDIATE: The Prescaler is loaded immediately
 *
 * @retval    None
 */
void TMR_ConfigPrescaler(TMR_T* tmr, uint16_t prescaler, TMR_PRESCALER_RELOAD_T pscReloadMode)
{
    tmr->PSC = prescaler;
    tmr->CEG_B.UEG = pscReloadMode;
}

/*!
 * @brief     Config counter mode
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     countMode:specifies the Counter Mode to be used
 *            The parameter can be one of following values:
 *            @arg TMR_COUNTER_MODE_UP            : Timer Up Counting Mode
 *            @arg TMR_COUNTER_MODE_DOWN          : Timer Down Counting Mode
 *            @arg TMR_COUNTER_MODE_CENTERALIGNED1: Timer Center Aligned Mode1
 *            @arg TMR_COUNTER_MODE_CENTERALIGNED2: Timer Center Aligned Mode2
 *            @arg TMR_COUNTER_MODE_CENTERALIGNED3: Timer Center Aligned Mode3
 *
 * @retval    None
 */
void TMR_ConfigCounterMode(TMR_T* tmr, TMR_COUNTER_MODE_T countMode)
{
    tmr->CTRL1_B.CNTDIR = BIT_RESET;
    tmr->CTRL1_B.CAMSEL = BIT_RESET;
    tmr->CTRL1 |= countMode;
}

/*!
 * @brief     Configs the Counter Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     counter: Counter register new value
 *
 * @retval    None
 */
void TMR_ConfigCounter(TMR_T* tmr, uint16_t counter)
{
    tmr->CNT = counter;
}

/*!
 * @brief     Configs the AutoReload Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     autoReload: autoReload register new value
 *
 * @retval    None
 */
void TMR_ConfigAutoreload(TMR_T* tmr, uint16_t autoReload)
{
    tmr->AUTORLD = autoReload;
}

/*!
 * @brief     Read the TMRx Counter value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    Counter Register value.
 */
uint16_t TMR_ReadCounter(TMR_T* tmr)
{
    return tmr->CNT;
}

/*!
 * @brief     Read the TMRx  Prescaler value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    Prescaler Register value.
 */
uint16_t TMR_ReadPrescaler(TMR_T* tmr)
{
    return tmr->PSC;
}

/*!
 * @brief     Enable the TMRx update event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_EnableNoUpdate(TMR_T* tmr)
{
    tmr->CTRL1_B.UD = ENABLE;
}

/*!
 * @brief     Disable the TMRx update event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_DisableNoUpdate(TMR_T* tmr)
{
    tmr->CTRL1_B.UD = DISABLE;
}

/*!
 * @brief     Configures the Update Request Interrupt source
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     updateSource: Config the Update source
 *            The parameter can be one of following values:
 *            @arg TMR_UPDATE_SOURCE_GLOBAL
 *            @arg TMR_UPDATE_SOURCE_REGULAR
 *
 * @retval    None
 */
void TMR_ConfigUpdateRequest(TMR_T* tmr, TMR_UPDATE_SOURCE_T updateSource)
{
    if (updateSource != TMR_UPDATE_SOURCE_GLOBAL)
    {
        tmr->CTRL1_B.URSSEL = BIT_SET;
    }
    else
    {
        tmr->CTRL1_B.URSSEL = BIT_RESET;
    }
}

/*!
 * @brief     Enables peripheral Preload register on AUTORLD.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_EnableAutoReload(TMR_T* tmr)
{
    tmr->CTRL1_B.ARPEN = ENABLE;
}

/*!
 * @brief     Disable peripheral Preload register on AUTORLD.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_DisableAutoReload(TMR_T* tmr)
{
    tmr->CTRL1_B.ARPEN = DISABLE;
}

/*!
 * @brief     Selects the Sing pulse Mode.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     singlePulseMode: specifies the Single Pulse Mode
 *            The parameter can be one of following values:
 *            @arg TMR_SPM_REPETITIVE
 *            @arg TMR_SPM_SINGLE
 *
 * @retval    None
 */
void TMR_SelectSinglePulseMode(TMR_T* tmr, TMR_SPM_T singlePulseMode)
{
    tmr->CTRL1_B.SPMEN = BIT_RESET;
    tmr->CTRL1_B.SPMEN = singlePulseMode;
}

/*!
 * @brief     Configs the Clock Division value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     clockDivision: specifies the clock division value.
 *            The parameter can be one of following values:
 *            @arg TMR_CLOCK_DIV_1: TDTS = Tck_tim
 *            @arg TMR_CLOCK_DIV_2: TDTS = 2*Tck_tim
 *            @arg TMR_CLOCK_DIV_4: TDTS = 4*Tck_tim
 *
 * @retval    None
 */
void TMR_ConfigClockDivision(TMR_T* tmr, TMR_CLOCK_DIV_T clockDivision)
{
    tmr->CTRL1_B.CLKDIV = 0x00;
    tmr->CTRL1 |= clockDivision;
}

/*!
 * @brief     Enable the specified TMR peripheral
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_Enable(TMR_T* tmr)
{
    tmr->CTRL1_B.CNTEN = ENABLE;
}

/*!
 * @brief     Disable the specified TMR peripheral
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @retval    None
 */
void TMR_Disable(TMR_T* tmr)
{
    tmr->CTRL1_B.CNTEN = DISABLE;
}

/*!
 * @brief     Configure channel 1 according to parameters
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocConfig: Pointer to a TMR_OCConfig_T structure
 *
 * @retval    None
 */
void TMR_ConfigOC1(TMR_T* tmr, TMR_OCConfig_T* ocConfig)
{
    tmr->CCEN_B.CC1EN = BIT_RESET;

    tmr->CCM1_COMPARE_B.CC1SEL = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC1MOD = ocConfig->mode;

    tmr->CCEN_B.CC1POL = ocConfig->polarity;
    tmr->CCEN_B.CC1EN = ocConfig->outputState;

    tmr->CCEN_B.CC1NPOL = ocConfig->nPolarity;


    tmr->CC1 = ocConfig->pulse;
}

/*!
 * @brief     Configure channel 2 according to parameters
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocConfig: Pointer to a TMR_OCConfig_T structure
 *
 * @retval    None
 */
void TMR_ConfigOC2(TMR_T* tmr, TMR_OCConfig_T* ocConfig)
{
    tmr->CCEN_B.CC2EN = BIT_RESET;

    tmr->CCM1_COMPARE_B.OC2MOD = BIT_RESET;
    tmr->CCM1_COMPARE_B.CC2SEL = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC2MOD = ocConfig->mode;

    tmr->CCEN_B.CC2POL = BIT_RESET;
    tmr->CCEN_B.CC2POL = ocConfig->polarity;
    tmr->CCEN_B.CC2EN = ocConfig->outputState;

    tmr->CCEN_B.CC2NPOL = BIT_RESET;
    tmr->CCEN_B.CC2NPOL = ocConfig->nPolarity;

    tmr->CC2 = ocConfig->pulse;
}

/*!
 * @brief     Configure channel 3 according to parameters
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocConfig: Pointer to a TMR_OCConfig_T structure
 *
 * @retval    None
 */
void TMR_ConfigOC3(TMR_T* tmr, TMR_OCConfig_T* ocConfig)
{
    tmr->CCEN_B.CC3EN = BIT_RESET;

    tmr->CCM2_COMPARE_B.OC3MOD = BIT_RESET;
    tmr->CCM2_COMPARE_B.CC3SEL = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC3MOD = ocConfig->mode;

    tmr->CCEN_B.CC3POL = BIT_RESET;
    tmr->CCEN_B.CC3POL = ocConfig->polarity;
    tmr->CCEN_B.CC3EN = ocConfig->outputState;

    tmr->CCEN_B.CC3NPOL = BIT_RESET;
    tmr->CCEN_B.CC3NPOL = ocConfig->nPolarity;

    tmr->CC3 = ocConfig->pulse;
}

/*!
 * @brief     Configure channel 4 according to parameters
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocConfig: Pointer to a TMR_OCConfig_T structure
 *
 * @retval    None
 */
void TMR_ConfigOC4(TMR_T* tmr, TMR_OCConfig_T* ocConfig)
{
    tmr->CCEN_B.CC4EN = BIT_RESET;

    tmr->CCM2_COMPARE_B.OC4MOD = BIT_RESET;
    tmr->CCM2_COMPARE_B.CC4SEL = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC4MOD = ocConfig->mode;

    tmr->CCEN_B.CC4POL = BIT_RESET;
    tmr->CCEN_B.CC4POL = ocConfig->polarity;
    tmr->CCEN_B.CC4EN = ocConfig->outputState;

    tmr->CCEN_B.CC4NPOL = BIT_RESET;
    tmr->CCEN_B.CC4NPOL = ocConfig->nPolarity;

    tmr->CC4 = ocConfig->pulse;
}

/*!
 * @brief     Initialize the OC timer with its default value.
 *
 * @param     ocConfig: pointer to a TMR_OCConfig_T
 *
 * @retval    None
 */
void TMR_ConfigOCStructInit(TMR_OCConfig_T* ocConfig)
{
    ocConfig->mode = TMR_OC_MODE_TMRING;
    ocConfig->outputState = TMR_OC_STATE_DISABLE;
    ocConfig->pulse = 0x0000;
    ocConfig->polarity = TMR_OC_POLARITY_HIGH;
    ocConfig->nPolarity = TMR_OC_NPOLARITY_HIGH;
}

/*!
 * @brief     Selects the Output Compare Mode.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     channel: specifies the Channel
 *            The parameter can be one of following values:
 *            @arg TMR_CHANNEL_1: Timer Channel 1
 *            @arg TMR_CHANNEL_2: Timer Channel 2
 *            @arg TMR_CHANNEL_3: Timer Channel 3 (TMR2/3 only)
 *            @arg TMR_CHANNEL_4: Timer Channel 4 (TMR2/3 only)
 *
 * @param     mode: specifies the Output Compare Mode
 *            The parameter can be one of following values:
 *            @arg TMR_OC_MODE_TMRING
 *            @arg TMR_OC_MODE_ACTIVE
 *            @arg TMR_OC_MODE_INACTIVE
 *            @arg TMR_OC_MODE_TOGGEL
 *            @arg TMR_OC_MODE_LOWLEVEL
 *            @arg TMR_OC_MODE_HIGHLEVEL
 *            @arg TMR_OC_MODE_PWM1
 *            @arg TMR_OC_MODE_PWM2
 *
 * @retval    None
 */
void TMR_SelectOCxMode(TMR_T* tmr, TMR_CHANNEL_T channel, TMR_OC_MODE_T mode)
{
    if (channel == TMR_CHANNEL_1)
    {
        tmr->CCM1_COMPARE_B.OC1MOD = BIT_RESET;
        tmr->CCM1_COMPARE_B.OC1MOD = mode;
    }
    else if (channel == TMR_CHANNEL_2)
    {
        tmr->CCM1_COMPARE_B.OC2MOD = BIT_RESET;
        tmr->CCM1_COMPARE_B.OC2MOD = mode;
    }
    else if (channel == TMR_CHANNEL_3)
    {
        tmr->CCM2_COMPARE_B.OC3MOD = BIT_RESET;
        tmr->CCM2_COMPARE_B.OC3MOD = mode;
    }
    else if (channel == TMR_CHANNEL_4)
    {
        tmr->CCM2_COMPARE_B.OC4MOD = BIT_RESET;
        tmr->CCM2_COMPARE_B.OC4MOD = mode;
    }
}

/*!
 * @brief     Configs the Capture Compare1 Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     compare1: specifies the Capture Compare1 value.
 *
 * @retval    None
 */
void TMR_ConfigCompare1(TMR_T* tmr, uint16_t compare1)
{
    tmr->CC1 = compare1;
}

/*!
 * @brief     Configs the Capture Compare2 Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     compare2: specifies the Capture Compare1 value.
 *
 * @retval    None
 */
void TMR_ConfigCompare2(TMR_T* tmr, uint16_t compare2)
{
    tmr->CC2 = compare2;
}

/*!
 * @brief     Configs the Capture Compare3 Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     compare3: specifies the Capture Compare1 value.
 *
 * @retval    None
 */
void TMR_ConfigCompare3(TMR_T* tmr, uint16_t compare3)
{
    tmr->CC3 = compare3;
}

/*!
 * @brief     Configs the Capture Compare4 Register value
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     compare4: specifies the Capture Compare1 value.
 *
 * @retval    None
 */
void TMR_ConfigCompare4(TMR_T* tmr, uint16_t compare4)
{
    tmr->CC4 = compare4;
}

/*!
 * @brief     Forces the output 1 waveform to active or inactive level.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     forcesAction: specifies the forced Action to be set to the output waveform
 *            The parameter can be one of following values:
 *            @arg TMR_FORCED_ACTION_ACTIVE:  Force active level on OC1REF
 *            @arg TMR_FORCED_ACTION_INACTIVE: Force inactive level on OC1REF
 *
 * @retval    None
 */
void TMR_ConfigForcedOC1(TMR_T* tmr, TMR_FORCED_ACTION_T forcesAction)
{
    tmr->CCM1_COMPARE_B.OC1MOD = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC1MOD = forcesAction;
}

/*!
 * @brief     Forces the output 2 waveform to active or inactive level.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     forcesAction: specifies the forced Action to be set to the output waveform
 *            The parameter can be one of following values:
 *            @arg TMR_FORCED_ACTION_ACTIVE:  Force active level on OC1REF
 *            @arg TMR_FORCED_ACTION_INACTIVE: Force inactive level on OC1REF
 *
 * @retval    None
 */
void TMR_ConfigForcedOC2(TMR_T* tmr, TMR_FORCED_ACTION_T forcesAction)
{
    tmr->CCM1_COMPARE_B.OC2MOD = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC2MOD = forcesAction;
}

/*!
 * @brief     Forces the output 3 waveform to active or inactive level.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     forcesAction: specifies the forced Action to be set to the output waveform
 *            The parameter can be one of following values:
 *            @arg TMR_FORCED_ACTION_ACTIVE:  Force active level on OC1REF
 *            @arg TMR_FORCED_ACTION_INACTIVE: Force inactive level on OC1REF
 *
 * @retval    None
 */
void TMR_ConfigForcedOC3(TMR_T* tmr, TMR_FORCED_ACTION_T forcesAction)
{
    tmr->CCM2_COMPARE_B.OC3MOD = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC3MOD = forcesAction;
}

/*!
 * @brief     Forces the output 4 waveform to active or inactive level.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     forcesAction: specifies the forced Action to be set to the output waveform
 *            The parameter can be one of following values:
 *            @arg TMR_FORCED_ACTION_ACTIVE:  Force active level on OC1REF
 *            @arg TMR_FORCED_ACTION_INACTIVE: Force inactive level on OC1REF
 *
 * @retval    None
 */
void TMR_ConfigForcedOC4(TMR_T* tmr, TMR_FORCED_ACTION_T forcesAction)
{
    tmr->CCM2_COMPARE_B.OC4MOD = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC4MOD = forcesAction;
}


/*!
 * @brief     Enables or disables the peripheral Preload register on CCM1.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocPreload: specifies the Output Compare Channel Preload
 *            The parameter can be one of following values:
 *            @arg TMR_OC_PRELOAD_DISABLE
 *            @arg TMR_OC_PRELOAD_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC1Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload)
{
    tmr->CCM1_COMPARE_B.OC1PEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC1PEN = ocPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CCM2.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocPreload: specifies the Output Compare Channel Preload
 *            The parameter can be one of following values:
 *            @arg TMR_OC_PRELOAD_DISABLE
 *            @arg TMR_OC_PRELOAD_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC2Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload)
{
    tmr->CCM1_COMPARE_B.OC2PEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC2PEN = ocPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CCM3.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocPreload: specifies the Output Compare Channel Preload
 *            The parameter can be one of following values:
 *            @arg TMR_OC_PRELOAD_DISABLE
 *            @arg TMR_OC_PRELOAD_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC3Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload)
{
    tmr->CCM2_COMPARE_B.OC3PEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC3PEN = ocPreload;
}

/*!
 * @brief     Enables or disables the peripheral Preload register on CCM4.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocPreload: specifies the Output Compare Channel Preload
 *            The parameter can be one of following values:
 *            @arg TMR_OC_PRELOAD_DISABLE
 *            @arg TMR_OC_PRELOAD_ENABLE
 *
 * @retval    Nonee
 */
void TMR_ConfigOC4Preload(TMR_T* tmr, TMR_OC_PRELOAD_T ocPreload)
{
    tmr->CCM2_COMPARE_B.OC4PEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC4PEN = ocPreload;
}

/*!
 * @brief     Configures the Output Compare 1 Fast feature.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocFast: specifies the Output Compare Channel Fast
 *            The parameter can be one of following values:
 *            @arg TMR_OC_FAST_DISABLE
 *            @arg TMR_OC_FAST_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC1Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast)
{
    tmr->CCM1_COMPARE_B.OC1FEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC1FEN = ocFast;
}

/*!
 * @brief     Configures the Output Compare 2 Fast feature.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     ocFast: specifies the Output Compare Channel Fast
 *            The parameter can be one of following values:
 *            @arg TMR_OC_FAST_DISABLE
 *            @arg TMR_OC_FAST_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC2Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast)
{
    tmr->CCM1_COMPARE_B.OC2FEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC2FEN = ocFast;
}

/*!
 * @brief     Configures the Output Compare 2 Fast feature.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocFast: specifies the Output Compare Channel Fast
 *            The parameter can be one of following values:
 *            @arg TMR_OC_FAST_DISABLE
 *            @arg TMR_OC_FAST_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC3Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast)
{
    tmr->CCM2_COMPARE_B.OC3FEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC3FEN = ocFast;
}

/*!
 * @brief     Configures the Output Compare 4 Fast feature.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocFast: specifies the Output Compare Channel Fast
 *            The parameter can be one of following values:
 *            @arg TMR_OC_FAST_DISABLE
 *            @arg TMR_OC_FAST_ENABLE
 *
 * @retval    None
 */
void TMR_ConfigOC4Fast(TMR_T* tmr, TMR_OC_FAST_T ocFast)
{
    tmr->CCM2_COMPARE_B.OC4FEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC4FEN = ocFast;
}

/*!
 * @brief     Clears or safeguards the OCREF1 signal on an external event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocClear: specifies the Output Compare Channel1 Clear
 *            The parameter can be one of following values:
 *            @arg TMR_OC_CLEAR_DISABLE
 *            @arg TMR_OC_CLEAR_ENABLE
 *
 * @retval    None
 */
void TMR_ClearOC1Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear)
{
    tmr->CCM1_COMPARE_B.OC1CEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC1CEN = ocClear;
}

/*!
 * @brief     Clears or safeguards the OCREF2 signal on an external event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocClear: specifies the Output Compare Channel1 Clear
 *            The parameter can be one of following values:
 *            @arg TMR_OC_CLEAR_DISABLE
 *            @arg TMR_OC_CLEAR_ENABLE
 *
 * @retval    None
 */
void TMR_ClearOC2Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear)
{
    tmr->CCM1_COMPARE_B.OC2CEN = BIT_RESET;
    tmr->CCM1_COMPARE_B.OC2CEN = ocClear;
}

/*!
 * @brief     Clears or safeguards the OCREF3 signal on an external event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocClear: specifies the Output Compare Channel1 Clear
 *            The parameter can be one of following values:
 *            @arg TMR_OC_CLEAR_DISABLE
 *            @arg TMR_OC_CLEAR_ENABLE
 *
 * @retval    None
 */
void TMR_ClearOC3Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear)
{
    tmr->CCM2_COMPARE_B.OC3CEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC3CEN = ocClear;
}

/*!
 * @brief     Clears or safeguards the OCREF4 signal on an external event
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     ocClear: specifies the Output Compare Channel1 Clear
 *            The parameter can be one of following values:
 *            @arg TMR_OC_CLEAR_DISABLE
 *            @arg TMR_OC_CLEAR_ENABLE
 *
 * @retval    None
 */
void TMR_ClearOC4Ref(TMR_T* tmr, TMR_OC_CLEAR_T ocClear)
{
    tmr->CCM2_COMPARE_B.OC4CEN = BIT_RESET;
    tmr->CCM2_COMPARE_B.OC4CEN = ocClear;
}

/*!
 * @brief     Configures the channel 1 polarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     polarity: specifies the OC1 Polarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_POLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_POLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC1Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity)
{
    tmr->CCEN_B.CC1POL = BIT_RESET;
    tmr->CCEN_B.CC1POL = polarity;
}

/*!
 * @brief     Configures the channel 1 nPolarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     nPolarity: specifies the OC1 nPolarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_NPOLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_NPOLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC1NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity)
{
    tmr->CCEN_B.CC1NPOL = BIT_RESET;
    tmr->CCEN_B.CC1NPOL = nPolarity;
}

/*!
 * @brief     Configures the channel 2 polarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     polarity: specifies the OC2 Polarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_POLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_POLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC2Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity)
{
    tmr->CCEN_B.CC2POL = BIT_RESET;
    tmr->CCEN_B.CC2POL = polarity;
}

/*!
 * @brief     Configures the channel 2 nPolarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     nPolarity: specifies the OC2 nPolarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_NPOLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_NPOLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC2NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity)
{
    tmr->CCEN_B.CC2NPOL = BIT_RESET;
    tmr->CCEN_B.CC2NPOL = nPolarity;
}

/*!
 * @brief     Configures the channel 3 polarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     polarity: specifies the OC3 Polarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_POLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_POLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC3Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity)
{
    tmr->CCEN_B.CC3POL = BIT_RESET;
    tmr->CCEN_B.CC3POL = polarity;
}

/*!
 * @brief     Configures the channel 3 nPolarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     nPolarity: specifies the OC3 nPolarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_NPOLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_NPOLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC3NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity)
{
    tmr->CCEN_B.CC3NPOL = BIT_RESET;
    tmr->CCEN_B.CC3NPOL = nPolarity;
}

/*!
 * @brief     Configures the channel 4 polarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     polarity: specifies the OC4 Polarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_POLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_POLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC4Polarity(TMR_T* tmr, TMR_OC_POLARITY_T polarity)
{
    tmr->CCEN_B.CC4POL = BIT_RESET;
    tmr->CCEN_B.CC4POL = polarity;
}

/*!
 * @brief     Configures the channel 4 nPolarity.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     nPolarity: specifies the OC3 nPolarity
 *            The parameter can be one of following values:
 *            @arg TMR_OC_NPOLARITY_HIGH: Output Compare active high
 *            @arg TMR_OC_NPOLARITY_LOW: Output Compare active low
 *
 * @retval    None
 */
void TMR_ConfigOC4NPolarity(TMR_T* tmr, TMR_OC_NPOLARITY_T nPolarity)
{
    tmr->CCEN_B.CC4NPOL = BIT_RESET;
    tmr->CCEN_B.CC4NPOL = nPolarity;
}

/*!
 * @brief     Enables the Capture Compare Channel x.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     channel: specifies the Channel
 *            The parameter can be one of following values:
 *            @arg TMR_CHANNEL_1: Timer Channel 1
 *            @arg TMR_CHANNEL_2: Timer Channel 2
 *            @arg TMR_CHANNEL_3: Timer Channel 3 (TMR2/3 only)
 *            @arg TMR_CHANNEL_4: Timer Channel 4 (TMR2/3 only)
 *
 * @retval    None
 */
void TMR_EnableCCxChannel(TMR_T* tmr, TMR_CHANNEL_T channel)
{
    if (channel == TMR_CHANNEL_1)
    {
        tmr->CCEN_B.CC1EN = BIT_RESET;
        tmr->CCEN_B.CC1EN = BIT_SET;
    }
    else if (channel == TMR_CHANNEL_2)
    {
        tmr->CCEN_B.CC2EN = BIT_RESET;
        tmr->CCEN_B.CC2EN = BIT_SET;
    }
    else if (channel == TMR_CHANNEL_3)
    {
        tmr->CCEN_B.CC3EN = BIT_RESET;
        tmr->CCEN_B.CC3EN = BIT_SET;
    }
    else if (channel == TMR_CHANNEL_4)
    {
        tmr->CCEN_B.CC4EN = BIT_RESET;
        tmr->CCEN_B.CC4EN = BIT_SET;
    }
}

/*!
 * @brief     Disables the Capture Compare Channel x.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     channel: specifies the Channel
 *            The parameter can be one of following values:
 *            @arg TMR_CHANNEL_1: Timer Channel 1
 *            @arg TMR_CHANNEL_2: Timer Channel 2
 *            @arg TMR_CHANNEL_3: Timer Channel 3 (TMR2/3 only)
 *            @arg TMR_CHANNEL_4: Timer Channel 4 (TMR2/3 only)
 *
 * @retval    None
 */
void TMR_DisableCCxChannel(TMR_T* tmr, TMR_CHANNEL_T channel)
{
    if (channel == TMR_CHANNEL_1)
    {
        tmr->CCEN_B.CC1EN = BIT_RESET;
    }
    else if (channel == TMR_CHANNEL_2)
    {
        tmr->CCEN_B.CC2EN = BIT_RESET;
    }
    else if (channel == TMR_CHANNEL_3)
    {
        tmr->CCEN_B.CC3EN = BIT_RESET;
    }
    else if (channel == TMR_CHANNEL_4)
    {
        tmr->CCEN_B.CC4EN = BIT_RESET;
    }
}

/*!
 * @brief     Configure Peripheral equipment
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     icConfig: Pointer to a TMR_ICConfig_T structure
 *
 * @note      TMR_CHANNEL_3 TMR_CHANNEL_4 is used only in TMR2/3
 *
 * @retval    None
 */
void TMR_ConfigIC(TMR_T* tmr, TMR_ICConfig_T* icConfig)
{
    if (icConfig->channel == TMR_CHANNEL_1)
    {
        TI1Config(tmr, icConfig->polarity, icConfig->selection, icConfig->filter);
        TMR_ConfigIC1Prescaler(tmr, icConfig->prescaler);
    }
    else if (icConfig->channel == TMR_CHANNEL_2)
    {
        TI2Config(tmr, icConfig->polarity, icConfig->selection, icConfig->filter);
        TMR_ConfigIC2Prescaler(tmr, icConfig->prescaler);
    }
    else if (icConfig->channel == TMR_CHANNEL_3)
    {
        TI3Config(tmr, icConfig->polarity, icConfig->selection, icConfig->filter);
        TMR_ConfigIC3Prescaler(tmr, icConfig->prescaler);
    }
    else if (icConfig->channel == TMR_CHANNEL_4)
    {
        TI4Config(tmr, icConfig->polarity, icConfig->selection, icConfig->filter);
        TMR_ConfigIC4Prescaler(tmr, icConfig->prescaler);
    }
}

/*!
 * @brief     Initialize the IC timer with its default value.
 *
 * @param     icConfig: pointer to a TMR_ICConfig_T
 *
 * @retval    None
 */
void TMR_ConfigICStructInit(TMR_ICConfig_T* icConfig)
{
    icConfig->channel = TMR_CHANNEL_1;
    icConfig->polarity = TMR_IC_POLARITY_RISING;
    icConfig->selection = TMR_IC_SELECTION_DIRECT_TI;
    icConfig->prescaler = TMR_IC_PSC_1;
    icConfig->filter = 0x00;
}

/*!
 * @brief     Config of TMR to PWM
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     pwmConfig: pointer to a TMR_ICConfig_T
 *
 * @note      TMR_CHANNEL_3 TMR_CHANNEL_4 is used only in TMR2/3
 *
 * @retval    None
 */
void TMR_ConfigPWM(TMR_T* tmr, TMR_ICConfig_T* pwmConfig)
{
    uint16_t icpolarity = TMR_IC_POLARITY_RISING;
    uint16_t icselection = TMR_IC_SELECTION_DIRECT_TI;

    if (pwmConfig->polarity == TMR_IC_POLARITY_RISING)
    {
        icpolarity = TMR_IC_POLARITY_FALLING;
    }
    else
    {
        icpolarity = TMR_IC_POLARITY_RISING;
    }

    if (pwmConfig->selection == TMR_IC_SELECTION_DIRECT_TI)
    {
        icselection = TMR_IC_SELECTION_INDIRECT_TI;
    }
    else
    {
        icselection = TMR_IC_SELECTION_DIRECT_TI;
    }

    if (pwmConfig->channel == TMR_CHANNEL_1)
    {
        TI1Config(tmr, pwmConfig->polarity, pwmConfig->selection, pwmConfig->filter);
        TMR_ConfigIC1Prescaler(tmr, pwmConfig->prescaler);
        TI2Config(tmr, icpolarity, icselection, pwmConfig->filter);
        TMR_ConfigIC2Prescaler(tmr, pwmConfig->prescaler);
    }
    else
    {
        TI2Config(tmr, pwmConfig->polarity, pwmConfig->selection, pwmConfig->filter);
        TMR_ConfigIC2Prescaler(tmr, pwmConfig->prescaler);
        TI1Config(tmr, icpolarity, icselection, pwmConfig->filter);
        TMR_ConfigIC1Prescaler(tmr, pwmConfig->prescaler);
    }
}

/*!
 * @brief     Read Input Capture 1 value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @retval    Capture Compare 1 Register value.
 */
uint16_t TMR_ReadCaputer1(TMR_T* tmr)
{
    return tmr->CC1;
}

/*!
 * @brief     Read Input Capture 2 value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @retval    Capture Compare 2 Register value.
 */
uint16_t TMR_ReadCaputer2(TMR_T* tmr)
{
    return tmr->CC2;
}

/*!
 * @brief     Read Input Capture 3 value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    Capture Compare 3 Register value.
 */
uint16_t TMR_ReadCaputer3(TMR_T* tmr)
{
    return tmr->CC3;
}

/*!
 * @brief     Read Input Capture 4 value.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    Capture Compare 4 Register value.
 */
uint16_t TMR_ReadCaputer4(TMR_T* tmr)
{
    return tmr->CC4;
}

/*!
 * @brief     Configs the TMRx Input Capture 1 prescaler.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     prescaler: specifies the Input Capture Channel1 Perscaler
 *            The parameter can be one of following values:
 *            @arg TMR_IC_PSC_1: no prescaler
 *            @arg TMR_IC_PSC_2: capture is done once every 2 events
 *            @arg TMR_IC_PSC_4: capture is done once every 4 events
 *            @arg TMR_IC_PSC_8: capture is done once every 8 events
 *
 * @retval    None
 */
void TMR_ConfigIC1Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler)
{
    tmr->CCM1_CAPTURE_B.IC1PSC = BIT_RESET;
    tmr->CCM1_CAPTURE_B.IC1PSC = prescaler;
}
/*!
 * @brief     Sets the TMRx Input Capture 2 prescaler.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     prescaler: specifies the Input Capture Channel2 Perscaler
 *            The parameter can be one of following values:
 *            @arg TMR_IC_PSC_1: no prescaler
 *            @arg TMR_IC_PSC_2: capture is done once every 2 events
 *            @arg TMR_IC_PSC_4: capture is done once every 4 events
 *            @arg TMR_IC_PSC_8: capture is done once every 8 events
 *
 * @retval    None
 */
void TMR_ConfigIC2Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler)
{
    tmr->CCM1_CAPTURE_B.IC2PSC = BIT_RESET;
    tmr->CCM1_CAPTURE_B.IC2PSC = prescaler;
}

/*!
 * @brief     Configs the TMRx Input Capture 3 prescaler.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     prescaler: specifies the Input Capture Channel3 Perscaler
 *            The parameter can be one of following values:
 *            @arg TMR_IC_PSC_1: no prescaler
 *            @arg TMR_IC_PSC_2: capture is done once every 2 events
 *            @arg TMR_IC_PSC_4: capture is done once every 4 events
 *            @arg TMR_IC_PSC_8: capture is done once every 8 events
 *
 * @retval    None
 */
void TMR_ConfigIC3Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler)
{
    tmr->CCM2_CAPTURE_B.IC3PSC = BIT_RESET;
    tmr->CCM2_CAPTURE_B.IC3PSC = prescaler;
}

/*!
 * @brief     Configs the TMRx Input Capture 4 prescaler.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     prescaler: specifies the Input Capture Channel4 Perscaler
 *            The parameter can be one of following values:
 *            @arg TMR_IC_PSC_1: no prescaler
 *            @arg TMR_IC_PSC_2: capture is done once every 2 events
 *            @arg TMR_IC_PSC_4: capture is done once every 4 events
 *            @arg TMR_IC_PSC_8: capture is done once every 8 events
 *
 * @retval    None
 */
void TMR_ConfigIC4Prescaler(TMR_T* tmr, TMR_IC_PSC_T prescaler)
{
    tmr->CCM2_CAPTURE_B.IC4PSC = BIT_RESET;
    tmr->CCM2_CAPTURE_B.IC4PSC = prescaler;
}

/*!
 * @brief     Enable interrupts
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be any combination of following values:
 *            @arg TMR_INT_UPDATE: Timer update Interrupt source
 *            @arg TMR_INT_CH1: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH2: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH3: Timer Capture Compare 3 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_CH4: Timer Capture Compare 4 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_TRG: Timer Trigger Interrupt source           (TMR 2/3 only)
 *
 * @retval    None
 */
void TMR_EnableInterrupt(TMR_T* tmr, uint16_t interrupt)
{
    tmr->DIEN |= interrupt;
}

/*!
 * @brief     Disable intterupts
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be any combination of following values:
 *            @arg TMR_INT_UPDATE: Timer update Interrupt source
 *            @arg TMR_INT_CH1: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH2: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH3: Timer Capture Compare 3 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_CH4: Timer Capture Compare 4 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_TRG: Timer Trigger Interrupt source           (TMR 2/3 only)
 *
 * @retval    None
 */
void TMR_DisableInterrupt(TMR_T* tmr, uint16_t interrupt)
{
    tmr->DIEN &= ~interrupt;
}

/*!
 * @brief     Configures the TMRx event to be generate by software.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     eventSources: specifies the TMR event sources
 *            The parameter can be any combination of following values:
 *            @arg TMR_EVENT_UPDATE: Timer update Interrupt source
 *            @arg TMR_EVENT_CH1: Timer Capture Compare 1 Event source (TMR 2/3/21/22 only)
 *            @arg TMR_EVENT_CH2: Timer Capture Compare 1 Event source (TMR 2/3/21/22 only)
 *            @arg TMR_EVENT_CH3: Timer Capture Compare 3 Event source (TMR 2/3 only)
 *            @arg TMR_EVENT_CH4: Timer Capture Compare 4 Event source (TMR 2/3 only)
 *            @arg TMR_EVENT_TRG: Timer Trigger Event source           (TMR 2/3 only)
 *
 * @retval    None
 */
void TMR_GenerateEvent(TMR_T* tmr, uint16_t eventSources)
{
    tmr->CEG = eventSources;
}

/*!
 * @brief     Check whether the flag is set or reset
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be any combination of following values:
 *            @arg TMR_FLAG_UPDATE: Timer update Flag
 *            @arg TMR_FLAG_CH1: Timer Capture Compare 1 Flag              (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CH2: Timer Capture Compare 2 Flag              (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CH3: Timer Capture Compare 3 Flag              (TMR 2/3 only)
 *            @arg TMR_FLAG_CH4: Timer Capture Compare 4 Flag              (TMR 2/3 only)
 *            @arg TMR_FLAG_TRG: Timer Trigger Flag                        (TMR 2/3 only)
 *            @arg TMR_FLAG_CC1RC: Timer Capture Compare 1 Repetition Flag (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CC2RC: Timer Capture Compare 2 Repetition Flag (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CC3RC: Timer Capture Compare 3 Repetition Flag (TMR 2/3 only)
 *            @arg TMR_FLAG_CC4RC: Timer Capture Compare 4 Repetition Flag (TMR 2/3 only)
 *
 * @retval    None
 */
uint16_t TMR_ReadStatusFlag(TMR_T* tmr, TMR_FLAG_T flag)
{
    if ((tmr->STS & flag) != RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief     Clears the TMR's pending flags.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be any combination of following values:
 *            @arg TMR_FLAG_UPDATE: Timer update Flag
 *            @arg TMR_FLAG_CH1: Timer Capture Compare 1 Flag              (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CH2: Timer Capture Compare 2 Flag              (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CH3: Timer Capture Compare 3 Flag              (TMR 2/3 only)
 *            @arg TMR_FLAG_CH4: Timer Capture Compare 4 Flag              (TMR 2/3 only)
 *            @arg TMR_FLAG_TRG: Timer Trigger Flag                        (TMR 2/3 only)
 *            @arg TMR_FLAG_CC1RC: Timer Capture Compare 1 Repetition Flag (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CC2RC: Timer Capture Compare 2 Repetition Flag (TMR 2/3/21/22 only)
 *            @arg TMR_FLAG_CC3RC: Timer Capture Compare 3 Repetition Flag (TMR 2/3 only)
 *            @arg TMR_FLAG_CC4RC: Timer Capture Compare 4 Repetition Flag (TMR 2/3 only)
 *
 * @retval    None
 */
void TMR_ClearStatusFlag(TMR_T* tmr, uint16_t flag)
{
    tmr->STS = ~flag;
}

/*!
 * @brief     Check whether the ITflag is set or reset
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be one of following values:
 *            @arg TMR_INT_UPDATE: Timer update Interrupt source
 *            @arg TMR_INT_CH1: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH2: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH3: Timer Capture Compare 3 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_CH4: Timer Capture Compare 4 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_TRG: Timer Trigger Interrupt source           (TMR 2/3 only)
 *
 * @retval    None
 */
uint16_t TMR_ReadIntFlag(TMR_T* tmr, TMR_INT_T flag)
{
    if (((tmr->STS & flag) != RESET ) && ((tmr->DIEN & flag) != RESET))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief     Clears the TMR's interrupt pending bits.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     interrupt: specifies the TMR interrupts sources
 *            The parameter can be any combination following values:
 *            @arg TMR_INT_UPDATE: Timer update Interrupt source
 *            @arg TMR_INT_CH1: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH2: Timer Capture Compare 1 Interrupt source (TMR 2/3/21/22 only)
 *            @arg TMR_INT_CH3: Timer Capture Compare 3 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_CH4: Timer Capture Compare 4 Interrupt source (TMR 2/3 only)
 *            @arg TMR_INT_TRG: Timer Trigger Interrupt source           (TMR 2/3 only)
 *
 * @retval    None
 */
void TMR_ClearIntFlag(TMR_T* tmr,  uint16_t flag)
{
    tmr->STS = ~flag;
}

/*!
 * @brief     Configures the TMRx's DMA interface.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     address: DMA Base address.
 *            This parameter can be one of the following values:
 *            @arg TMR_DMA_BASE_CTRL1
 *            @arg TMR_DMA_BASE_CTRL2
 *            @arg TMR_DMA_BASE_SMCTRL
 *            @arg TMR_DMA_BASE_DIEN
 *            @arg TMR_DMA_BASE_STS
 *            @arg TMR_DMA_BASE_CEG
 *            @arg TMR_DMA_BASE_CCM1
 *            @arg TMR_DMA_BASE_CCM2
 *            @arg TMR_DMA_BASE_CCEN
 *            @arg TMR_DMA_BASE_CNT
 *            @arg TMR_DMA_BASE_PSC
 *            @arg TMR_DMA_BASE_AUTORLD
 *            @arg TMR_DMA_BASE_CC1
 *            @arg TMR_DMA_BASE_CC2
 *            @arg TMR_DMA_BASE_CC3
 *            @arg TMR_DMA_BASE_CC4
 *            @arg TMR_DMA_BASE_OPT
 *
 * @param     lenght: DMA Burst length. This parameter can be one value
 *            between: TMR_DMA_BURSTLENGTH_1TRANSFER and TMR_DMA_BURSTLENGTH_18TRANSFERS.
 *
 * @retval    None
 */
void TMR_ConfigDMA(TMR_T* tmr, TMR_DMA_BASE_T address, TMR_DMA_BURSTLENGTH_T lenght)
{
    tmr->DCTRL = (uint32_t)address | (uint32_t)lenght;
}

/*!
 * @brief     Enable TMRx Requests.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7
 *
 * @param     souces: specifies the TMR DMA souces
 *            The parameter can be any combination of following values:
 *            @arg TMR_DMA_SOURCE_UPDATE: TMR update DMA souces
 *            @arg TMR_DMA_SOURCE_CH1:    TMR Capture Compare 1 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH2:    TMR Capture Compare 2 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH3:    TMR Capture Compare 3 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH4:    TMR Capture Compare 4 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_TRG:    TMR Trigger DMA souces           (TMR 2/3 only)
 *
 * @retval    None
 *
 * @note
 */
void TMR_EnableDMASoure(TMR_T* tmr, uint16_t dmaSource)
{
    tmr->DIEN |= dmaSource;
}

/*!
 * @brief     Disable TMRx Requests.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7
 *
 * @param     souces: specifies the TMR DMA souces
 *            The parameter can be any combination of following values:
 *            @arg TMR_DMA_SOURCE_UPDATE: TMR update DMA souces
 *            @arg TMR_DMA_SOURCE_CH1:    TMR Capture Compare 1 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH2:    TMR Capture Compare 2 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH3:    TMR Capture Compare 3 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_CH4:    TMR Capture Compare 4 DMA souces (TMR 2/3 only)
 *            @arg TMR_DMA_SOURCE_TRG:    TMR Trigger DMA souces           (TMR 2/3 only)
 *
 * @retval    None
 *
 * @note
 */
void TMR_DisableDMASoure(TMR_T* tmr, uint16_t dmaSource)
{
    tmr->DIEN &= ~dmaSource;
}

/*!
 * @brief     Enable Capture Compare DMA source.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    None
 */
void TMR_EnableCCDMA(TMR_T* tmr)
{
    tmr->CTRL2_B.CCDSEL = ENABLE;
}

/*!
 * @brief     Disable Capture Compare DMA source.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    None
 */
void TMR_DisableCCDMA(TMR_T* tmr)
{
    tmr->CTRL2_B.CCDSEL = DISABLE;
}

/*!
 * @brief     Configures the TMRx internal Clock
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @retval    None
 */
void TMR_ConfigInternalClock(TMR_T* tmr)
{
    tmr->SMCTRL_B.SMFSEL = BIT_RESET;
}

/*!
 * @brief     Configures the TMRx Internal Trigger as External Clock
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     triggerSource: specifies the TMR trigger souces
 *            The parameter can be one of following values:
 *            @arg TMR_TRIGGER_SOURCE_ITR0: TMR Internal Trigger 0
 *            @arg TMR_TRIGGER_SOURCE_ITR1: TMR Internal Trigger 1
 *            @arg TMR_TRIGGER_SOURCE_ITR2: TMR Internal Trigger 2 (TMR2/3 only)
 *
 * @retval    None
 */
void TMR_ConfigITRxExternalClock(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource)
{
    TMR_SelectInputTrigger(tmr, triggerSource);
    tmr->SMCTRL_B.SMFSEL = 0x07;
}

/*!
 * @brief     Configures the TMRx  Trigger as External Clock
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     triggerSource: specifies the TMR trigger souces
 *            The parameter can be one of following values:
 *            @arg TMR_TRIGGER_SOURCE_TI1F_ED:  TI1 Edge Detector
 *            @arg TMR_TRIGGER_SOURCE_TI1FP1:   Filtered Timer Input 1
 *            @arg TMR_TRIGGER_SOURCE_TI2FP2:   Filtered Timer Input 2
 *
 * @param     icPolarity: specifies the TMR IC polarity
 *            The parameter can be one of following values:
 *            @arg TMR_IC_POLARITY_RISING:  TMR IC polarity rising
 *            @arg TMR_IC_POLARITY_FALLING: TMR IC polarity falling
 *
 * @param     icFilter: This parameter must be a value between 0x00 and 0x0F.
 *
 * @retval    None
 */
void TMR_ConfigTIxExternalClock(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource,
                                TMR_IC_POLARITY_T icPolarity, uint16_t icFilter)
{
    if (triggerSource == TMR_TRIGGER_SOURCE_TI2FP2)
    {
        TI2Config(tmr, icPolarity, TMR_IC_SELECTION_DIRECT_TI, icFilter);
    }
    else
    {
        TI1Config(tmr, icPolarity, TMR_IC_SELECTION_DIRECT_TI, icFilter);
    }

    TMR_SelectInputTrigger(tmr, triggerSource);
    tmr->SMCTRL_B.SMFSEL = 0x07;
}

/*!
 * @brief     Configures the External clock Mode1
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     prescaler: specifies the external Trigger Prescaler
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_PSC_OFF:  ETRP Prescaler OFF
 *            @arg TMR_EXTTRG_PSC_DIV2: ETRP frequency divided by 2
 *            @arg TMR_EXTTRG_PSC_DIV4: ETRP frequency divided by 4
 *            @arg TMR_EXTTRG_PSC_DIV8: ETRP frequency divided by 8
 *
 * @param     polarity: specifies the TMR IC polarity
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_POL_INVERTED:  Active low or falling edge active
 *            @arg TMR_EXTTGR_POL_NONINVERTED: Active high or rising edge active
 *
 * @param     filter: This parameter must be a value between 0x00 and 0x0F.
 *
 * @retval    None
 */
void TMR_ConfigETRClockMode1(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                             TMR_EXTTRG_POL_T polarity, uint16_t filter)
{
    uint16_t temp1 = 0;
    TMR_ConfigETR(tmr, prescaler, polarity, filter);

    temp1 = tmr->SMCTRL;

    temp1 &= 0xFFF8;
    temp1 |= 0x0007;

    temp1 &= 0xFF8F;
    temp1 |= 0x0070;

    tmr->SMCTRL = temp1;
}

/*!
 * @brief     Configures the External clock Mode2
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     prescaler: specifies the external Trigger Prescaler
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_PSC_OFF:  ETRP Prescaler OFF
 *            @arg TMR_EXTTRG_PSC_DIV2: ETRP frequency divided by 2
 *            @arg TMR_EXTTRG_PSC_DIV4: ETRP frequency divided by 4
 *            @arg TMR_EXTTRG_PSC_DIV8: ETRP frequency divided by 8
 *
 * @param     polarity: specifies the TMR IC polarity
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_POL_INVERTED:  Active low or falling edge active
 *            @arg TMR_EXTTGR_POL_NONINVERTED: Active high or rising edge active
 *
 * @param     filter: This parameter must be a value between 0x00 and 0x0F.
 *
 * @retval    None
 */
void TMR_ConfigETRClockMode2(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                             TMR_EXTTRG_POL_T polarity, uint16_t filter)
{
    TMR_ConfigETR(tmr, prescaler, polarity, filter);
    tmr->SMCTRL_B.ECEN = ENABLE;
}

/*!
 * @brief     Selects the Input Trigger source
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     triggerSource: specifies the Input Trigger source
 *            The parameter can be one of following values:
 *            @arg TMR_TRIGGER_SOURCE_ITR0: Internal Trigger 0
 *            @arg TMR_TRIGGER_SOURCE_ITR1: Internal Trigger 1
 *            @arg TMR_TRIGGER_SOURCE_ITR2: Internal Trigger 2 (TMR2/3 only)
 *            @arg TMR_TRIGGER_SOURCE_TI1F_ED: TI1 Edge Detector
 *            @arg TMR_TRIGGER_SOURCE_TI1FP1: Filtered Timer Input 1
 *            @arg TMR_TRIGGER_SOURCE_TI2FP2: Filtered Timer Input 2
 *            @arg TMR_TRIGGER_SOURCE_ETRF: External Trigger input (TMR2/3 only)
 *
 * @retval    None
 */
void TMR_SelectInputTrigger(TMR_T* tmr, TMR_TRIGGER_SOURCE_T triggerSource)
{
    tmr->SMCTRL_B.TRGSEL = BIT_RESET;
    tmr->SMCTRL_B.TRGSEL = triggerSource;
}

/*!
 * @brief     Selects the Trigger Output Mode.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 6 7 21 22
 *
 * @param     trgoSource: specifies the Trigger Output source
 *            The parameter can be one of following values:
 *            @arg TMR_TRGO_SOURCE_RESET
 *            @arg TMR_TRGO_SOURCE_ENABLE
 *            @arg TMR_TRGO_SOURCE_UPDATE
 *            @arg TMR_TRGO_SOURCE_OC1    (TMR2/3/21/22 only)
 *            @arg TMR_TRGO_SOURCE_OC1REF (TMR2/3/21/22 only)
 *            @arg TMR_TRGO_SOURCE_OC2REF (TMR2/3/21/22 only)
 *            @arg TMR_TRGO_SOURCE_OC3REF (TMR2/3 only)
 *            @arg TMR_TRGO_SOURCE_OC4REF (TMR2/3 only)
 *
 * @retval    None
 */
void TMR_SelectOutputTrigger(TMR_T* tmr, TMR_TRGO_SOURCE_T trgoSource)
{
    tmr->CTRL2_B.MMSEL = BIT_RESET;
    tmr->CTRL2_B.MMSEL = trgoSource;
}

/*!
 * @brief     Selects the Slave Mode.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     slaveMode: specifies the Timer Slave Mode.
 *            The parameter can be one of following values:
 *            @arg TMR_SLAVE_MODE_DISABLE
 *            @arg TMR_SLAVE_MODE_ECODER1
 *            @arg TMR_SLAVE_MODE_ECODER2
 *            @arg TMR_SLAVE_MODE_ECODER3
 *            @arg TMR_SLAVE_MODE_RESET
 *            @arg TMR_SLAVE_MODE_GATED
 *            @arg TMR_SLAVE_MODE_TRIGGER
 *            @arg TMR_SLAVE_MODE_EXTERNAL1
 *
 * @retval    None
 */
void TMR_SelectSlaveMode(TMR_T* tmr, TMR_SLAVE_MODE_T slaveMode)
{
    tmr->SMCTRL_B.SMFSEL = slaveMode;
}

/*!
 * @brief     Enable the Master Slave Mode
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @retval    None
 */
void TMR_EnableMasterSlaveMode(TMR_T* tmr)
{
    tmr->SMCTRL_B.MSMEN = ENABLE;
}

/*!
 * @brief     Disable the Master Slave Mode
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @retval    None
 */
void TMR_DisableMasterSlaveMode(TMR_T* tmr)
{
    tmr->SMCTRL_B.MSMEN = DISABLE;
}

/*!
 * @brief     Configures the TMRx External Trigger (ETR).
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     prescaler: specifies the external Trigger Prescaler
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_PSC_OFF:  ETRP Prescaler OFF
 *            @arg TMR_EXTTRG_PSC_DIV2: ETRP frequency divided by 2
 *            @arg TMR_EXTTRG_PSC_DIV4: ETRP frequency divided by 4
 *            @arg TMR_EXTTRG_PSC_DIV8: ETRP frequency divided by 8
 *
 * @param     polarity: specifies the TMR IC polarity
 *            The parameter can be one of following values:
 *            @arg TMR_EXTTRG_POL_INVERTED:  Active low or falling edge active
 *            @arg TMR_EXTTGR_POL_NONINVERTED: Active high or rising edge active
 *
 * @param     filter: This parameter must be a value between 0x00 and 0x0F.
 *
 * @retval    None
 */
void TMR_ConfigETR(TMR_T* tmr, TMR_EXTTRG_PSC_T prescaler,
                   TMR_EXTTRG_POL_T polarity, uint16_t filter)
{
    tmr->SMCTRL &= 0x00FF;
    tmr->SMCTRL_B.ETPCFG = prescaler;
    tmr->SMCTRL_B.ETPOL = polarity;
    tmr->SMCTRL_B.ETFCFG = filter;
}

/*!
 * @brief     Configures the Encoder Interface.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     encodeMode: specifies the Encoder Mode
 *            The parameter can be one of following values:
 *            @arg TMR_ENCODER_MODE_TI1   : Counter counts on TI1FP1 edge depending on TI2FP2 level.
 *            @arg TMR_ENCODER_MODE_TI2   : Counter counts on TI2FP2 edge depending on TI1FP1 level.
 *            @arg TMR_ENCODER_MODE_TI12  : Counter counts on both TI1FP1 and TI2FP2 edges depending
 *                                          on the level of the other input.
 * @param     ic1Polarity: specifies the TMR IC1 polarity
 *            The parameter can be one of following values:
 *            @arg TMR_IC_POLARITY_RISING:  TMR IC polarity rising
 *            @arg TMR_IC_POLARITY_FALLING: TMR IC polarity falling
 *
 * @param     ic2Polarity: specifies the TMR IC2 polarity
 *            The parameter can be one of following values:
 *            @arg TMR_IC_POLARITY_RISING:  TMR IC polarity rising
 *            @arg TMR_IC_POLARITY_FALLING: TMR IC polarity falling
 *
 * @retval    None
 */
void TMR_ConfigEncodeInterface(TMR_T* tmr, TMR_ENCODER_MODE_T encodeMode, TMR_IC_POLARITY_T ic1Polarity,
                               TMR_IC_POLARITY_T ic2Polarity)
{
    tmr->SMCTRL_B.SMFSEL = BIT_RESET;
    tmr->SMCTRL_B.SMFSEL = encodeMode;

    /** Select the Capture Compare 1 and the Capture Compare 2 as input */
    tmr->CCM1_CAPTURE_B.CC1SEL = BIT_RESET;
    tmr->CCM1_CAPTURE_B.CC2SEL = BIT_RESET;
    tmr->CCM1_CAPTURE_B.CC1SEL = BIT_SET;
    tmr->CCM1_CAPTURE_B.CC2SEL = BIT_SET;

    tmr->CCEN &= 0xFFFFFF55;
    tmr->CCEN |= (ic1Polarity | ic2Polarity<<4);
}

/*!
 * @brief     Enables Hall sensor interface.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    None
 */
void TMR_EnableHallSensor(TMR_T* tmr)
{
    tmr->CTRL2_B.TI1SEL = ENABLE;
}

/*!
 * @brief     Disable Hall sensor interface.
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @retval    None
 */
void TMR_DisableHallSensor(TMR_T* tmr)
{
    tmr->CTRL2_B.TI1SEL = DISABLE;
}

/*!
 * @brief     Configures the TMR2 TMR3 TMR21 and TMR22 Remapping input capabilities.
 *
 * @param     tmr: The TMRx can be 2 3 21 22
 *
 * @param     remap: specifies the MR input remapping source.
 *            The parameter can be one of the following values:
 *            @arg TMR2_ETR_GPIO            : TMR2 ETR input is connected to GPIO
 *            @arg TMR2_ETR_HSI16           : TMR2 ETR input is connected to HSI16(When RCM_CTRL[HSI16OUTEN]=1)
 *            @arg TMR2_ETR_HSI48           : TMR2 ETR input is connected to HSI48
 *            @arg TMR2_ETR_LSE             : TMR2 ETR input is connected to LSE
 *            @arg TMR2_ETR_COMP2_OUT       : TMR2 ETR input is connected to COMP2_OUT
 *            @arg TMR2_ETR_COMP1_OUT       : TMR2 ETR input is connected to COMP1_OUT
 *            @arg TMR2_TI4_GPIO            : TMR2 T11 input is connected to GPIO
 *            @arg TMR2_TI4_COMP2_OUT       : TMR2 T11 input is connected to COMP2_OUT
 *            @arg TMR2_TI4_COMP1_OUT       : TMR2 T11 input is connected to COMP1_OUT
 *            @arg TMR3_ETR_PE2_PD2         : TMR3 ETR input is connected to PE2 or PD2
 *            @arg TMR3_ETR_HSI48DIV6       : TMR3 ETR input is connected to HSI48 divided by 6(When RCM_CTRL[HSI48DIVEN]=1)
 *            @arg TMR3_TI1_USB_SOF         : TMR3 T11 input is connected to USB_SOF
 *            @arg TMR3_TI1_PE3_PA6_PC6_PB4 : TMR3 T11 input is connected to PE3 PA6 PC6 or PB4
 *            @arg TMR3_TI2_PB5_TMR22_CH2   : TMR3 remap on PB5, TMR22_CH2 selected
 *            @arg TMR3_TI2_PB5_TMR3_CH2    : TMR3 remap on PB5, TMR3_CH2 selected
 *            @arg TMR3_TI4_PC9_USB_NOE     : TMR3 remap on PC9, USB_NOE selected
 *            @arg TMR3_TI4_PC9_TMR3_CH4    : TMR3 remap on PC9, TMR3_CH4 selected
 *            @arg TMR21_ETR_GPIO           : TMR21 ETR input is connected to GPIO
 *            @arg TMR21_ETR_COMP2_OUT      : TMR21 ETR input is connected to COMP2_OUT
 *            @arg TMR21_ETR_COMP1_OUT      : TMR21 ETR input is connected to COMP1_OUT
 *            @arg TMR21_ETR_LSECLK         : TMR21 ETR input is connected to LSECLK
 *            @arg TMR21_TI1_GPIO           : TMR21 T11 input is connected to GPIO
 *            @arg TMR21_TI1_RTC_WKPINT     : TMR21 T11 input is connected to RTC wake up interrupt
 *            @arg TMR21_TI1_HSE_RTC        : TMR21 T11 input is connected to HSE_RTC
 *            @arg TMR21_TI1_MSICLK         : TMR21 T11 input is connected to MSICLK
 *            @arg TMR21_TI1_LSECLK         : TMR21 T11 input is connected to LSECLK
 *            @arg TMR21_TI1_LSICLK         : TMR21 T11 input is connected to LSICLK
 *            @arg TMR21_TI1_COMP1_OUT      : TMR21 T11 input is connected to COMP1_OUT
 *            @arg TMR21_TI1_MCOCLK         : TMR21 T11 input is connected to MCOCLK
 *            @arg TMR21_TI2_GPIO           : TMR21 T12 input is connected to GPIO
 *            @arg TMR21_TI2_COMP2_OUT      : TMR21 T12 input is connected to COMP2_OUT
 *            @arg TMR22_ETR_GPIO           : TMR22 ETR input is connected to GPIO
 *            @arg TMR22_ETR_COMP2_OUT      : TMR22 ETR input is connected to COMP2_OUT
 *            @arg TMR22_ETR_COMP1_OUT      : TMR22 ETR input is connected to COMP1_OUT
 *            @arg TMR22_ETR_LSECLK         : TMR22 ETR input is connected to LSECLK
 *            @arg TMR22_TI1_GPIO           : TMR22 T11 input is connected to GPIO
 *            @arg TMR22_TI1_COMP2_OUT      : TMR22 T11 input is connected to COMP2_OUT
 *            @arg TMR22_TI1_COMP1_OUT      : TMR22 T11 input is connected to COMP1_OUT
 *
 * @retval    None
 */
void TMR_ConfigRemap(TMR_T* tmr, uint32_t remap)
{
    tmr->OPT = remap;
}

/*!
 * @brief     Configure the TI1 as Input
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     icPolarity: TMR_IC_POLARITY_T
 *
 * @param     icSelection: TMR_IC_SELECTION_T
 *
 * @param     icFilter: This parameter must be a value between 0x00 and 0x0F
 *
 * @retval    None
 */
static void TI1Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter)
{
    uint16_t tmpchctrl = 0;

    tmr->CCEN_B.CC1EN = BIT_RESET;

    tmr->CCM1_CAPTURE_B.CC1SEL = BIT_RESET;
    tmr->CCM1_CAPTURE_B.IC1F = BIT_RESET;
    tmr->CCM1_CAPTURE_B.CC1SEL = icSelection;
    tmr->CCM1_CAPTURE_B.IC1F = icFilter;

    tmr->CCEN_B.CC1POL = BIT_RESET;
    tmr->CCEN_B.CC1NPOL = BIT_RESET;
    tmr->CCEN_B.CC1EN = BIT_SET;
    tmpchctrl = tmr->CCEN;
    tmpchctrl |= icPolarity;
    tmr->CCEN = tmpchctrl;
}

/*!
 * @brief     Configure the TI2 as Input
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3 21 22
 *
 * @param     icPolarity: TMR_IC_POLARITY_T
 *
 * @param     icSelection: TMR_IC_SELECTION_T
 *
 * @param     icFilter: This parameter must be a value between 0x00 and 0x0F
 *
 * @retval    None
 */
static void TI2Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter)
{
    uint16_t tmpchctrl = 0;

    tmr->CCEN_B.CC2EN = BIT_RESET;

    tmr->CCM1_CAPTURE_B.CC2SEL = BIT_RESET;
    tmr->CCM1_CAPTURE_B.IC2F = BIT_RESET;
    tmr->CCM1_CAPTURE_B.CC2SEL = icSelection;
    tmr->CCM1_CAPTURE_B.IC2F = icFilter;

    tmr->CCEN_B.CC2POL = BIT_RESET;
    tmr->CCEN_B.CC2NPOL = BIT_RESET;
    tmr->CCEN_B.CC2EN = BIT_SET;
    tmpchctrl = tmr->CCEN;
    tmpchctrl |= icPolarity;
    tmr->CCEN = tmpchctrl;
}

/*!
 * @brief     Configure the TI3 as Input
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     icPolarity: pointer to a TMR_IC_POLARITY_T
 *
 * @param     icSelection: pointer to a TMR_IC_SELECTION_T
 *
 * @param     icFilter: This parameter must be a value between 0x00 and 0x0F
 *
 * @retval    None
 */
static void TI3Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter)
{
    uint16_t tmpchctrl = 0;

    tmr->CCEN_B.CC3EN = BIT_RESET;

    tmr->CCM2_CAPTURE_B.CC3SEL = BIT_RESET;
    tmr->CCM2_CAPTURE_B.IC3F = BIT_RESET;
    tmr->CCM2_CAPTURE_B.CC3SEL = icSelection;
    tmr->CCM2_CAPTURE_B.IC3F = icFilter;

    tmr->CCEN_B.CC3POL = BIT_RESET;
    tmr->CCEN_B.CC3NPOL = BIT_RESET;
    tmr->CCEN_B.CC3EN = BIT_SET;
    tmpchctrl = tmr->CCEN;
    tmpchctrl |= icPolarity;
    tmr->CCEN = tmpchctrl;
}

/*!
 * @brief     Configure the TI4 as Input
 *
 * @param     tmr: Select TMRx peripheral, The x can be 2 3
 *
 * @param     icPolarity: pointer to a TMR_IC_POLARITY_T
 *
 * @param     icSelection: pointer to a TMR_IC_SELECTION_T
 *
 * @param     icFilter: This parameter must be a value between 0x00 and 0x0F
 *
 * @retval    None
 */
static void TI4Config(TMR_T* tmr, uint16_t icPolarity, uint16_t icSelection, uint16_t icFilter)
{
    uint16_t tmpchctrl = 0;

    tmr->CCEN_B.CC4EN = BIT_RESET;

    tmr->CCM2_CAPTURE_B.CC4SEL = BIT_RESET;
    tmr->CCM2_CAPTURE_B.IC4F = BIT_RESET;
    tmr->CCM2_CAPTURE_B.CC4SEL = icSelection;
    tmr->CCM2_CAPTURE_B.IC4F = icFilter;

    tmr->CCEN_B.CC4POL = BIT_RESET;
    tmr->CCEN_B.CC4NPOL = BIT_RESET;
    tmr->CCEN_B.CC4EN = BIT_SET;
    tmpchctrl = tmr->CCEN;
    tmpchctrl |= icPolarity;
    tmr->CCEN = tmpchctrl;
}

/**@} end of group TMR_Fuctions*/
/**@} end of group TMR_Driver*/
/**@} end of group Peripherals_Library*/
