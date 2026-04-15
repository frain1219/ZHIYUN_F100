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

This example describes how to use ethernet PHY by using APM32F10x_ETH_Driver library.
After configured ethernet mainboard will be use USART1 to printf static IP address.
if computer ping static IP address 169.254.90.123, computer will be visit mainboard normality.

The phenomenon of data interaction process can be displayed using cmd.exe.

notes: This example uses the RMII interface by default. Change "#define RMII_MODE" and "#define MII_MODE"
comment to use different interface in board_DP83848.h.

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

  - ETH/ETH_Ping/Source/apm32f10x_int.c     Interrupt handlers
  - ETH/ETH_Ping/Source/main.c              Main program
  - ETH/ETH_Ping/Source/board_DP83848.c     provides functions for configurate DP83848

&par IDE environment

  - MDK-ARM V5.36
  - EWARM V8.50.4.26131

&par Hardware and Software environment

  - This example runs on APM32F107_MINI Devices, need to define APM32F107_MINI.
  - This example runs on APM32F107_ETH Devices, need to define APM32F107_ETH.
