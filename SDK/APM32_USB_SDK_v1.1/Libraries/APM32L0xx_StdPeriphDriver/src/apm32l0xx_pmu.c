/*!
 * @file        apm32l0xx_pmu.c
 *
 * @brief       This file contains all the functions for the PMU peripheral
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

#include "apm32l0xx_pmu.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup PMU_Driver  PMU Driver
  @{
*/

/** @addtogroup  PMU_Fuctions Fuctions
  @{
*/

/*!
 * @brief   Reset the PMU peripheral registers to their default reset values
 *
 * @param   None
 *
 * @retval  None
 */
void PMU_Reset(void)
{
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_PMU);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_PMU);
}

/*!
 * @brief   Enable access to the Backup domain registers
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_EnableBackupAccess(void)
{
    PMU->CTRL_B.BPWEN = BIT_SET;
}

/*!
 * @brief   Disable access to the Backup domain registers
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_DisableBackupAccess(void)
{
    PMU->CTRL_B.BPWEN = BIT_RESET;
}

/*!
 * @brief   Configure the PMU PVD Level
 *
 * @param   level: specifies the PVD Level
 *                 This parameter can be one of the following values
 *                 @arg PMU_PVDLEVEL_0,   //!< 1.9V
 *                 @arg PMU_PVDLEVEL_1,   //!< 2.1V
 *                 @arg PMU_PVDLEVEL_2,   //!< 2.3V
 *                 @arg PMU_PVDLEVEL_3,   //!< 2.5V
 *                 @arg PMU_PVDLEVEL_4,   //!< 2.7V
 *                 @arg PMU_PVDLEVEL_5,   //!< 2.9V
 *                 @arg PMU_PVDLEVEL_6,   //!< 3.1V
 *                 @arg PMU_PVDLEVEL_7    //!< External input analog voltage (Compare internally to V REFINT )
 *
 * @retval  None
 *
 * @note   PVD_IN input (PB7) has to be configured as analog input when CTRL_B.PLSEL = PMU_PVDLEVEL_7.
 *
 */
void PMU_ConfigPVDLevel(PMU_PVDLEVEL_T level)
{
    PMU->CTRL_B.PLSEL = level;
}

/*!
 * @brief   Enable Power voltage detector
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_EnablePVD(void)
{
    PMU->CTRL_B.PVDEN = BIT_SET;
}
/*!
 * @brief   Disable Power voltage detector
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_DisablePVD(void)
{
    PMU->CTRL_B.PVDEN = BIT_RESET;
}

/*!
 * @brief   Enable the WakeUp Pin functionality
 *
 * @param   pin: specifies the WakeUpPin
 *               This parameter can be one of the following values
 *               @arg PMU_WAKEUPPIN_1
 *               @arg PMU_WAKEUPPIN_2
 *               @arg PMU_WAKEUPPIN_3
 *
 * @retval  None
 *
 */
void PMU_EnableWakeUpPin(PMU_WAKEUPPIN_T pin)
{
    PMU->CSTS |= pin;
}

/*!
 * @brief   Disable the WakeUp Pin functionality
 *
 * @param   pin: specifies the WakeUpPin
 *               This parameter can be one of the following values
 *               @arg PMU_WAKEUPPIN_1
 *               @arg PMU_WAKEUPPIN_2
 *               @arg PMU_WAKEUPPIN_3
 *
 * @retval  None
 *
 */
void PMU_DisableWakeUpPin(PMU_WAKEUPPIN_T pin)
{
    PMU->CSTS &= ~pin;
}

/*!
 * @brief   Enters Sleep mode.
 *
 * @param   regulator: Specifies the regulator state in SLEEP mode.
 *                     This parameter can be one of the following values:
 *                     @arg PMU_REGULATOR_MAIN:     SLEEP mode with regulator ON
 *                     @arg PMU_REGULATOR_LOWPOWER: SLEEP mode with low power regulator ON
 *
 * @param   entry: Specifies if SLEEP mode is entered with WFI or WFE instruction.
 *                      This parameter can be one of the following values:
 *                      @arg PMU_SLEEP_ENTRY_WFI: enter SLEEP mode with WFI instruction
 *                      @arg PMU_SLEEP_ENTRY_WFE: enter SLEEP mode with WFE instruction
 *
 * @retval   None
 *
 * @note     When WFI entry is used, tick interrupt have to be disabled if not desired as
 *           the interrupt wake up source.
 */
void PMU_EnterSleepMode(PMU_REGULATOR_T regulator, PMU_SLEEP_ENTRY_T entry)
{
    uint32_t ulpBit, vrefenBit;

    ulpBit = PMU->CTRL_B.ULP;
    vrefenBit = SYSCFG->PMCFG2_B.VREFEN;

    /** Clear PDDSCFG and LPDSCFG bits */
    PMU->CTRL_B.PDDSCFG = BIT_RESET;
    PMU->CTRL_B.LPDSCFG = BIT_RESET;

    /** It is forbidden to configure both VREFEN=1 and ULP=1 if the device is
     in Stop mode or in Sleep/Low-power sleep mode */
    if((ulpBit != 0) && (vrefenBit != 0))
    {
        PMU->CTRL_B.ULP = BIT_RESET;
    }

    /** Set LPSM bit according to PMU_Regulator value */
    PMU->CTRL_B.LPDSCFG = regulator;

    /** Clear SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= ~((uint32_t)0x04);

    /** Select STOP mode entry*/
    if (entry == PMU_SLEEP_ENTRY_WFI)
    {
        /** Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /** Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }

    if((ulpBit != 0) && (vrefenBit != 0))
    {
        PMU->CTRL_B.ULP = BIT_SET;
    }

    __NOP();
}

/*!
 * @brief   Enters Stop mode.
 *
 * @param   Regulator: Specifies the regulator state in Stop mode.
 *                     This parameter can be one of the following values:
 *                     @arg PMU_REGULATOR_MAIN: Stop mode with regulator ON
 *                     @arg PMU_REGULATOR_LOWPOWER: Stop mode with low power regulator ON
 *
 * @param   entry: Specifies if Stop mode in entered with WFI or WFE instruction.
 *                     This parameter can be one of the following values:
 *                     @arg PMU_STOP_ENTRY_WFI: Enter Stop mode with WFI instruction
 *                     @arg PMU_STOP_ENTRY_WFE: Enter Stop mode with WFE instruction
 *
 * @retval  None
 */
void PMU_EnterStopMode(PMU_REGULATOR_T regulator, PMU_STOP_ENTRY_T entry)
{
    uint32_t ulpBit, vrefenBit;

    ulpBit = PMU->CTRL_B.ULP;
    vrefenBit = SYSCFG->PMCFG2_B.VREFEN;

    /** Clear PDDSCFG and LPDSCFG bits */
    PMU->CTRL_B.PDDSCFG = BIT_RESET;
    PMU->CTRL_B.LPDSCFG = BIT_RESET;

    /** It is forbidden to configure both VREFEN=1 and ULP=1 if the device is
    in Stop mode or in Sleep/Low-power sleep mode */
    if((ulpBit != 0) && (vrefenBit != 0))
    {
        PMU->CTRL_B.ULP = BIT_RESET;
    }

    /** Set LPSM bit according to PMU_Regulator value */
    PMU->CTRL_B.LPDSCFG = regulator;

    /** Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= ((uint32_t)0x04);

    /** Select Stop mode entry */
    if(entry == PMU_STOP_ENTRY_WFI)
    {
        /** Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /** Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }

    /** Clear SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= ~((uint32_t)0x04);

    if((ulpBit != 0) && (vrefenBit != 0))
    {
         PMU->CTRL_B.ULP = BIT_SET;
    }
}

/*!
 * @brief   Enters Standby mode.
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_EnterStandbyMode(void)
{
    /** Select Standby mode */
    PMU->CTRL_B.PDDSCFG = BIT_SET;

    /** Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= ((uint32_t)0x04);

    /** This option is used to ensure that store operations are completed */
    #if defined ( __CC_ARM)
    __force_stores();
    #endif

    /** Request Wait For Interrupt */
    __WFI();
}

/*!
 * @brief   Indicates Sleep-On-Exit when returning from Handler mode to Thread mode.
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_EnableSleepOnExit(void)
{
    /** Set EXITONSLEEP bit of Cortex System Control Register */
    SCB->SCR |= ((uint32_t)0x02);
}

/*!
 * @brief   Disables Sleep-On-Exit feature when returning from Handler mode to Thread mode.
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_DisableSleepOnExit(void)
{
    /** Clear SLEEPONEXIT bit of Cortex System Control Register */
    SCB->SCR &= ~((uint32_t)0x02);
}

/*!
 * @brief   Enables CORTEX M0+ SEVONPEND bit.
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    Sets SEVONPEND bit of SCR register. When this bit is set, this causes
 *          WFE to wake up when an interrupt moves from inactive to pended.
 *
 */
void PMU_EnableSEVOnPend(void)
{
    /** Set SEVONPEND bit of Cortex System Control Register */
    SCB->SCR |= ((uint32_t)0x10);
}

/*!
 * @brief   Disables CORTEX M0+ SEVONPEND bit.
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    Clears SEVONPEND bit of SCR register. When this bit is set, this causes
 *          WFE to wake up when an interrupt moves from inactive to pended.
 *
 */
void PMU_DisableSEVOnPend(void)
{
    /** Clear SEVONPEND bit of Cortex System Control Register */
    SCB->SCR &= ~((uint32_t)0x10);
}

/*!
 * @brief   Config Voltage Scaling Range.
 *
 * @param   range: Specifies if Stop mode in entered with WFI or WFE instruction.
 *                     This parameter can be one of the following values:
 *                     @arg PMU_VOLTAGE_RANGE1: 1.8V
 *                     @arg PMU_VOLTAGE_RANGE2: 1.5V
 *                     @arg PMU_VOLTAGE_RANGE3: 1.2V
 *
 * @retval  None
 *
 */
void PMU_ConfigVoltageRange(PMU_VOLTAGE_RANGE_T range)
{
    PMU->CTRL_B.VSRSEL = range;
}


/*!
 * @brief   Return Voltage Scaling Range.
 *
 * @param   None
 *
 * @retval  PMU_VOLTAGE_RANGE_T: Specifies if Stop mode in entered with WFI or WFE instruction.
 *                               This parameter can be one of the following values:
 *                               @arg PMU_VOLTAGE_RANGE1: 1.8V
 *                               @arg PMU_VOLTAGE_RANGE2: 1.5V
 *                               @arg PMU_VOLTAGE_RANGE3: 1.2V
 *
 */
PMU_VOLTAGE_RANGE_T PMU_ReadVoltageRange(void)
{
    return  ((PMU_VOLTAGE_RANGE_T)(PMU->CTRL_B.VSRSEL));
}

/*!
 * @brief   Enable the Fast WakeUp from Ultra Low Power mode.
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    This bit works in conjunction with ULP bit.
 *          Means, when ULP = 1 and FWUP = 1 :VREFEN startup time is ignored
 *          when exiting from low power mode.
 *
 */
void PMU_EnableFastWakeUp(void)
{
    /** Enable the fast wake up */
    PMU->CTRL_B.FWUP = BIT_SET;
}

/*!
 * @brief   Disable the Fast WakeUp from Ultra Low Power mode.
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_DisableFastWakeUp(void)
{
    /** Disable the fast wake up */
    PMU->CTRL_B.FWUP = BIT_RESET;
}

/*!
 * @brief   Enable the Ultra Low Power mode
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_EnableUltraLowPower(void)
{
    /** Enable the Ultra Low Power mode */
    PMU->CTRL_B.ULP = BIT_SET;
}

/*!
 * @brief   Disable the Ultra Low Power mode
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PMU_DisableUltraLowPower(void)
{
    /** Disable the Ultra Low Power mode */
    PMU->CTRL_B.ULP = BIT_RESET;
}

/*!
 * @brief   Enter the Low Power Run mode.
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    Low power run mode can only be entered when VSRSEL is in range 2.
 *          In addition, the dynamic voltage scaling must not be used when Low
 *          power run mode is selected. Only Stop and Sleep modes with regulator
 *          configured in Low power mode is allowed when Low power run mode is
 *          selected.
 *
 * @note    The frequency of the system clock must be decreased to not exceed the
 *          frequency of about 131.072KHZ.
 *
 * @note    In Low power run mode, all I/O pins keep the same state as in Run mode.
 *
 */
void PMU_EnterLowPowerRunMode(void)
{
    /** Enters the Low Power Run mode */
    PMU->CTRL_B.LPDSCFG = BIT_SET;
    PMU->CTRL_B.LPRUNCFG = BIT_SET;
}

/*!
 * @brief   Disable the Low Power Run mode.
 *
 * @param   None
 *
 * @retval  PMU_BITSTATUS_T
 *
 * @note    Before PMU_DisableLowPowerRunMode() completion, the function checks that
 *          REGLPFLG has been properly reset (otherwise, PMU_DisableLowPowerRunMode
 *          returns PMU_TIME_OUT status). The system clock frequency can then be
 *          increased above 2 MHz.
 *
 */
PMU_BITSTATUS_T PMU_DisableLowPowerRunMode(void)
{
    uint32_t waitLoop = 0;
    uint32_t tmp = 0;

    /** Exit the Low Power Run mode */
    PMU->CTRL_B.LPRUNCFG = BIT_RESET;
    PMU->CTRL_B.LPDSCFG = BIT_RESET;

    /** Wait until REGLPF is reset */
    waitLoop = (((uint32_t)0x32) * (SystemCoreClock / 1000000U));

    tmp = PMU->CSTS_B.REGLPFLG;
    while ((waitLoop != 0) && tmp)
    {
        waitLoop--;
    }

    tmp = PMU->CSTS_B.REGLPFLG;
    if (tmp)
    {
        return PMU_TIME_OUT;
    }

    return PMU_OK;
}

/*!
 * @brief   Checks whether the specified PMU flag is set or not
 *
 * @param   flag: specifies the flag to check
 *                This parameter can be one of the following values:
 *                @arg PMU_FLAG_WUEFLG:     Wake Up flag
 *                @arg PMU_FLAG_SBFLG:      StandBy flag
 *                @arg PMU_FLAG_PVDOFLG:    PVD output flag
 *                @arg PMU_FLAG_VREFINTRDY: Internal voltage reference (V REFINT ) ready flag
 *                @arg PMU_FLAG_VSFLG:      Voltage Scaling select flag
 *                                          SET:   Regulator voltage output is changing to the required VOS level.
 *                                          RESET: Regulator is ready in the selected voltage range
 *                @arg PMU_FLAG_REGLPFLG:   Regulator LP flag
 *
 * @retval  SET or RESET
 */
uint8_t PMU_ReadStatusFlag(PMU_FLAG_T flag)
{
    if ((PMU->CSTS & flag) != (uint32_t)RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*!
 * @brief   Clears the PMU's pending flags
 *
 * @param   flag: specifies the flag to clear
 *                This parameter can be one of the following values:
 *                @arg PMU_FLAG_WUEFLG: Wake Up flag
 *                @arg PMU_FLAG_SBFLG: StandBy flag
 *
 * @retval  None
 */
void PMU_ClearStatusFlag(uint8_t flag)
{
    if (flag == PMU_FLAG_WUEFLG)
    {
        PMU->CTRL_B.WUFLGCLR = BIT_SET;
    }
    else if (flag == PMU_FLAG_SBFLG)
    {
        PMU->CTRL_B.SBFLGCLR = BIT_SET;
    }
}

/**@} end of group PMU_Fuctions*/
/**@} end of group PMU_Driver */
/**@} end of group Peripherals_Library*/
