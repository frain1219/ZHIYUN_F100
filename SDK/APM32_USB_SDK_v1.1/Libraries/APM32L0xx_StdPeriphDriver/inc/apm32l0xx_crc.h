/*!
 * @file        apm32l0xx_crc.h
 *
 * @brief       This file contains all the functions prototypes for the CRC firmware library
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

#ifndef __APM32L0xx_CRC_H
#define __APM32L0xx_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup CRC_Driver CRC Driver
  @{
*/

/** @addtogroup CRC_Enumerations Enumerations
  @{
*/

/**
 * @brief   CRC Reverse Input Data
 */
typedef enum
{
    CRC_REVERSE_INPUT_DATA_NO ,
    CRC_REVERSE_INPUT_DATA_8B ,
    CRC_REVERSE_INPUT_DATA_16B,
    CRC_REVERSE_INPUT_DATA_32B,
} CRC_REVERSE_INPUT_DATA_T;

/**
 * @brief   CRC Polynomial Size
 */
typedef enum
{
    CRC_POLYNOMIAL_SIZE_32,
    CRC_POLYNOMIAL_SIZE_16,
    CRC_POLYNOMIAL_SIZE_8 ,
    CRC_POLYNOMIAL_SIZE_7 ,
} CRC_POLYNOMIAL_SIZE_T;

/**@} end of group CRC_Enumerations*/

/** @addtogroup CRC_Fuctions Fuctions
  @{
*/

/** Reset CRC */
void CRC_Reset(void);

/** Reset DATA */
void CRC_ResetDATA(void);

/** CRC Polynomial */
void CRC_SetPolynomialSize(CRC_POLYNOMIAL_SIZE_T polynomialSize);
void CRC_SetPolynomialValue(uint32_t polynomialValue);

/** Performed on input data */
void CRC_SelectReverseInputData(CRC_REVERSE_INPUT_DATA_T revInData);

/** Enable and Disable Reverse Output Data */
void CRC_EnableReverseOutputData(void);
void CRC_DisableReverseOutputData(void);

/** Write INITVAL register */
void CRC_WriteInitRegister(uint32_t initValue);

/** Calculate CRC */
uint32_t CRC_CalculateCRC(uint32_t data);
uint32_t CRC_CalculateCRC8bits(uint8_t data);
uint32_t CRC_CalculateCRC16bits(uint16_t data);
uint32_t CRC_CalculateBlockCRC(uint32_t pBuffer[], uint32_t bufferLength);

/** Read CRC */
uint32_t CRC_ReadCRC(void);

/** Independent Data(ID) */
void CRC_WriteIDRegister(uint8_t IDValue);
uint8_t CRC_ReadIDRegister(void);

/**@} end of group CRC_Fuctions*/
/**@} end of group CRC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_CRC_H */
