/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2023-03-27
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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

This example describes how to use sram array and OTG HS1 to simulate a fake U disk.
The external USB HS PHY is USB3300.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

    - VDD                   -------------> USB3300 VDD3V3
    - GND                   -------------> USB3300 GND
    - PA3(OTG HS ULPI D0)   -------------> USB3300 D0
    - PB0(OTG HS ULPI D1)   -------------> USB3300 D1
    - PB1(OTG HS ULPI D2)   -------------> USB3300 D2
    - PB10(OTG HS ULPI D3)  -------------> USB3300 D3
    - PB11(OTG HS ULPI D4)  -------------> USB3300 D4
    - PB12(OTG HS ULPI D5)  -------------> USB3300 D5
    - PB13(OTG HS ULPI D6)  -------------> USB3300 D6
    - PB5(OTG HS ULPI D7)   -------------> USB3300 D7
    - PA5(OTG HS ULPI CK)   -------------> USB3300 CK
    - PC0(OTG HS ULPI STP)  -------------> USB3300 STP
    - PI11(OTG HS ULPI DIR) -------------> USB3300 DIR
    - PH4(OTG HS ULPI NXT)  -------------> USB3300 NXT

&par Directory contents

  - Device_Examples/OTGD_MSC_HS1/Source/apm32f4xx_int.c          Interrupt handlers
  - Device_Examples/OTGD_MSC_HS1/Source/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.29
  - EWARM V8.50.4.26131
  - ECLIPSE-EMB V4.24.0

&par Hardware and Software environment

  - This example runs on APM32F407 MINI Devices.
