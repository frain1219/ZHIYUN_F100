/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.3
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

/** @addtogroup TMR_TimeBase
  @{
*/

/** @defgroup TMR_TimeBase_Variables Variables
  @{
*/

volatile uint32_t tick = 0;

/**@} end of group TMR_TimeBase_Variables */

/** @defgroup TMR_TimeBase_Functions Functions
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
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    /* System clocks configuration */
    SystemClockConfig();

    BOARD_LEDInit(LED2);

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = 71;
    TMR_BaseConfigStruct.period = 999;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_UP_IRQn, 0, 0);

    TMR_Enable(TMR1);

    while(1)
    {
        BOARD_LEDToggle(LED2);
        Delay();
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;

    while(tick < 500);
}

/**@} end of group TMR_TimeBase_Functions */
/**@} end of group TMR_TimeBase */
/**@} end of group Examples */
