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

This example describes how to use SPI peripheral.
by making a board, the master/slave full duplex communication between the SPI and UART1.
If communication success, LED2 will turn on, LED3 will blink.

Connect UART1 and SPI pins as follows:
UART1 TX   (PA9)   to SPI MOSI (PA7)
UART1 RX   (PA10)  to SPI MISO (PA6)
UART1 CLK  (PA8)   to SPI SCK  (PA5)

&par Directory contents

  - SPI/SPI_FullDuplex/src/apm32f10x_int.c     Interrupt handlers
  - SPI/SPI_FullDuplex/src/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
