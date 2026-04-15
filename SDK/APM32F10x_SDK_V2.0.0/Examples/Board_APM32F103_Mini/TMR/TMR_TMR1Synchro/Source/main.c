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
 *  Copyright (C) 2021-2022 Geehy Semiconductor
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
#include "Board.h"
#include "apm32f10x_tmr.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_TMR1Synchro
  @{
*/

/** @defgroup TMR_TMR1Synchro_Functions Functions
  @{
*/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    GPIO_Config_T GPIO_ConfigStruct;
    TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    /* System clocks configuration */
    SystemClockConfig();

    /* Enable peripheral clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2|RCM_APB1_PERIPH_TMR3);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);

    /* Configure TMR1, TMR2, TMR3 channel 1 GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_0| GPIO_PIN_6 | GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* config TMR1, TMR2, TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.division = 0;
    TMR_TimeBaseStruct.period = 71;
    TMR_TimeBaseStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 99;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 49;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    TMR_ConfigOCStructInit(&OCcongigStruct);

    /* Configure TMR1, TMR2, TMR3 channel 1 */
    OCcongigStruct.mode = TMR_OC_MODE_PWM1;
    OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
    OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.idleState = TMR_OC_IDLE_STATE_SET;
    OCcongigStruct.outputNState = TMR_OC_NSTATE_DISABLE;
    OCcongigStruct.nPolarity = TMR_OC_NPOLARITY_HIGH;
    OCcongigStruct.nIdleState = TMR_OC_NIDLE_STATE_RESET;

    OCcongigStruct.pulse = 36;
    TMR_ConfigOC1(TMR1, &OCcongigStruct);

    OCcongigStruct.pulse = 50;
    TMR_ConfigOC1(TMR2, &OCcongigStruct);

    OCcongigStruct.pulse = 25;
    TMR_ConfigOC1(TMR3, &OCcongigStruct);

    /* Enable TMR1 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR1);
    TMR_SelectOutputTrigger(TMR1, TMR_TRGO_SOURCE_UPDATE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR2, TMR_SLAVE_MODE_GATED);
    TMR_SelectInputTrigger(TMR2, TMR_TRIGGER_SOURCE_ITR0);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR3, TMR_SLAVE_MODE_GATED);
    TMR_SelectInputTrigger(TMR3, TMR_TRIGGER_SOURCE_ITR0);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR1);
    TMR_Enable(TMR2);
    TMR_Enable(TMR3);

    TMR_EnablePWMOutputs(TMR1);

    while(1)
    {
    }
}

/**@} end of group TMR_TMR1Synchro_Functions */
/**@} end of group TMR_TMR1Synchro */
/**@} end of group Examples */
