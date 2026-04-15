/*!
 * @file       tcp_server_test.c
 *
 * @brief      tcp server test program body
 *
 * @version    V1.0.0
 *
 * @date       2022-12-01
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

/* Includes */
#include <stdio.h>
#include <string.h>

#include "lwip/tcpip.h"
#include "lwip/sockets.h"

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_FreeRTOS_TcpServer
  * @{
  */

/** @defgroup ETH_FreeRTOS_TcpServer_Macros Macros
  @{
*/

/* tcp server receive data buffer size */
#define BUFF_SIZE       (128)

/**@} end of group ETH_FreeRTOS_TcpServer_Macros */

/** @defgroup ETH_FreeRTOS_TcpServer_Variables Variables
  * @{
  */

/* tcp server receive data buffer */
static char recv_buff[BUFF_SIZE];

/* tcp server port */
static int port = 5000;

/**@} end of group ETH_FreeRTOS_TcpServer_Variables */

/** @defgroup ETH_FreeRTOS_TcpServer_Functions Functions
  * @{
  */

/*!
 * @brief       Tcp Server task
 *
 * @param       pvParameters: passed into the task function as the function parameters
 *
 * @retval      None
 */
static void tcp_server_task(void *pvParameters)
{
    int ret;
    int sock, connected, bytes_received;
    struct sockaddr_in server_addr, client_addr;

    struct timeval timeout;
    fd_set readset, readset_c;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        printf("Create socket error\r\n");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Unable to bind\r\n");
        closesocket(sock);
    }

    if (listen(sock, 5) == -1)
    {
        printf("Listen error\r\n");
        closesocket(sock);
    }

    printf("\nTCPServer Waiting for client on port %d...\r\n", port);

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (1)
    {
        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        printf("Waiting for a new connection...\r\n");

        if (select(sock + 1, &readset, NULL, NULL, &timeout) == 0)
            continue;

        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        if (connected < 0)
        {
            printf("accept connection failed!");
            continue;
        }

        printf("I got a connection from (%s , %d)\r\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1)
        {
            FD_ZERO(&readset_c);
            FD_SET(connected, &readset_c);

            /* Wait for read or write */
            if (select(connected + 1, &readset_c, NULL, NULL, &timeout) == 0)
                continue;
            
            memset(recv_buff, 0, BUFF_SIZE);
            bytes_received = recv(connected, recv_buff, BUFF_SIZE, 0);
            if (bytes_received < 0)
            {
                printf("Received error, close the connect.\r\n");
                closesocket(connected);
                connected = -1;
                break;
            }
            else if (bytes_received == 0)
            {
                printf("Received warning, recv function return 0.\r\n");
                closesocket(connected);
                connected = -1;
                break;
            }
            else
            {
                recv_buff[bytes_received] = '\0';
                printf("Received data: %s\r\n", recv_buff);
            }

            ret = send(connected, recv_buff, bytes_received, 0);
            if (ret < 0)
            {
                printf("send error, close the connect.\r\n");
                closesocket(connected);
                connected = -1;
                break;
            }
            else if (ret == 0)
            {
                printf("Send warning, send function return 0.\r\n");
                closesocket(connected);
                connected = -1;
                break;
            }
        }
    }
}

/*!
 * @brief       Initialize Tcp Server task.
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void tcp_server_task_init(void)
{
    sys_thread_new("tcp_server_task", tcp_server_task, NULL, 512, TCPIP_THREAD_PRIO);
}

/**@} end of group ETH_FreeRTOS_TcpServer_Functions */
/**@} end of group ETH_FreeRTOS_TcpServer */
/**@} end of group Examples */
