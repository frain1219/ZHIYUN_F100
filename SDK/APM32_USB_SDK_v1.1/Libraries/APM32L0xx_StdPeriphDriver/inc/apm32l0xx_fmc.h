/*!
 * @file        apm32l0xx_fmc.h
 *
 * @brief       This file contains all the functions prototypes for the FMC firmware
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

#ifndef __APM32L0xx_FMC_H
#define __APM32L0xx_FMC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup FMC_Driver FMC Driver
  @{
*/

/** @addtogroup FMC_Enumerations Enumerations
  @{
*/

/**
 * @brief FMC Wait Period
 */
typedef enum
{
    FMC_WAIT_PERIOD_0,
    FMC_WAIT_PERIOD_1
} FMC_WAIT_PERIOD_T;

/**
 * @brief   Fmc Status
 */
typedef enum
{
    FMC_STATE_COMPLETE,       //!< Operation complete
    FMC_STATE_BUSY,           //!< Busy
    FMC_STATE_PG_ERR,         //!< Program error
    FMC_STATE_WRP_ERR,        //!< Write Protection error
    FMC_STATE_TIMEOUT         //!< Time out
} FMC_STATE_T;

/**
 * @brief   Fmc Keys
 */
typedef enum
{
    FMC_KEY_PD1   = (int32_t)0x04152637,
    FMC_KEY_PD2   = (int32_t)0xFAFBFCFD,
    FMC_KEY_UL1   = (int32_t)0x89ABCDEF,
    FMC_KEY_UL2   = (int32_t)0x02030405,
    FMC_KEY_PGES1 = (int32_t)0x8C9DAEBF,
    FMC_KEY_PGES2 = (int32_t)0x13141516,
    FMC_KEY_OPT1  = (int32_t)0xFBEAD9C8,
    FMC_KEY_OPT2  = (int32_t)0x24252627
} FMC_KEY_T;

/**
 * @brief   Fmc write protect page definition 1
 */
typedef enum
{
    FMC_OB_PCROP1_Pages0to7      = BIT0,
    FMC_OB_PCROP1_Pages8to15     = BIT1,
    FMC_OB_PCROP1_Pages16to23    = BIT2,
    FMC_OB_PCROP1_Pages24to31    = BIT3,
    FMC_OB_PCROP1_Pages32to39    = BIT4,
    FMC_OB_PCROP1_Pages40to47    = BIT5,
    FMC_OB_PCROP1_Pages48to55    = BIT6,
    FMC_OB_PCROP1_Pages56to63    = BIT7,
    FMC_OB_PCROP1_Pages64to71    = BIT8,
    FMC_OB_PCROP1_Pages72to79    = BIT9,
    FMC_OB_PCROP1_Pages80to87    = BIT10,
    FMC_OB_PCROP1_Pages88to95    = BIT11,
    FMC_OB_PCROP1_Pages96to103   = BIT12,
    FMC_OB_PCROP1_Pages104to111  = BIT13,
    FMC_OB_PCROP1_Pages112to119  = BIT14,
    FMC_OB_PCROP1_Pages120to127  = BIT15,
    FMC_OB_PCROP1_Pages128to135  = BIT16,
    FMC_OB_PCROP1_Pages136to143  = BIT17,
    FMC_OB_PCROP1_Pages144to151  = BIT18,
    FMC_OB_PCROP1_Pages152to159  = BIT19,
    FMC_OB_PCROP1_Pages160to167  = BIT20,
    FMC_OB_PCROP1_Pages168to175  = BIT21,
    FMC_OB_PCROP1_Pages176to183  = BIT22,
    FMC_OB_PCROP1_Pages184to191  = BIT23,
    FMC_OB_PCROP1_Pages192to199  = BIT24,
    FMC_OB_PCROP1_Pages200to207  = BIT25,
    FMC_OB_PCROP1_Pages208to215  = BIT26,
    FMC_OB_PCROP1_Pages216to223  = BIT27,
    FMC_OB_PCROP1_Pages224to231  = BIT28,
    FMC_OB_PCROP1_Pages232to239  = BIT29,
    FMC_OB_PCROP1_Pages240to247  = BIT30,
    FMC_OB_PCROP1_Pages248to255  = (int32_t)BIT31,
    FMC_OB_PCROP_AllPages        = (int32_t)0xFFFFFFFFU
} FMC_OB_PCROP1_T;

/**
 * @brief   Fmc write protect page definition 2
 */
typedef enum
{
    FMC_OB_PCROP2_Pages256to263 = BIT0,
    FMC_OB_PCROP2_Pages264to271 = BIT1,
    FMC_OB_PCROP2_Pages272to179 = BIT2,
    FMC_OB_PCROP2_Pages280to287 = BIT3,
    FMC_OB_PCROP2_Pages288to295 = BIT4,
    FMC_OB_PCROP2_Pages296to303 = BIT5,
    FMC_OB_PCROP2_Pages304to311 = BIT6,
    FMC_OB_PCROP2_Pages312to319 = BIT7,
    FMC_OB_PCROP2_Pages320to327 = BIT8,
    FMC_OB_PCROP2_Pages328to335 = BIT9,
    FMC_OB_PCROP2_Pages336to343 = BIT10,
    FMC_OB_PCROP2_Pages344to351 = BIT11,
    FMC_OB_PCROP2_Pages352to359 = BIT12,
    FMC_OB_PCROP2_Pages360to367 = BIT13,
    FMC_OB_PCROP2_Pages368to375 = BIT14,
    FMC_OB_PCROP2_Pages376to383 = BIT15,
    FMC_OB_PCROP2_AllPages      = 0x0000FFFFU
} FMC_OB_PCROP2_T;

/**
 * @brief   Protection Level
 */
typedef enum
{
    FMC_OB_RDP_LEVEL0 = 0xAA,
    FMC_OB_RDP_LEVEL1 = 0xBB,
//    FMC_OB_RDP_LEVEL2 = 0xCC
} FMC_OB_RDP_T;

/**
 * @brief   Option Bytes BOR Level
 */
typedef enum
{
    FMC_OB_BOR_OFF    = 0x00,
    FMC_OB_BOR_LEVEL1 = 0x08,
    FMC_OB_BOR_LEVEL2 = 0x09,
    FMC_OB_BOR_LEVEL3 = 0x0A,
    FMC_OB_BOR_LEVEL4 = 0x0B,
    FMC_OB_BOR_LEVEL5 = 0x0C
} FMC_OB_BOR_T;

/**
 * @brief   Option byte WDG mode activation
 */
typedef enum
{
    FMC_OB_IWDT_HW    = ((uint8_t)0X00),     //!< activated by hardware
    FMC_OB_IWDT_SW    = ((uint8_t)0X10),     //!< activated by software
} FMC_OB_IWDT_T;

/**
 * @brief   Option byte STOP mode activation
 */
typedef enum
{
    FMC_OB_STOP_RESET = ((uint8_t)0X00),     //!< Reset generated when entering in STOP
    FMC_OB_STOP_NRST  = ((uint8_t)0X20),     //!< No reset generated when entering in STOP
} FMC_OB_STOP_T;

/**
 * @brief   Option byte STDBY mode activation
 */
typedef enum
{
    FMC_OB_STDBY_RESET = ((uint8_t)0X00),    //!< Reset generated when entering in STDBY
    FMC_OB_STDBY_NRST  = ((uint8_t)0X40),    //!< No reset generated when entering in STDBY
} FMC_OB_STDBY_T;

/**
 * @brief   Option Bytes BOOTBit1
 */
typedef enum
{
    FMC_OB_BOOT1_RESET,    //!< Reset generated when entering in STDBY
    FMC_OB_BOOT1_SET  ,    //!< No reset generated when entering in STDBY
} FMC_OB_BOOT1_T;

/**
 * @brief   FMC WRP State
 */
typedef enum
{
    FMC_OB_WRPS_TATE_DISABLE,
    FMC_OB_WRPS_TATE_ENABLE
} FMC_OB_WRPS_TATE_T;

/**
 * @brief   Option Bytes BOOTB2
 */
typedef enum
{
    FMC_OB_BOOTB2_RESET,    //!< Reset Boot from Bank 2
    FMC_OB_BOOTB2_SET  ,    //!< No reset Boot from Bank 2
} FMC_OB_BOOB2_T;

/**
 * @brief   Interrupt source
 */
typedef enum
{
    FMC_INT_EOPINTEN  = BIT16,   //!< End of Programming Interrupt Enable
    FMC_INT_ERRINTEN  = BIT17,   //!< Error Interrupt Enable
} FMC_INT_T;

typedef enum
{
    FMC_FLAG_BUSY      = BIT0,
    FMC_FLAG_EOP       = BIT1,
    FMC_FLAG_WEHV      = BIT2,
    FMC_FLAG_RDY       = BIT3,
    FMC_FLAG_WPROTERR  = BIT8,
    FMC_FLAG_PGALGERR  = BIT9,
    FMC_FLAG_SIZEERR   = BIT10,
    FMC_FLAG_ERROR     = BIT11,
    FMC_FLAG_RDPROTERR = BIT13,
    FMC_FLAG_NZERR     = BIT16,
    FMC_FLAG_WEERR     = BIT17,
} FMC_FLAG_T;

/** @addtogroup FMC_Macros Macros
  @{
*/

/** Macros description */

/** Delay definition */
#define FMC_DELAY_ERASE       ((uint32_t)0x00008000)
#define FMC_DELAY_PROGRAM     FMC_DELAY_ERASE
/**@} end of group FMC_Macros*/

/** @addtogroup FMC_Fuctions Fuctions
  @{
*/

/** Wait Period */
void FMC_ConfigWaitPeriod(FMC_WAIT_PERIOD_T waitPeriod);

void FMC_EnablePrefetch(void);
void FMC_DisablePrefetch(void);
void FMC_EnableSleepMode(void);
void FMC_DisableSleepMode(void);
void FMC_EnableBuffer(void);
void FMC_DisableBuffer(void);
void FMC_EnablePreRead(void);
void FMC_DisablePreRead(void);

/** Lock */
void FMC_Unlock(void);
void FMC_Lock(void);

/** Erase and Program */
FMC_STATE_T FMC_ErasePage(uint32_t pageAddr);
FMC_STATE_T FMC_FastProgramWord(uint32_t addr, uint32_t data);

/** FMC Option Bytes Programming functions */
void FMC_UnlockOptionByte(void);
void FMC_LockOptionByte(void);
void FMC_LaunchOptionByte(void);
FMC_STATE_T FMC_ConfigReadProtection(FMC_OB_RDP_T rdp);
FMC_STATE_T FMC_ConfigBrownoutReset(FMC_OB_BOR_T bor);
FMC_STATE_T FMC_ConfigBOOTBit1(FMC_OB_BOOT1_T boot1);
uint8_t FMC_ReadOptionByteUser(void);
uint8_t FMC_ReadOptionByteRDP(void);
uint8_t FMC_ReadOptionByteBOR(void);
uint8_t FMC_ReadOptionByteBOOTBit1(void);
uint32_t FMC_ReadOptionByteWRP1(void);
uint16_t FMC_ReadOptionByteWRP2(void);

FMC_STATE_T FMC_EnableWriteProtection1(uint32_t wrp);
FMC_STATE_T FMC_DisableWriteProtection1(uint32_t wrp);
FMC_STATE_T FMC_EnableWriteProtection2(uint32_t wrp);
FMC_STATE_T FMC_DisableWriteProtection2(uint32_t wrp);
FMC_STATE_T FMC_ConfigOptionByteUser(FMC_OB_IWDT_T iwdt, FMC_OB_STOP_T stop, FMC_OB_STDBY_T stdb);
FMC_STATE_T FMC_ConfigOptionBootBank(FMC_OB_BOOB2_T bootB2);

/** Interrupt and Flag */
void FMC_EnableInterrupt(uint32_t interrupt);
void FMC_DisableInterrupt(uint32_t interrupt);
uint8_t FMC_ReadStatusFlag(FMC_FLAG_T flag);
void FMC_ClearStatusFlag(FMC_FLAG_T flag);

/** State management */
FMC_STATE_T FMC_ReadStatus(void);
FMC_STATE_T FMC_WaitForReady(uint32_t timeOut);

/**@} end of group FMC_Fuctions*/
/**@} end of group FMC_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_FMC_H */
