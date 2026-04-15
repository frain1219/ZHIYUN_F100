/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
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

&par Example Description

This example shows how to synchronize TMR1 and TMR peripherals in parallel mode.

  - parallel mode:
  - TMR1 Master for TMR2 and TMR3, Configure PWM Mode and TMR1 Update event is used as Trigger Output
  - TMR2 is slave for TMR1,
  - Configure PWM Mode
  - The ITR0(TMR1) is used as input trigger
  - TMR3 is slave for TMR1,
  - Configure PWM Mode
  - The ITR0(TMR1) is used as input trigger

  - The TMR1 peripherals clock is 72MHz.

  - The Master Timer TMR1 frequency :
  - TMR1 frequency = (72M)/ (71 + 1) = 1 MHz
  - duty cycle = 36/(71 + 1) = 50%.

  - The TMR2 is running at:
  - (1 MHz)/ (99 + 1) = 10 KHz
  - duty cycle = 50/(99 + 1) = 50%

  - The TMR3 is running at:
  - (1 MHz)/ (49 + 1) = 20 KHz
  - duty cycle = 25/(49 + 1) = 50%

&par Hardware Description

  - using TMR1 CHANNEL1(PA8) to output PWM
  - using TMR2 CHANNEL1(PA0) to output PWM
  - using TMR3 CHANNEL1(PA6) to output PWM

&par Directory contents

  - TMR/TMR_TMR1Synchro/Source/apm32f10x_int.c        Interrupt handlers
  - TMR/TMR_TMR1Synchro/Source/main.c                 Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
