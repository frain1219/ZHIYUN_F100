/*!
 * @file        apm32l0xx_aes.c
 *
 * @brief       This file provides all the AES firmware functions
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

#include "apm32l0xx_aes.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup AES_Driver AES Driver
  @{
*/

/** @addtogroup AES_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Reset AES
 *
 * @param     None
 *
 * @retval    None
 */
void AES_Reset(void)
{
    RCM_EnableAHBPeriphReset(RCM_AHB_PERIPH_CRYP);
    RCM_DisableAHBPeriphReset(RCM_AHB_PERIPH_CRYP);
}

/*!
 * @brief     Configure AES
 *
 * @param     aesConfig: pointer to a AES_Config_T structure
 *
 * @retval    None
 */
void AES_Config(AES_Config_T* aesConfig)
{
    AES->CTRL_B.DATASEL  = aesConfig->dataType;
    AES->CTRL_B.CHAINSEL = aesConfig->chainMode;
    AES->CTRL_B.AESSEL   = aesConfig->modeSlect;
}

/*!
 * @brief     Default the AES_ConfigStructInit member value.
 *
 * @param     crcyConfig: pointer to a AES_Config_T structure
 *
 * @retval    None
 */
void AES_ConfigStructInit(AES_Config_T* aesConfig)
{
    aesConfig->dataType  = AES_DATATYPE_32B;
    aesConfig->chainMode = AES_CHMOD_ECB;
    aesConfig->modeSlect = AES_OPERATING_MODE_ENCRYPT;
}

/*!
 * @brief     Configure AES Keys
 *
 * @param     keyConfig: pointer to a AES_KeyConfig_T structure.
 *
 * @retval    None
 */
void AES_ConfigKey(AES_KeyConfig_T* keyConfig)
{
    AES->KEY0 = keyConfig->key0;
    AES->KEY1 = keyConfig->key1;
    AES->KEY2 = keyConfig->key2;
    AES->KEY3 = keyConfig->key3;
}

/*!
 * @brief     Default the AES_KeyConfig_T member value.
 *
 * @param     keyConfig: pointer to a AES_KeyConfig_T structure.
 *
 * @retval    None
 */
void AES_ConfigKeyStructInit(AES_KeyConfig_T* keyConfig)
{
    keyConfig->key0 = 0;
    keyConfig->key1 = 0;
    keyConfig->key2 = 0;
    keyConfig->key3 = 0;
}

/*!
 * @brief     Configure AES Initialization Vectors
 *
 * @param     IVConfig: pointer to a AES_INIVConfig_T structure.
 *
 * @retval    None
 */
void AES_ConfigINIV(AES_INIVConfig_T* INIVConfig)
{
    AES->INIV0 = INIVConfig->INIV0;
    AES->INIV1 = INIVConfig->INIV1;
    AES->INIV2 = INIVConfig->INIV2;
    AES->INIV3 = INIVConfig->INIV3;
}

/*!
 * @brief     Default the AES_ConfigINIVStructInit member value.
 *
 * @param     keyConfig: pointer to a AES_INIVConfig_T structure.
 *
 * @retval    None
 */
void AES_ConfigINIVStructInit(AES_INIVConfig_T* INIVConfig)
{
    INIVConfig->INIV0 = 0;
    INIVConfig->INIV1 = 0;
    INIVConfig->INIV2 = 0;
    INIVConfig->INIV3 = 0;
}

/*!
 * @brief     Enables the AES peripheral.
 *
 * @param     None
 *
 * @retval    None
 */
void AES_Enable(void)
{
    AES->CTRL_B.AESEN = SET;
}

/*!
 * @brief     Disable the AES peripheral.
 *
 * @param     None
 *
 * @retval    None
 */
void AES_Disable(void)
{
    AES->CTRL_B.AESEN = RESET;
}

/*!
 * @brief     Writes data in the Data Input register.
 *
 * @param     Data: data to write in Data Input register
 *
 * @retval    None
 *
 */
void AES_InData(uint32_t data)
{
    AES->DATAIN = data;
}

/*!
 * @brief     Returns the last data into the output FIFO.
 *
 * @param     None
 *
 * @retval    Last data into the output FIFO.
 */
uint32_t AES_OutData(void)
{
    return AES->DATAOUT;
}

/*!
 * @brief     Enable the AES DMA.
 *
 * @param     dmaReq: AES DMA request.
 *            This parameter can be one of the following values:
 *              @arg AES_DMAREQ_IN:  DMA Input Enable.
 *              @arg AES_DMAREQ_OUT: DMA Output Enable.
 *
 * @retval    None
 */
void AES_EnableDMA(AES_DMAREQ_T dmaReq)
{
    AES->CTRL |= dmaReq;
}

/*!
 * @brief     Disable the AES DMA.
 *
 * @param     dmaReq: AES DMA request.
 *            This parameter can be one of the following values:
 *              @arg AES_DMAREQ_IN:  DMA Input Enable.
 *              @arg AES_DMAREQ_OUT: DMA Output Enable.
 *
 * @retval    None
 */
void AES_DisableDMA(AES_DMAREQ_T dmaReq)
{
    AES->CTRL &= ~dmaReq;
}

/*!
 * @brief     Enable the AES Interrupt.
 *
 * @param     interrupt: specifies the AES interrupt sources
 *            This parameter can be any combination of the following values:
 *              @AES_INT_CCMP: Computation Complete interrupt
 *              @AES_INT_ERR:  Error interrupt(Write error and Read error)
 *
 * @retval    None
 */
void AES_EnableInterrupt(uint32_t interrupt)
{
    AES->CTRL |= interrupt;
}

/*!
 * @brief     Disable the AES Interrupt.
 *
 * @param     interrupt: specifies the AES interrupt sources
 *            This parameter can be any combination of the following values:
 *              @AES_INT_CCMP: Computation Complete interrupt
 *              @AES_INT_ERR:  Error interrupt(Write error and Read error)
 *
 * @retval    None
 */
void AES_DisableInterrupt(uint32_t interrupt)
{
    AES->CTRL &= ~interrupt;
}

/*!
 * @brief     Reads the AES flag.
 *
 * @param     flag: specifies the flag to check.
 *            This parameter can be one of the following values:
 *              @arg AES_FLAG_CCMPFLG:  Computation Complete Flag.
 *              @arg AES_FLAG_RDERR:    Read Error Flag.
 *              @arg AES_FLAG_WERR:     Write Error Flag
 *
 * @retval    SET or RESET.
 */
uint8_t AES_ReadStatusFlag(AES_FLAG_T flag)
{
       return (AES->STS & flag) ? SET : RESET;
}

/*!
 * @brief     Clear the AES status Flag.
 *
 * @param     flag: specifies the AES interrupt sources
 *                  This parameter can be any combination of the following values:
 *                  @AES_FLAG_CLR_CCMP: Computation Complete interrupt
 *                  @AES_FLAG_CLR_ERR:  Error interrupt(Write error and Read error)
 *
 * @retval    None
 */
void AES_ClearStatusFlag(uint32_t flag)
{
    AES->CTRL |= flag;
}

/*!
 * @brief     Returns the AES peripheral Cmd
 *
 * @param     None
 *
 * @retval    ENABLE or DISABLE
 */
uint8_t AES_ReadCmdStatus(void)
{
    return (AES->CTRL_B.AESEN);
}

/**@} end of group AES_Fuctions*/
/**@} end of group AES_Driver*/
/**@} end of group Peripherals_Library*/
