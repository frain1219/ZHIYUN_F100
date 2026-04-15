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

This example describes how to configure a communication the CAN. CAN transmit a message
from one board to another.

Transmit：
If press the KEY1, CAN1 transmit, the USART1 print "CAN1 Transmit 0x55", LED2 turns on.
If press the KEY2, CAN2 transmit, the USART1 print "CAN2 Transmit 0xAA", LED2 turns on.

Receive：
If CAN1 receive success, the USART1 print "CAN1 Receive 0x55", LED2 turns on,
else "CAN1 Receive Error", LED3 turns on.
If CAN2 receive success, the USART1 print "CAN2 Receive 0xAA", LED2 turns on,
else "CAN2 Receive Error", LED3 turns on.

The CAN connection as follows:
     _____________                                     ________________
    |                  |                                   |                       |
    |                  |-PD0(CAN1_RX)  <--------->  CAN_RX-| CAN Transceiver|-CANH  <-----
    |                  |-PD1(CAN1_TX)  <--------->  CAN_TX-|                          |-CANL  <--     |
    |     MCU     |                                   | ________________|                                            |    |
    |                  |                                    ________________                                             |    |
    |                  |                                   |                       |                                            |    |
    |                  |-PB5(CAN2_RX)  <--------->  CAN_RX-| CAN Transceiver|-CANL  <--     |
    |                  |-PB6(CAN2_TX)  <--------->  CAN_TX-|                           |-CANH  <-----
    |_____________|                                   |________________|

&par Hardware Description

    USART1_TX(PA9)
    USART1_RX(PA10)

  - USART1 configured as follow:
  - BaudRate = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit = USART_StopBits_1
  - Parity = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - CAN/CAN_Normal/src/apm32f10x_int.c     Interrupt handlers
  - CAN/CAN_Normal/src/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.5.26295

&par Hardware and Software environment

  - This example runs on APM32F103_MINI Devices only support APM32F103xC. 
