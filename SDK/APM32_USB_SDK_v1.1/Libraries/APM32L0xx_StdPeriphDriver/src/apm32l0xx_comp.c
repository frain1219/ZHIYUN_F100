/*!
 * @file        apm32l0xx_comp.c
 *
 * @brief       This file provides all the COMP firmware functions
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

#include "apm32l0xx_comp.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup COMP_Driver COMP Driver
  @{
*/

/** @addtogroup COMP_Fuctions Fuctions
  @{
*/

/*!
 * @brief    Reset COMP peripheral registers to their default values.
 *
 * @param    compSelect: selects the comparator to be locked
 *                       This parameter can be one of the following values:
 *                       @arg COMP_SELECT_COMP1
 *                       @arg COMP_SELECT_COMP2
 *
 * @retval   None
 *
 * @note     Deinitialization can't be performed if the COMP configuration is locked.
 *           To unlock the configuration, perform a system reset.
 */
void COMP_Reset(COMP_SELECT_T compSelect)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        COMP->COMP1_CSTS = ((uint32_t)0x00000000);
    }
    else
    {
        COMP->COMP2_CSTS = ((uint32_t)0x00000000);
    }
}

/*!
 * @brief    Configs the COMP peripheral according to the specified parameters
 *           in COMP_Config_T
 *
 * @param    compSelect: the selected comparator.
 *                       This parameter can be one of the following values:
 *                       @arg COMP_SELECT_COMP1: COMP1 selected
 *                       @arg COMP_SELECT_COMP2: COMP2 selected
 *
 * @param    compConfig: pointer to an COMP_Config_T structure that contains
 *           the configuration information for the specified COMP peripheral.
 *
 * @retval   None
 *
 * @note     If the selected comparator is locked, initialization can't be performed.
 *           To unlock the configuration, perform a system reset.
 *
 */
void COMP_Config(COMP_SELECT_T compSelect, COMP_Config_T* compConfig)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        COMP->COMP1_CSTS_B.INMCCFG1   = compConfig->invertingInput;
        COMP->COMP1_CSTS_B.WMODESEL1  = compConfig->windowSelect;
        COMP->COMP1_CSTS_B.LPTMRIN1_1 = compConfig->outputPropagate1;
        COMP->COMP1_CSTS_B.POLCFG1    = compConfig->outputPol;
    }
    else
    {
        COMP->COMP2_CSTS_B.SPEEDM2    = compConfig->speedSelecct;
        COMP->COMP2_CSTS_B.INMCCFG2   = compConfig->invertingInput;
        COMP->COMP2_CSTS_B.INPCCFG2   = compConfig->inputConnect;
        COMP->COMP2_CSTS_B.LPTMRIN2_1 = compConfig->outputPropagate1;
        COMP->COMP2_CSTS_B.LPTMRIN2_2 = compConfig->outputPropagate2;
        COMP->COMP2_CSTS_B.POLCFG2    = compConfig->outputPol;
    }
}

/*!
 * @brief    Fills each compConfig member with initial value value.
 *
 * @param    compConfig: pointer to an COMP_InitTypeDef structure which will
 *           be initialized.
 *
 * @retval   None
 */
void COMP_ConfigStructInit(COMP_SELECT_T compSelect, COMP_Config_T* compConfig)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        compConfig->invertingInput     = COMP_INVERTING_INPUT_VREFINT;
        compConfig->windowSelect       = COMP_WINDOW_PA1;
        compConfig->outputPropagate1   = COMP_OUTPUTPROPAGATE1_GATE;
        compConfig->outputPol          = COMP_OUTPUTPOL_NONINVERTED;
    }
    else
    {
        compConfig->speedSelecct       = COMP_SPEED_LOW;
        compConfig->invertingInput     = COMP_INVERTING_INPUT_VREFINT;
        compConfig->inputConnect       = COMP_INPUT_CONNECT_PA3;
        compConfig->outputPropagate1   = COMP_OUTPUTPROPAGATE1_GATE;
        compConfig->outputPropagate2   = COMP_OUTPUTPROPAGATE2_GATE;
        compConfig->outputPol          = COMP_OUTPUTPOL_NONINVERTED;
    }
}

/*!
 * @brief    Enable the COMP peripheral.
 *
 * @param    compSelect: the selected comparator.
 *                       This parameter can be one of the following values:
 *                       @arg COMP_SELECT_COMP1: COMP1 selected
 *                       @arg COMP_SELECT_COMP2: COMP2 selected
 *
 * @retval   None
 *
 * @note     If the selected comparator is locked, enable can't be performed.
 *           To unlock the configuration, perform a system reset.
 */
void COMP_Enable(COMP_SELECT_T compSelect)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        COMP->COMP1_CSTS_B.EN1 = SET;
    }
    else
    {
        COMP->COMP2_CSTS_B.EN2 = SET;
    }
}

/*!
 * @brief    Disable the COMP peripheral.
 *
 * @param    compSelect: the selected comparator.
 *                       This parameter can be one of the following values:
 *                       @arg COMP_SELECT_COMP1: COMP1 selected
 *                       @arg COMP_SELECT_COMP2: COMP2 selected
 *
 * @retval   None
 *
 * @note     If the selected comparator is locked, disable can't be performed.
 *           To unlock the configuration, perform a system reset.
 */
void COMP_Disable(COMP_SELECT_T compSelect)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        COMP->COMP1_CSTS_B.EN1 = RESET;
    }
    else
    {
        COMP->COMP2_CSTS_B.EN2 = RESET;
    }
}

/*!
 * @brief    Return the output level (high or low) of the selected comparator.
 *
 * @param    compSelect: the selected comparator.
 *                       This parameter can be one of the following values:
 *                       @arg COMP_SELECT_COMP1: COMP1 selected
 *                       @arg COMP_SELECT_COMP2: COMP2 selected
 *
 * @retval   Returns the selected comparator output level: low or high.
 */
uint32_t COMP_ReadOutPutLevel(COMP_SELECT_T compSelect)
{
    uint32_t compOut = 0;

    if(compSelect == COMP_SELECT_COMP1)
    {
        compOut = COMP->COMP1_CSTS_B.OUTVAL1;
    }
    else
    {
        compOut = COMP->COMP2_CSTS_B.OUTVAL2;
    }

    return compOut;
}

/*!
 * @brief    Lock the selected comparator configuration.
 *
 * @param    compSelect: selects the comparator to be locked
 *                       This parameter can be one of  the following values:
 *                       @arg COMP_SELECT_COMP1: COMP1 configuration is locked.
 *                       @arg COMP_SELECT_COMP2: COMP2 configuration is locked.
 *
 * @retval   None
 */
void COMP_ConfigLOCK(COMP_SELECT_T compSelect)
{
    if(compSelect == COMP_SELECT_COMP1)
    {
        COMP->COMP1_CSTS_B.LOCK1 = SET;
    }
    else
    {
        COMP->COMP2_CSTS_B.LOCK2 = SET;
    }
}

/**@} end of group COMP_Fuctions*/
/**@} end of group COMP_Driver*/
/**@} end of group Peripherals_Library*/
