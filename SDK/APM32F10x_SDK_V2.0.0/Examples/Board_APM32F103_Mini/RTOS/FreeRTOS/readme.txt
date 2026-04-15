/*!
 * @file       readme.txt
 *
 * @brief      This file is routine instruction
 *
 * @version     V1.0.2
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

This example describes show how to use FreeRTOS create multiple tasks.

USART test task: USART1 and USART2 send or received data to each other. Verification will occur after transmission.

Led toggle task: The IO of LED2 is configured to toggle constantly.

The phenomenon of LED2 constantly flickered alternately,
if data transmission pass, LED3 will be on,
if data transmission fault, LED3 will be off.

&par Software Description


&par Hardware Description

USART1_TX(PA9)  ----> USART2_RX(PA3)
USART1_RX(PA10) ----> USART2_TX(PA2)

  - USART1 and USART2 configured as follow:
  - BaudRate    = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit    = USART_StopBits_1
  - Parity      = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - Examples/src/main.c                      Main program
  - Examples/src/apm32f10x_int.c             Interrupt handlers
  - Examples/inc/FreeRTOSConfig.h            FreeRTOS config functions

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
