/*!
 * @file       BOARD_DP83848.c
 *
 * @brief      This file provides all the config DP83848 functions
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
#include "board_DP83848.h"
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/netif.h"
#include <stdint.h>

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_FreeRTOS_TcpServer
  * @{
  */

/** @defgroup ETH_FreeRTOS_TcpServer_Variables Variables
  @{
*/

/* ETH Init Structure */
ETH_Config_T ETH_InitStructure;

/**@} end of group ETH_FreeRTOS_TcpServer_Variables */

/** @defgroup ETH_FreeRTOS_TcpServer_Functions Functions
  @{
*/

/* Configure the GPIO ports for ethernet pins */
static void Ethernet_GPIOConfig(void);
/* Configure the Ethernet MAC/DMA */
static void Ethernet_MACDMAConfig(void);
/* Ethernet Reset Delay */
static void Ethernet_ResetDelay(__IO uint32_t count);

/*!
 * @brief  configurate Ethernet.
 *
 * @param  None
 *
 * @retval None
 *
 * @note
 */
void Ethernet_Config(void)
{
    /* Configure the GPIO ports for ethernet pins */
    Ethernet_GPIOConfig();

    /* Configure the Ethernet MAC/DMA */
    Ethernet_MACDMAConfig();
    
    /* Enable ETH interrput */
    ETH_EnableDMAInterrupt(ETH_DMA_INT_NIS|ETH_DMA_INT_RX);
    NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);
    NVIC_EnableIRQRequest(ETH_IRQn, 6, 0);
}

/*!
 * @brief  configurate Ethernet Interface.
 *
 * @param  None
 *
 * @retval None
 *
 * @note
 */
static void Ethernet_MACDMAConfig(void)
{
    /* Enable ETHERNET clock  */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_ETH_MAC | RCM_AHB_PERIPH_ETH_MAC_TX |
                              RCM_AHB_PERIPH_ETH_MAC_RX);

    /* Reset ETH on AHB Bus */
    ETH_Reset();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    while (ETH_ReadSoftwareReset() == SET);
    /* ETH Config Struct */
    ETH_ConfigStructInit(&ETH_InitStructure);

    /* ETH Config MAC */
    /*
    *ETH_InitStructure.autoNegotiation = ETH_AUTONEGOTIATION_DISABLE;
    *ETH_InitStructure.speed = ETH_SPEED_100M;
    *ETH_InitStructure.mode = ETH_MODE_FULLDUPLEX;
    */
    ETH_InitStructure.autoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    ETH_InitStructure.loopbackMode = ETH_LOOPBACKMODE_DISABLE;
    ETH_InitStructure.retryTransmission = ETH_RETRYTRANSMISSION_DISABLE;
    ETH_InitStructure.automaticPadCRCStrip = ETH_AUTOMATICPADCRCSTRIP_DISABLE;
    ETH_InitStructure.receiveAll = ETH_RECEIVEAll_DISABLE;
    ETH_InitStructure.broadcastFramesReception = ETH_BROADCASTFRAMESRECEPTION_ENABLE;
    ETH_InitStructure.promiscuousMode = ETH_PROMISCUOUS_MODE_DISABLE;
    ETH_InitStructure.multicastFramesFilter = ETH_MULTICASTFRAMESFILTER_PERFECT;
    ETH_InitStructure.unicastFramesFilter = ETH_UNICASTFRAMESFILTER_PERFECT;

#ifdef HARDWARE_CHECKSUM
    ETH_InitStructure.checksumOffload = ETH_CHECKSUMOFFLAOD_ENABLE;
#endif

    /* ETH Config DMA */
    ETH_InitStructure.dropTCPIPChecksumErrorFrame = ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE;
    ETH_InitStructure.receiveStoreForward = ETH_RECEIVESTOREFORWARD_ENABLE;
    ETH_InitStructure.flushReceivedFrame = ETH_FLUSHRECEIVEDFRAME_DISABLE;
    ETH_InitStructure.transmitStoreForward = ETH_TRANSMITSTOREFORWARD_ENABLE;

    ETH_InitStructure.forwardErrorFrames = ETH_FORWARDERRORFRAMES_DISABLE;
    ETH_InitStructure.forwardUndersizedGoodFrames = ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE;
    ETH_InitStructure.secondFrameOperate = ETH_SECONDFRAMEOPERARTE_ENABLE;
    ETH_InitStructure.addressAlignedBeats = ETH_ADDRESSALIGNEDBEATS_ENABLE;
    ETH_InitStructure.fixedBurst = ETH_FIXEDBURST_ENABLE;
    ETH_InitStructure.rxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    ETH_InitStructure.txDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    ETH_InitStructure.DMAArbitration = ETH_DMAARBITRATION_ROUNDROBIN_RXTX_2_1;

    /* Configure Ethernet */
    ETH_Config(&ETH_InitStructure, DP83848_PHY_ADDRESS);
}

/*!
 * @brief  configurate Ethernet GPIO.
 *
 * @param  None
 *
 * @retval None
 *
 * @note
 */
void Ethernet_GPIOConfig(void)
{
    GPIO_Config_T configStruct;
    /*
    *   ETH_RESET#                          PB15
    *   ETH_INT                             PB14
    *   ETH_MDIO                            PA2
    *   ETH_MDC                             PC1
    *   ETH_MCO                             PA8
    */
    
    /* Enable GPIOs clocks */
    RCM_EnableAPB2PeriphClock(ETH_MDC_GPIO_CLK    | ETH_MDIO_GPIO_CLK  |
                              ETH_RESET_GPIO_CLK  | ETH_INT_GPIO_CLK   |
                              ETH_MCO_GPIO_CLK);

    /* Enable SYSCFG clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_AFIO);

    /* Configure ETH_RESET# */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = ETH_INT_PIN;
    configStruct.speed = GPIO_SPEED_50MHz;
    configStruct.mode = GPIO_MODE_OUT_PP;
    GPIO_Config(ETH_RESET_PORT, &configStruct);

    /* Configure ETH_MII_INT */
    configStruct.pin = ETH_INT_PIN;
    GPIO_Config(ETH_INT_PORT, &configStruct);
    ETH_INT_PIN_HIGH();

    /* Configure ETH_MDC */
    configStruct.pin = ETH_MDC_PIN;
    configStruct.mode = GPIO_MODE_AF_PP;
    GPIO_Config(ETH_MDC_PORT, &configStruct);

    /* Configure ETH_MDIO */
    configStruct.pin = ETH_MDIO_PIN;
    GPIO_Config(ETH_MDIO_PORT, &configStruct);

    /* RESET ETH PHY */
    ETH_RESET_PIN_LOW();
    Ethernet_ResetDelay(DP83848_RESET_DELAY);
    ETH_RESET_PIN_HIGH();
    Ethernet_ResetDelay(DP83848_RESET_DELAY);

    /* Configure PB10 ,PB11 ,PB12 ,PB13 as alternate function push-pull */
    configStruct.pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_Config(GPIOB, &configStruct);

    /* if BOARD_DP83848.h define MII_MODE, this function configurate MII_MODE */
#ifdef MII_MODE /** Mode MII */

    /* Enable GPIOs clocks */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA    | RCM_APB2_PERIPH_GPIOB  |
                              RCM_APB2_PERIPH_GPIOC  | RCM_APB2_PERIPH_GPIOD);

    /* Configure PC2  as alternate function push-pull */
    configStruct.pin = GPIO_PIN_8;
    GPIO_Config(GPIOB, &configStruct);

    /* Configure PC2  as alternate function push-pull */
    configStruct.pin = GPIO_PIN_2;
    GPIO_Config(GPIOC, &configStruct);

    GPIO_ConfigPinRemap(GPIO_REMAP_ETH_MAC);

    /* Configure PA1 and PA3 PA7 as input */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = GPIO_PIN_1| GPIO_PIN_3 | GPIO_PIN_7;
    configStruct.mode = GPIO_MODE_IN_FLOATING;
    GPIO_Config(GPIOA, &configStruct);

    /* Configure PB10 as input */
    configStruct.pin = GPIO_PIN_10;
    GPIO_Config(GPIOB, &configStruct);

    /* Configure PC3 as input */
    configStruct.pin = GPIO_PIN_3;
    GPIO_Config(GPIOC, &configStruct);

    /* Configure PD8, PD9, PD10 PD11 PD12 as input */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10| GPIO_PIN_11| GPIO_PIN_12;
    configStruct.mode = GPIO_MODE_IN_FLOATING;
    GPIO_Config(GPIOD, &configStruct);

#ifdef PHY_CLOCK_MCO
    /* Configure ETH_MCO */
    configStruct.pin = ETH_MCO_PIN;
    configStruct.mode = GPIO_MODE_AF_PP;
    configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(ETH_MCO_PORT, &configStruct);
    
    /* Get HSE clock = 25MHz on PA8 pin (MCO) */
    RCM_ConfigMCO(RCM_MCOCLK_HSE);

#endif /** PHY_CLOCK_MCO */

    GPIO_ConfigPinRemap(GPIO_REMAP_MACEISEL_MII);

#elif defined RMII_MODE  /* Mode RMII */

    /*
    *  ETH_RMII_REF_CLK    PA1
    *  ETH_RMII_CRS_DV     PD8
    *  ETH_RMII_RXD0       PD9
    *  ETH_RMII_RXD1       PD10
    *  ETH_RMII_TX_EN      PB11
    *  ETH_RMII_TXD0       PB12
    *  ETH_RMII_TXD1       PB13
    */

    /* Enable GPIOs clocks */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA    | RCM_APB2_PERIPH_GPIOB  |
                              RCM_APB2_PERIPH_GPIOC  | RCM_APB2_PERIPH_GPIOD);

    /* ETHERNET pins remapp in APM32107-MINI board: RX_DV and RxD[3:0] */
    GPIO_ConfigPinRemap(GPIO_REMAP_ETH_MAC);

    /* Configure PA1 and PA3 as input */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = GPIO_PIN_1;
    configStruct.mode = GPIO_MODE_IN_FLOATING;
    GPIO_Config(GPIOA, &configStruct);

    /* Configure PD8, PD9, PD10 as input */
    GPIO_ConfigStructInit(&configStruct);
    configStruct.pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    configStruct.mode = GPIO_MODE_IN_FLOATING;
    GPIO_Config(GPIOD, &configStruct);

#ifdef PHY_CLOCK_MCO
    /* Configure ETH_MCO */
    configStruct.pin = ETH_MCO_PIN;
    configStruct.mode = GPIO_MODE_AF_PP;
    configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(ETH_MCO_PORT, &configStruct);

    /* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
    RCM_ConfigPLL3(RCM_PLL3MF_10);

    /* Enable PLL3 */
    RCM_EnablePLL3();

    /* Wait till PLL3 is ready */
    while(RCM_ReadStatusFlag(RCM_FLAG_PLL3RDY) == RESET)
    {
        
    }
    
    /* Get PLL3 clock on PA8 pin (MCO) */
    RCM_ConfigMCO(RCM_MCOCLK_PLL3CLK);

#endif /** PHY_CLOCK_MCO */

    GPIO_ConfigPinRemap(GPIO_REMAP_MACEISEL_RMII);
#endif
}

/*!
 * @brief  Ethernet Reset Delay.
 *
 * @param  count  Reset Delay Time
 *
 * @retval None
 *
 * @note
 */
void Ethernet_ResetDelay(__IO uint32_t count)
{
    __IO uint32_t i = 0;
    
    for (i = count; i != 0; i--)
    {
    }
}

/**@} end of group ETH_FreeRTOS_TcpServer_Functions */
/**@} end of group ETH_FreeRTOS_TcpServer */
/**@} end of group Examples */
