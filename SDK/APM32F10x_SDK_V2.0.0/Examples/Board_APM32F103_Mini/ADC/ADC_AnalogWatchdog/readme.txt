/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
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

&par Example Description

This example describes how to use ADC1 to monitor the voltage of ADC1_Channel14(PC4) continuously.
When input Voltage of ADC1_Channel14(PC4) voltage is lower than 0.62v or higher than 2.27V, analog 
watchdog interrupt will be generated and LED2 is on.

&par Directory contents

  - ADC/ADC_AnalogWatchdog/src/apm32f10x_int.c     Interrupt handlers
  - ADC/ADC_AnalogWatchdog/src/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103_MINI Devices.
