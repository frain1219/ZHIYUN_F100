/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2025-04-23
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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

This routine describes how to convert the COM data from the host computer
into I2C data for transmission.
Enabling users to connect with the I2C interface of the Bootloader through the
adapter board which has been programmed with this routine, using the Geehy Prog tool,
thus allowing normal use of the Bootloader functions.

UART1 and I2C pins connection method as follows:
I2C CLK  (PB6) to Bootloader I2C CLK
I2C SDA  (PB7) to Bootloader I2C SDA

USART TX (PA9)  to PC COM RX
USART RX (PA10) to PC COM TX

&par Directory contents

  - Bootloader/USARTTOI2C/src/apm32f10x_int.c     Interrupt handlers
  - Bootloader/USARTTOI2C/src/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
