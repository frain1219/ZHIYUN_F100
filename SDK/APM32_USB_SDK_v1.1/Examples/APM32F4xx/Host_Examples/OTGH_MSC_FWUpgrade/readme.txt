/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
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

This example describes how to use the usb host to enum a U disk.
And use it to upgrade application firmware.
MSC_FWUpgradeBootloader include all operations for USB upgrade firmware.
MSC_FWUpgradeApp include enum U disk and scan and read or write file through KEY1 / KEY2.

Running Steps:
- Load the binary image of the "MSC_FWUpgradeApp" into the root directory
of the U disk and rename it to image.bin.

- Download "MSC_FWUpgradeBootloader" into the internal flash memory.

- Insert the U disk into the USB FS connector.

- Press the KEY1 button and reset system.

- After waiting for the UART1 prompt to release the KEY1 button and release
KEY1, the program will enter USB upgrade mode.

- After upgrade is completed, the program will jump to application.

Flash space:
0x08000000 - 0x0800FFFF : USB FW upgrade bootloader
0x08010000 - 0x080FFFFF : USB FW upgrade application

note:
- The starting address of the user application program can be changed by "APPLICATION_ADDRESS"
in the bsp_flash.h file.

- The image download and upload information can be changed in the usb_iap.h file.
    - IAP_UPLOAD_FILENAME   : The name of binary file to be upload.
    - IAP_DOWNLOAD_FILENAME : The name of binary file to be download.
    - IAP_FILE_PATH         : The path of image file.
    - IAP_DISK_PATH         : The path of U disk.
    - IAP_BUFFER_SIZE       : Buffer for IAP download. In this project is fixed to 32KB.

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

&par Directory contents

  - Host_Examples/OTGH_MSC_FWUpgrade/MSC_FWUpgradeApp              USB MSC Firmware upgrade application
  - Host_Examples/OTGH_MSC_FWUpgrade/MSC_FWUpgradeBootloader       USB MSC Firmware upgrade bootloader

&par IDE environment

  - MDK-ARM V5.29
  - EWARM V8.50.4.26131
  - ECLIPSE-EMB V4.24.0

&par Hardware and Software environment

  - This example runs on APM32F407 MINI Devices.
