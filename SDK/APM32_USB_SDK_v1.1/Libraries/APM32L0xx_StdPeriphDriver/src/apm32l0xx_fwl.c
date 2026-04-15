/*!
 * @file        apm32l0xx_fwl.c
 *
 * @brief       This file contains all the functions for the FWL peripheral
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

#include "apm32l0xx_fwl.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup FWL_Driver FWL Driver
  @{
*/

/** @addtogroup FWL_Fuctions Fuctions
  @{
*/

/*!
 * @brief       Config the firewall peripheral according to the specified parameters in the fwlConfig
 *
 * @param       fwlConfig:  Pointer to a FWL_Config_T structure that
 *                          contains the configuration information for the firewall peripheral
 *
 * @retval      None
 */
void FWL_Config(FWL_Config_T* fwlConfig)
{
    FWL->CSSTADD    = fwlConfig->codeStartAddress;
    FWL->CSL        = fwlConfig->codeLength;
    FWL->NVDSSTADD  = fwlConfig->nonVDataStartAddress;
    FWL->NVDSL      = fwlConfig->nonVDataLength;
    FWL->VDSSTADD   = fwlConfig->vDataStartAddress;
    FWL->VDSL       = fwlConfig->vDataLength;
}

/*!
 * @brief       Fills each fwlConfig member with its default value
 *
 * @param       fwlConfig:  Pointer to a FWL_Config_T structure which will be initialized
 *
 * @retval      None
 */
void FWL_ConfigStructInit(FWL_Config_T* fwlConfig)
{
    fwlConfig->codeStartAddress      = 0x00000000;
    fwlConfig->codeLength            = 0x00000000;
    fwlConfig->nonVDataStartAddress  = 0x00000000;
    fwlConfig->nonVDataLength        = 0x00000000;
    fwlConfig->vDataStartAddress     = 0x00000000;
    fwlConfig->vDataLength           = 0x00000000;
}

/*!
 * @brief       Enable the firewall access
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_Enable(void)
{
    SYSCFG->PMCFG1_B.FWLDIS = BIT_RESET;
}

/*!
 * @brief       Enable the firewall pre arm
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_EnablePreArm(void)
{
    FWL->CFG_B.CODESEL = BIT_SET;
}

/*!
 * @brief       Disable the firewall pre arm
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_DisablePreArm(void)
{
    FWL->CFG_B.CODESEL = BIT_RESET;
}

/*!
 * @brief       Enable the firewall volatile data segment share
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_EnableVolatileDataShare(void)
{
    FWL->CFG_B.VDSSHR = BIT_SET;
}

/*!
 * @brief       Disable the firewall volatile data segment share
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_DisableVolatileDataShare(void)
{
    FWL->CFG_B.VDSSHR = BIT_RESET;
}

/*!
 * @brief       Enable the firewall volatile data segment execute
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_EnableVolatileDataExecute(void)
{
    FWL->CFG_B.VDSE = BIT_SET;
}

/*!
 * @brief       Disable the firewall volatile data segment execute
 *
 * @param       None
 *
 * @retval      None
 */
void FWL_DisableVolatileDataExecute(void)
{
    FWL->CFG_B.VDSE = BIT_RESET;
}

/*!
 * @brief       Reads the Specifies firewall peripheral mode configuration register flag
 *
 * @param       None
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t FWL_ReadPeripheralFlag(void)
{
    uint32_t status;

    status = SYSCFG->PMCFG1_B.FWLDIS;

    if (status != (uint32_t)RESET)
    {
        return RESET;
    }

    return SET;
}

/*!
 * @brief       Clear the specified firewall peripheral mode configuration register flag
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        enable firewall
 */
void FWL_ClearPeripheralFlag(void)
{
    SYSCFG->PMCFG1_B.FWLDIS = BIT_RESET;
}

/*!
 * @brief       Reads the Specifies firewall configuration register flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be one of the following values:
 *                          @arg FWL_FLAG_CODESEL
 *                          @arg FWL_FLAG_VDSSHR
 *                          @arg FWL_FLAG_VDSE
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint8_t FWL_ReadStatusFlag(FWL_FLAG_T flag)
{
    uint32_t status;

    status = FWL->CFG;

    if ((status & flag) != (uint32_t)RESET)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified firewall configuration register flag
 *
 * @param       flag:   Specifies the flag to check
 *                      This parameter can be any combination of the following values:
 *                          @arg FWL_FLAG_CODESEL
 *                          @arg FWL_FLAG_VDSSHR
 *                          @arg FWL_FLAG_VDSE
 *
 * @retval      None
 */
void FWL_ClearStatusFlag(FWL_FLAG_T flag)
{
    FWL->CFG &= (uint32_t)~flag;

}

/**@} end of group FWL_Fuctions*/
/**@} end of group FWL_Driver*/
/**@} end of group Peripherals_Library*/
