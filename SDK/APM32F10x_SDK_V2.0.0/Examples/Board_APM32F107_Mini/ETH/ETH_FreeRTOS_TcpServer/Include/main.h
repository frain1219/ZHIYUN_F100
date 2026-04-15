/*!
 * @file        main.h
 *
 * @brief       Header for main.c module
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

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes */
#include "Board.h"

#include "apm32f10x.h"
#include "apm32f10x_eth.h"
#include "apm32f10x_usart.h"
#include "apm32f10x_gpio.h"
#include "lwip/tcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwip/tcp_impl.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_FreeRTOS_TcpServer
  * @{
  */

/** @defgroup ETH_FreeRTOS_TcpServer_Variables Variables
  * @{
  */

/* lwip recvive dada Semaphore */
extern xSemaphoreHandle eth_rx_sem;

/**@} end of group ETH_FreeRTOS_TcpServer_Variables */

/** @defgroup ETH_FreeRTOS_TcpServer_Functions Functions
  * @{
  */

/* Initialize Tcp Server task */
void tcp_server_task_init(void);

/**@} end of group ETH_FreeRTOS_TcpServer_Functions */
/**@} end of group ETH_FreeRTOS_TcpServer */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
