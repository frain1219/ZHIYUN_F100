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

This example describes how to configure the communication of USART and CAN, and how to transform the serial port data into CAN data for communication with the Bootloader.
After connection, the CAN rate can be selected by pressing the KEY1 button, and the CAN rate can be determined by the LED.
125K:    LED2 OFF,LED3 OFF
250K:    LED2 OFF,LED3 ON
500K:    LED2 ON ,LED3 OFF
1000K:  LED2 ON ,LED3 ON

&par Hardware Description
    CAN1_RX(PB8)
    CAN1_TX(PB9)

    USART1_TX(PA9)
    USART1_RX(PA10)

  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_EVEN
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - CAN/USARTTOCAN/src/apm32f10x_int.c     Interrupt handlers
  - CAN/USARTTOCAN/src/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.36

&par Hardware and Software environment

  - This example runs on APM32F103_MINI Devices only support APM32F103xC.
