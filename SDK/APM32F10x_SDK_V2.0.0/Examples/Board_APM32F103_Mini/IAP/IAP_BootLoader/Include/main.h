/*!
 * @file        main.h
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
* @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2022 Geehy Semiconductor
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

/* Includes */
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"
#include "apm32f10x_fmc.h"
#include "apm32f10x_usart.h"
#include "apm32f10x_misc.h"

#include "bsp_usart.h"
#include "bsp_flash.h"
#include "common.h"
#include "ymodem.h"
#include "menu.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @defgroup IAP_BootLoader_Functions Functions
  @{
  */

/*!
 * @brief    function hook
 */
typedef  void (*pFunction)(void);

/* extern variables */
extern volatile uint32_t tick;
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

/* function declaration */
void Delay(void);
void Jump_to_App(uint8_t Application);

/**@} end of group IAP_BootLoader_Functions */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */

#endif
