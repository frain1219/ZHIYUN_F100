/*!
 * @file        apm32l0xx_rcm.c
 *
 * @brief       This file provides all the RCM firmware functions
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

#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup RCM_Driver RCM Driver
  @{
*/

/** @addtogroup RCM_Fuctions Fuctions
  @{
*/

/*!
 * @brief   Resets the clock configuration
 *
 * @param   None
 *
 * @retval  None
 */
void RCM_Reset(void)
{
    /** Set MSIEN bit */
    RCM->CTRL_B.MSIEN = BIT_SET;

    /** Set MSIR and HSI16TIM bits to the rest value */
    RCM->ICLKCAL &= (uint32_t)0x00FF00FF;
    RCM->ICLKCAL |= (uint32_t)0x0000B000;

    /** Reset SCLKSEL, AHBPSC, APB1PSC, APB2PSC, MCOSEL and MCOPSC bits */
    RCM->CFG &= (uint32_t)0x80FFC00C;

    /** Reset HSI, HSE, PLL */
    RCM->CTRL &= (uint32_t)0xFEFEFFD4;

    /** Reset HSEBCFG bit */
    RCM->CTRL_B.HSEBCFG = BIT_RESET;

    /** Set RTCPSC to 0b00*/
    RCM->CTRL_B.RTCPSC = BIT_SET;

    /** Reset CFG register */
    RCM->CFG = (uint32_t)0x00000000;

    /** Reset HSI48EN and  HSI48DIVEN bits */
    RCM->HSI48CFG_B.HSI48EN = BIT_RESET;
    RCM->HSI48CFG_B.HSI48DIVEN = BIT_RESET;

    /** Disable all interrupts and interrupt flags */
    RCM->INTEN = (uint32_t)0x00000000;
    RCM->INTCLR = (uint32_t)0xFFFFFFFF;
}

/*!
 * @brief       Configures HSE
 *
 * @param       state: state of the HSE
 *                     This parameter can be one of the following values:
 *                     @arg RCM_HSE_CLOSE: turn OFF the HSE oscillator
 *                     @arg RCM_HSE_OPEN: turn ON the HSE oscillator
 *                     @arg RCM_HSE_BYPASS: HSE oscillator bypassed with external clock
 *
 * @retval      None
 *
 * @note        HSE can not be stopped if it is used directly or through the PLL as system clock
 */
void RCM_ConfigHSE(RCM_HSE_T state)
{
    RCM->CTRL_B.HSEEN = BIT_RESET;

    RCM->CTRL_B.HSEBCFG = BIT_RESET;

    if (state == RCM_HSE_OPEN)
    {
        RCM->CTRL_B.HSEEN = BIT_SET;
    }
    else if (state == RCM_HSE_BYPASS)
    {
        RCM->CTRL_B.HSEBCFG = BIT_SET;
        RCM->CTRL_B.HSEEN = BIT_SET;
    }
}

/*!
 * @brief       Waits for HSE be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     HSE oscillator is stable and ready to use
 *              ERROR       HSE oscillator not yet ready
 */
uint8_t RCM_WaitHSEReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CTRL_B.HSERDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Enable HSI16
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        HSI can not be stopped if it is used directly or through the PLL as system clock
 */
void RCM_EnableHSI16(void)
{
    RCM->CTRL_B.HSI16EN = BIT_SET;
}

/*!
 * @brief       Disable HSI16
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        HSI can not be stopped if it is used directly or through the PLL as system clock
 */
void RCM_DisableHSI16(void)
{
    RCM->CTRL_B.HSI16EN = BIT_RESET;
}

/*!
 * @brief       Enable 16MHz High Speed Internal Clock Force Enable
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableHSI16Force(void)
{
    RCM->CTRL_B.HSI16FEN = BIT_SET;
}

/*!
 * @brief       Disable 16MHz High Speed Internal Clock Force Enable
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableHSI16Force(void)
{
    RCM->CTRL_B.HSI16FEN = BIT_RESET;
}

/*!
 * @brief       Enable 16MHz HSI Divider by 4
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableHSI16DIV(void)
{
    RCM->CTRL_B.HSI16DIVEN = BIT_SET;
}

/*!
 * @brief       Disable 16MHz HSI Divider by 4
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableHSI16DIV(void)
{
    RCM->CTRL_B.HSI16DIVEN = BIT_RESET;
}

/*!
 * @brief       Enable HSICLK16 Output
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableHSI16Out(void)
{
    RCM->CTRL_B.HSI16OUTEN = BIT_SET;
}

/*!
 * @brief       Disable HSICLK16 Output
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableHSI16Out(void)
{
    RCM->CTRL_B.HSI16OUTEN = BIT_RESET;
}

/*!
 * @brief       Waits for HSI16 be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     HSI16 oscillator is stable and ready to use
 *              ERROR       HSI16 oscillator not yet ready
 */
uint8_t RCM_WaitHSI16Ready(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CTRL_B.HSI16RDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Waits for HSI16DIV be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     16MHz HSI divider by 4
 *              ERROR       16MHz HSI not divider by 4
 */
uint8_t RCM_WaitHSI16DIVReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CTRL_B.HSI16DIVFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Enable MSI
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableMSI(void)
{
    RCM->CTRL_B.MSIEN = BIT_SET;
}

/*!
 * @brief       Disable MSI
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableMSI(void)
{
    RCM->CTRL_B.MSIEN = BIT_RESET;
}

/*!
 * @brief       Waits for MSI be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     MSI oscillator is stable and ready to use
 *              ERROR       MSI oscillator not yet ready
 */
uint8_t RCM_WaitMSIReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CTRL_B.MSIRDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Enable Clock Security System Enable
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableCSS(void)
{
    RCM->CTRL_B.CSSEN = BIT_SET;
}

/*!
 * @brief       Disable Clock Security System Enable
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableCSS(void)
{
    RCM->CTRL_B.CSSEN = BIT_RESET;
}

/*!
 * @brief       Configures RTC Clock Prescaler Factor Configure
 *
 * @param       state:   state of the RTCPSC
 *                       This parameter can be one of the following values:
 *                       @arg RCM_RTCPSC_2
 *                       @arg RCM_RTCPSC_4
 *                       @arg RCM_RTCPSC_8
 *                       @arg RCM_RTCPSC_16
 *
 * @retval      None
 */
void RCM_ConfigRTC(RCM_RTCPSC_T state)
{
    RCM->CTRL_B.RTCPSC = state;
}

/*!
 * @brief       Enable PLL
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnablePLL(void)
{
    RCM->CTRL_B.PLLEN = BIT_SET;
}

/*!
 * @brief       Disable PLL
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisablePLL(void)
{
    RCM->CTRL_B.PLLEN = BIT_RESET;
}

/*!
 * @brief       Waits for PLL be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     PLL Clock is stable and ready to use
 *              ERROR       PLL Clock not yet ready
 */
uint8_t RCM_WaitPLLReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CTRL_B.PLLRDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Set HSI16 trimming value
 *
 * @param        hsi16Trim: HSI16 trimming value
 *                          This parameter must be a number between 0 and 0x1F.
 *
 * @retval      None
 */
void RCM_SetHSI16Trim(uint8_t hsi16Trim)
{
    RCM->ICLKCAL_B.HSI16TMR = hsi16Trim;
}

/*!
 * @brief       Set MSICLK trimming value
 *
 * @param        msiTrim: MSICLK trimming value
 *                        This parameter must be a number between 0 and 0x1F.
 *
 * @retval      None
 */
void RCM_SetMSITrim(uint8_t msiTrim)
{
    RCM->ICLKCAL_B.MSITIM = msiTrim;
}

/*!
 * @brief       Configures MSICLK Frequency Range
 *
 * @param       state: state of the MSICLK Frequency Range
 *                     This parameter can be one of the following values:
 *                     @arg RCM_MSIR_0
 *                     @arg RCM_MSIR_1
 *                     @arg RCM_MSIR_2
 *                     @arg RCM_MSIR_3
 *                     @arg RCM_MSIR_4
 *                     @arg RCM_MSIR_5
 *                     @arg RCM_MSIR_6
 *
 * @retval      None
 */
void RCM_ConfigMSIFreqRange(RCM_MSIR_T state)
{
    RCM->ICLKCAL_B.MSIR = state;
}

/*!
 * @brief       Enable HSI48
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableHSI48(void)
{
    RCM->HSI48CFG_B.HSI48EN = BIT_SET;
}

/*!
 * @brief       Disable HSI48
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableHSI48(void)
{
    RCM->HSI48CFG_B.HSI48EN = BIT_RESET;
}

/*!
 * @brief       Enable HSI48DIV
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableHSI48DIV(void)
{
    RCM->HSI48CFG_B.HSI48DIVEN = BIT_SET;
}

/*!
 * @brief       Disable HSI48DIV
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableHSI48DIV(void)
{
    RCM->HSI48CFG_B.HSI48DIVEN = BIT_RESET;
}

/*!
 * @brief       Waits for HSI48 be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     HSI48 is stable and ready to use
 *              ERROR       HSI48 not yet ready
 */
uint8_t RCM_WaitHSI48Ready(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->HSI48CFG_B.HSI48RDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Configures the system clock
 *
 * @param       sysClkSelect:   specifies the clock source used as system clock
 *                              This parameter can be one of the following values:
 *                              @arg RCM_SYSCLK_SEL_MSI:  MSI selected as system clock source
 *                              @arg RCM_SYSCLK_SEL_HSI16:  HSI16 selected as system clock source
 *                              @arg RCM_SYSCLK_SEL_HSE:  HSE selected as system clock source
 *                              @arg RCM_SYSCLK_SEL_PLL:  PLL selected as system clock source,It's only for 072 devices
 *
 * @retval      None
 */
void RCM_ConfigSYSClock(RCM_SYSCLK_SEL_T sysClkSelect)
{
    RCM->CFG_B.SCLKSEL = sysClkSelect;
}

/*!
 * @brief       returns the clock source used as system clock
 *
 * @param       None
 *
 * @retval      The clock source used as system clock
 */
RCM_SYSCLK_SEL_T RCM_ReadSYSClockSource(void)
{
    RCM_SYSCLK_SEL_T sysClock;

    sysClock = (RCM_SYSCLK_SEL_T)RCM->CFG_B.SCLKSWSTS;

    return sysClock;
}

/*!
 * @brief       Configures the AHB clock
 *
 * @param       ahbDiv:   AHB divider number. This clock is derived from the system clock (SYSCLK)
 *                        This parameter can be one of the following values:
 *                        @arg RCM_SYSCLK_DIV_1:   AHB clock = SYSCLK
 *                        @arg RCM_SYSCLK_DIV_2:   AHB clock = SYSCLK/2
 *                        @arg RCM_SYSCLK_DIV_4:   AHB clock = SYSCLK/4
 *                        @arg RCM_SYSCLK_DIV_8:   AHB clock = SYSCLK/8
 *                        @arg RCM_SYSCLK_DIV_16:  AHB clock = SYSCLK/16
 *                        @arg RCM_SYSCLK_DIV_64:  AHB clock = SYSCLK/64
 *                        @arg RCM_SYSCLK_DIV_128: AHB clock = SYSCLK/128
 *                        @arg RCM_SYSCLK_DIV_256: AHB clock = SYSCLK/256
 *                        @arg RCM_SYSCLK_DIV_512: AHB clock = SYSCLK/512
 *
 * @retval      None
 */
void RCM_ConfigAHB(RCM_AHBDIV_T ahbDiv)
{
    RCM->CFG_B.AHBPSC = ahbDiv;
}

/*!
 * @brief       Configures the APB1 clock
 *
 * @param       apbDiv:   defines the APB clock divider. This clock is derived from the AHB clock (HCLK)
 *                        This parameter can be one of the following values:
 *                        @arg RCM_HCLK_DIV_1:  APB clock = HCLK
 *                        @arg RCM_HCLK_DIV_2:  APB clock = HCLK/2
 *                        @arg RCM_HCLK_DIV_4:  APB clock = HCLK/4
 *                        @arg RCM_HCLK_DIV_8:  APB clock = HCLK/8
 *                        @arg RCM_HCLK_DIV_16: APB clock = HCLK/16
 *
 * @retval      None
 */
void RCM_ConfigAPB1(RCM_APBDIV_T apbDiv)
{
    RCM->CFG_B.APB1PSC = apbDiv;
}

/*!
 * @brief       Configures the APB2 clock
 *
 * @param       apbDiv:   defines the APB clock divider. This clock is derived from the AHB clock (HCLK)
 *                        This parameter can be one of the following values:
 *                        @arg RCM_HCLK_DIV_1:  APB clock = HCLK
 *                        @arg RCM_HCLK_DIV_2:  APB clock = HCLK/2
 *                        @arg RCM_HCLK_DIV_4:  APB clock = HCLK/4
 *                        @arg RCM_HCLK_DIV_8:  APB clock = HCLK/8
 *                        @arg RCM_HCLK_DIV_16: APB clock = HCLK/16
 *
 * @retval      None
 */
void RCM_ConfigAPB2(RCM_APBDIV_T apbDiv)
{
    RCM->CFG_B.APB2PSC = apbDiv;
}

/*!
 * @brief       Configures the clock wake up from STOP moudle
 *
 * @param       wuClkSelect:   specifies the clock source used as system clock
 *                              This parameter can be one of the following values:
 *                              @arg RCM_WUCLK_SEL_MSI
 *                              @arg RCM_WUCLK_SEL_HSI16
 *
 * @retval      None
 */
void RCM_ConfigWakeUpClock(RCM_WUCLK_SEL_T wuClkSelect)
{
    RCM->CFG_B.WUCLKSEL = wuClkSelect;
}

/*!
 * @brief       Configures the PLL Input Clock
 *
 * @param       pllInSelect:   This parameter can be one of the following values:
 *                              @arg RCM_PLLIN_SEL_HSI16
 *                              @arg RCM_PLLIN_SEL_HSE
 *
 * @retval      None
 */
void RCM_ConfigPLLInClock(RCM_PLLIN_SEL_T pllInSelect)
{
    RCM->CFG_B.PLLINSEL = pllInSelect;
}

/*!
 * @brief       Configures the PLL Multiplication Factor
 *
 * @param       pllMul:   This parameter can be one of the following values:
 *                        @arg RCM_PLL_MUL_3
 *                        @arg RCM_PLL_MUL_4
 *                        @arg RCM_PLL_MUL_6
 *                        @arg RCM_PLL_MUL_8
 *                        @arg RCM_PLL_MUL_12
 *                        @arg RCM_PLL_MUL_16
 *                        @arg RCM_PLL_MUL_24
 *                        @arg RCM_PLL_MUL_32
 *                        @arg RCM_PLL_MUL_48
 *
 * @retval      VCO = PLLCLK*pllMul
 */
void RCM_ConfigPLLMUL(RCM_PLL_MUL_T pllMul)
{
    RCM->CFG_B.PLLMUL = pllMul;
}

/*!
 * @brief       Configures the PLL Output Clock Divider Factor
 *
 * @param       pllOutDiv:   This parameter can be one of the following values:
 *                           @arg RCM_PLLOUT_DIV_2
 *                           @arg RCM_PLLOUT_DIV_3
 *                           @arg RCM_PLLOUT_DIV_4
 *
 * @retval      f output = VCO/PLLOUTDIV
 */
void RCM_ConfigPLLOut(RCM_PLLOUT_DIV_T pllOutDiv)
{
    RCM->CFG_B.PLLOUTDIV = pllOutDiv;
}

/*!
 * @brief       Selects MCO clock ouput source
 *
 * @param       mcoClock:  specifies the MCO clock source to output
 *                         This parameter can be one of the following values:
 *                         @arg RCM_MCO_SEL_NOCLOCK
 *                         @arg RCM_MCO_SEL_SYS
 *                         @arg RCM_MCO_SEL_HSI16
 *                         @arg RCM_MCO_SEL_MSI
 *                         @arg RCM_MCO_SEL_HSE
 *                         @arg RCM_MCO_SEL_PLL
 *                         @arg RCM_MCO_SEL_LSI
 *                         @arg RCM_MCO_SEL_LSE
 *                         @arg RCM_MCO_SEL_HSI
 *
 * @retval      None
 */
void RCM_ConfigMCOClock(RCM_MCO_SEL_T mcoClock)
{
    RCM->CFG_B.MCOSEL = mcoClock;
}

/*!
 * @brief       Configures MCO Clock Output Prescaler Factor
 *
 * @param       mcoPsc:   This parameter can be one of the following values:
 *                           @arg RCM_MCO_PSC_1
 *                           @arg RCM_MCO_PSC_2
 *                           @arg RCM_MCO_PSC_4
 *                           @arg RCM_MCO_PSC_8
 *                           @arg RCM_MCO_PSC_16
 *
 * @retval      None
 */
void RCM_ConfigMCO(RCM_MCO_PSC_T mcoPsc)
{
    RCM->CFG_B.MCOPSC = mcoPsc;
}

/*!
 * @brief       Configures the USART1 clock
 *
 * @param       usart1Clk:   defines the USART1 clock divider.
 *                           This parameter can be one of the following values:
 *                           @arg RCM_CLKS_CEL_APB
 *                           @arg RCM_CLKS_CEL_SYS
 *                           @arg RCM_CLKS_CEL_HSI16
 *                           @arg RCM_CLKS_CEL_LSE
 *
 * @retval      None
 */
void RCM_ConfigUSART1Clock(RCM_CLKS_CEL_T usart1Clk)
{
    RCM->CLKSEL_B.USART1SEL = usart1Clk;
}

/*!
 * @brief       Configures the USART2 clock
 *
 * @param       usart2Clk:   defines the USART2 clock divider.
 *                           This parameter can be one of the following values:
 *                           @arg RCM_CLKS_CEL_APB
 *                           @arg RCM_CLKS_CEL_SYS
 *                           @arg RCM_CLKS_CEL_HSI16
 *                           @arg RCM_CLKS_CEL_LSE
 *
 * @retval      None
 */
void RCM_ConfigUSART2Clock(RCM_CLKS_CEL_T usart2Clk)
{
    RCM->CLKSEL_B.USART2SEL = usart2Clk;
}

/*!
 * @brief       Configures the LPUART1 clock
 *
 * @param       lpUart1Clk:   defines the LPUART1 clock divider.
 *                           This parameter can be one of the following values:
 *                           @arg RCM_CLKS_CEL_APB
 *                           @arg RCM_CLKS_CEL_SYS
 *                           @arg RCM_CLKS_CEL_HSI16
 *                           @arg RCM_CLKS_CEL_LSE
 *
 * @retval      None
 */
void RCM_ConfigLPUART1Clock(RCM_CLKS_CEL_T lpUart1Clk)
{
    RCM->CLKSEL_B.LPUART1SEL = lpUart1Clk;
}

/*!
 * @brief       Configures the I2C1 clock
 *
 * @param       i2c1Clk:   defines the I2C1 clock divider.
 *                         This parameter can be one of the following values:
 *                         @arg RCM_CLKS_CEL_APB
 *                         @arg RCM_CLKS_CEL_SYS
 *                         @arg RCM_CLKS_CEL_HSI16
 *
 * @retval      None
 */
void RCM_ConfigI2C1Clock(RCM_CLKS_CEL_T i2c1Clk)
{
    RCM->CLKSEL_B.I2C1SEL = i2c1Clk;
}

/*!
 * @brief       Configures the I2C3 clock
 *
 * @param       i2c3Clk:   defines the I2C3 clock divider.
 *                         This parameter can be one of the following values:
 *                         @arg RCM_CLKS_CEL_APB
 *                         @arg RCM_CLKS_CEL_SYS
 *                         @arg RCM_CLKS_CEL_HSI16
 *
 * @retval      None
 */
void RCM_ConfigI2C3Clock(RCM_CLKS_CEL_T i2c3Clk)
{
    RCM->CLKSEL_B.I2C3SEL = i2c3Clk;
}

/*!
 * @brief       Configures the LPTMR1 clock
 *
 * @param       lpTmr1Clk:   defines the LPTMR1 clock divider.
 *                           This parameter can be one of the following values:
 *                           @arg RCM_LPTMR1_CLKS_APB
 *                           @arg RCM_LPTMR1_CLKS_LSI
 *                           @arg RCM_LPTMR1_CLKS_HSI
 *                           @arg RCM_LPTMR1_CLKS_LSE
 *
 * @retval      None
 */
void RCM_ConfigLPTMR1Clock(RCM_LPTMR1_CLKS_T lpTmr1Clk)
{
    RCM->CLKSEL_B.LPTMR1SEL = lpTmr1Clk;
}

/*!
 * @brief       Configures the HSICLK48 clock
 *
 * @param       hsi48Clk:   defines the HSICLK48 clock divider.
 *                          This parameter can be one of the following values:
 *                          @arg RCM_HSI48_SEL_PLL_USB
 *                          @arg RCM_HSI48_SEL_HSI48
 *
 * @retval      None
 */
void RCM_ConfigHSI48Clock(RCM_HSI48_SEL_T hsi48Clk)
{
    RCM->CLKSEL_B.HSI48SEL = hsi48Clk;
}

/*!
 * @brief       Enable Low-Speed Internal Oscillator
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableLSI(void)
{
    RCM->CSTS_B.LSIEN = BIT_SET;
}

/*!
 * @brief       Disable Low-Speed Internal Oscillator
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableLSI(void)
{
    RCM->CSTS_B.LSIEN = BIT_RESET;
}

/*!
 * @brief       Waits for LSI be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     LSI oscillator is stable and ready to use
 *              ERROR       LSI oscillator not yet ready
 */
uint8_t RCM_WaitLSIReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CSTS_B.LSIRDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Configures LSE
 *
 * @param       state:   state of the LSE
 *                       This parameter can be one of the following values:
 *                       @arg RCM_LSE_CLOSE: turn OFF the LSE oscillator
 *                       @arg RCM_LSE_OPEN: turn ON the LSE oscillator
 *                       @arg RCM_LSE_BYPASS: LSE oscillator bypassed with external clock
 *
 * @retval      None
 */
void RCM_ConfigLSE(RCM_LSE_T state)
{
    RCM->CSTS_B.LSEEN = BIT_RESET;

    RCM->CSTS_B.LSEBCFG = BIT_RESET;

    if (state == RCM_LSE_OPEN)
    {
        RCM->CSTS_B.LSEEN = BIT_SET;
    }
    else if (state == RCM_LSE_BYPASS)
    {
        RCM->CSTS_B.LSEBCFG = BIT_SET;
        RCM->CSTS_B.LSEEN = BIT_SET;
    }
}

/*!
 * @brief       Waits for LSE be ready
 *
 * @param       None
 *
 * @retval      SUCCESS     LSE oscillator is stable and ready to use
 *              ERROR       LSE oscillator not yet ready
 */
uint8_t RCM_WaitLSEReady(void)
{
    __IO uint32_t cnt;

    for (cnt = 0; cnt < (uint32_t)0x5000; cnt++)
    {
        if (RCM->CSTS_B.LSERDYFLG == BIT_SET)
        {
            return SUCCESS;
        }
    }
    return ERROR;
}

/*!
 * @brief       Configures LSE drive capability
 *
 * @param       state:   State of the LSE Drive
 *                       This parameter can be one of the following values:
 *                       @arg RCM_LSE_DRIVE_Low: LSE oscillator low drive capability.
 *                       @arg RCM_LSE_DRIVE_MediumLow: LSE oscillator medium low drive capability.
 *                       @arg RCM_LSE_DRIVE_MediumHigh: LSE oscillator medium high drive capability.
 *                       @arg RCM_LSE_DRIVE_High: LSE oscillator high drive capability.
 *
 * @retval      None
 */
void RCM_ConfigDriveLSE(RCM_LSE_DRIVE_T state)
{
    RCM->CSTS_B.LSEDRIVE = state;
}

/*!
 * @brief        Enable Clock Security System on LSE Clock
 *
 * @param        None
 *
 * @retval       None
 *
 * @note         This position can be set to 1 only after lsiclk and lsiclk are enabled and ready.
 */
void RCM_EnableCSSLSE(void)
{
    RCM->CSTS_B.CSSLEN = BIT_SET;
}

/*!
 * @brief        Disable Clock Security System on LSE Clock
 *
 * @param        None
 *
 * @retval       None
 */
void RCM_DisableCSSLSE(void)
{
    RCM->CSTS_B.CSSLEN = BIT_RESET;
}

/*!
 * @brief       CSS on LSECLK Detecte Failure Flag
 *
 * @param       None
 *
 * @retval      SUCCESS     CSS on LSECLK fault detected
 *              ERROR       CSS on LSECLK not fault detected
 */
uint8_t RCM_CSSLSEFail(void)
{
    if (RCM->CSTS_B.CSSLFAIL == BIT_SET)
    {
        return SUCCESS;
    }
    return ERROR;
}

/*!
 * @brief       Selects RTC Clock Source
 *
 * @param       rtcClock:  This parameter can be one of the following values:
 *                         @arg RCM_RTC_SEL_LSE
 *                         @arg RCM_RTC_SEL_LSI
 *                         @arg RCM_RTC_SEL_HSE
 *
 * @retval      None
 */
void RCM_ConfigRTCClock(RCM_RTC_SEL_T rtcClock)
{
    RCM->CSTS_B.RTCSEL = rtcClock;
}

/*!
 * @brief       Enable RTC Clock
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableRTCClock(void)
{
    RCM->CSTS_B.RTCEN = BIT_SET;
}

/*!
 * @brief       Disable RTC Clock
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableRTCClock(void)
{
    RCM->CSTS_B.RTCEN = BIT_RESET;
}

/*!
 * @brief       Enable RTC Reset
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_EnableRTCReset(void)
{
    RCM->CSTS_B.RTCRST = BIT_SET;
}

/*!
 * @brief       Disable RTC Reset
 *
 * @param       None
 *
 * @retval      None
 */
void RCM_DisableRTCReset(void)
{
    RCM->CSTS_B.RTCRST = BIT_RESET;
}

/*!
 * @brief       Read frequency of SYSCLK
 *
 * @param       None
 *
 * @retval      Return frequency of SYSCLK
 */
uint32_t RCM_ReadSYSClockFreq(void)
{
    uint32_t sysClock = 0, pllmul = 0, plldiv = 0, pllsource = 0, msirange = 0;
    const uint8_t PLLMulTable[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};

    sysClock = RCM->CFG_B.SCLKSWSTS;

    switch (sysClock)
    {
        /** sys clock is MSI */
        case 0:
            msirange = RCM->ICLKCAL_B.MSIR;
            SystemCoreClock = (32768 * (1 << (msirange + 1)));
            break;

        /** sys clock is HSI16 */
        case 1:
            SystemCoreClock = HSI16_VALUE;
            break;

        /** sys clock is HSE */
        case 2:
            SystemCoreClock = HSE_VALUE;
            break;

        /** sys clock is PLL */
        case 3:
            /** Get PLL clock source and multiplication factor */
            pllmul = PLLMulTable[RCM->CFG_B.PLLMUL];
            plldiv = RCM->CFG_B.PLLOUTDIV+1;
            pllsource = RCM->CFG_B.PLLINSEL;

            if (pllsource == 0)
            {
                /** HSI16 oscillator clock selected as PLL clock entry */
                SystemCoreClock = (((HSI16_VALUE) * pllmul) / plldiv);
            }
            else
            {
                /** HSE selected as PLL clock entry */
                SystemCoreClock = (((HSE_VALUE) * pllmul) / plldiv);
            }
            break;

        /** MSI used as system clock */
        default:
            msirange = RCM->ICLKCAL_B.MSIR;
            SystemCoreClock = (32768 * (1 << (msirange + 1)));
        break;
    }

    return SystemCoreClock;
}

/*!
 * @brief       Read frequency of AHBCLK
 *
 * @param       None
 *
 * @retval      Return frequency of AHBCLK
 */
uint32_t RCM_ReadAHBClockFreq(void)
{
    uint32_t divider;
    uint32_t sysClk, ahbClk;
    uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    sysClk = RCM_ReadSYSClockFreq();
    divider = AHBPrescTable[RCM->CFG_B.AHBPSC];
    ahbClk = sysClk >> divider;

    return ahbClk;
}

/*!
 * @brief       Read frequency of APB1CLK
 *
 * @param       None
 *
 * @retval      apb1Clk   return frequency of APB1CLK
 */
uint32_t RCM_ReadAPB1ClockFreq(void)
{
    uint32_t ahbClk, apb1Clk, divider;
    uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    ahbClk = RCM_ReadSYSClockFreq();

    divider = APBPrescTable[RCM->CFG_B.APB1PSC];
    apb1Clk = ahbClk >> divider;

    return apb1Clk;
}

/*!
 * @brief       Read frequency of APB2CLK
 *
 * @param       None
 *
 * @retval      apb2Clk   return frequency of APB2CLK
 */
uint32_t RCM_ReadAPB2ClockFreq(void)
{
    uint32_t ahbClk, apb2Clk, divider;
    uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

    ahbClk = RCM_ReadSYSClockFreq();

    divider = APBPrescTable[RCM->CFG_B.APB2PSC];
    apb2Clk = ahbClk >> divider;

    return apb2Clk;
}

/*!
 * @brief       Read frequency of PLLCLK
 *
 * @param       None
 *
 * @retval      pllClk   return frequency of PLLCLK
 */
uint32_t RCM_ReadPLLClockFreq(void)
{
    uint32_t pllmul, plldiv, pllsource, pllClk;
    const uint8_t PLLMulTable[9] = {3, 4, 6, 8, 12, 16, 24, 32, 48};

    pllmul = PLLMulTable[(RCM->CFG_B.PLLMUL)];
    plldiv = RCM->CFG_B.PLLOUTDIV+1;

    pllsource = RCM->CFG_B.PLLINSEL;

    if (pllsource == 0)
    {
        if(RCM->CTRL_B.HSI16DIVFLG == BIT_SET)
        {
            pllClk = (((HSI16_VALUE/4) * pllmul) / plldiv);
        }
        else
        {
            pllClk = (((HSI16_VALUE) * pllmul) / plldiv);
        }
    }
    else
    {
        pllClk = (((HSE_VALUE) * pllmul) / plldiv);
    }
    return pllClk;
}

/*!
 * @brief       Read frequency of USART1 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of USART1 CLK
 */
uint32_t RCM_ReadUSART1ClockFreq(void)
{
    uint32_t usart1Clk = 0;

    if (RCM->CLKSEL_B.USART1SEL==0x00)
    {
        usart1Clk = RCM_ReadAPB2ClockFreq();
    }
    else if (RCM->CLKSEL_B.USART1SEL==0x01)
    {
        usart1Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.USART1SEL==0x02)
    {
        usart1Clk = HSI16_VALUE;
    }
    else if (RCM->CLKSEL_B.USART1SEL==0x03)
    {
        usart1Clk = LSE_VALUE;
    }

    return usart1Clk;
}

/*!
 * @brief       Read frequency of USART2 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of USART2 CLK
 */
uint32_t RCM_ReadUSART2ClockFreq(void)
{
    uint32_t usart2Clk = 0;

    if (RCM->CLKSEL_B.USART2SEL==0x00)
    {
        usart2Clk = RCM_ReadAPB1ClockFreq();
    }
    else if (RCM->CLKSEL_B.USART2SEL==0x01)
    {
        usart2Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.USART2SEL==0x02)
    {
        usart2Clk = HSI16_VALUE;
    }
    else if (RCM->CLKSEL_B.USART2SEL==0x03)
    {
        usart2Clk = LSE_VALUE;
    }

    return usart2Clk;
}

/*!
 * @brief       Read frequency of LPUART1 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of LPUART1 CLK
 */
uint32_t RCM_ReadLPUART1ClockFreq(void)
{
    uint32_t lpUsart1Clk = 0;

    if (RCM->CLKSEL_B.LPUART1SEL==0x00)
    {
        lpUsart1Clk = RCM_ReadAPB1ClockFreq();
    }
    else if (RCM->CLKSEL_B.LPUART1SEL==0x01)
    {
        lpUsart1Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.LPUART1SEL==0x02)
    {
        lpUsart1Clk = HSI16_VALUE;
    }
    else if (RCM->CLKSEL_B.LPUART1SEL==0x03)
    {
        lpUsart1Clk = LSE_VALUE;
    }

    return lpUsart1Clk;
}

/*!
 * @brief       Read frequency of I2C1 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of I2C1 CLK
 */
uint32_t RCM_ReadI2C1ClockFreq(void)
{
    uint32_t i2c1Clk = 0;

    if (RCM->CLKSEL_B.I2C1SEL==0x00)
    {
        i2c1Clk = RCM_ReadAPB1ClockFreq();
    }
    else if (RCM->CLKSEL_B.I2C1SEL==0x01)
    {
        i2c1Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.I2C1SEL==0x02)
    {
        i2c1Clk = HSI16_VALUE;
    }

    return i2c1Clk;
}

/*!
 * @brief       Read frequency of I2C3 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of I2C3 CLK
 */
uint32_t RCM_ReadI2C3ClockFreq(void)
{
    uint32_t i2c3Clk = 0;

    if (RCM->CLKSEL_B.I2C3SEL==0x00)
    {
        i2c3Clk = RCM_ReadAPB1ClockFreq();
    }
    else if (RCM->CLKSEL_B.I2C3SEL==0x01)
    {
        i2c3Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.I2C3SEL==0x02)
    {
        i2c3Clk = HSI16_VALUE;
    }

    return i2c3Clk;
}

/*!
 * @brief       Read frequency of LPTMR1 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of LPTMR1 CLK
 */
uint32_t RCM_ReadLPTMR1ClockFreq(void)
{
    uint32_t lpTmr1Clk = 0;

    if (RCM->CLKSEL_B.LPTMR1SEL==0x00)
    {
        lpTmr1Clk = RCM_ReadAPB1ClockFreq();
    }
    else if (RCM->CLKSEL_B.LPTMR1SEL==0x01)
    {
        lpTmr1Clk = RCM_ReadSYSClockFreq();
    }
    else if (RCM->CLKSEL_B.LPTMR1SEL==0x02)
    {
        lpTmr1Clk = HSI16_VALUE;
    }
    else if (RCM->CLKSEL_B.LPTMR1SEL==0x03)
    {
        lpTmr1Clk = LSE_VALUE;
    }

    return lpTmr1Clk;
}

/*!
 * @brief       Read frequency of HSI48 CLK
 *
 * @param       None
 *
 * @retval      Return frequency of HSI48 CLK
 */
uint32_t RCM_ReadHSI48ClockFreq(void)
{
    uint32_t hsi48Clk;

    if (RCM->CLKSEL_B.HSI48SEL==0x00)
    {
        hsi48Clk = RCM_ReadPLLClockFreq();
    }
    else
    {
        hsi48Clk = HSI48_VALUE;
    }

    return hsi48Clk;
}

/*!
 * @brief       Enables GPIO clock
 *
 * @param       gpioClock:   specifies the GPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_EnableGPIOClock(uint32_t gpioClock)
{
    RCM->GPIOEN |= gpioClock;
}

/*!
 * @brief       Disable GPIO clock
 *
 * @param       gpioClock:   specifies the GPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_DisableGPIOClock(uint32_t gpioClock)
{
    RCM->GPIOEN &= (uint32_t)~gpioClock;
}

/*!
 * @brief       Enables AHB peripheral clock
 *
 * @param       AHBPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->AHBEN |= AHBPeriph;
}

/*!
 * @brief       Disable AHB peripheral clock
 *
 * @param       AHBPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_DisableAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->AHBEN &= (uint32_t)~AHBPeriph;
}

/*!
 * @brief       Enables APB2 peripheral clock
 *
 * @param       APB2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_FWL
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph)
{
    RCM->APB2EN |= APB2Periph;
}

/*!
 * @brief       Disable APB2 peripheral clock
 *
 * @param       APB2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_FWL
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_DisableAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph)
{
    RCM->APB2EN &= (uint32_t)~APB2Periph;
}

/*!
 * @brief       Enables APB1 peripheral clock
 *
 * @param       APB1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_LCD
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph)
{
    RCM->APB1EN |= APB1Periph;
}

/*!
 * @brief       Disable APB1 peripheral clock
 *
 * @param       APB1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_LCD
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_DisableAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph)
{
    RCM->APB1EN &= (uint32_t)~APB1Periph;
}

/*!
 * @brief       Enables SLPGPIO clock
 *
 * @param       gpioClock:   specifies the SLPGPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_EnableSleepGPIOClock(RCM_GPIO_CLOCK_T gpioClock)
{
    RCM->SLPGPIOEN |= gpioClock;
}

/*!
 * @brief       Disable SLPGPIO clock
 *
 * @param       gpioClock:   specifies the SLPGPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_DisableSleepGPIOClock(RCM_GPIO_CLOCK_T gpioClock)
{
    RCM->SLPGPIOEN &= (uint32_t)~gpioClock;
}

/*!
 * @brief       Enables AHB peripheral clock
 *
 * @param       AHBPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_SRAM
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_EnableSleepAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->SLPAHBEN |= AHBPeriph;
}

/*!
 * @brief       Disable SLPAHB peripheral clock
 *
 * @param       AHBPeriph:   specifies the SLPAHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_SRAM
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_DisableSleepAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->SLPAHBEN &= (uint32_t)~AHBPeriph;
}

/*!
 * @brief       Enables SLPAPB2 peripheral clock
 *
 * @param       APB2Periph:  specifies the SLPAPB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_EnableSleepAPB2PeriphClock(RCM_APB2_PERIPH_T apb2Periph)
{
    RCM->SLPAPB2EN |= apb2Periph;
}

/*!
 * @brief       Disable SLPAPB2 peripheral clock
 *
 * @param       APB2Periph:  specifies the SLPAPB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_DisableSleepAPB2PeriphClock(RCM_APB2_PERIPH_T apb2Periph)
{
    RCM->SLPAPB2EN &= (uint32_t)~apb2Periph;
}

/*!
 * @brief       Enables SLPAPB1 peripheral clock
 *
 * @param       APB1Periph:  specifies the SLPAPB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_LCD
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_EnableSleepAPB1PeriphClock(RCM_APB1_PERIPH_T apb1Periph)
{
    RCM->SLPAPB1EN |= apb1Periph;
}

/*!
 * @brief       Disable SLPAPB1 peripheral clock
 *
 * @param       APB1Periph:  specifies the SLPAPB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_LCD
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_DisableSleepAPB1PeriphClock(RCM_APB1_PERIPH_T apb1Periph)
{
    RCM->SLPAPB1EN &= (uint32_t)~apb1Periph;
}

/*!
 * @brief       Enables GPIO Reset
 *
 * @param       gpioClock:   specifies the GPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_EnableGPIOReset(RCM_GPIO_CLOCK_T gpioClock)
{
    RCM->GPIORST |= gpioClock;
}

/*!
 * @brief       Disable GPIO Reset
 *
 * @param       gpioClock:   specifies the GPIO to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_GPIO_CLOCK_A
 *                           @arg RCM_GPIO_CLOCK_B
 *                           @arg RCM_GPIO_CLOCK_C
 *                           @arg RCM_GPIO_CLOCK_D
 *                           @arg RCM_GPIO_CLOCK_E
 *                           @arg RCM_GPIO_CLOCK_H
 *
 * @retval      None
 */
void RCM_DisableGPIOReset(RCM_GPIO_CLOCK_T gpioClock)
{
    RCM->GPIORST &= (uint32_t)~gpioClock;
}

/*!
 * @brief       Enables AHB peripheral Reset
 *
 * @param       AHBPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_EnableAHBPeriphReset(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->AHBRST |= AHBPeriph;
}

/*!
 * @brief       Disable AHB peripheral Reset
 *
 * @param       AHBPeriph:   specifies the AHB peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_AHB_PERIPH_DMA
 *                           @arg RCM_AHB_PERIPH_MEMITF
 *                           @arg RCM_AHB_PERIPH_CRC
 *                           @arg RCM_AHB_PERIPH_TSC
 *                           @arg RCM_AHB_PERIPH_RNG
 *                           @arg RCM_AHB_PERIPH_CRYP
 *
 * @retval      None
 */
void RCM_DisableAHBPeriphReset(RCM_AHB_PERIPH_T AHBPeriph)
{
    RCM->AHBRST &= (uint32_t)~AHBPeriph;
}

/*!
 * @brief       Enables APB2 peripheral Reset
 *
 * @param       APB2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_T apb2Periph)
{
    RCM->APB2RST |= apb2Periph;
}

/*!
 * @brief       Disable APB2 peripheral Reset
 *
 * @param       APB2Periph:  specifies the APB2 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB2_PERIPH_SYSCFG
 *                           @arg RCM_APB2_PERIPH_TMR21
 *                           @arg RCM_APB2_PERIPH_TMR22
 *                           @arg RCM_APB2_PERIPH_ADC
 *                           @arg RCM_APB2_PERIPH_SPI1
 *                           @arg RCM_APB2_PERIPH_USART1
 *                           @arg RCM_APB2_PERIPH_DBG
 *
 * @retval      None
 */
void RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_T apb2Periph)
{
    RCM->APB2RST &= (uint32_t)~apb2Periph;
}

/*!
 * @brief       Enables APB1 peripheral Reset
 *
 * @param       APB1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_T apb1Periph)
{
    RCM->APB1RST |= apb1Periph;
}

/*!
 * @brief       Disable APB1 peripheral Reset
 *
 * @param       APB1Periph:  specifies the APB1 peripheral to gates its clock
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_APB1_PERIPH_TMR2
 *                           @arg RCM_APB1_PERIPH_TMR3
 *                           @arg RCM_APB1_PERIPH_TMR6
 *                           @arg RCM_APB1_PERIPH_TMR7
 *                           @arg RCM_APB1_PERIPH_WWDT
 *                           @arg RCM_APB1_PERIPH_SPI2
 *                           @arg RCM_APB1_PERIPH_USART2
 *                           @arg RCM_APB1_PERIPH_LPUSART1
 *                           @arg RCM_APB1_PERIPH_USART4
 *                           @arg RCM_APB1_PERIPH_USART5
 *                           @arg RCM_APB1_PERIPH_I2C1
 *                           @arg RCM_APB1_PERIPH_I2C2
 *                           @arg RCM_APB1_PERIPH_USB
 *                           @arg RCM_APB1_PERIPH_CRS
 *                           @arg RCM_APB1_PERIPH_PMU
 *                           @arg RCM_APB1_PERIPH_DAC
 *                           @arg RCM_APB1_PERIPH_I2C3
 *                           @arg RCM_APB1_PERIPH_LPTMR1
 *
 * @retval      None
 */
void RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_T apb1Periph)
{
    RCM->APB1RST &= (uint32_t)~apb1Periph;
}

/*!
 * @brief       Enable the specified RCM interrupts
 *
 * @param       interrupt:   specifies the RCM interrupt source to check
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_INT_LSI
 *                           @arg RCM_INT_LSE
 *                           @arg RCM_INT_HSI16
 *                           @arg RCM_INT_HSE
 *                           @arg RCM_INT_PLL
 *                           @arg RCM_INT_MSI
 *                           @arg RCM_INT_HSI48
 *                           @arg RCM_INT_CSSL
 *
 * @retval  None
 */
void RCM_EnableInterrupt(RCM_INT_T interrupt)
{
    RCM->INTEN |= (uint8_t)interrupt;
}

/*!
 * @brief       Disable the specified RCM interrupts
 *
 * @param       interrupt:   specifies the RCM interrupt source to check
 *                           This parameter can be any combination of the following values:
 *                           @arg RCM_INT_LSI
 *                           @arg RCM_INT_LSE
 *                           @arg RCM_INT_HSI16
 *                           @arg RCM_INT_HSE
 *                           @arg RCM_INT_PLL
 *                           @arg RCM_INT_MSI
 *                           @arg RCM_INT_HSI48
 *                           @arg RCM_INT_CSSL
 *
 * @retval  None
 */
void RCM_DisableInterrupt(uint8_t interrupt)
{
    RCM->INTEN &= (uint8_t)~interrupt;
}

/*!
 * @brief       Read the specified RCM flag status
 *
 * @param       flag:   specifies the flag to check
 *                      This parameter can be one of the following values:
 *                      @arg RCM_FLAG_HSI16RDY
 *                      @arg RCM_FLAG_HSI16DIV
 *                      @arg RCM_FLAG_MSIRDY
 *                      @arg RCM_FLAG_HSERDY
 *                      @arg RCM_FLAG_PLLRDY
 *                      @arg RCM_FLAG_HSI48RDY
 *                      @arg RCM_FLAG_LSIRDY
 *                      @arg RCM_FLAG_LSERDY
 *                      @arg RCM_FLAG_CSSLFAIL
 *                      @arg RCM_FLAG_FWLRST
 *                      @arg RCM_FLAG_OBLRST
 *                      @arg RCM_FLAG_PINRST
 *                      @arg RCM_FLAG_PODRRST
 *                      @arg RCM_FLAG_SWRST
 *                      @arg RCM_FLAG_IWDTRST
 *                      @arg RCM_FLAG_WWDTRST
 *                      @arg RCM_FLAG_LPRRST
 *
 * @retval      The new state of flag (SET or RESET)
 */
uint16_t RCM_ReadStatusFlag(RCM_FLAG_T flag)
{
    uint32_t reg, bit;

    bit = (uint32_t)(1 << (flag & 0xff));

    reg = (flag >> 8) & 0xff;

    switch (reg)
    {
        case 0:
            reg = RCM->CTRL;
            break;

        case 1:
            reg = RCM->HSI48CFG;
            break;

        case 2:
            reg = RCM->CSTS;
            break;

        default:
            break;
    }

    if (reg & bit)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clears the RCM reset flags
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        The reset flags are:
 *              @arg RCM_FLAG_FWLRST
 *              @arg RCM_FLAG_OBLRST
 *              @arg RCM_FLAG_PINRST
 *              @arg RCM_FLAG_PODRRST
 *              @arg RCM_FLAG_SWRST
 *              @arg RCM_FLAG_IWDTRST
 *              @arg RCM_FLAG_WWDTRST
 *              @arg RCM_FLAG_LPRRST
 */
void RCM_ClearStatusFlag(void)
{
    RCM->CSTS_B.RSTFLGCLR = BIT_SET;
}

/*!
 * @brief       Read the specified RCM interrupt Flag
 *
 * @param       flag:   specifies the RCM interrupt source to check
 *                      This parameter can be one of the following values:
 *                      @arg RCM_INT_LSI
 *                      @arg RCM_INT_LSE
 *                      @arg RCM_INT_HSI16
 *                      @arg RCM_INT_HSE
 *                      @arg RCM_INT_PLL
 *                      @arg RCM_INT_MSI
 *                      @arg RCM_INT_HSI48
 *                      @arg RCM_INT_CSSL
 *                      @arg RCM_INT_HCSS
 *
 * @retval      The new state of intFlag (SET or RESET)
 */
uint8_t RCM_ReadIntFlag(RCM_INT_T flag)
{
    uint8_t ret;

    ret = (RCM->INTSTS & flag) ? SET : RESET;

    return  ret;
}

/*!
 * @brief       Clears the interrupt flag
 *
 * @param       flag:   specifies the RCM interrupt source to check
 *                      This parameter can be any combination of the following values:
 *                      @arg RCM_INT_LSI
 *                      @arg RCM_INT_LSE
 *                      @arg RCM_INT_HSI16
 *                      @arg RCM_INT_HSE
 *                      @arg RCM_INT_PLL
 *                      @arg RCM_INT_MSI
 *                      @arg RCM_INT_HSI48
 *                      @arg RCM_INT_CSSL
 *                      @arg RCM_INT_HCSS
 *
 * @retval      None
 */
void RCM_ClearIntFlag(uint8_t flag)
{
    RCM->INTCLR |= flag;
}

/**@} end of group RCM_Fuctions*/
/**@} end of group RCM_Driver*/
/**@} end of group Peripherals_Library*/
