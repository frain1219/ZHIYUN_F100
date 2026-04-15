/*!
 * @file        apm32l0xx_rcm.h
 *
 * @brief       This file contains all the functions prototypes for the RCM firmware library
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

#ifndef __APM32L0xx_RCM_H
#define __APM32L0xx_RCM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup RCM_Driver RCM Driver
  @{
*/
/** @addtogroup RCM_Enumerations Enumerations
  @{
*/

/**
 * @brief   HSE enum
 */
typedef enum
{
    RCM_HSE_CLOSE,
    RCM_HSE_OPEN,
    RCM_HSE_BYPASS
} RCM_HSE_T;

/**
 * @brief   RTCPSC enum
 */
typedef enum
{
    RCM_RTCPSC_2,
    RCM_RTCPSC_4,
    RCM_RTCPSC_8,
    RCM_RTCPSC_16
} RCM_RTCPSC_T;

/**
 * @brief   MSIR enum
 */
typedef enum
{
    RCM_MSIR_0,
    RCM_MSIR_1,
    RCM_MSIR_2,
    RCM_MSIR_3,
    RCM_MSIR_4,
    RCM_MSIR_5,
    RCM_MSIR_6
} RCM_MSIR_T;

/**
 * @brief   System clock select
 */
typedef enum
{
    RCM_SYSCLK_SEL_MSI,
    RCM_SYSCLK_SEL_HSI16,
    RCM_SYSCLK_SEL_HSE,
    RCM_SYSCLK_SEL_PLL
} RCM_SYSCLK_SEL_T;

/**
 * @brief   AHB divider Number
 */
typedef enum
{
    RCM_SYSCLK_DIV_1 = 7,
    RCM_SYSCLK_DIV_2,
    RCM_SYSCLK_DIV_4,
    RCM_SYSCLK_DIV_8,
    RCM_SYSCLK_DIV_16,
    RCM_SYSCLK_DIV_64,
    RCM_SYSCLK_DIV_128,
    RCM_SYSCLK_DIV_256,
    RCM_SYSCLK_DIV_512,
} RCM_AHBDIV_T;

/**
 * @brief   APB divider Number
 */
typedef enum
{
    RCM_HCLK_DIV_1 = 3,
    RCM_HCLK_DIV_2,
    RCM_HCLK_DIV_4,
    RCM_HCLK_DIV_8,
    RCM_HCLK_DIV_16
} RCM_APBDIV_T;

/**
 * @brief   Wake up clock select
 */
typedef enum
{
    RCM_WUCLK_SEL_MSI,
    RCM_WUCLK_SEL_HSI16
} RCM_WUCLK_SEL_T;

/**
 * @brief   Wake up clock select
 */
typedef enum
{
    RCM_PLLIN_SEL_HSI16,
    RCM_PLLIN_SEL_HSE
} RCM_PLLIN_SEL_T;

/**
 * @brief   PLL Multiplication Factor Configure
 */
typedef enum
{
    RCM_PLL_MUL_3,
    RCM_PLL_MUL_4,
    RCM_PLL_MUL_6,
    RCM_PLL_MUL_8,
    RCM_PLL_MUL_12,
    RCM_PLL_MUL_16,
    RCM_PLL_MUL_24,
    RCM_PLL_MUL_32,
    RCM_PLL_MUL_48
} RCM_PLL_MUL_T;

/**
 * @brief   PLL Output Clock Divider Factor Configure
 */
typedef enum
{
    RCM_PLLOUT_DIV_2 = 1,
    RCM_PLLOUT_DIV_3,
    RCM_PLLOUT_DIV_4
} RCM_PLLOUT_DIV_T;

/**
 * @brief   MCO Clock output
 */
typedef enum
{
    RCM_MCO_SEL_NOCLOCK,
    RCM_MCO_SEL_SYS,
    RCM_MCO_SEL_HSI16,
    RCM_MCO_SEL_MSI,
    RCM_MCO_SEL_HSE,
    RCM_MCO_SEL_PLL,
    RCM_MCO_SEL_LSI,
    RCM_MCO_SEL_LSE,
    RCM_MCO_SEL_HSI
} RCM_MCO_SEL_T;

/**
 * @brief   MCO Clock Output Prescaler Factor Configure
 */
typedef enum
{
    RCM_MCO_PSC_1,
    RCM_MCO_PSC_2,
    RCM_MCO_PSC_4,
    RCM_MCO_PSC_8,
    RCM_MCO_PSC_16
} RCM_MCO_PSC_T;

/**
 * @brief   USART/LPUART1/I2C Clock Source Select
 */
typedef enum
{
    RCM_CLKS_CEL_APB,
    RCM_CLKS_CEL_SYS,
    RCM_CLKS_CEL_HSI16,
    RCM_CLKS_CEL_LSE
} RCM_CLKS_CEL_T;

/**
 * @brief   LPTMR1 Clock Source Select
 */
typedef enum
{
    RCM_LPTMR1_CLKS_APB,
    RCM_LPTMR1_CLKS_LSI,
    RCM_LPTMR1_CLKS_HSI,
    RCM_LPTMR1_CLKS_LSE
} RCM_LPTMR1_CLKS_T;

/**
 * @brief   HSICLK48 Clock Source Select
 */
typedef enum
{
    RCM_HSI48_SEL_PLL_USB,
    RCM_HSI48_SEL_HSI48
} RCM_HSI48_SEL_T;

/**
 * @brief   LSE enum
 */
typedef enum
{
    RCM_LSE_CLOSE,
    RCM_LSE_OPEN,
    RCM_LSE_BYPASS
} RCM_LSE_T;

/**
 * @brief   LSE_Drive enum
 */
typedef enum
{
    RCM_LSE_DRIVE_LOW,
    RCM_LSE_DRIVE_MEDIUMLOW,
    RCM_LSE_DRIVE_MEDIUMHIGH,
    RCM_LSE_DRIVE_HIGH
} RCM_LSE_DRIVE_T;

/**
 * @brief   RTC Clock Source Select
 */
typedef enum
{
    RCM_RTC_SEL_LSE = 0x01,
    RCM_RTC_SEL_LSI,
    RCM_RTC_SEL_HSE
} RCM_RTC_SEL_T;

/**
 * @brief   GPIO Clock
 */
typedef enum
{
    RCM_GPIO_CLOCK_A    = BIT0,
    RCM_GPIO_CLOCK_B    = BIT1,
    RCM_GPIO_CLOCK_C    = BIT2,
    RCM_GPIO_CLOCK_D    = BIT3,
    RCM_GPIO_CLOCK_E    = BIT4,
    RCM_GPIO_CLOCK_H    = BIT7
} RCM_GPIO_CLOCK_T;

/**
 * @brief   AHB peripheral
 */
typedef enum
{
    RCM_AHB_PERIPH_DMA       = BIT0,
    RCM_AHB_PERIPH_MEMITF    = BIT8,
    RCM_AHB_PERIPH_SRAM      = BIT9,
    RCM_AHB_PERIPH_CRC       = BIT12,
    RCM_AHB_PERIPH_TSC       = BIT16,
    RCM_AHB_PERIPH_RNG       = BIT20,
    RCM_AHB_PERIPH_CRYP      = BIT24
} RCM_AHB_PERIPH_T;

/**
 * @brief   APB2 peripheral
 */
typedef enum
{
    RCM_APB2_PERIPH_SYSCFG  = BIT0,
    RCM_APB2_PERIPH_TMR21   = BIT2,
    RCM_APB2_PERIPH_TMR22   = BIT5,
    RCM_APB2_PERIPH_FWL     = BIT7,
    RCM_APB2_PERIPH_ADC     = BIT9,
    RCM_APB2_PERIPH_SPI1    = BIT12,
    RCM_APB2_PERIPH_USART1  = BIT14,
    RCM_APB2_PERIPH_DBG     = BIT22
} RCM_APB2_PERIPH_T;

/**
 * @brief   APB1 peripheral
 */
typedef enum
{
    RCM_APB1_PERIPH_TMR2      = BIT0,
    RCM_APB1_PERIPH_TMR3      = BIT1,
    RCM_APB1_PERIPH_TMR6      = BIT4,
    RCM_APB1_PERIPH_TMR7      = BIT5,
    RCM_APB1_PERIPH_LCD       = BIT9,
    RCM_APB1_PERIPH_WWDT      = BIT11,
    RCM_APB1_PERIPH_SPI2      = BIT14,
    RCM_APB1_PERIPH_USART2    = BIT17,
    RCM_APB1_PERIPH_LPUART1   = BIT18,
    RCM_APB1_PERIPH_USART4    = BIT19,
    RCM_APB1_PERIPH_USART5    = BIT20,
    RCM_APB1_PERIPH_I2C1      = BIT21,
    RCM_APB1_PERIPH_I2C2      = BIT22,
    RCM_APB1_PERIPH_USB       = BIT23,
    RCM_APB1_PERIPH_CRS       = BIT27,
    RCM_APB1_PERIPH_PMU       = BIT28,
    RCM_APB1_PERIPH_DAC       = BIT29,
    RCM_APB1_PERIPH_I2C3      = BIT30,
    RCM_APB1_PERIPH_LPTMR1    = (int)BIT31
} RCM_APB1_PERIPH_T;

/**
 * @brief   RCM Interrupt Source
 */
typedef enum
{
    RCM_INT_LSI      = BIT0,
    RCM_INT_LSE      = BIT1,
    RCM_INT_HSI16    = BIT2,
    RCM_INT_HSE      = BIT3,
    RCM_INT_PLL      = BIT4,
    RCM_INT_MSI      = BIT5,
    RCM_INT_HSI48    = BIT6,
    RCM_INT_CSSL     = BIT7,
    RCM_INT_HCSS     = BIT8
} RCM_INT_T;

/**
 * @brief   RCM FLAG define
 */
typedef enum
{
    RCM_FLAG_HSI16RDY   = 0x002,
    RCM_FLAG_HSI16DIV   = 0x004,
    RCM_FLAG_MSIRDY     = 0x009,
    RCM_FLAG_HSERDY     = 0x011,
    RCM_FLAG_PLLRDY     = 0x019,
    RCM_FLAG_HSI48RDY   = 0x101,
    RCM_FLAG_LSIRDY     = 0x201,
    RCM_FLAG_LSERDY     = 0x209,
    RCM_FLAG_CSSLFAIL   = 0x20E,
    RCM_FLAG_FWLRST     = 0x218,
    RCM_FLAG_OBLRST     = 0x219,
    RCM_FLAG_PINRST     = 0x21A,
    RCM_FLAG_PODRRST    = 0x21B,
    RCM_FLAG_SWRST      = 0x21C,
    RCM_FLAG_IWDTRST    = 0x21D,
    RCM_FLAG_WWDTRST    = 0x21E,
    RCM_FLAG_LPRRST     = 0x21F
} RCM_FLAG_T;

/**@} end of group RCM_Enumerations*/

/** @addtogroup RCM_Fuctions Fuctions
  @{
*/

/** Function description */
void RCM_Reset(void);

/** configuration functions of Internal/external clocks, PLL, CSS, MCO, etc */
void RCM_ConfigHSE(RCM_HSE_T state);
uint8_t RCM_WaitHSEReady(void);
void RCM_EnableHSI16(void);
void RCM_DisableHSI16(void);
void RCM_EnableHSI16Force(void);
void RCM_DisableHSI16Force(void);
void RCM_EnableHSI16DIV(void);
void RCM_DisableHSI16DIV(void);
void RCM_EnableHSI16Out(void);
void RCM_DisableHSI16Out(void);
uint8_t RCM_WaitHSI16Ready(void);
uint8_t RCM_WaitHSI16DIVReady(void);
void RCM_EnableMSI(void);
void RCM_DisableMSI(void);
uint8_t RCM_WaitMSIReady(void);
void RCM_EnableCSS(void);
void RCM_DisableCSS(void);
void RCM_ConfigRTC(RCM_RTCPSC_T state);
void RCM_EnablePLL(void);
void RCM_DisablePLL(void);
uint8_t RCM_WaitMSIReady(void);
void RCM_SetHSI16Trim(uint8_t hsi16Trim);
void RCM_SetMSITrim(uint8_t msiTrim);
void RCM_ConfigMSIFreqRange(RCM_MSIR_T state);
void RCM_EnableHSI48(void);
void RCM_DisableHSI48(void);
void RCM_EnableHSI48DIV(void);
void RCM_DisableHSI48DIV(void);
uint8_t RCM_WaitHSI48Ready(void);
void RCM_ConfigSYSClock(RCM_SYSCLK_SEL_T sysClkSelect);
RCM_SYSCLK_SEL_T RCM_ReadSYSClockSource(void);
void RCM_ConfigAHB(RCM_AHBDIV_T ahbDiv);
void RCM_ConfigAPB1(RCM_APBDIV_T apbDiv);
void RCM_ConfigAPB2(RCM_APBDIV_T apbDiv);
void RCM_ConfigWakeUpClock(RCM_WUCLK_SEL_T wuClkSelect);
void RCM_ConfigPLLInClock(RCM_PLLIN_SEL_T pllInSelect);
void RCM_ConfigPLLMUL(RCM_PLL_MUL_T pllMul);
void RCM_ConfigPLLOut(RCM_PLLOUT_DIV_T pllOutDiv);
uint8_t RCM_WaitPLLReady(void);
void RCM_ConfigMCOClock(RCM_MCO_SEL_T mcoClock);
void RCM_ConfigMCO(RCM_MCO_PSC_T mcoPsc);
void RCM_ConfigUSART1Clock(RCM_CLKS_CEL_T usart1Clk);
void RCM_ConfigUSART2Clock(RCM_CLKS_CEL_T usart2Clk);
void RCM_ConfigLPUART1Clock(RCM_CLKS_CEL_T lpUart1Clk);
void RCM_ConfigI2C1Clock(RCM_CLKS_CEL_T i2c1Clk);
void RCM_ConfigI2C3Clock(RCM_CLKS_CEL_T i2c3Clk);
void RCM_ConfigLPTMR1Clock(RCM_LPTMR1_CLKS_T lpTmr1Clk);
void RCM_ConfigHSI48Clock(RCM_HSI48_SEL_T hsi48Clk);
void RCM_EnableLSI(void);
void RCM_DisableLSI(void);
uint8_t RCM_WaitLSIReady(void);
void RCM_ConfigLSE(RCM_LSE_T state);
uint8_t RCM_WaitLSEReady(void);
void RCM_ConfigDriveLSE(RCM_LSE_DRIVE_T state);
void RCM_EnableCSSLSE(void);
void RCM_DisableCSSLSE(void);
uint8_t RCM_CSSLSEFail(void);
void RCM_ConfigRTCClock(RCM_RTC_SEL_T rtcClock);
void RCM_EnableRTCClock(void);
void RCM_DisableRTCClock(void);
void RCM_EnableRTCReset(void);
void RCM_DisableRTCReset(void);

/** busses clocks configuration functions */
uint32_t RCM_ReadSYSClockFreq(void);
uint32_t RCM_ReadAHBClockFreq(void);
uint32_t RCM_ReadAPB1ClockFreq(void);
uint32_t RCM_ReadAPB2ClockFreq(void);
uint32_t RCM_ReadPLLClockFreq(void);
uint32_t RCM_ReadUSART1ClockFreq(void);
uint32_t RCM_ReadUSART2ClockFreq(void);
uint32_t RCM_ReadLPUART1ClockFreq(void);
uint32_t RCM_ReadI2C1ClockFreq(void);
uint32_t RCM_ReadI2C3ClockFreq(void);
uint32_t RCM_ReadLPTMR1ClockFreq(void);
uint32_t RCM_ReadHSI48ClockFreq(void);

/** Peripheral clocks configuration functions */
void RCM_EnableGPIOClock(uint32_t gpioClock);
void RCM_DisableGPIOClock(uint32_t gpioClock);
void RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_DisableAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph);
void RCM_DisableAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph);
void RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph);
void RCM_DisableAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph);
void RCM_EnableSleepGPIOClock(RCM_GPIO_CLOCK_T gpioClock);
void RCM_DisableSleepGPIOClock(RCM_GPIO_CLOCK_T gpioClock);
void RCM_EnableSleepAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_DisableSleepAHBPeriphClock(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_EnableSleepAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph);
void RCM_DisableSleepAPB2PeriphClock(RCM_APB2_PERIPH_T APB2Periph);
void RCM_EnableSleepAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph);
void RCM_DisableSleepAPB1PeriphClock(RCM_APB1_PERIPH_T APB1Periph);
void RCM_EnableGPIOReset(RCM_GPIO_CLOCK_T gpioClock);
void RCM_DisableGPIOReset(RCM_GPIO_CLOCK_T gpioClock);
void RCM_EnableAHBPeriphReset(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_DisableAHBPeriphReset(RCM_AHB_PERIPH_T AHBPeriph);
void RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_T APB2Periph);
void RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_T APB2Periph);
void RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_T APB1Periph);
void RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_T APB1Periph);

/** Interrupts and flags management functions */
void RCM_EnableInterrupt(RCM_INT_T interrupt);
void RCM_DisableInterrupt(uint8_t interrupt);
uint16_t RCM_ReadStatusFlag(RCM_FLAG_T flag);
void RCM_ClearStatusFlag(void);
uint8_t RCM_ReadIntFlag(RCM_INT_T flag);
void RCM_ClearIntFlag(uint8_t flag);

/**@} end of group RCM_Fuctions*/
/**@} end of group RCM_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /** __APM32L0xx_RCM_H */
