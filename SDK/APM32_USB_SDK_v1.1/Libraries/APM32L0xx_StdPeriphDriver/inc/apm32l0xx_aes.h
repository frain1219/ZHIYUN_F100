/*!
 * @file        apm32l0xx_aes.h
 *
 * @brief       This file contains all the functions prototypes for the AES firmware library
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

#ifndef  __APM32L0xx_AES_H
#define  __APM32L0xx_AES_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup AES_Driver AES Driver
  @{
*/

/** @addtogroup AES_Enumerations Enumerations
  @{
*/

/**
 * @brief AES operating mode
 */
typedef enum
{
    AES_OPERATING_MODE_ENCRYPT,       //!< Encryption
    AES_OPERATING_MODE_EKEY,          //!< Key derivation (or key preparation for ECB/CBC decryption)
    AES_OPERATING_MODE_DECRYPT,       //!< Decryption
    AES_OPERATING_MODE_EKEY_DECRYPT   //!< Key derivation then single decryption
} AES_OPERATING_MODE_T;

/**
 * @brief AES chaining mode
 */
typedef enum
{
    AES_CHMOD_ECB,  //!< Electronic codebook
    AES_CHMOD_CBC,  //!< Cipher-Block Chaining
    AES_CHMOD_CTR   //!< Counter Mode
} AES_CHMOD_T;

/**
 * @brief AES Data Type
 */
typedef enum
{
    AES_DATATYPE_32B,  //!< None
    AES_DATATYPE_16B,  //!< Half-word (16-bit)
    AES_DATATYPE_8B,   //!< Byte (8-bit)
    AES_DATATYPE_1B    //!< Bit
} AES_DATATYPE_T;

/**
 * @brief AES interrupt source
 */
typedef enum
{
    AES_INT_CCMP = BIT9,  //!< Computation Complete interrupt
    AES_INT_ERR  = BIT10  //!< Error interrupt(Write error and Read error)
} AES_INT_T;

/**
 * @brief AES flag to clear
 */
typedef enum
{
    AES_FLAG_CLR_CCMP = BIT7,  //!< Clear Computation Complete flag
    AES_FLAG_CLR_ERR  = BIT8   //!< Clear Error flag(Write error and Read error)
} AES_FLAG_CLR_T;

/**
  * @brief AES flag
  */
typedef enum
{
    AES_FLAG_CCMPFLG = BIT0,   //!< Computation Complete Flag
    AES_FLAG_RDERR   = BIT1,   //!< Read Error Flag
    AES_FLAG_WERR    = BIT2    //!< Write Error Flag
} AES_FLAG_T;

/**
  * @brief AES DMA request
  */
typedef enum
{
    AES_DMAREQ_IN  = BIT11,
    AES_DMAREQ_OUT = BIT12
} AES_DMAREQ_T;

/**
 * @brief AES Encryption/Decryption_mode
 */
typedef enum
{
    AES_MODE_DECRYPT,
    AES_MODE_ENCRYPT
} AES_MODE_T;

/**@} end of group AES_Enumerations*/

/** @addtogroup AES_Structure Data Structure
  @{
*/

/**
 * @brief   AES KeyConfig structure definition
 */
typedef struct
{
    uint32_t key0;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
} AES_KeyConfig_T;

/**
 * @brief   AES Initialization Vectors structure definition
 */
typedef struct
{
    uint32_t INIV0;
    uint32_t INIV1;
    uint32_t INIV2;
    uint32_t INIV3;
} AES_INIVConfig_T;

/**
 * @brief   AES Initialization Vectors structure definition
 */
typedef struct
{
    AES_CHMOD_T          chainMode;       //!< Chaining Mode Select
    AES_DATATYPE_T       dataType;        //!< Data Type Select
    AES_OPERATING_MODE_T modeSlect;       //!< AES operating mode Select
} AES_Config_T;

/**@} end of group AES_Structure*/

/** @addtogroup AES_Fuctions Fuctions
  @{
*/

/** AES Configuration */
void AES_Reset(void);
void AES_Config(AES_Config_T* aesConfig);
void AES_ConfigStructInit(AES_Config_T* aesConfig);
void AES_ConfigKey(AES_KeyConfig_T* keyConfig);
void AES_ConfigKeyStructInit(AES_KeyConfig_T* keyConfig);
void AES_ConfigINIV(AES_INIVConfig_T* INIVConfig);
void AES_ConfigINIVStructInit(AES_INIVConfig_T* INIVConfig);
void AES_Enable(void);
void AES_Disable(void);

/** AES Data processing */
void AES_InData(uint32_t data);
uint32_t AES_OutData(void);

/** AES DMA */
void AES_EnableDMA(AES_DMAREQ_T dmaReq);
void AES_DisableDMA(AES_DMAREQ_T dmaReq);

/** AES Interrupt and flag */
void AES_EnableInterrupt(uint32_t interrupt);
void AES_DisableInterrupt(uint32_t interrupt);
uint8_t AES_ReadStatusFlag(AES_FLAG_T flag);
void AES_ClearStatusFlag(uint32_t flag);
uint8_t AES_ReadCmdStatus(void);

/** High Level AES */
uint8_t AES_ECB(AES_MODE_T mode, uint32_t length,
                   uint8_t *key,
                   uint8_t *input, uint8_t *output);

uint8_t AES_CBC(AES_MODE_T mode, uint32_t length,
                   uint8_t *key, uint8_t INIV[16],
                   uint8_t *input, uint8_t *output);

uint8_t AES_CTR(AES_MODE_T mode, uint32_t length,
                   uint8_t *key, uint8_t INIV[16],
                   uint8_t *input, uint8_t *output);

/**@} end of group AES_Fuctions*/
/**@} end of group AES_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_AES_H */
