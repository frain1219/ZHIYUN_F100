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

This example describes how to use external NOR flash to simulate a U disk.

note: If encounter problems during initialization, you can set the W25QXX_ERASE_CHIP macro to 1
to erase the W25QXX whole chip.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

    - OTG_FS_VBUS(PA9) ----------> X(Use to USART1)
    - OTG_FS_ID(PA10)  ----------> X(Use to USART1)
    - VCC5V            ----------> VBUS
    - OTG_FS_DM(PA11)  ----------> USB_DN
    - OTG_FS_DP(PA12)  ----------> USB_DP
    - GND              ----------> GND

    - SPI_SCK(PB3)     ----------> W25QXX CLK
    - SPI_MISO(PB4)    ----------> W25QXX DO
    - SPI MOSI(PB5)    ----------> W25QXX DI
    - SPI CS(PF5)      ----------> W25QXX CS

&par Directory contents

  - Device_Examples/OTGD_MSC_NorFlash_HS2/Source/apm32f4xx_int.c          Interrupt handlers
  - Device_Examples/OTGD_MSC_NorFlash_HS2/Source/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.29
  - EWARM V8.50.4.26131
  - ECLIPSE-EMB V4.24.0

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
