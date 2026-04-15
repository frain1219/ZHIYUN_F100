/*!
 * @file       main.c
 *
 * @brief      Main program body
 *
 * @version    V1.0.0
 *
 * @date       2022-12-01
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
#include "Board.h"
#include "rtthread.h"

/** @addtogroup Examples
  * @{
*/

/** @addtogroup RT-thread
  * @{
*/

/** @defgroup RT-thread_Functions Functions
  * @{
*/

static rt_thread_t led1_thread = RT_NULL;
static void led1_thread_entry(void* parameter);

static void led1_thread_entry(void* parameter)
{
    while (1)
    {
        BOARD_LEDToggle(LED3);
        rt_thread_delay(1000);
        BOARD_LEDToggle(LED2);
    }
}

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

    led1_thread = rt_thread_create("led1",
                                   led1_thread_entry,
                                   RT_NULL,
                                   512,
                                   3,
                                   20);
    rt_thread_startup(led1_thread);
}

/**@} end of group RT-thread_Functions */
/**@} end of group RT-thread */
/**@} end of group Examples */
