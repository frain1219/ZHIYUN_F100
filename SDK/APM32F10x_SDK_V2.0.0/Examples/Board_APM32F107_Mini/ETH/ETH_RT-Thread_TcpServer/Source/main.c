/*!
 * @file       main.c
 *
 * @brief      ETH Main program body
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
#include "main.h"
#include "board_DP83848.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_RTT_TcpServer
  * @{
  */

/** @defgroup ETH_RTT_TcpServer_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART         USART1

/* Configurate local ETH information */
#define ETH_IP_ADDR         "169.254.90.123"
#define ETH_NET_MASK        "255.255.255.0"
#define ETH_GATEWAY         "169.254.90.1"

/**@} end of group ETH_RTT_TcpServer_Macros */

/** @defgroup ETH_RTT_TcpServer_Variables Variables
  * @{
  */

/* lwip network interface structure for ethernetif */
struct netif UserNetif;

/**@} end of group ETH_RTT_TcpServer_Variables*/

/** @defgroup ETH_RTT_TcpServer_Functions Functions
  * @{
  */

void LwIP_Init(void);

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    USART_Config_T usartConfigStruct;
    
    /* System clocks configuration */
    SystemClockConfig();

    /* USART Config */
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &usartConfigStruct);
    
    BOARD_LEDInit(LED2);
    
    printf("\r\nAPM32F107 MINI Board RT-Thread Tcp Server Example.\r\n");
    
    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    Ethernet_Config();

    /* Initilaize the LwIP stack */
    LwIP_Init();
    
    /* Initilaize the Tcp Server Task */
    tcp_server_task_init();

    while (1)
    {
        rt_thread_delay(1000);
        BOARD_LEDToggle(LED2);
    }
}

/*!
 * @brief     This function initializes the lwIP stack
 *
 * @param     None
 *
 * @retval    None
 */
void LwIP_Init(void)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;

    int ipAddrTemp[4];
    int netMaskTemp[4];
    int gatewayTemp[4];
    
    tcpip_init(NULL, NULL);

    /* Initializes the dynamic memory heap */
    mem_init();

    /* Initializes the memory pools */
    memp_init();

    sscanf(ETH_IP_ADDR, "%d.%d.%d.%d", &ipAddrTemp[0], &ipAddrTemp[1], &ipAddrTemp[2], &ipAddrTemp[3]);
    sscanf(ETH_NET_MASK, "%d.%d.%d.%d", &netMaskTemp[0], &netMaskTemp[1], &netMaskTemp[2], &netMaskTemp[3]);
    sscanf(ETH_GATEWAY, "%d.%d.%d.%d", &gatewayTemp[0], &gatewayTemp[1], &gatewayTemp[2], &gatewayTemp[3]);

    IP4_ADDR(&ipaddr, ipAddrTemp[0], ipAddrTemp[1], ipAddrTemp[2], ipAddrTemp[3]);
    IP4_ADDR(&netmask, netMaskTemp[0], netMaskTemp[1], netMaskTemp[2], netMaskTemp[3]);
    IP4_ADDR(&gw, gatewayTemp[0], gatewayTemp[1], gatewayTemp[2], gatewayTemp[3]);

    /* Add a network interface to the list of lwIP netifs */
    netif_add(&UserNetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&UserNetif);

    /* When the netif is fully configured this function must be called */
    netif_set_up(&UserNetif);

#ifdef MII_MODE
    printf("MII_MODE\r\n");
#elif defined RMII_MODE  /* Mode RMII */
    printf("RMII_MODE\r\n");
#endif
    printf("Static ETH Information:\r\n");
    printf("ETH_IP       : %d.%d.%d.%d\r\n", ipAddrTemp[0], ipAddrTemp[1], ipAddrTemp[2], ipAddrTemp[3]);
    printf("ETH_Net_Mask : %d.%d.%d.%d\r\n", netMaskTemp[0], netMaskTemp[1], netMaskTemp[2], netMaskTemp[3]);
    printf("ETH_Gateway  : %d.%d.%d.%d\r\n", gatewayTemp[0], gatewayTemp[1], gatewayTemp[2], gatewayTemp[3]);
}

/*!
 * @brief     Redirect C Library function printf to serial port.
 *            After Redirection, you can use printf function.
 *
 * @param     ch:  The characters that need to be send.
 *
 * @param     *f:  pointer to a FILE that can recording all information
 *            needed to control a stream
 *
 * @retval    The characters that need to be send.
 */
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

/**@} end of group ETH_RTT_TcpServer_Functions */
/**@} end of group ETH_RTT_TcpServer */
/**@} end of group Examples */
