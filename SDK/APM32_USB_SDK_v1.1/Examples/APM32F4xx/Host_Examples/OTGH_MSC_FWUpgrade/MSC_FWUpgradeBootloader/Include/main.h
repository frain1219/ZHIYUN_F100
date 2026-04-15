/*!
 * @file        main.h
 *
 * @brief       Header for main.c module
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
#ifndef _MAIN_H_
#define _MAIN_H_

/* Includes */
#include "board.h"

#include "apm32f4xx.h"
#include "apm32f4xx_can.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_usart.h"
#include "apm32f4xx_misc.h"

/** @addtogroup Examples
  * @brief OTG MSC Upgrade Bootloader examples
  @{
  */

/** @addtogroup OTG_MSC_FWUpgradeBootloader
  @{
  */

/** @defgroup OTG_MSC_FWUpgradeBootloader_Enumerates Enumerates
  @{
  */

/**
 * @brief User application state
 */
typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
    USER_APP_JUMP,
} USER_APP_STATE_T;

/**
 * @brief User application error state
 */
typedef enum
{
    USER_APP_OK,
    USER_APP_ERR_FATFS,
    USER_APP_ERR_DISK_WP,
    USER_APP_ERR_FSIZE_OVER,
    USER_APP_ERR_FMC_ERASE,
    USER_APP_ERR_FMC_PG,
    USER_APP_ERR_FMC_WP,
    USER_APP_ERR_DISK_DISCONNECTED,
} USER_APP_ERR_T;

/**
 * @brief USB user state
 */
typedef enum
{
    USER_OK,
    USER_WAITING,
    USER_ERROR
} USER_STATE_T;

/**@} end of group OTG_MSC_FWUpgradeBootloader_Enumerates*/

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

extern void USER_ErrorHandler(uint8_t errCode);

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
