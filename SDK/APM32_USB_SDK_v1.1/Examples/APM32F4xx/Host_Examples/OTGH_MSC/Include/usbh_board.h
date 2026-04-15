/*!
 * @file        usbh_board.h
 *
 * @brief       Header file for USB Board
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

/* Define to prevent recursive inclusion */
#ifndef _USBH_BOARD_H_
#define _USBH_BOARD_H_

/* Includes */
#include "apm32f4xx.h"
#include "apm32f4xx_usb.h"
#include "bsp_delay.h"

/** @addtogroup Examples
  * @brief OTGH MSC examples
  @{
  */

/** @addtogroup OTGH_MSC
  @{
  */

/** @defgroup OTGH_MSC_Macros Macros
  @{
*/

#define USBH_Delay                          APM_DelayMs

#define USBH_CHANNEL_MAX_NUM                0x10    //!<  Host channel number
#define USBH_DATA_BUF_MAX_NUM               0x200
#define INTERFACE_DESC_MAX_NUM              2
#define ENDPOINT_DESC_MAX_NUM               2
#define USBH_SUP_CLASS_MAX_NUM              1

#define USBH_DEBUG_LEVEL                    1U

#if (USBH_DEBUG_LEVEL > 0U)
#define USBH_USR_LOG(...)   do { \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBH_USR_LOG(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 1U)
#define USBH_USR_Debug(...)   do { \
                            printf("Debug:"); \
                            printf(__VA_ARGS__); \
                            printf("\r\n"); \
} while(0)
#else
#define USBH_USR_Debug(...) do {} while (0)
#endif

/**@} end of group OTGH_MSC_Macros*/

/** @defgroup OTGH_MSC_Functions Functions
  @{
  */

/**@} end of group OTGH_MSC_Functions */
/**@} end of group OTGH_MSC */
/**@} end of group Examples */

#endif
