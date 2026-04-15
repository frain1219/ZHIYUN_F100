/*!
 * @file        main.h
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

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include <stdio.h>
#include "Board.h"
#include "dmc_sdram.h"
#include "apm32f10x_usart.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup DMC_SDRAM
  @{
  */

/** @defgroup DMC_SDRAM_Variables Variables
  @{
  */

extern volatile uint32_t tick;

/**@} end of group DMC_SDRAM_Variables*/

/** @defgroup DMC_SDRAM_Functions Functions
  @{
  */

void Delay(void);
void SystemCoreClockConfig(uint32_t pllMf, uint32_t latency);

/**@} end of group DMC_SDRAM_Functions */
/**@} end of group DMC_SDRAM */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif
