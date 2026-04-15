/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.4
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

&par Example Description

This example aims to show how to use WWDT.
When KEY1 is not pressed, System is not reset due to feeding dog timely, and LED2 Toggle.
Pressing KEY1 to stop feed dog will trigger system reset when the counter reach a given
timeout value. LED3 will be lighted when a system reset is triggered by WWDT.

&par Directory contents

  - WWDT/WWDT_Reset/src/apm32f10x_it.c     Interrupt handlers
  - WWDT/WWDT_Reset/src/main.c             Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
