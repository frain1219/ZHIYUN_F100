/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2023-11-13
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

This example describes how to use the usb host HS2 to enum a U disk and use OTG FS to simulate a CDC device.
And use FATFS to write and read file to U disk.Press KEY1 to write file to U disk and
press KEY2 to read file from U disk.
When CDC device receive data will write file to U disk.

&par Hardware Description

The USART1 is configured as follows:
    - TX:PA9, RX:PA10
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

    - OTG_HS_VBUS(PB13) ----------> NC
    - OTG_HS_ID(PB12)   ----------> NC
    - VCC5V             ----------> VBUS
    - OTG_HS_DM(PB14)   ----------> USB_DN
    - OTG_HS_DP(PB15)   ----------> USB_DP
    - GND               ----------> GND

&par Directory contents

  - Host_Examples/MSC_CDC_DualCore/Source/apm32f4xx_int.c          Interrupt handlers
  - Host_Examples/MSC_CDC_DualCore/Source/main.c                   Main program

&par IDE environment

  - MDK-ARM V5.29
  - EWARM V8.50.4.26131
  - ECLIPSE-EMB V4.24.0

&par Hardware and Software environment

  - This example runs on APM32F407 EVAL Devices.
