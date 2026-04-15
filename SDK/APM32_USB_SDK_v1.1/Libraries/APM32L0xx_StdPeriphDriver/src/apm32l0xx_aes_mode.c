/*!
 * @file        apm32l0xx_aes_mode.c
 *
 * @brief       This file provides high level functions to encrypt and decrypt,
 *              Input message using AES in ECB/CBC/CTR chaining modes.
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
 * @brief     Encrypt and decrypt using AES in ECB Chaining mode
 *
 * @param     mode: Pointer to a AES_MODE_T structure.
 *            This parameter can be one of the following values:
 *                @arg AES_MODE_ENCRYPT: Encryption
 *                @arg AES_MODE_DECRYPT: Decryption
 *
 * @param     key:     Key used for ECB Chaining mode.
 * @param     input:   Input buffer.
 * @param     length:  length of the Input buffer, must be a multiple of 16.
 * @param     output:  Output buffer.
 *
 * @retval    SUCCESS or ERROR
 */
uint8_t AES_ECB(AES_MODE_T mode, uint32_t length,
                   uint8_t *key,
                   uint8_t *input, uint8_t *output)
 {
    AES_Config_T    aesConfig;
    AES_KeyConfig_T keyConfig;

    uint32_t keyAddr    = (uint32_t)key;
    uint32_t inputAddr  = (uint32_t)input;
    uint32_t outputAddr = (uint32_t)output;

    uint32_t i = 0;
    uint32_t flag = 0;
    uint32_t status = SUCCESS;
    __IO uint32_t counter = 0;

    AES_ConfigStructInit(&aesConfig);

    keyConfig.key0  = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key1  = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key2  = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key3  = __REV(*(uint32_t*)(keyAddr));

    if(mode == AES_MODE_ENCRYPT)
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_ENCRYPT;
    }
    else
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_EKEY_DECRYPT;
    }

    AES_Disable();
    aesConfig.chainMode = AES_CHMOD_ECB;
    aesConfig.dataType  = AES_DATATYPE_8B;

    AES_Config(&aesConfig);
    AES_ConfigKey(&keyConfig);
    AES_Enable();

    for(i=0; i<length; i+=16)
    {
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;

        counter = 0;
        do
        {
            flag = AES_ReadStatusFlag(AES_FLAG_CCMPFLG);
            counter++;
        }
        while((counter != 0x00010000) && (flag != SET));

        if(flag == RESET)
        {
            status = ERROR;
        }
        else
        {
            AES_ClearStatusFlag(AES_FLAG_CLR_CCMP);
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
        }
    }
    AES_Disable();
    return status;
}

/*!
 * @brief     Encrypt and decrypt using AES in CBC Chaining mode.
 *
 * @param     mode: Pointer to a AES_MODE_T structure.
 *            This parameter can be one of the following values:
 *                @arg AES_MODE_ENCRYPT: Encryption
 *                @arg AES_MODE_DECRYPT: Decryption
 *
 * @param     key:     Key used for CBC Chaining mode.
 * @param     INIV:    Initialisation Vectors used for CBC Chaining mode.
 * @param     input:   Input buffer.
 * @param     length:  length of the Input buffer, must be a multiple of 16.
 * @param     output:  Output buffer.
 *
 * @retval    SUCCESS or ERROR
 */
uint8_t AES_CBC(AES_MODE_T mode, uint32_t length,
                   uint8_t *key, uint8_t INIV[16],
                   uint8_t *input, uint8_t *output)
{
    AES_Config_T     aesConfig;
    AES_KeyConfig_T  keyConfig;
    AES_INIVConfig_T INIVConfig;

    uint32_t keyAddr    = (uint32_t)key;
    uint32_t inputAddr  = (uint32_t)input;
    uint32_t outputAddr = (uint32_t)output;
    uint32_t INIVAddr   = (uint32_t)INIV;

    uint32_t i = 0;
    uint32_t flag = 0;
    uint32_t status = SUCCESS;
    __IO uint32_t counter = 0;

    AES_ConfigStructInit(&aesConfig);


    keyConfig.key0   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key1   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key2   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key3   = __REV(*(uint32_t*)(keyAddr));

    INIVConfig.INIV0  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV1  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV2  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV3  = __REV(*(uint32_t*)(INIVAddr));

    if(mode == AES_MODE_ENCRYPT)
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_ENCRYPT;
    }
    else
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_EKEY_DECRYPT;
    }

    AES_Disable();
    aesConfig.chainMode = AES_CHMOD_CBC;
    aesConfig.dataType  = AES_DATATYPE_8B;

    AES_Config(&aesConfig);
    AES_ConfigKey(&keyConfig);
    AES_ConfigINIV(&INIVConfig);
    AES_Enable();

    for(i=0; i<length; i+=16)
    {
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;

        counter = 0;
        do
        {
            flag = AES_ReadStatusFlag(AES_FLAG_CCMPFLG);
            counter++;
        }
        while((counter != 0x00010000) && (flag != SET));

        if(flag == RESET)
        {
            status = ERROR;
        }
         else
        {
            AES_ClearStatusFlag(AES_FLAG_CLR_CCMP);
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
        }
    }
    AES_Disable();
    return status;
}

/*!
 * @brief     Encrypt and decrypt using AES in CTR Mode
 *
 * @param     mode: Pointer to a AES_MODE_T structure.
 *            This parameter can be one of the following values:
 *                @arg AES_MODE_ENCRYPT: Encryption
 *                @arg AES_MODE_DECRYPT: Decryption
 *
 * @param     key:     Key used for AES algorithm.
 * @param     INIV:    Initialisation Vectors used for AES Chaining mode.
 * @param     input:   Input buffer.
 * @param     length:  length of the Input buffer, must be a multiple of 16.
 * @param     output:  Output buffer.
 *
 * @retval    SUCCESS or ERROR
 */
uint8_t AES_CTR(AES_MODE_T mode, uint32_t length,
                   uint8_t *key, uint8_t INIV[16],
                   uint8_t *input, uint8_t *output)
{
    AES_Config_T     aesConfig;
    AES_KeyConfig_T  keyConfig;
    AES_INIVConfig_T INIVConfig;

    uint32_t keyAddr    = (uint32_t)key;
    uint32_t inputAddr  = (uint32_t)input;
    uint32_t outputAddr = (uint32_t)output;
    uint32_t INIVAddr   = (uint32_t)INIV;

    uint32_t i = 0;
    uint32_t flag  = 0;
    uint32_t status = SUCCESS;
    __IO uint32_t counter  = 0;
//    __IO uint32_t keyCounter = 0;

    AES_ConfigStructInit(&aesConfig);

    keyConfig.key0   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key1   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key2   = __REV(*(uint32_t*)(keyAddr));
    keyAddr += 0x04;
    keyConfig.key3   = __REV(*(uint32_t*)(keyAddr));

    INIVConfig.INIV0  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV1  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV2  = __REV(*(uint32_t*)(INIVAddr));
    INIVAddr += 0x04;
    INIVConfig.INIV3  = __REV(*(uint32_t*)(INIVAddr));

    if(mode == AES_MODE_ENCRYPT)
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_ENCRYPT;
    }
    else
    {
        aesConfig.modeSlect = AES_OPERATING_MODE_EKEY_DECRYPT;
    }

    AES_Disable();
    aesConfig.chainMode = AES_CHMOD_CTR;
    aesConfig.dataType  = AES_DATATYPE_8B;

    AES_Config(&aesConfig);
    AES_ConfigKey(&keyConfig);
    AES_ConfigINIV(&INIVConfig);
    AES_Enable();

    for(i=0; i<length; i+=16)
    {
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;
        AES_InData(*(uint32_t*)(inputAddr));
        inputAddr += 0x04;

        counter = 0;
        do
        {
            flag = AES_ReadStatusFlag(AES_FLAG_CCMPFLG);
            counter++;
        }
        while((counter != 0x00010000) && (flag != SET));

        if(flag == RESET)
        {
            status = ERROR;
        }
         else
        {
            AES_ClearStatusFlag(AES_FLAG_CLR_CCMP);
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
            *(uint32_t*)(outputAddr) = AES_OutData();
            outputAddr += 0x04;
        }
    }
    AES_Disable();
    return status;
}
