/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.2
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "main.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup PMU_STANDBY
  @{
  */

/** @defgroup PMU_STANDBY_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    RCM_EnableAPB1PeriphClock((RCM_APB1_PERIPH_T)(RCM_APB1_PERIPH_PMU | RCM_APB1_PERIPH_BAKR));

    /* System clocks configuration */
    SystemClockConfig();

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);

    BOARD_LEDOn(LED2);
    BOARD_LEDOff(LED3);

    PMU_EnableWakeUpPin();
    PMU_EnableBackupAccess();

    RTC_Init();

    SysTick_Init();

    while(1)
    {
    }
}

/*!
 * @brief       RTC Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void RTC_Init(void)
{
    if(PMU_ReadStatusFlag(PMU_FLAG_SB) == SET)
    {
        BOARD_LEDOn(LED3);

        PMU_ClearStatusFlag(PMU_FLAG_SB);

        RTC_WaitForSynchro();
    }
    else
    {
        BAKPR_Reset();

        RCM_ConfigLSE(RCM_LSE_OPEN);
        while(RCM_ReadStatusFlag(RCM_FLAG_LSERDY) == RESET);
        RCM_ConfigRTCCLK(RCM_RTCCLK_LSE);
        RCM_EnableRTCCLK();

        RTC_WaitForSynchro();
        RTC_ConfigPrescaler(32767);
        RTC_WaitForLastTask();
    }
}

/*!
 * @brief       Systick Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SysTick_Init(void)
{
    SysTick_Config((SystemCoreClock / 32));
    SysTick_ConfigCLKSource(SYSTICK_CLK_SOURCE_HCLK_DIV8);

    NVIC_SetPriority(SysTick_IRQn, 0X04);
}

/**@} end of group PMU_STANDBY_Functions */
/**@} end of group PMU_STANDBY */
/**@} end of group Examples */
