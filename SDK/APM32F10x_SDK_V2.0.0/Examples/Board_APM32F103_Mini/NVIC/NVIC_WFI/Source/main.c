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

/** @addtogroup NVIC_WFI
  @{
  */

/** @defgroup NVIC_WFI_Variables Variables
  @{
*/

/* interrupt status */
uint8_t intStatus = 0;

/**@} end of group NVIC_WFI_Variables*/

/** @defgroup NVIC_WFI_Functions Functions
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
    /* System clocks configuration */
    SystemClockConfig();

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);

    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);

    while(1)
    {
        if (intStatus == 1)
        {
            BOARD_LEDOff(LED2);
            BOARD_LEDOff(LED3);

            /* Wait fot interrupt */
            __WFI();

            intStatus = 0;
        }

        BOARD_LEDToggle(LED2);
        BOARD_LEDToggle(LED3);

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
    volatile uint32_t delay = 0x5FFFF;

    while(delay--);
}

/**@} end of group NVIC_WFI_Functions */
/**@} end of group NVIC_WFI */
/**@} end of group Examples */
