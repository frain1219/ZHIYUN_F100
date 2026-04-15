/*!
 * @file        usbh_board.h
 *
 * @brief       Header file for USB Board
 *
 * @version     V1.0.0
 *
 * @date        2026-03-01
 *
 * @attention
 *
 *  Copyright (C) 2023-2026 Geehy Semiconductor
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
#ifndef _USBH_BOARD_H_
#define _USBH_BOARD_H_

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f10x.h"
#include "apm32f10x_usb.h"

/* Exported macro *********************************************************/
#define USBH_CHANNEL_MAX_NUM                0x10    /*!<  Host channel number*/
#define USBH_DATA_BUF_MAX_NUM               0x200
#define INTERFACE_DESC_MAX_NUM              2
#define ENDPOINT_DESC_MAX_NUM               2
#define USBH_SUP_CLASS_MAX_NUM              1

#define USBH_DEBUG_LEVEL                    2U

#if (USBH_DEBUG_LEVEL > 0U)
#define USBH_USR_LOG(...)   do { \
                            printf("INFO :"); \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBH_USR_LOG(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 1U)
#define USBH_USR_DBG(...)   do { \
                            printf("DEBUG:"); \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBH_USR_DBG(...)   do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 2U)
#define USBH_USR_ERR(...)   do { \
                            printf("ERROR:"); \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBH_USR_ERR(...)   do {} while (0)
#endif

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USBH_BOARD_H_ */
