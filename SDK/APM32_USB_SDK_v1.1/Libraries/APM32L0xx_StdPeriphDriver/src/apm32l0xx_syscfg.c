/*!
 * @file       apm32l0xx_syscfg.c
 *
 * @brief        This file provides firmware functions to manage the SYSCFG peripheral.
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

#include "apm32l0xx_syscfg.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library
  @{
*/

/** @addtogroup SYSCFG_Driver
  @{
*/

/** @addtogroup SYSCFG_Fuctions
  @{
*/


/*!
 * @brief        Set SYSCFG  EINTCFG1/2/3/4 register to reset value
 *
 * @param        None
 *
 * @retval       None
 */

void SYSCFG_Reset(void)
{
    SYSCFG->MMSEL &= (uint32_t) SYSCFG_CFG1_MEMMODE;
    SYSCFG->PMCFG1 = 1;
    SYSCFG->PMCFG2 = 0;
    SYSCFG->EINTCFG1 = 1;
    SYSCFG->EINTCFG2 = 0;
    SYSCFG->EINTCFG3 = 0;
    SYSCFG->EINTCFG4 = 0;
}

/*!
 * @brief       SYSCFG Memory Remap selects
 *
 * @param       memory: selects the memory remapping
 *                      The parameter can be one of following values:
 *                      @arg SYSCFG_MEMORY_REMAP_FMC:      SYSCFG MemoryRemap Flash
 *                      @arg SYSCFG_MEMORY_REMAP_SYSTEM:   SYSCFG MemoryRemap SystemMemory
 *                      @arg SYSCFG_MEMORY_REMAP_SMC:      SYSCFG MemoryRemap SM
 *                      @arg SYSCFG_MEMORY_REMAP_SRAM:     SYSCFG MemoryRemap SRAM
 *
 * @retval      None
 */
void SYSCFG_MemoryRemapSelect(uint8_t memory)
{
    SYSCFG->MMSEL = (uint8_t)memory;
}


/*!
 * @brief       Enables SYSCFG GPIO Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_GPIO_FMP_PB6:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB7:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB8:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB9:    PB6 Fast mode plus
 *
 * @retval      None
 */
void SYSCFG_EnableGPIOFastModePlus(uint32_t pin)
{
    SYSCFG->PMCFG1 |= (uint32_t)pin;
}

/*!
 * @brief       Disables SYSCFG GPIO Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_GPIO_FMP_PB6:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB7:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB8:    PB6 Fast mode plus
 *                       @arg SYSCFG_GPIO_FMP_PB9:    PB6 Fast mode plus
 *
 * @retval      None
 */
void SYSCFG_DisableGPIOFastModePlus(uint32_t pin)
{
    SYSCFG->PMCFG1 &= (uint32_t)~pin;
}



/*!
 * @brief       Enables SYSCFG I2C Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_I2C_FMP_I2C1:     Fast Mode Plus on I2C1 pins
 *                       @arg SYSCFG_I2C_FMP_I2C2:     Fast Mode Plus on I2C2 pins
 *                       @arg SYSCFG_I2C_FMP_I2C3:     Fast Mode Plus on I2C3 pins
 *
 * @retval      None
 */
void SYSCFG_EnableI2CFastModePlus(uint32_t pin)
{
    SYSCFG->PMCFG1 |= (uint32_t)pin;
}

/*!
 * @brief       Disables SYSCFG I2C Fast Mode Plus
 *
 * @param       pin:     selects the pin.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_I2C_FMP_I2C1:     Fast Mode Plus on I2C1 pins
 *                       @arg SYSCFG_I2C_FMP_I2C2:     Fast Mode Plus on I2C2 pins
 *                       @arg SYSCFG_I2C_FMP_I2C3:     Fast Mode Plus on I2C3 pins
 *
 * @retval      None
 */
void SYSCFG_DisableI2CFastModePlus(uint32_t pin)
{
    SYSCFG->PMCFG1 &= (uint32_t)~pin;
}


/*!
 * @brief       Selects the GPIO pin used as EINT Line.
 *
 * @param       port:   selects the port can be GPIOA/B/C/D/E/H
 *
 * @param       pin:    selects the pin can be SYSCFG_PIN_(0..15)
 *
 * @retval      None
 *
 * @note        GPIOE only for APM32L072
 */

void SYSCFG_EINTLine(SYSCFG_PORT_T port, SYSCFG_PIN_T pin)
{
    uint32_t status;

    status = (((uint32_t)0x0F) & port) << (0x04 * (pin & (uint8_t)0x03));

    if ( pin <= 0x03)
    {
        SYSCFG->EINTCFG1 |= status;
    }
    else if (( 0x04 <= pin) & (pin <= 0x07))
    {
        SYSCFG->EINTCFG2 |= status;
    }
    else if (( 0x08 <= pin) & (pin <= 0x0B))
    {
        SYSCFG->EINTCFG3 |= status;
    }
    else if (( 0x0C <= pin) & (pin <= 0x0F))
    {
        SYSCFG->EINTCFG4 |= status;
    }
}

/*!
 * @brief       Enables SYSCFG Buffer
 *
 * @param       module:     selects the module.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_MODULE_ADC:       For ADC Module
 *                       @arg SYSCFG_MODULE_TS:        For Temperature Module
 *                       @arg SYSCFG_MODULE_COMP2:     For COMP2 Module
 *                       @arg SYSCFG_MODULE_HSI48:     For HSICLK48 Module
 *
 * @retval      None
 */
void SYSCFG_EnableModuleBuffer(uint32_t module)
{
    SYSCFG->PMCFG2 |= (uint32_t)module;
}

/*!
 * @brief       Disables SYSCFG Buffer
 *
 * @param       module:     selects the module.
 *                       The parameter can be combination of following values:
 *                       @arg SYSCFG_MODULE_ADC:       For ADC Module
 *                       @arg SYSCFG_MODULE_TS:        For Temperature Module
 *                       @arg SYSCFG_MODULE_COMP2:     For COMP2 Module
 *                       @arg SYSCFG_MODULE_HSI48:     For HSICLK48 Module
 *
 * @retval      None
 */
void SYSCFG_DisableModuleBuffer(uint32_t module)
{
    SYSCFG->PMCFG2 &= (uint32_t)~module;
}

/**@} end of group SYSCFG_Fuctions*/
/**@} end of group SYSCFG_Driver*/
/**@} end of group Peripherals_Library*/
