/*!
 * @file        apm32l0xx_dbgmcu.c
 *
 * @brief       This file provides all the DBGMCU firmware functions
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

#include "apm32l0xx_dbgmcu.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup DBGMCU_Driver DBGMCU Driver
  @{
*/

/** @addtogroup DBGMCU_Fuctions Fuctions
  @{
*/

/*!
 * @brief     Read Device Identifier
 *
 * @param     None
 *
 * @retval    The value of the Device Identifier
 */
uint32_t DBGMCU_ReadDevId(void)
{
    return ((uint32_t)DBG->IDCODE_B.EQR);
}

/*!
 * @brief     Read Revision Identifier
 *
 * @param     None
 *
 * @retval    The value of the Revision Identifier
 */
uint32_t DBGMCU_ReadRevId(void)
{
    return ((uint32_t)DBG->IDCODE_B.WVR);
}

/*!
 * @brief     Enable Debug Mode
 *
 * @param     mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
 *                  @arg DBGMCU_MODE_SLEEP:    Keep debugger connection during SLEEP mode
 *                  @arg DBGMCU_MODE_STOP:     Keep debugger connection during STOP mode
 *                  @arg DBGMCU_MODE_STANDBY:  Keep debugger connection during STANDBY mode
 * @retval    None
 */
void DBGMCU_EnableDebugMode(uint32_t mode)
{
    DBG->CFG |= (uint32_t)mode;
}

/*!
 * @brief     Disable Debug Mode
 *
 * @param     mode: specifies the low power mode.
 *                  The parameter can be combination of following values:
  *                 @arg DBGMCU_MODE_SLEEP:    End debugger connection during SLEEP mode
 *                  @arg DBGMCU_MODE_STOP:     End debugger connection during STOP mode
 *                  @arg DBGMCU_MODE_STANDBY:  End debugger connection during STANDBY mode
 * @retval    None
 */
void DBGMCU_DisableDebugMode(uint32_t mode)
{
    DBG->CFG &= (uint32_t)~mode;
}

/*!
 * @brief     Enable APB1 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBGMCU_APB1_PER_TMR2_STOP:    TMR2  counter stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR3_STOP:    TMR3  counter stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR6_STOP:    TMR6  counter stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR7_STOP:    TMR7  counter stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_RTC_STOP:     RTC   counter stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_WWDT_STOP:    Debug WWDT stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_IWDT_STOP:    Debug IWDT stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_LPTMR1_STOP:  Debug LPTMR1 stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_I2C1_SMBUS_TIMEOUT_STOP: I2C1 SMBUS timeout mode stopped when Core is halted
 *                        @arg DBGMCU_APB1_PER_I2C3_SMBUS_TIMEOUT_STOP: I2C3 SMBUS timeout mode stopped when Core is halted
 * @retval      None
 */
void DBGMCU_EnableAPB1Periph(uint32_t peripheral)
{
    DBG->APB1F |= (uint32_t)peripheral;
}

/*!
 * @brief     Disable APB1 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB1 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBGMCU_APB1_PER_TMR2_STOP:    TMR2  counter continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR3_STOP:    TMR3  counter continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR6_STOP:    TMR6  counter continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_TMR7_STOP:    TMR7  counter continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_RTC_STOP:     RTC   counter continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_WWDT_STOP:    Debug WWDT continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_IWDT_STOP:    Debug IWDT continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_LPTMR1_STOP:  Debug LPTMR1 continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_I2C1_SMBUS_TIMEOUT_STOP: I2C1 SMBUS timeout mode continued when Core is halted
 *                        @arg DBGMCU_APB1_PER_I2C3_SMBUS_TIMEOUT_STOP: I2C3 SMBUS timeout mode continued when Core is halted
 * @retval      None
 */
void DBGMCU_DisableAPB1Periph(uint32_t peripheral)
{
    DBG->APB1F &= (uint32_t)~peripheral;
}

/*!
 * @brief     Enable APB2 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBGMCU_APB2_PER_TMR21_STOP:   TMR21 counter stopped when Core is halted
 *                        @arg DBGMCU_APB2_PER_TMR22_STOP:   TMR22 counter stopped when Core is halted
 * @retval      None
 */
void DBGMCU_EnableAPB2Periph(uint32_t peripheral)
{
    DBG->APB2F |= (uint32_t)peripheral;
}

/*!
 * @brief     Disable APB2 peripheral in Debug mode.
 *
 * @param     peripheral: Specifies the APB2 peripheral.
 *                        The parameter can be combination of following values:
 *                        @arg DBGMCU_APB2_PER_TMR21_STOP:   TMR21 counter continued when Core is halted
 *                        @arg DBGMCU_APB2_PER_TMR22_STOP:   TMR22 counter continued when Core is halted
 * @retval      None
 */
void DBGMCU_DisableAPB2Periph(uint32_t peripheral)
{
    DBG->APB2F &= (uint32_t)~peripheral;
}

/**@} end of group DBGMCU_Fuctions*/
/**@} end of group DBGMCU_Driver */
/**@} end of group Peripherals_Library*/
