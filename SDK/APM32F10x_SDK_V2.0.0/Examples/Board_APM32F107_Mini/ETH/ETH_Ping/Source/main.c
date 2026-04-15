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

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_Ping
  * @{
  */

/** @defgroup ETH_Ping_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART         USART1

/* Configurate local ETH information*/
#define ETH_IP_ADDR         "169.254.90.123"
#define ETH_NET_MASK        "255.255.255.0"
#define ETH_GATEWAY         "169.254.90.1"

/**@} end of group ETH_Ping_Macros*/

/** @defgroup ETH_Ping_Variables Variables
  * @{
  */

/* Global pointers on Rx descriptor used to transmit and receive descriptors */
extern ETH_DMADescConfig_T* DMARxDescToGet;

/* current Ethernet LocalTime value */
volatile uint32_t ETHTimer = 0;
/* lwip network interface structure for ethernetif */
struct netif UserNetif;

/* TCP periodic Timer */
uint32_t TCPTimer = 0;
/* ARP periodic Timer */
uint32_t ARPTimer = 0;
/* MAC address */
uint8_t SetMACaddr[6] = {0, 0, 0, 0, 0, 8};

/**@} end of group ETH_Ping_Variables*/

/** @defgroup ETH_Ping_Functions Functions
  * @{
  */

/* Configure SysTick */
void ConfigSysTick(void);
/* User config the different system Clock */
void UserRCMClockConfig(void);
/* Configures COM port */
void APM_BOARD_COMInit(COM_TypeDef COM, USART_Config_T* configStruct);

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    USART_Config_T usartConfig;

    /* System clocks configuration */
    SystemClockConfig();

    /* User config the different system Clock */
    UserRCMClockConfig();

    /* Configure SysTick */
    ConfigSysTick();

    /* Configure USART
    - USARTx configured as follow:
    - BaudRate = 115200
    - Word Length = USART_WordLength_8b
    - Stop Bit = USART_StopBits_1
    - Parity = USART_Parity_No
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    usartConfig.baudRate = 115200;
    usartConfig.wordLength = USART_WORD_LEN_8B;
    usartConfig.stopBits = USART_STOP_BIT_1;
    usartConfig.parity = USART_PARITY_NONE ;
    usartConfig.mode = USART_MODE_TX_RX;
    usartConfig.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    APM_BOARD_COMInit(COM1, &usartConfig);

    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    Ethernet_Config();

    /* Initilaize the LwIP stack */
    LwIP_Init();

    while (1)
    {
        /* check if any packet received */
        if (ETH_ReadRxPacketSize(DMARxDescToGet))
        {
            /* process received ethernet packet */
            LwIP_Pkt_Handle();
        }

        /* handle periodic timers for LwIP */
        LwIP_Periodic_Handle(ETHTimer);
    }
}

/*!
* @brief     Configure SysTick
*
* @param     None
*
* @retval    None
*/
void ConfigSysTick(void)
{
    if (SysTick_Config(SystemCoreClock / 100))
    {
        /* Capture error */
        while (1);
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

    /* Config MAC Address */
    ETH_ConfigMACAddress(ETH_MAC_ADDRESS0, SetMACaddr);

    /* Add a network interface to the list of lwIP netifs */
    netif_add(&UserNetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

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
 * @brief     This function received ethernet packet
 *
 * @param     None
 *
 * @retval    None
 */
void LwIP_Pkt_Handle(void)
{
    /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
    ethernetif_input(&UserNetif);
}

/*!
 * @brief     This function LwIP periodic tasks
 *
 * @param     ETHTimer the current Ethernet Timer value
 *
 * @retval    None
 */
void LwIP_Periodic_Handle(__IO uint32_t ETHTimer)
{
#if LWIP_TCP

    /* TCP periodic process every 250 ms */
    if (ETHTimer - TCPTimer >= TCP_TMR_INTERVAL)
    {
        TCPTimer =  ETHTimer;
        tcp_tmr();
    }

#endif

    /* ARP periodic process every 5s */
    if ((ETHTimer - ARPTimer) >= ARP_TMR_INTERVAL)
    {
        ARPTimer =  ETHTimer;
        etharp_tmr();
    }
}

/*!
 * @brief     User config the different system Clock
 *
 * @param     None
 *
 * @retval    None
 */
void UserRCMClockConfig(void)
{
    SystemInit();
}

/*!
 * @brief     Configures COM port.
 *
 * @param     COM: Specifies the COM port to be configured.
 *            This parameter can be one of following parameters:
 *            @arg COM1
 *            @arg COM2
 *
 * @retval    None
 */
void APM_BOARD_COMInit(COM_TypeDef COM, USART_Config_T* configStruct)
{
    BOARD_COMInit(COM1, configStruct);
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group ETH_Ping_Functions */
/**@} end of group ETH_Ping */
/**@} end of group Examples */
