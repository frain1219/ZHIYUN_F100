/*!
 * @file       BOARD_DP83848.h
 *
 * @brief      This file provides all the config DP83848 functions
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

/** jumpers setting
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

/* Define to prevent recursive inclusion */
#ifndef __BOARD_DP83848_H
#define __BOARD_DP83848_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "main.h"

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_RTT_TcpServer
  * @{
  */

/** @defgroup ETH_RTT_TcpServer_Macros Macros
  @{
*/

/* DP83848_PHY_ADDRESS */
#define DP83848_PHY_ADDRESS       0x01

/* PHY USE MOC1 */
#define PHY_CLOCK_MCO

/* MII and RMII mode selection */
#define RMII_MODE
//#define MII_MODE

/* Reset delay */
#define DP83848_RESET_DELAY        PHY_RESET_DELAY

/*
  ETH_RESET#                          PB15
  ETH_INT                             PB14
  ETH_MDIO                            PA2
  ETH_MDC                             PC1
  ETH_MCO                             PA8
*/

/* ETH_RESET# PB15 */
#define ETH_RESET_GPIO_CLK              RCM_APB2_PERIPH_GPIOB
#define ETH_RESET_PORT                  GPIOB
#define ETH_RESET_PIN                   GPIO_PIN_15
#define ETH_RESET_PIN_HIGH()            GPIO_SetBit(ETH_RESET_PORT,ETH_RESET_PIN)
#define ETH_RESET_PIN_LOW()             GPIO_ResetBit(ETH_RESET_PORT,ETH_RESET_PIN)
/* ETH_INT PB14 */      
#define ETH_INT_GPIO_CLK                RCM_APB2_PERIPH_GPIOB
#define ETH_INT_PORT                    GPIOB
#define ETH_INT_PIN                     GPIO_PIN_14
#define ETH_INT_PIN_HIGH()              GPIO_SetBit(ETH_INT_PORT,ETH_INT_PIN)
#define ETH_INT_PIN_LOW()               GPIO_ResetBit(ETH_INT_PORT,ETH_INT_PIN)
/* ETH_MII_MDC PC1 */       
#define ETH_MDC_GPIO_CLK                RCM_APB2_PERIPH_GPIOC
#define ETH_MDC_PORT                    GPIOC
#define ETH_MDC_PIN                     GPIO_PIN_1
#define ETH_MDC_AF                      GPIO_AF_ETH
#define ETH_MDC_SOURCE                  GPIO_PIN_SOURCE_1
/* ETH_MII_MDIO PA2 */      
#define ETH_MDIO_GPIO_CLK               RCM_APB2_PERIPH_GPIOA
#define ETH_MDIO_PORT                   GPIOA
#define ETH_MDIO_PIN                    GPIO_PIN_2
#define ETH_MDIO_AF                     GPIO_AF_ETH
#define ETH_MDIO_SOURCE                 GPIO_PIN_SOURCE_2
/* ETH_MCO PA8 */       
#define ETH_MCO_GPIO_CLK                RCM_APB2_PERIPH_GPIOA
#define ETH_MCO_PORT                    GPIOA
#define ETH_MCO_PIN                     GPIO_PIN_8
#define ETH_MCO_AF                      GPIO_AF_MCO
#define ETH_MCO_SOURCE                  GPIO_PIN_SOURCE_8

/**@} end of group ETH_RTT_TcpServer_Macros*/

/** @defgroup ETH_RTT_TcpServer_Functions Functions
  @{
*/

void Ethernet_Config(void);

/**@} end of group ETH_RTT_TcpServer_Functions */
/**@} end of group ETH_RTT_TcpServer */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_DP83848_H */
