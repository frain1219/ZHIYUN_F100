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

This example describes how to use TMR5 Channel2(PA1) measure frequency of external signal.
User can view the "Frequency" value through serial terminal.

TMR5_IRQHandler Function to calculate:
Frequency = (pclk1 * 2) / (Capture) Hz.
The minimum frequency value to measure is 1 Hz.

&par Hardware Description

using USART1(TX:PA9, RX:PA10).
  - USART1 configured as follow:
  - BaudRate    = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit    = USART_StopBits_1
  - Parity      = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

  - using TMR5 CHANNEL2(PA0) connect the external signal

&par Directory contents

  - TMR/TMR_InputCapture/Source/apm32f10x_int.c     Interrupt handlers
  - TMR/TMR_InputCapture/Source/main.c              Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
