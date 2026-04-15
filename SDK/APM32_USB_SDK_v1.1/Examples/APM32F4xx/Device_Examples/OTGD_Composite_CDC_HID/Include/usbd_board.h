/*!
 * @file        usbd_board.h
 *
 * @brief       Header file for USB Board
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

/* Define to prevent recursive inclusion */
#ifndef _USBD_BOARD_H_
#define _USBD_BOARD_H_

/* Includes */
#include "apm32f4xx.h"
#include "apm32f4xx_usb.h"
#include "apm32f4xx_usb_device.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup OTGD_Composite_CDC_HID
  @{
  */

/** @defgroup OTGD_Composite_CDC_HID_Macros Macros
  @{
*/

#define USBD_FS_EP0_FIFO_SIZE                   0x40
#define USBD_FS_EP1_FIFO_SIZE                   0x40
#define USBD_FS_EP2_FIFO_SIZE                   0x40

#define USBD_CUSTOM_HID_IN_EP_ADDR              0x81U
#define USBD_CUSTOM_HID_OUT_EP_ADDR             0x01U

#define USBD_CDC_DATA_IN_EP_ADDR                0x82U
#define USBD_CDC_DATA_OUT_EP_ADDR               0x02U
#define USBD_CDC_CMD_EP_ADDR                    0x83U

#define USBD_CUSTOM_HID_INTERFACE_NUM           0
#define USBD_CUSTOM_HID_INTERFACE_CNT           1

#define USBD_CUSTOM_HID_FS_MP_SIZE              0x40
#define USBD_CUSTOM_HID_HS_MP_SIZE              0x200

#define USBD_CUSTOM_HID_IN_EP_SIZE              0x04
#define USBD_CUSTOM_HID_OUT_EP_SIZE             0x04

#define USBD_CUSTOM_HID_HS_INTERVAL             7
#define USBD_CUSTOM_HID_FS_INTERVAL             10

#define USBD_CDC_CTRL_INTERFACE_NUM             1
#define USBD_CDC_DATA_INTERFACE_NUM             2
#define USBD_CDC_INTERFACE_CNT                  2
#define USBD_CDC_HS_INTERVAL                    0x10U
#define USBD_CDC_FS_INTERVAL                    0x10U
#define USBD_CDC_HS_MP_SIZE                     512U
#define USBD_CDC_FS_MP_SIZE                     64U
#define USBD_CDC_DATA_MP_SIZE                   0x07
#define USBD_CDC_CMD_MP_SIZE                    8U

#define USBD_SUP_CLASS_MAX_NUM              1
#define USBD_SUP_INTERFACE_MAX_NUM          1
#define USBD_SUP_CONFIGURATION_MAX_NUM      1
#define USBD_SUP_STR_DESC_MAX_NUM           512

/* Only support LPM USB device */
#define USBD_SUP_LPM                        0
#define USBD_SUP_SELF_PWR                   1
#define USBD_DEBUG_LEVEL                    1U

#if (USBD_DEBUG_LEVEL > 0U)
#define USBD_USR_LOG(...)   do { \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBD_USR_LOG(...) do {} while (0)
#endif

#if (USBD_DEBUG_LEVEL > 1U)
#define USBD_USR_Debug(...)   do { \
                            printf("Debug:"); \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBD_USR_Debug(...) do {} while (0)
#endif

/**@} end of group OTGD_Composite_CDC_HID_Macros*/

/** @defgroup OTGD_Composite_CDC_HID_Functions Functions
  @{
  */

/**@} end of group OTGD_Composite_CDC_HID_Functions */
/**@} end of group OTGD_Composite_CDC_HID */
/**@} end of group Examples */

#endif
