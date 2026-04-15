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

This example describes how to use RT-Thread for APM32F10x.
The IO of LED2 and LED3 is configed to toggle constantly.
The phenomenon of LED2 and LED3 constantly flickered alternately.

&par Software Description


&par Hardware Description

LED2 and LED3

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate    = 115200
    - Word Length = USART_WordLength_8b
    - Stop Bit    = USART_StopBits_1
    - Parity      = USART_Parity_No
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

&par Directory contents

  - Source/main.c                       Main program
  - Source/apm32f10x_int.c              Interrupt handlers
  - MDK_Project/RTE/RTOS/board.c        RT-Thread firmware functions
  - MDK_Project/RTE/RTOS/finsh_port.c   RT-Thread finsh port functions
  - MDK_Project/RTE/RTOS/rtconfig.h     RT-Thread config functions

&par Hardware and Software environment

  - This example runs on APM32F103 MINI Devices.
