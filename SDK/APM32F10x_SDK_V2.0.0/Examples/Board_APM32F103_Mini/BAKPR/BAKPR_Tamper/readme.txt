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

This example describes how to write the backup registers. After initialization,
System enters into a infinite loop. if the data in the backup registers is equal to
the data write to the registers before, LED2 keeps blinking, otherwise, LED3 keeps
blinking. TAMPER pin is also enabled, if the pin changes from 0 to 1 or from 1 to 0,
The TAMPER pin generates a Tamper detection event to reset all data backup registers.

&par Directory contents

  - BAKPR/BAKPR_Tamper/src/apm32f10x_int.c     Interrupt handlers
  - BAKPR/BAKPR_Tamper/src/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103_MINI Devices.
