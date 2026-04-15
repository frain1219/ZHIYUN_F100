/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2025-12-01
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

This routine describes how to convert the COM data from the host computer
into SPI data for transmission.
Enabling users to connect with the SPI interface of the Bootloader through the
adapter board which has been programmed with this routine, using the Geehy Prog tool,
thus allowing normal use of the Bootloader functions.

Connect UART1 and SPI pins as follows:
SPI MOSI (PA7) to Bootloader SPI MOSI
SPI MISO (PA6) to Bootloader SPI MISO
SPI SCK  (PA5) to Bootloader SPI SCK
SPI NSS  (PA4) to Bootloader SPI NSS

USART TX (PA9)  to PC COM RX
USART RX (PA10) to PC COM TX

&par Directory contents

  - Bootloader/USARTTOSPI/src/apm32f10x_int.c     Interrupt handlers
  - Bootloader/USARTTOSPI/src/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
