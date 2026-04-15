/*!
 * @file       readme.txt
 *
 * @brief      This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2022 Geehy Semiconductor
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

This example shows how to create the TCP server based on the FreeRTOS V10.4.3. Received and
send data to TCP Client by tcp assistant.

phenomenon:
  - After initialization, you can see the system information on serial assistant through USART1.
  - you can connect and send data to the APM32F107 MINI board by TCP assistant.

notes: This example uses the RMII interface by default. Changes "#define RMII_MODE" and "#define MII_MODE"
comment to use different interface in board_DP83848.h.

Please install the following dependencies beforehand.
  - Middlewares/FreeRTOS/FreeRTOSv202012.00-LTS.exe   FreeRTOS Software Components

&par Hardware Description

runs on APM32F107_MINI Devices
 /**  jumpers setting
*    ==========================================================================================+
*      Jumper |       MII mode configuration            |      RMII mode configuration         +
*    ==========================================================================================+
*      JP3    | 2-3 provide 25MHz clock by MCO(PA8)     |  2-3 provide 25MHz clock by MCO(PA8) +
*             | 1-2 provide 25MHz clock by ext. Crystal |                                      +
*     -----------------------------------------------------------------------------------------+
*      JP2    |          2-3                            |  1-2                                 +
*     -----------------------------------------------------------------------------------------+
*      JP1    |          Open                           |  Close                               +
*    ==========================================================================================+
  */

using USART1(TX:PA9, RX:PA10).
  - USART1 configured as follow:
  - BaudRate    = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit    = USART_StopBits_1
  - Parity      = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

more read DP83848_PIN_CONNECT.xlsx

&par Directory contents

  - ETH/ETH_FreeRTOS_TcpServer/Source/apm32f10x_int.c     Interrupt handlers
  - ETH/ETH_FreeRTOS_TcpServer/Source/main.c              Main program
  - ETH/ETH_FreeRTOS_TcpServer/Source/board_DP83848.c     provides functions for configurate DP83848

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.4.26131

&par Hardware and Software environment

  - This example runs on APM32F107_MINI Devices, need to define APM32F107_MINI.
  - This example runs on APM32F107_ETH Devices, need to define APM32F107_ETH.
