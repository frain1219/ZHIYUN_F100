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

This example shows how to synchronize TMR peripherals in parallel mode.

  - parallel mode:
  - TMR3 Master for TMR2 and TMR4, Configure PWM Mode and TMR3 Update event is used as Trigger Output
  - TMR2 is slave for TMR3
  - Configure PWM Mode
  - The ITR1(TMR3) is used as input trigger
  - TMR4 is slave for TMR2,
  - Configure PWM Mode
  - The ITR1(TMR3) is used as input trigger

  - The TMR3 peripherals clock is 72 MHz.

  - The Master Timer TMR3 frequency :
  - TMR3 frequency = (72M)/ (71 + 1) = 1 MHz
  - duty cycle = 36/(71 + 1) = 50%.

  - The TMR2 is running at:
  - (1 MHz)/ (3 + 1) = 250 KHz and
  - duty cycle = 2/(3 + 1) = 50%

  - The TMR4 is running at:
  - (1 MHz)/ (1 + 1) = 500 KHz
  - duty cycle = 1/(1 + 1) = 50%

&par Hardware Description

  - using TMR2 CHANNEL1(PA0) to output PWM
  - using TMR3 CHANNEL1(PA6) to output PWM
  - using TMR4 CHANNEL1(PB6) to output PWM

&par Directory contents

  - TMR/TMR_ParallelSynchro/src/apm32f10x_int.c           Interrupt handlers
  - TMR/TMR_ParallelSynchro/src/main.c                    Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103_MINI Devices.
