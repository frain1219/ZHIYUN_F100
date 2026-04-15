/*!
 * @file        usbh_board.c
 *
 * @brief       This file provides firmware functions to USB board
 *
 * @version     V1.0.0
 *
 * @date        2023-03-27
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

/* Includes */
#include "board.h"
#include "bsp_delay.h"
#include "usbh_board.h"
#include "usbh_core.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usb_host.h"

/** @addtogroup Examples
  * @brief OTGH MSC HS1 examples
  @{
  */

/** @addtogroup OTGH_MSC_HS1
  @{
  */

/** @defgroup OTGH_MSC_HS1_Variables Variables
  @{
  */

USBH_HANDLE_T usbHostHandler;

/**@} end of group OTGH_MSC_HS1_Variables*/

/** @defgroup OTGH_MSC_HS1_Functions Functions
  @{
  */

/*!
 * @brief       Init USB hardware
 *
 * @param       usbInfo:
 *
 * @retval      None
 */
void USBH_HardwareInit(USBH_INFO_T* usbInfo)
{
    GPIO_Config_T gpioConfig;

    /* Set systick as USB OTG delay clock source*/
    APM_DelayInit();

    if (usbInfo->hostSpeed == USBH_SPEED_FS)
    {
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        /* Configure USB OTG */
        RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_OTG_FS);

        /* Configure USB OTG GPIO */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

        /* Link structure */
        usbHostHandler.usbGlobal    = USB_OTG_FS;
        usbHostHandler.usbHost      = USB_OTG_FS_H;
        usbHostHandler.usbFifo      = USB_OTG_FS_FIFO;
        usbHostHandler.usbPower     = USB_OTG_FS_PWR;

        /* Link data */
        usbHostHandler.dataPoint    = usbInfo;
        usbInfo->dataPoint          = &usbHostHandler;

        usbHostHandler.usbCfg.mode              = USB_OTG_MODE_HOST;
        usbHostHandler.usbCfg.phyType           = USB_OTG_PHY_EMB;
        usbHostHandler.usbCfg.dmaStatus         = DISABLE;
        usbHostHandler.usbCfg.sofStatus         = DISABLE;
        usbHostHandler.usbCfg.speed             = USB_OTG_SPEED_FSLS;
        usbHostHandler.usbCfg.hostChannelNum    = 8;

        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_11, GPIO_AF_OTG1_FS);
        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_12, GPIO_AF_OTG1_FS);

        /* USB DM, DP pin configuration */
        gpioConfig.mode = GPIO_MODE_AF;
        gpioConfig.speed = GPIO_SPEED_100MHz;
        gpioConfig.otype = GPIO_OTYPE_PP;
        gpioConfig.pupd = GPIO_PUPD_NOPULL;
        gpioConfig.pin = GPIO_PIN_11 | GPIO_PIN_12;
        GPIO_Config(GPIOA, &gpioConfig);

        /* NVIC */
        NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);
        NVIC_EnableIRQRequest(OTG_FS_IRQn, 1, 0);

        /* Disable USB OTG all global interrupt */
        USB_OTG_DisableAllGlobalInterrupt(usbHostHandler.usbGlobal);

        /* Init USB Core */
        USBH_Config(&usbHostHandler);

        USBH_StartCallback(usbInfo);
    }
    else if (usbInfo->hostSpeed == USBH_SPEED_HS)
    {
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        
        /* Configure USB OTG*/
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS);

        /* Configure USB OTG GPIO */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA | \
                                  RCM_AHB1_PERIPH_GPIOB | \
                                  RCM_AHB1_PERIPH_GPIOC | \
                                  RCM_AHB1_PERIPH_GPIOH | \
                                  RCM_AHB1_PERIPH_GPIOI);
        
        /* Link structure */
        usbHostHandler.usbGlobal    = USB_OTG_HS;
        usbHostHandler.usbHost      = USB_OTG_HS_H;
        usbHostHandler.usbFifo      = USB_OTG_HS_FIFO;
        usbHostHandler.usbPower     = USB_OTG_HS_PWR;

        /* Link data */
        usbHostHandler.dataPoint    = usbInfo;
        usbInfo->dataPoint          = &usbHostHandler;
        
        usbHostHandler.usbCfg.mode              = USB_OTG_MODE_HOST;
        usbHostHandler.usbCfg.phyType           = USB_OTG_PHY_EX;
        usbHostHandler.usbCfg.dmaStatus         = DISABLE;
        usbHostHandler.usbCfg.sofStatus         = DISABLE;
        usbHostHandler.usbCfg.speed             = USB_OTG_SPEED_HSFSLS;
        usbHostHandler.usbCfg.hostChannelNum    = 12;
        
        /** OTG HS GPIO
            PA3  -------------> OTG HS ULPI D0
            PB0  -------------> OTG HS ULPI D1
            PB1  -------------> OTG HS ULPI D2
            PB10 -------------> OTG HS ULPI D3
            PB11 -------------> OTG HS ULPI D4
            PB12 -------------> OTG HS ULPI D5
            PB13 -------------> OTG HS ULPI D6
            PB5  -------------> OTG HS ULPI D7
            PA5  -------------> OTG HS ULPI CK
            PC0  -------------> OTG HS ULPI STP
            PI11 -------------> OTG HS ULPI DIR
            PH4  -------------> OTG HS ULPI NXT
        */

        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_OTG_HS); /* OTG HS ULPI D0 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_0, GPIO_AF_OTG_HS); /* OTG HS ULPI D1 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_1, GPIO_AF_OTG_HS); /* OTG HS ULPI D2 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_OTG_HS); /* OTG HS ULPI D3 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_11, GPIO_AF_OTG_HS); /* OTG HS ULPI D4 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_12, GPIO_AF_OTG_HS); /* OTG HS ULPI D5 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_13, GPIO_AF_OTG_HS); /* OTG HS ULPI D6 */
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_OTG_HS); /* OTG HS ULPI D7 */
        GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_OTG_HS); /* OTG HS ULPI CK */
        GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_0, GPIO_AF_OTG_HS); /* OTG HS ULPI STP */
        GPIO_ConfigPinAF(GPIOI, GPIO_PIN_SOURCE_11, GPIO_AF_OTG_HS); /* OTG HS ULPI DIR */
        GPIO_ConfigPinAF(GPIOH, GPIO_PIN_SOURCE_4, GPIO_AF_OTG_HS); /* OTG HS ULPI NXT */

        gpioConfig.pin = GPIO_PIN_3 | GPIO_PIN_5;
        gpioConfig.mode = GPIO_MODE_AF;
        gpioConfig.speed = GPIO_SPEED_100MHz;
        gpioConfig.otype = GPIO_OTYPE_PP;
        gpioConfig.pupd = GPIO_PUPD_NOPULL;
        GPIO_Config(GPIOA, &gpioConfig);

        gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                         GPIO_PIN_13 | GPIO_PIN_5;
        GPIO_Config(GPIOB, &gpioConfig);

        gpioConfig.pin = GPIO_PIN_0;
        GPIO_Config(GPIOC, &gpioConfig);

        gpioConfig.pin = GPIO_PIN_11;
        GPIO_Config(GPIOI, &gpioConfig);

        gpioConfig.pin = GPIO_PIN_4;
        GPIO_Config(GPIOH, &gpioConfig);

        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS | RCM_AHB1_PERIPH_OTG_HS_ULPI);

        /* NVIC */
        NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);
        NVIC_EnableIRQRequest(OTG_HS1_IRQn, 1, 0);

        /* Disable USB OTG all global interrupt */
        USB_OTG_DisableAllGlobalInterrupt(usbHostHandler.usbGlobal);

        /* Init USB Core */
        USBH_Config(&usbHostHandler);

        USBH_StartCallback(usbInfo);
    }
}

/*!
 * @brief       USB hardware reset
 *
 * @param       usbInfo:usb handler information
 *
 * @retval      None
 */
void USBH_HardwareReset(USBH_INFO_T* usbInfo)
{
    if (usbInfo->hostSpeed == USBH_SPEED_FS)
    {
        RCM_DisableAHB2PeriphClock(RCM_AHB2_PERIPH_OTG_FS);

        NVIC_DisableIRQRequest(OTG_FS_IRQn);
    }
    else if (usbInfo->hostSpeed == USBH_SPEED_HS)
    {
        RCM_DisableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS | RCM_AHB1_PERIPH_OTG_HS_ULPI);

        NVIC_DisableIRQRequest(OTG_HS1_IRQn);
    }
}

/*!
 * @brief       USB host SOF event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_SOFCallback(USBH_HANDLE_T* usbhh)
{
    USBH_IncTimer(usbhh->dataPoint);
}

/*!
 * @brief       USB host URB status update event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_UpdateUrbCallback(USBH_HANDLE_T* usbhh)
{

}

/*!
 * @brief       USB host port enable event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_PortEnableCallback(USBH_HANDLE_T* usbhh)
{
    USBH_PortEnable(usbhh->dataPoint);
}

/*!
 * @brief       USB host port disable event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_PortDisableCallback(USBH_HANDLE_T* usbhh)
{
    USBH_PortDisable(usbhh->dataPoint);
}

/*!
 * @brief       USB host connect event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_ConnectCallback(USBH_HANDLE_T* usbhh)
{
    USBH_Connect(usbhh->dataPoint);
}

/*!
 * @brief       USB host disconnect event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_DisconnectCallback(USBH_HANDLE_T* usbhh)
{
    USBH_Disconnect(usbhh->dataPoint);
}

/*!
 * @brief       USB stop host event callback function
 *
 * @param       usbInfo
 *
 * @retval      None
 */
void USBH_StopHostCallback(USBH_INFO_T* usbInfo)
{
    USBH_OTG_StopHost(usbInfo->dataPoint);
}

/*!
 * @brief       USB host start event callback function
 *
 * @param       usbInfo
 *
 * @retval      None
 */
void USBH_StartCallback(USBH_INFO_T* usbInfo)
{
    USBH_Start(usbInfo->dataPoint);
}

/*!
 * @brief       USB host reset event callback function
 *
 * @param       usbInfo
 *
 * @retval      None
 */
void USBH_ResetCallback(USBH_INFO_T* usbInfo)
{
    USBH_OTG_ResetHost(usbInfo->dataPoint);
}

/*!
 * @brief       USB host read speed event callback function
 *
 * @param       usbInfo
 *
 * @retval      speed
 */
uint8_t USBH_ReadSpeedCallback(USBH_INFO_T* usbInfo)
{
    return (USBH_OTG_ReadSpeed(usbInfo->dataPoint));
}

/*!
 * @brief     USB host config the channel to transfer event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel numer
 *
 * @param     endPointNum : end point number
 *
 * @param     devAddr : USB device address
 *
 * @param     devSpeed : USB device speed
 *
 * @param     epType : end point type
 *
 * @param     packetMaxSize : max size of packet
 *
 * @retval    None
 */
void USBH_OpenChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                              uint8_t endPointNum, uint8_t devAddr, \
                              uint8_t devSpeed, uint8_t epType, uint16_t packetMaxSize)
{
    USBH_OTG_OpenChannel(usbInfo->dataPoint, channelNum, endPointNum, devAddr, devSpeed, epType, packetMaxSize);
}

/*!
 * @brief     USB submit URB event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     chNum : channel number
 *
 * @param     dir : channel direction
 *
 * @param     epType : endpoint type
 *
 * @param     tokenType : tokenType
 *
 * @param     buffer : URB data
 *
 * @param     length : length of URB data
 *
 * @param     pingStatus : ping status
 *
 * @retval    None
 */
void USBH_UrbSubmitCallback(USBH_INFO_T* usbInfo, uint8_t chNum, uint8_t dir, \
                            uint8_t epType, uint8_t tokenType, uint8_t* buffer, \
                            uint16_t length, uint8_t pingStatus)
{
    USBH_OTG_ChannelSubReq(usbInfo->dataPoint, chNum, dir, \
                           epType, tokenType, buffer, \
                           length, pingStatus);
}

/*!
 * @brief     USB host read URB status event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @retval    URB status
 */
uint8_t USBH_ReadUrbStatusCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    return USBH_OTG_ReadUrbStatus(usbInfo->dataPoint, channelNum);
}

/*!
 * @brief     USB host configure data PID callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @param     dataPid: data PID
 *
 * @retval    None
 */
void USBH_ConfigDataPidCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, uint8_t dataPid)
{
    USBH_ConfigDataPid(usbInfo->dataPoint, channelNum, dataPid);
}

/*!
 * @brief     USB host close channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    None
 */
void USBH_CloseChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    USBH_CloseChannel(usbInfo->dataPoint, channelNum);
}

/*!
 * @brief     USB host read size of last xfer callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    xfer size
 */
uint32_t USBH_ReadLastXferSizeCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    return (uint32_t)USBH_OTG_ReadXferSize(usbInfo->dataPoint, channelNum);
}

/*!
 * @brief     USB host read current toggle of channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    xfer size
 */
uint8_t USBH_ReadToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    return (uint8_t)USBH_OTG_ReadToggle(usbInfo->dataPoint, channelNum);
}

/*!
 * @brief     USB host configure current toggle of channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @param     toggle: toggle
 *
 * @retval    xfer size
 */
void USBH_ConfigToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, uint8_t toggle)
{
    USBH_OTG_ConfigToggle(usbInfo->dataPoint, channelNum, toggle);
}

/*!
 * @brief       USB host hardware reset event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBH_HardwareResetCallback(USBH_HANDLE_T* usbhh)
{
    USBH_HardwareReset(usbhh->dataPoint);
}

/**@} end of group OTGH_MSC_HS1_Functions */
/**@} end of group OTGH_MSC_HS1 */
/**@} end of group Examples */
