/*!
 * @file        usbd_board.c
 *
 * @brief       This file provides firmware functions to USB board
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

/* Includes */
#include "board.h"
#include "bsp_delay.h"
#include "usbd_board.h"
#include "usbd_core.h"
#include "apm32f4xx_gpio.h"
#include "apm32f4xx_rcm.h"
#include "apm32f4xx_misc.h"
#include "apm32f4xx_usb_device.h"

/** @addtogroup Examples
  * @brief OTGD Custom HID examples
  @{
  */

/** @addtogroup OTGD_Custom_HID
  @{
  */

/** @defgroup OTGD_Custom_HID_Macros Macros
  @{
  */

#define USBD_FS_RX_FIFO_SIZE                128
#define USBD_FS_TX_FIFO_0_SIZE              64
#define USBD_FS_TX_FIFO_1_SIZE              128

#define USBD_HS_RX_FIFO_SIZE                512
#define USBD_HS_TX_FIFO_0_SIZE              128
#define USBD_HS_TX_FIFO_1_SIZE              372

/**@} end of group OTGD_Custom_HID_Macros */

/** @defgroup OTGD_Custom_HID_Variables Variables
  @{
  */

USBD_HANDLE_T usbDeviceHandler;

/**@} end of group OTGD_Custom_HID_Variables*/

/** @defgroup OTGD_Custom_HID_Functions Functions
  @{
  */

/*!
 * @brief       Init USB hardware
 *
 * @param       usbInfo:
 *
 * @retval      None
 */
void USBD_HardwareInit(USBD_INFO_T* usbInfo)
{
    GPIO_Config_T gpioConfig;

    /* Set systick as USB OTG delay clock source*/
    APM_DelayInit();

    if (usbInfo->devSpeed == USBD_SPEED_FS)
    {
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        /* Configure USB OTG */
        RCM_EnableAHB2PeriphClock(RCM_AHB2_PERIPH_OTG_FS);

        /* Configure USB OTG GPIO */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOA);

        /* Link structure */
        usbDeviceHandler.usbGlobal    = USB_OTG_FS;
        usbDeviceHandler.usbDevice    = USB_OTG_FS_D;
        usbDeviceHandler.usbFifo      = USB_OTG_FS_FIFO;
        usbDeviceHandler.usbPower     = USB_OTG_FS_PWR;

        /* Link data */
        usbDeviceHandler.dataPoint    = usbInfo;
        usbInfo->dataPoint            = &usbDeviceHandler;

        usbDeviceHandler.usbCfg.mode                = USB_OTG_MODE_DEVICE;
        usbDeviceHandler.usbCfg.phyType             = USB_OTG_PHY_EMB;
        usbDeviceHandler.usbCfg.dmaStatus           = DISABLE;
        usbDeviceHandler.usbCfg.sofStatus           = DISABLE;
        usbDeviceHandler.usbCfg.speed               = USB_OTG_SPEED_FSLS;
        usbDeviceHandler.usbCfg.speedChannel        = USBD_SPEED_CH_FS;
        usbDeviceHandler.usbCfg.devEndpointNum      = 4;
        usbDeviceHandler.usbCfg.lowPowerStatus      = DISABLE;
        usbDeviceHandler.usbCfg.powerManageStatus   = DISABLE;
        usbDeviceHandler.usbCfg.vbusSense           = DISABLE;
        usbDeviceHandler.usbCfg.ep1Status           = DISABLE;

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
        USB_OTG_DisableAllGlobalInterrupt(usbDeviceHandler.usbGlobal);

        /* Init USB Core */
        USBD_Config(&usbDeviceHandler);

        /* Configure FIFO */
        /* Configure RX FIFO */
        USB_OTG_ConfigRxFifoSize(usbDeviceHandler.usbGlobal, USBD_FS_RX_FIFO_SIZE);

        /* Configure TX FIFO */
        USBD_OTG_ConfigDeviceTxFifo(&usbDeviceHandler, 0, USBD_FS_TX_FIFO_0_SIZE);
        USBD_OTG_ConfigDeviceTxFifo(&usbDeviceHandler, 1, USBD_FS_TX_FIFO_1_SIZE);

        USBD_StartCallback(usbInfo);
    }
    else if (usbInfo->devSpeed == USBD_SPEED_HS)
    {
        /* Configure USB OTG*/
        RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);
        
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS);
        
        /* Configure USB OTG GPIO */
        RCM_EnableAHB1PeriphClock(RCM_AHB1_PERIPH_GPIOB);
        
        /* Link structure */
        usbDeviceHandler.usbGlobal    = USB_OTG_HS;
        usbDeviceHandler.usbDevice    = USB_OTG_HS_D;
        usbDeviceHandler.usbFifo      = USB_OTG_HS_FIFO;
        usbDeviceHandler.usbPower     = USB_OTG_HS_PWR;

        /* Link data */
        usbDeviceHandler.dataPoint    = usbInfo;
        usbInfo->dataPoint            = &usbDeviceHandler;

        usbDeviceHandler.usbCfg.mode                = USB_OTG_MODE_DEVICE;
        usbDeviceHandler.usbCfg.phyType             = USB_OTG_PHY_EMB;
        usbDeviceHandler.usbCfg.dmaStatus           = DISABLE;
        usbDeviceHandler.usbCfg.sofStatus           = DISABLE;
        usbDeviceHandler.usbCfg.speed               = USB_OTG_SPEED_HSFSLS;
        usbDeviceHandler.usbCfg.speedChannel        = USBD_SPEED_CH_HS;
        usbDeviceHandler.usbCfg.devEndpointNum      = 6;
        usbDeviceHandler.usbCfg.lowPowerStatus      = DISABLE;
        usbDeviceHandler.usbCfg.powerManageStatus   = DISABLE;
        usbDeviceHandler.usbCfg.vbusSense           = DISABLE;
        usbDeviceHandler.usbCfg.ep1Status           = DISABLE;

        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_OTG_HS_FS);
        GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_OTG_HS_FS);
        
        /* USB DM, DP pin configuration */
        gpioConfig.mode = GPIO_MODE_AF;
        gpioConfig.speed = GPIO_SPEED_100MHz;
        gpioConfig.otype = GPIO_OTYPE_PP;
        gpioConfig.pupd = GPIO_PUPD_NOPULL;
        gpioConfig.pin = GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_Config(GPIOB, &gpioConfig);

        /* NVIC */
        NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_4);
        NVIC_EnableIRQRequest(OTG_HS1_IRQn, 1, 0);

        /* Disable USB OTG all global interrupt */
        USB_OTG_DisableAllGlobalInterrupt(usbDeviceHandler.usbGlobal);

        /* Init USB Core */
        USBD_Config(&usbDeviceHandler);

        /* Configure FIFO */
        /* Configure RX FIFO */
        USB_OTG_ConfigRxFifoSize(usbDeviceHandler.usbGlobal, USBD_HS_RX_FIFO_SIZE);

        /* Configure TX FIFO */
        USBD_OTG_ConfigDeviceTxFifo(&usbDeviceHandler, 0, USBD_HS_TX_FIFO_0_SIZE);
        USBD_OTG_ConfigDeviceTxFifo(&usbDeviceHandler, 1, USBD_HS_TX_FIFO_1_SIZE);

        USBD_StartCallback(usbInfo);
    }
}

/*!
 * @brief       Reset USB hardware
 *
 * @param       usbInfo:usb handler information
 *
 * @retval      None
 */
void USBD_HardwareReset(USBD_INFO_T* usbInfo)
{
    if (usbInfo->devSpeed == USBD_SPEED_FS)
    {
        RCM_DisableAHB2PeriphClock(RCM_AHB2_PERIPH_OTG_FS);
        
        NVIC_DisableIRQRequest(OTG_FS_IRQn);
    }
    else if (usbInfo->devSpeed == USBD_SPEED_HS)
    {
        RCM_DisableAHB1PeriphClock(RCM_AHB1_PERIPH_OTG_HS);
        
        NVIC_DisableIRQRequest(OTG_HS1_IRQn);
    }
}

/*!
 * @brief       USB device start event callback function
 *
 * @param       usbInfo
 *
 * @retval      None
 */
void USBD_StartCallback(USBD_INFO_T* usbInfo)
{
    USBD_Start(usbInfo->dataPoint);
}

/*!
 * @brief     USB device stop handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
void USBD_StopCallback(USBD_INFO_T* usbInfo)
{
    USBD_Stop(usbInfo->dataPoint);
}

/*!
 * @brief     USB device stop device mode handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
void USBD_StopDeviceCallback(USBD_INFO_T* usbInfo)
{
    USBD_StopDevice(usbInfo->dataPoint);
}

/*!
 * @brief     USB OTG device resume callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_ResumeCallback(USBD_HANDLE_T* usbdh)
{
    USBD_Resume(usbdh->dataPoint);
}

/*!
 * @brief     USB OTG device suspend callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_SuspendCallback(USBD_HANDLE_T* usbdh)
{
    USBD_Suspend(usbdh->dataPoint);

    if((usbdh->usbCfg.phyType == USB_OTG_PHY_EMB) && \
        (usbdh->usbCfg.speed == USB_OTG_SPEED_HSFSLS))
    {
        /* Embedded HS PHY can not stop clock */
    }
    else
    {
        /* Stop PHY clock */
        usbdh->usbPower->PCGCTRL_B.PCLKSTOP = BIT_SET;
    }

    if (usbdh->usbCfg.lowPowerStatus == ENABLE)
    {
        /* Set SLEEPDEEP bit and SLEEPONEXIT SCR */
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
}

/*!
 * @brief     USB OTG device enum done callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_EnumDoneCallback(USBD_HANDLE_T* usbdh)
{
    USBD_DEVICE_SPEED_T speed = USBD_DEVICE_SPEED_FS;

    switch (usbdh->usbCfg.speed)
    {
        case USB_OTG_SPEED_HSFSLS:
            speed = USBD_DEVICE_SPEED_HS;
            break;

        case USB_OTG_SPEED_FSLS:
            speed = USBD_DEVICE_SPEED_FS;
            break;

        default:
            /* Speed error status */
            break;
    }

    /* Set USB core speed */
    USBD_SetSpeed(usbdh->dataPoint, speed);

    /* Reset device */
    USBD_Reset(usbdh->dataPoint);
}

/*!
 * @brief     USB OTG device SETUP stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_SetupStageCallback(USBD_HANDLE_T* usbdh)
{
    USBD_SetupStage(usbdh->dataPoint, (uint8_t*)usbdh->setup);
}

/*!
 * @brief     USB OTG device data OUT stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
void USBD_DataOutStageCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    USBD_DataOutStage(usbdh->dataPoint, epNum, usbdh->epOUT[epNum].buffer);
}

/*!
 * @brief     USB OTG device data IN stage callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
void USBD_DataInStageCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    USBD_DataInStage(usbdh->dataPoint, epNum, usbdh->epIN[epNum].buffer);
}

/*!
 * @brief     USB device set EP on stall status callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
USBD_STA_T USBD_EP_StallCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_EP_Stall(usbInfo->dataPoint, epAddr);

    return usbStatus;
}

/*!
 * @brief     USB device clear EP stall status callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
USBD_STA_T USBD_EP_ClearStallCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_EP_ClearStall(usbInfo->dataPoint, epAddr);

    return usbStatus;
}

/*!
 * @brief     USB device read EP stall status callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @retval    Stall status
 */
uint8_t USBD_EP_ReadStallStatusCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    return (USBD_EP_ReadStallStatus(usbInfo->dataPoint, epAddr));
}

/*!
 * @brief     USB device read EP last receive data size callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @retval    size of last receive data
 */
uint32_t USBD_EP_ReadRxDataLenCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    return USBD_EP_ReadRxDataLen(usbInfo->dataPoint, epAddr);
}

/*!
 * @brief     USB device open EP callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @param     epType: endpoint type
 *
 * @param     epMps: endpoint maxinum of packet size
 *
 * @retval    None
 */
void USBD_EP_OpenCallback(USBD_INFO_T* usbInfo, uint8_t epAddr, \
                          uint8_t epType, uint16_t epMps)
{
    USBD_EP_Open(usbInfo->dataPoint, epAddr, epType, epMps);
}

/*!
 * @brief     USB device close EP callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr: endpoint address
 *
 * @retval    None
 */
void USBD_EP_CloseCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    USBD_EP_Close(usbInfo->dataPoint, epAddr);
}

/*!
 * @brief     USB device EP receive handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    usb device status
 */
USBD_STA_T USBD_EP_ReceiveCallback(USBD_INFO_T* usbInfo, uint8_t epAddr, \
                                   uint8_t* buffer, uint32_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_EP_Receive(usbInfo->dataPoint, epAddr, buffer, length);

    return usbStatus;
}

/*!
 * @brief     USB device EP transfer handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr : endpoint address
 *
 * @param     buffer : data buffer
 *
 * @param     length : length of data
 *
 * @retval    usb device status
 */
USBD_STA_T USBD_EP_TransferCallback(USBD_INFO_T* usbInfo, uint8_t epAddr, \
                                    uint8_t* buffer, uint32_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_EP_Transfer(usbInfo->dataPoint, epAddr, buffer, length);

    return usbStatus;
}

/*!
 * @brief     USB device flush EP handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epAddr : endpoint address
 *
 * @retval    usb device status
 */
USBD_STA_T USBD_EP_FlushCallback(USBD_INFO_T* usbInfo, uint8_t epAddr)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_EP_Flush(usbInfo->dataPoint, epAddr);

    return usbStatus;
}

/*!
 * @brief     USB device set device address handler callback
 *
 * @param     usbInfo : usb handler information
 *
 * @param     address : address
 *
 * @retval    usb device status
 */
USBD_STA_T USBD_SetDevAddressCallback(USBD_INFO_T* usbInfo, uint8_t address)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_SetDevAddress(usbInfo->dataPoint, address);

    return usbStatus;
}

/*!
 * @brief       USB OTG device SOF event callback function
 *
 * @param       usbhh: USB host handler.
 *
 * @retval      None
 */
void USBD_SOFCallback(USBD_HANDLE_T* usbdh)
{
    USBD_HandleSOF(usbdh->dataPoint);
}

/*!
 * @brief     USB OTG device ISO IN in complete callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
void USBD_IsoInInCompleteCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    USBD_IsoInInComplete(usbdh->dataPoint, epNum);
}

/*!
 * @brief     USB OTG device ISO OUT in complete callback
 *
 * @param     usbdh: USB device handler
 *
 * @param     epNum: endpoint number
 *
 * @retval    None
 */
void USBD_IsoOutInCompleteCallback(USBD_HANDLE_T* usbdh, uint8_t epNum)
{
    USBD_IsoOutInComplete(usbdh->dataPoint, epNum);
}

/*!
 * @brief     USB OTG device connect callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_ConnectCallback(USBD_HANDLE_T* usbdh)
{
    USBD_Connect(usbdh->dataPoint);
}

/*!
 * @brief     USB OTG device disconnect callback
 *
 * @param     usbdh: USB device handler
 *
 * @retval    None
 */
void USBD_DisconnectCallback(USBD_HANDLE_T* usbdh)
{
    USBD_Disconnect(usbdh->dataPoint);
}

/**@} end of group OTGD_Custom_HID_Functions */
/**@} end of group OTGD_Custom_HID */
/**@} end of group Examples */
