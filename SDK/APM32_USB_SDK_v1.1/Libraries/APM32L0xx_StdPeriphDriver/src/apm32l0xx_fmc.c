/*!
 * @file        apm32l0xx_fmc.c
 *
 * @brief       This file contains all the functions for the FMC peripheral
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

#include "apm32l0xx_fmc.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup FMC_Driver FMC Driver
  @{
*/

/** @addtogroup FMC_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Configs the code waitPeriod value.
 *
 * @param     waitPeriod: the FMC waitperiod value.
 *            This parameter can be one of the following values:
 *            @arg FMC_WAIT_PERIOD_0: FMC Zero Wait Period
 *            @arg FMC_WAIT_PERIOD_1: FMC One Wait Period
 *
 * @retval    None
 */
void FMC_ConfigWaitPeriod(FMC_WAIT_PERIOD_T waitPeriod)
{
    uint32_t tmp = 0;
    tmp = FMC->ACCTRL;
    tmp &= ~BIT0;
    tmp |= waitPeriod;
    FMC->ACCTRL_B.WAITP = tmp;
}

/*!
 * @brief     Enable the Prefetch.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_EnablePrefetch(void)
{
    FMC->ACCTRL_B.PREFEN = BIT_SET;
}

/*!
 * @brief     Disable the Prefetch.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_DisablePrefetch(void)
{
    FMC->ACCTRL_B.PREFEN = BIT_RESET;
}

/*!
 * @brief     Enable the Sleep Mode.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_EnableSleepMode(void)
{
    FMC->ACCTRL_B.SLPNVMSEL = BIT_SET;
}

/*!
 * @brief     Disable the Sleep Mode.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_DisableSleepMode(void)
{
    FMC->ACCTRL_B.SLPNVMSEL = BIT_RESET;
}

/*!
 * @brief     Enable the Buffer.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_EnableBuffer(void)
{
    FMC->ACCTRL_B.BUFDIS = BIT_RESET;
}

/*!
 * @brief     Disable the Buffer.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_DisableBuffer(void)
{
    FMC->ACCTRL_B.BUFDIS = BIT_SET;
}

/*!
 * @brief     Enable the Pre_read.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_EnablePreRead(void)
{
    FMC->ACCTRL_B.PRERDEN = BIT_SET;
}

/*!
 * @brief     Disable the Pre_read.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_DisablePreRead(void)
{
    FMC->ACCTRL_B.PRERDEN = BIT_RESET;
}

/*!
 * @brief     Unlocks the FMC program memory access.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_Unlock(void)
{
    if((FMC->PGESCTRL & BIT1) != RESET)
    {
        FMC->ULKEY = (uint32_t)0x89ABCDEF;
        FMC->ULKEY = (uint32_t)0x02030405;
        FMC->PGESKEY = (uint32_t)0x8C9DAEBF;
        FMC->PGESKEY = (uint32_t)0x13141516;
    }
}

/*!
 * @brief     Locks the FMC program memory access.
 *
 * @param     None
 *
 * @retval    None
 */
void FMC_Lock(void)
{
    FMC->PGESCTRL |= BIT0;
}

/*!
 * @brief       Erases a specified fmc page
 *
 * @param       pageAddr:   Specifies the page address
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_TIMEOUT
 */
FMC_STATE_T FMC_ErasePage(uint32_t pageAddr)
{
    FMC_STATE_T state;

    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if (state == FMC_STATE_COMPLETE)
    {
        FMC->PGESCTRL_B.ESREQ = BIT_SET;
        FMC->PGESCTRL_B.PGM = BIT_SET;

        *(__IO uint32_t *)pageAddr = 0x00000000;

        state = FMC_WaitForReady(FMC_DELAY_ERASE);

        FMC->PGESCTRL_B.ESREQ = BIT_RESET;
        FMC->PGESCTRL_B.PGM = BIT_RESET;
    }

    return state;
}

/*!
 * @brief       Program a word at a specified address
 *
 * @param       addr:   Specifies the address to be programmed
 *
 * @param       data:   Specifies the data to be programmed
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_TIMEOUT
 */
FMC_STATE_T FMC_FastProgramWord(uint32_t addr, uint32_t data)
{
    FMC_STATE_T state;

    state = FMC_WaitForReady(FMC_DELAY_PROGRAM);

    if (state == FMC_STATE_COMPLETE)
    {
        *(__IO uint32_t*)addr = (uint32_t)data;

        state = FMC_WaitForReady(FMC_DELAY_PROGRAM);
    }

    return state;
}

/*!
 * @brief       Unlocks the option bytes block access
 *
 * @param       None
 *
 * @retval      None
 */
void FMC_UnlockOptionByte(void)
{
    if((FMC->PGESCTRL & BIT2) != RESET)
    {
        FMC->OPTKEY = (uint32_t)FMC_KEY_OPT1;
        FMC->OPTKEY = (uint32_t)FMC_KEY_OPT2;
    }
}

/*!
 * @brief       Locks the option bytes block access
 *
 * @param       None
 *
 * @retval      None
 */
void FMC_LockOptionByte(void)
{
    FMC->PGESCTRL_B.OBLOCK = BIT_SET;
}

/*!
 * @brief       Launch the option byte loading
 *
 * @param       None
 *
 * @retval      None
 */
void FMC_LaunchOptionByte(void)
{
    FMC->PGESCTRL_B.OBLREQ = BIT_SET;
}

/*!
 * @brief       Read protection configuration.
 *
 * @param       rdp:   specifies the read protection level
 *                     This parameter can be any combination of the flowing values:
 *                     @arg FMC_OB_RDP_LEVEL0
 *                     @arg FMC_OB_RDP_LEVEL1
 *                     @arg FMC_OB_RDP_LEVEL2
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                     @arg FMC_STATE_COMPLETE
 *                     @arg FMC_STATE_PG_ERR
 *                     @arg FMC_STATE_WRP_ERR
 *                     @arg FMC_STATE_TIMEOU
 */
FMC_STATE_T FMC_ConfigReadProtection(FMC_OB_RDP_T rdp)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint16_t temp1 = 0;
    uint32_t temp2 = 0;

    temp1 = OB->RDP & ~0x00FF;
    temp2 = (uint32_t)~(rdp | temp1) << 16;
    temp2 |= (rdp | temp1);
    
    /** Wait for last operation to be completed */
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    
    if(state == FMC_STATE_COMPLETE)
    {
        OB->RDP = temp2;
    }
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    return state;
}

/*!
 * @brief       FMC brownout reset threshold level configuration.
 *
 * @param       bor:   specifies the brownout reset threshold level
 *                     This parameter can be any combination of the flowing values:
 *                     @arg FMC_OB_BOR_LEVEL1
 *                     @arg FMC_OB_BOR_LEVEL2
 *                     @arg FMC_OB_BOR_LEVEL3
 *                     @arg FMC_OB_BOR_LEVEL4
 *                     @arg FMC_OB_BOR_LEVEL5
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                     @arg FMC_STATE_COMPLETE
 *                     @arg FMC_STATE_PG_ERR
 *                     @arg FMC_STATE_WRP_ERR
 *                     @arg FMC_STATE_TIMEOU
 */
FMC_STATE_T FMC_ConfigBrownoutReset(FMC_OB_BOR_T bor)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint16_t temp1 = 0;
    uint32_t temp2 = 0;

    temp1 = OB->USER & ~0x000F;
    temp2 = (uint32_t)~(bor | temp1) << 16;
    temp2 |= (bor | temp1);

    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        OB->USER = temp2;
    }
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    return state;
}

/*!
 * @brief       FMC BOOT bit1 option bit configuration.
 *
 * @param       boot1:   Set or Reset the BOOT bit1 option bit.
 *                     This parameter can be any combination of the flowing values:
 *                     @arg FMC_OB_BOOT1_RESET
 *                     @arg FMC_OB_BOOT1_SET
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                     @arg FMC_STATE_COMPLETE
 *                     @arg FMC_STATE_PG_ERR
 *                     @arg FMC_STATE_WRP_ERR
 *                     @arg FMC_STATE_TIMEOU
 */
FMC_STATE_T FMC_ConfigBOOTBit1(FMC_OB_BOOT1_T boot1)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint32_t temp = 0;

    temp = OB->USER & (~(FMC_OB_BOOT1_SET<<15));
    temp |= boot1 <<15;
    temp &= ~(((uint32_t)FMC_OB_BOOT1_SET)<<31);
    temp |= (~boot1)<<31;
    

    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        OB->USER = temp;
    }
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    return state;
}

/*!
 * @brief       Read the FMC User Option Bytes values
 *
 * @param       None
 *
 * @retval      Returns the FMC User Option Bytes.
 */
uint8_t FMC_ReadOptionByteUser(void)
{
    return (uint8_t)((FMC->OPT & 0x00700000)>> 20);
}

/*!
 * @brief       Read the FMC Read Protection level
 *
 * @param       None
 *
 * @retval      Returns the FMC Read Protection level
 */
uint8_t FMC_ReadOptionByteRDP(void)
{
    uint8_t rdpLevel = (uint8_t)(FMC->OPT & 0x00FF);
    if((rdpLevel != FMC_OB_RDP_LEVEL0) && (rdpLevel != 0xCC))
    {
        return (FMC_OB_RDP_LEVEL1);
    }
    else
    {
        return rdpLevel;
    }
}

/*!
 * @brief       Read the FMC BOR level
 *
 * @param       None
 *
 * @retval      Returns the FMC BOR level
 */
uint8_t FMC_ReadOptionByteBOR(void)
{
    return (uint8_t)((FMC->OPT & 0x000F0000)>> 16);
}

/*!
 * @brief       Read the FMC BOOT bit1 value
 *
 * @param       None
 *
 * @retval      Returns the FMC BOOT bit1 value
 */
uint8_t FMC_ReadOptionByteBOOTBit1(void)
{
    return (uint8_t)((FMC->OPT & 0x80000000)>> 31);
}

/*!
 * @brief       Read the FMC Write Protection Option Bytes value
 *
 * @param       None
 *
 * @retval      Returns the FMC Write Protection Option Bytes value
 */
uint32_t FMC_ReadOptionByteWRP1(void)
{
    return (uint32_t)(FMC->WPROT1);
}

/*!
 * @brief       Read the FMC Write Protection Option Bytes value
 *
 * @param       None
 *
 * @retval      Returns the FMC Write Protection Option Bytes value
 */
uint16_t FMC_ReadOptionByteWRP2(void)
{
    return (uint16_t)(FMC->WPROT2);
}

/*!
 * @brief       Enable write protection of the 128KB desired pages of the Fmc
 *
 * @param       wrp: specifies the address of the pages to be write protected.
 *
 * @retval      None
 */
FMC_STATE_T FMC_EnableWriteProtection1(uint32_t wrp)
{
    uint32_t wrp1_1Data = 0, wrp1_2Data = 0;

    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint32_t temp1 = 0, temp2 = 0;

    /** Wait for last operation to be completed */
    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        wrp1_1Data = (uint16_t)(wrp | (0x0000FFFF & OB->WRP1_1));
        wrp1_2Data = (uint16_t)((((wrp & 0xFFFF0000)>>16 | OB->WRP1_2)));
        temp1 = (uint32_t)((~wrp1_1Data) << 16)|(wrp1_1Data);
        OB->WRP1_1 = temp1;
        temp2 = (uint32_t)((~wrp1_2Data) << 16)|(wrp1_2Data);
        OB->WRP1_2 = temp2;

        state = FMC_WaitForReady(FMC_DELAY_ERASE);
    }
    return state;
}

/*!
 * @brief       Disable write protection of the 128KB desired pages of the Fmc
 *
 * @param       wrp: specifies the address of the pages to be write protected.
 *
 * @retval      None
 */
FMC_STATE_T FMC_DisableWriteProtection1(uint32_t wrp)
{
    uint32_t wrp1_1Data = 0, wrp1_2Data = 0;

    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint32_t temp1 = 0, temp2 = 0;

    /** Wait for last operation to be completed */
    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        wrp1_1Data = (uint16_t)(~wrp & (0x0000FFFF & OB->WRP1_1));
        wrp1_2Data = (uint16_t)((((~wrp & 0xFFFF0000)>>16 & OB->WRP1_2)));
        temp1 = (uint32_t)(~(wrp1_1Data) << 16)|(wrp1_1Data);
        OB->WRP1_1 = temp1;
        temp2 = (uint32_t)(~(wrp1_2Data) << 16)|(wrp1_2Data);
        OB->WRP1_2 = temp2;

        state = FMC_WaitForReady(FMC_DELAY_ERASE);
    }

    return state;
}

/*!
 * @brief       Enable write protection of the 64KB desired pages of the Fmc
 *
 * @param       wrp: specifies the address of the pages to be write protected.
 *
 * @retval      None
 */
FMC_STATE_T FMC_EnableWriteProtection2(uint32_t wrp)
{
    uint32_t wrp2Data = 0;

    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint32_t temp1 = 0;

    /** Wait for last operation to be completed */
    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        wrp2Data = (uint16_t)(wrp | (0x0000FFFF & OB->WRP2));
        temp1 = (uint32_t)((~wrp2Data) << 16)|(wrp2Data);
        OB->WRP2 = temp1;

        state = FMC_WaitForReady(FMC_DELAY_ERASE);
    }
    return state;
}

/*!
 * @brief       Disable write protection of the 64KB desired pages of the Fmc
 *
 * @param       wrp: specifies the address of the pages to be write protected.
 *
 * @retval      None
 */
FMC_STATE_T FMC_DisableWriteProtection2(uint32_t wrp)
{
    uint32_t wrp2Data = 0;

    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint32_t temp1 = 0;

    /** Wait for last operation to be completed */
    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        wrp2Data = (uint16_t)((~wrp & (0x0000FFFF & OB->WRP2)));
        temp1 = (uint32_t)(~(wrp2Data) << 16)|(wrp2Data);
        OB->WRP2 = temp1;

        state = FMC_WaitForReady(FMC_DELAY_ERASE);
    }
    return state;
}



/*!
 * @brief       Write the Fmc User Option Byte
 *
 * @param       iwdt: Selects the WDT mode.
 *                    This parameter can be:
 *                    @arg FMC_OB_IWDT_HW
 *                    @arg FMC_OB_IWDT_SW
 * @param       stop: Reset event when entering STOP mode.
 *                    This parameter can be:
 *                    @arg FMC_OB_STOP_RESET
 *                    @arg FMC_OB_STOP_NRST
 * @param       stdb: Reset event when entering Standby mode
 *                    This parameter can be:
 *                    @arg FMC_OB_STDBY_RESET
 *                    @arg FMC_OB_STDBY_NRST
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                     @arg FMC_STATE_COMPLETE
 *                     @arg FMC_STATE_PG_ERR
 *                     @arg FMC_STATE_WRP_ERR
 *                     @arg FMC_STATE_TIMEOU
 */
FMC_STATE_T FMC_ConfigOptionByteUser(FMC_OB_IWDT_T iwdt, FMC_OB_STOP_T stop, FMC_OB_STDBY_T stdb)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint16_t temp1 = 0;
    uint32_t temp2 = 0;

    /** Get the User Option byte register */
    temp1 = (uint16_t)(OB->USER) & (~0x0070);
    temp2 = (~(((uint8_t)(iwdt)| (uint8_t)(stop)| (uint8_t)(stdb))| temp1) << 16U);
    temp2 |= ((uint32_t)iwdt | (uint32_t)stop | (uint32_t)stdb | temp1);

    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        OB->USER = temp2;
    }
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    return state;
}

/*!
 * @brief       Configures to boot from Bank1 or Bank2
 *
 * @param       bootB2: select the FMC Bank
 *                    This parameter can be:
 *                    @arg FMC_OB_BOOTB2_RESET
 *                    @arg FMC_OB_BOOTB2_SET
 *
 * @retval      None
 */
FMC_STATE_T FMC_ConfigOptionBootBank(FMC_OB_BOOB2_T bootB2)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;
    uint16_t temp1 = 0;
    uint32_t temp2 = 0;

    /** Get the User Option byte register */
    temp1 = OB->USER & (~0x0080);
    temp2 = ((~(uint32_t)((bootB2<<7) | temp1)) << 16U);
    temp2 |= ((bootB2<<7) | temp1);

    state = FMC_WaitForReady(FMC_DELAY_ERASE);

    if(state == FMC_STATE_COMPLETE)
    {
        OB->USER = temp2;
    }
    state = FMC_WaitForReady(FMC_DELAY_ERASE);
    return state;
}

/*!
 * @brief       Enable the specified fmc interrupts
 *
 * @param       interrupt:  Specifies the fmc interrupt sources
 *                          The parameter can be combination of following values:
 *                          @arg FMC_INT_EOPINTEN:    End of Programming Interrupt Enable
 *                          @arg FMC_INT_ERRINTEN:    Error Interrupt Enable
 *
 * @retval      None
 */
void FMC_EnableInterrupt(uint32_t interrupt)
{
    FMC->PGESCTRL |= interrupt;
}

/*!
 * @brief       Disable the specified fmc interrupts
 *
 * @param       interrupt:  Specifies the fmc interrupt sources
 *                          The parameter can be combination of following values:
 *                          @arg FMC_INT_EOPINTEN:    End of Programming Interrupt Enable
 *                          @arg FMC_INT_ERRINTEN:    Error Interrupt Enable
 *
 * @retval      None
 */
void FMC_DisableInterrupt(uint32_t interrupt)
{
    FMC->PGESCTRL &= ~interrupt;
}

/*!
 * @brief       Checks whether the specified fmc flag is set or not
 *

 * @param       flag:   Specifies the fmc flag to check
 *                      The parameter can be one of following values:
 *                      @arg FMC_FLAG_BUSY
 *                      @arg FMC_FLAG_EOP
 *                      @arg FMC_FLAG_WEHV
 *                      @arg FMC_FLAG_RDY
 *                      @arg FMC_FLAG_WPROTERR
 *                      @arg FMC_FLAG_PGALGERR
 *                      @arg FMC_FLAG_SIZEERR
 *                      @arg FMC_FLAG_ERROR
 *                      @arg FMC_FLAG_RDPROTERR
 *                      @arg FMC_FLAG_NZERR
 *                      @arg FMC_FLAG_WEERR
 *
 * @retval      None
 */
uint8_t FMC_ReadStatusFlag(FMC_FLAG_T flag)
{
    uint32_t status = 0;

    if (flag & 0xffffffff)
    {
        status = (FMC->STS)& flag;
    }

    if (status)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified fmc flag
 *
 * @param       flag:   Specifies the fmc flag to clear
 *                      This parameter can be any combination of the following values:
 *                      @arg FMC_FLAG_EOP
 *                      @arg FMC_FLAG_WPROTERR
 *                      @arg FMC_FLAG_PGALGERR
 *                      @arg FMC_FLAG_SIZEERR
 *                      @arg FMC_FLAG_ERROR
 *                      @arg FMC_FLAG_RDPROTERR
 *                      @arg FMC_FLAG_NZERR
 *                      @arg FMC_FLAG_WEERR
 *
 * @retval      None
 */
void FMC_ClearStatusFlag(FMC_FLAG_T flag)
{
    if (flag & 0xffffffff)
    {
        FMC->STS = flag;
    }
}

/*!
 * @brief       Read fmc state
 *
 * @param       None
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_BUSY
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 */
FMC_STATE_T FMC_ReadStatus(void)
{
    FMC_STATE_T state = FMC_STATE_COMPLETE;

    if ((FMC->STS & FMC_FLAG_BUSY) == FMC_FLAG_BUSY)
    {
        state = FMC_STATE_BUSY;
    }
    else if ((FMC->STS & FMC_FLAG_WPROTERR) != RESET)
    {
        state = FMC_STATE_WRP_ERR;
    }
    else if ((FMC->STS & (uint32_t)0x0E00) != RESET)
    {
        state = FMC_STATE_PG_ERR;
    }
    else
    {
        state = FMC_STATE_COMPLETE;
    }

    return state;
}

/*!
 * @brief       Wait for fmc controler ready
 *
 * @param       timeOut:    Specifies the time to wait
 *
 * @retval      Returns the fmc state.It can be one of value:
 *                 @arg FMC_STATE_BUSY
 *                 @arg FMC_STATE_COMPLETE
 *                 @arg FMC_STATE_PG_ERR
 *                 @arg FMC_STATE_WRP_ERR
 *                 @arg FMC_STATE_TIMEOUT
 */
FMC_STATE_T FMC_WaitForReady(uint32_t timeOut)
{
    FMC_STATE_T state;

    do
    {
        state = FMC_ReadStatus();
        timeOut--;
    }
    while ((state == FMC_STATE_BUSY) && (timeOut));

    if (!timeOut)
    {
        state = FMC_STATE_TIMEOUT;
    }

    return state;
}

/**@} end of group FMC_Fuctions*/
/**@} end of group FMC_Driver*/
/**@} end of group Peripherals_Library*/
