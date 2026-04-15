/*!
 * @file        apm32l0xx_i2c.h
 *
 * @brief       This file contains all the functions prototypes for the I2C firmware library
 *
 * @version     V1.0.0
 *
 * @date        2021-12-25
 *
 * @attention
 *
 *  Copyright (C) 2020-2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#ifndef __APM32L0xx_I2C_H
#define __APM32L0xx_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup I2C_Driver I2C Driver
  @{
*/

/** @addtogroup I2C_Enumerations Enumerations
  @{
*/

/**
 * @brief   I2C Analog noise filter
 */
typedef enum
{
    I2C_ANALOG_FILTER_ENABLE , //!< Analog noise filter enabled
    I2C_ANALOG_FILTER_DISABLE, //!< Analog noise filter disabled
} I2C_ANALOG_FILTER_T;

/**
 * @brief   I2C  Digital noise filter
 */
typedef enum
{
    I2C_DIGITAL_FILTER_0 ,  //!< Digital filter disabled
    I2C_DIGITAL_FILTER_1 ,  //!< Digital filter enabled and filtering capability is 1 T_i2cclk
    I2C_DIGITAL_FILTER_2 ,  //!< Digital filter enabled and filtering capability is 3 T_i2cclk
    I2C_DIGITAL_FILTER_3 ,  //!< Digital filter enabled and filtering capability is 3 T_i2cclk
    I2C_DIGITAL_FILTER_4 ,  //!< Digital filter enabled and filtering capability is 4 T_i2cclk
    I2C_DIGITAL_FILTER_5 ,  //!< Digital filter enabled and filtering capability is 5 T_i2cclk
    I2C_DIGITAL_FILTER_6 ,  //!< Digital filter enabled and filtering capability is 6 T_i2cclk
    I2C_DIGITAL_FILTER_7 ,  //!< Digital filter enabled and filtering capability is 7 T_i2cclk
    I2C_DIGITAL_FILTER_8 ,  //!< Digital filter enabled and filtering capability is 8 T_i2cclk
    I2C_DIGITAL_FILTER_9 ,  //!< Digital filter enabled and filtering capability is 9 T_i2cclk
    I2C_DIGITAL_FILTER_10,  //!< Digital filter enabled and filtering capability is 10 T_i2cclk
    I2C_DIGITAL_FILTER_11,  //!< Digital filter enabled and filtering capability is 11 T_i2cclk
    I2C_DIGITAL_FILTER_12,  //!< Digital filter enabled and filtering capability is 12 T_i2cclk
    I2C_DIGITAL_FILTER_13,  //!< Digital filter enabled and filtering capability is 13 T_i2cclk
    I2C_DIGITAL_FILTER_14,  //!< Digital filter enabled and filtering capability is 14 T_i2cclk
    I2C_DIGITAL_FILTER_15,  //!< Digital filter enabled and filtering capability is 15 T_i2cclk
} I2C_DIGITAL_FILTER_T;

/**
 * @brief    I2C mode
 */
typedef enum
{
    I2C_MODE_I2C         = 0,
    I2C_MODE_SMBUSDEVICE = BIT20, //!< SMBus Device Default address enable
    I2C_MODE_SMBUSHOST   = BIT21, //!< SMBus Host address enable
} I2C_MODE_T;

/**
 * @brief    I2C acknowledge
 */
typedef enum
{
    I2C_ACK_ENABLE ,            //!< ACK generation (slave mode)
    I2C_ACK_DISABLE,            //!< NACK generation (slave mode)
} I2C_ACK_T;

/**
 * @brief    I2C acknowledge address
 */
typedef enum
{
    I2C_ACK_ADDRESS_7BIT ,      //!< ACK generation (slave mode)
    I2C_ACK_ADDRESS_10BIT,      //!< NACK generation (slave mode)
} I2C_ACK_ADDRESS_T;


/**
 * @brief   I2C transfer direction
 */
typedef enum
{
    I2C_DIRECTION_TX,           //!< Transmission direction
    I2C_DIRECTION_RX,           //!< Reception direction
} I2C_DIRECTION_T;

/**
 * @brief   I2C DMA requests
 */
typedef enum
{
    I2C_DMA_REQ_TX,             //!< DMA transmission requests enable
    I2C_DMA_REQ_RX,             //!< DMA reception requests enable
} I2C_DMA_REQ_T;

/**
 * @brief   I2C own address2 mask
 */
typedef enum
{
    I2C_ADDR2MSK_NOMASK,      //!< No masked
    I2C_ADDR2MSK_MASK01,      //!< Don't care masked ADDR2[1:0]
    I2C_ADDR2MSK_MASK02,      //!< Don't care masked ADDR2[2:1]
    I2C_ADDR2MSK_MASK03,      //!< Don't care masked ADDR2[3:1]
    I2C_ADDR2MSK_MASK04,      //!< Don't care masked ADDR2[4:1]
    I2C_ADDR2MSK_MASK05,      //!< Don't care masked ADDR2[5:1]
    I2C_ADDR2MSK_MASK06,      //!< Don't care masked ADDR2[6:1]
    I2C_ADDR2MSK_MASK07,      //!< Don't care masked ADDR2[7:1]
} I2C_ADDR2MSK_T;

/**
 * @brief   I2C registers
 */
typedef enum
{
    I2C_REGISTER_CTRL1    = ((uint8_t)0x00),   //!< CTRL1 register
    I2C_REGISTER_CTRL2    = ((uint8_t)0x04),   //!< CTRL2 register
    I2C_REGISTER_ADDR1    = ((uint8_t)0x08),   //!< ADDR1 register
    I2C_REGISTER_ADDR2    = ((uint8_t)0x0C),   //!< ADDR2 register
    I2C_REGISTER_TIMING   = ((uint8_t)0x10),   //!< TIMING register
    I2C_REGISTER_TIMEOUT  = ((uint8_t)0x14),   //!< TIMEOUT register
    I2C_REGISTER_STS      = ((uint8_t)0x18),   //!< STS register
    I2C_REGISTER_INTFCLR  = ((uint8_t)0x1C),   //!< INTFCLR register
    I2C_REGISTER_PEC      = ((uint8_t)0x20),   //!< PEC register
    I2C_REGISTER_RXDATA   = ((uint8_t)0x24),   //!< RXDATA register
    I2C_REGISTER_TXDATA   = ((uint8_t)0x28),   //!< TXDATA register
} I2C_REGISTER_T;



/**
 * @brief   I2C interrupts source
 */
typedef enum
{
    I2C_INT_TXIE     = BIT1,        //!< TX Interrupt enable
    I2C_INT_RXIE     = BIT2,        //!< RX Interrupt enable
    I2C_INT_ADDRIE   = BIT3,        //!< Address match interrupt enable (slave only)
    I2C_INT_NACKIE   = BIT4,        //!< Not acknowledge received interrupt enable
    I2C_INT_STOPIE   = BIT5,        //!< STOP detection Interrupt enable
    I2C_INT_TXCIE    = BIT6,        //!< Transfer complete interrupt enable
    I2C_INT_ERRIE    = BIT7,        //!< Error interrupts enable
} I2C_INT_T;

/**
 * @brief   I2C Flags
 */
typedef enum
{
    /** STS1 register flag */
    I2C_FLAG_TXBE    = BIT0 ,     //!< Transmit buffer data register empty flag
    I2C_FLAG_TXINT   = BIT1 ,     //!< Transmit interrupt flag
    I2C_FLAG_RXBNE   = BIT2 ,     //!< Read buffer data register not empty flag
    I2C_FLAG_ADDR    = BIT3 ,     //!< Address Sent/Matched (master/slave) flag
    I2C_FLAG_NACK    = BIT4 ,     //!< Not acknowledge received flag
    I2C_FLAG_STOP    = BIT5 ,     //!< Stop detected flag
    I2C_FLAG_TXCF    = BIT6 ,     //!< Transfer complete flag
    I2C_FLAG_TCRF    = BIT7 ,     //!< Transfer complete reload flag
    I2C_FLAG_BUSERR  = BIT8 ,     //!< Bus error flag
    I2C_FLAG_ALF     = BIT9 ,     //!< Arbitration Loss flag
    I2C_FLAG_OVR     = BIT10,     //!< Overrun/Underrun flag
    I2C_FLAG_PECERR  = BIT11,     //!< PEC error flag
    I2C_FLAG_TIMEOUT = BIT12,     //!< Timeout or t_low detection flag
    I2C_FLAG_ALERT   = BIT13,     //!< SMBus alert flag
    I2C_FLAG_BUSY    = BIT15,     //!< Bus Busy Flag
    I2C_FLAG_TXDIR   = BIT16,     //!< Transfer Direction Flag
} I2C_FLAG_T;

/**
 * @brief   I2C Interrupt flag
 */
typedef enum
{
    I2C_INT_FLAG_TXINT   = BIT1 , //!< Transmit interrupt flag
    I2C_INT_FLAG_RXBNE   = BIT2 , //!< Read Buffer Data Register Not Empty interrupt flag
    I2C_INT_FLAG_ADDR    = BIT3 , //!< Address Sent/Matched (master/slave) interrupt flag
    I2C_INT_FLAG_NACK    = BIT4 , //!< Not acknowledge received interrupt flag
    I2C_INT_FLAG_STOP    = BIT5 , //!< Stop detected interrupt flag
    I2C_INT_FLAG_TXCF    = BIT6 , //!< Transfer complete interrupt flag
    I2C_INT_FLAG_TCRF    = BIT7 , //!< Transfer Complete Reloadinterrupt flag
    I2C_INT_FLAG_BUSERR  = BIT8 , //!< Bus error interrupt flag
    I2C_INT_FLAG_ALF     = BIT9 , //!< Arbitration Loss interrupt flag
    I2C_INT_FLAG_OVR     = BIT10, //!< Overrun/Underrun interrupt flag
    I2C_INT_FLAG_PECERR  = BIT11, //!< PEC error interrupt flag
    I2C_INT_FLAG_TIMEOUT = BIT12, //!< Timeout or t_low detection interrupt flag
    I2C_INT_FLAG_ALERT   = BIT13, //!< SMBus alert interrupt flag
} I2C_INT_FLAG_T;

/**
 * @brief    I2C Reload End Mode
 */
typedef enum
{
    I2C_RELOAD_MODE_RELOAD    = BIT24,  //!< Enable Reload mode
    I2C_RELOAD_MODE_AUTOEND   = BIT25,  //!< Enable Automatic end mode
    I2C_RELOAD_MODE_SOFTEND   = 0,  //!< Enable Software end mode
} I2C_RELOAD_MODE_T;

/**
 * @brief    I2C Start/Stop Mode
 */
typedef enum
{
    I2C_GENERATE_NO_STARTSTOP =  0,  //!< Don't Generate stop and start condition.
    I2C_GENERATE_START_WRITE  =  BIT13,  //!< Generate Restart for write request
    I2C_GENERATE_STOP         =  BIT14,  //!< Generate stop condition
    I2C_GENERATE_START_READ   =  BIT10 | BIT13,  //!< Generate Restart for read request
} I2C_GENERATE_T;

/**@} end of group I2C_Enumerations*/


/** @addtogroup I2C_Macros Macros
  @{
*/

#define I2C_CTRL2_SADD          ((uint32_t)0x000003FF)    //<! Slave address (master mode)
#define I2C_CTRL2_NUMBYT        ((uint32_t)0x00FF0000)    //<! Number of bytes
#define I2C_CTRL2_MASK          ((uint32_t)0x03006400)    //<! ENDCFG,RELOADEN,STOP,START,TXDIR clear CTRL2 register Mask

/** I2C Register reset value */
#define  I2C_CTRL1_RESET_VALUE      ((uint32_t)0x00)
#define  I2C_CTRL2_RESET_VALUE      ((uint32_t)0x00)
#define  I2C_ADDR1_RESET_VALUE      ((uint32_t)0x00)
#define  I2C_ADDR2_RESET_VALUE      ((uint32_t)0x00)
#define  I2C_TIMING_RESET_VALUE     ((uint32_t)0x00)
#define  I2C_TIMEOUT_RESET_VALUE    ((uint32_t)0x00)
#define  I2C_STS_RESET_VALUE        ((uint32_t)0x01)
#define  I2C_INTFCLR_RESET_VALUE    ((uint32_t)0x00)
#define  I2C_PEC_RESET_VALUE        ((uint32_t)0x00)
#define  I2C_TXDATA_RESET_VALUE     ((uint32_t)0x00)
#define  I2C_RXDATA_RESET_VALUE     ((uint32_t)0x00)

/** I2C_channels */
#define  I2C_Channel_TempSensor   ((uint32_t)I2C_CHANNEL_16)
#define  I2C_Channel_Vrefint      ((uint32_t)I2C_CHANNEL_17)

/**@} end of group I2C_Macros*/


/** @addtogroup I2C_Structure Data Structure
  @{
*/

/**
 * @brief   I2C Config struct definition
 */
typedef struct
{
    uint32_t              timing;        //!< Specifies the I2C_TIMINGR_register value.
    uint32_t              address1;      //!< Specifies the device own address 1.
    I2C_ACK_T             ack;           //!< Enables or disables the acknowledgement.
    I2C_MODE_T            mode;          //!< Specifies the I2C mode.
    I2C_ACK_ADDRESS_T     ackaddress;    //!< Specifies if 7-bit or 10-bit address is acknowledged.
    I2C_ANALOG_FILTER_T   analogfilter;  //!< Enables or disables analog noise filter.
    I2C_DIGITAL_FILTER_T  digitalfilter; //!< Configures the digital noise filter.
} I2C_Config_T;

/**@} end of group I2C_Structure*/


/** @addtogroup I2C_Fuctions Fuctions
  @{
*/

/** I2C reset and configuration */
void I2C_Reset(I2C_T* i2c);
void I2C_Config(I2C_T* i2c, I2C_Config_T* i2cConfig);
void I2C_ConfigStructInit(I2C_Config_T* i2cConfig);
void I2C_Enable(I2C_T* i2c);
void I2C_Disable(I2C_T* i2c);
void I2C_SoftwareReset(I2C_T* i2c);
void I2C_EnableAutoEnd(I2C_T* i2c);
void I2C_DisableAutoEnd(I2C_T* i2c);
void I2C_EnableReload(I2C_T* i2c);
void I2C_DisableReload(I2C_T* i2c);
void I2C_ConfigNumberOfBytes(I2C_T* i2c, uint8_t number);
void I2C_ConfigMasterRequest(I2C_T* i2c, I2C_DIRECTION_T  direction);
void I2C_EnableGenerateStart(I2C_T* i2c);
void I2C_DisableGenerateStart(I2C_T* i2c);
void I2C_EnableGenerateStop(I2C_T* i2c);
void I2C_DisableAcknowledge(I2C_T* i2c);
uint8_t I2C_ReadAddressMatched(I2C_T* i2c);
uint16_t I2C_ReadTransferDirection(I2C_T* i2c);
void I2C_HandlingTransfer(I2C_T* i2c, uint16_t address, uint8_t number, I2C_RELOAD_MODE_T reloadend, I2C_GENERATE_T generates);

void I2C_ConfigTimeoutA(I2C_T* i2c, uint16_t timeout);
void I2C_ConfigTimeoutB(I2C_T* i2c, uint16_t timeout);
uint32_t I2C_ReadRegister(I2C_T* i2c, uint8_t registers);

/** Interrupt */
void I2C_EnableInterrupt(I2C_T* i2c, uint8_t interrupt);
void I2C_DisableInterrupt(I2C_T* i2c, uint8_t interrupt);

/** Clock Stretch */
void I2C_EnableStretchClock(I2C_T* i2c);
void I2C_DisableStretchClock(I2C_T* i2c);

/** Stop Mode */
void I2C_EnableStopMode(I2C_T* i2c);
void I2C_DisableStopMode(I2C_T* i2c);

/** Own address 2*/
void I2C_EnableOwnAddress2(I2C_T* i2c);
void I2C_DisableOwnAddress2(I2C_T* i2c);
void I2C_OwnAddress2Mask(I2C_T* i2c, uint16_t address, I2C_ADDR2MSK_T mask);

/** Broadcast */
void I2C_EnableBroadcastCall(I2C_T* i2c);
void I2C_DisableBroadcastCall(I2C_T* i2c);

/** slave */
void I2C_EnableSlaveByteControl(I2C_T* i2c);
void I2C_DisableSlaveByteControl(I2C_T* i2c);
void I2C_SlaveAddress(I2C_T* i2c, uint16_t address);

/**  master */
void I2C_Enable10BitAddressingMode(I2C_T* i2c);
void I2C_Disable10BitAddressingMode(I2C_T* i2c);
void I2C_Enable10BitAddressHeader(I2C_T* i2c);
void I2C_Disable10BitAddressHeader(I2C_T* i2c);

/**  Clock */
void I2C_EnableClockTimeout(I2C_T* i2c);
void I2C_DisableClockTimeout(I2C_T* i2c);
void I2C_EnableExtendClockTimeout(I2C_T* i2c);
void I2C_DisableExtendClockTimeout(I2C_T* i2c);
void I2C_EnableIdleClockTimeout(I2C_T* i2c);
void I2C_DisableIdleClockTimeout(I2C_T* i2c);

/** Transmit and receive */
void I2C_TxData(I2C_T* i2c, uint8_t data);
uint8_t I2C_RxData(I2C_T* i2c);

/** DMA request */
void I2C_EnableDMA(I2C_T* i2c, I2C_DMA_REQ_T request);
void I2C_DisableDMA(I2C_T* i2c, I2C_DMA_REQ_T request);

/** flag */
uint8_t I2C_ReadStatusFlag(I2C_T* i2c, I2C_FLAG_T flag);
void I2C_ClearStatusFlag(I2C_T* i2c, uint32_t flag);
uint8_t I2C_ReadIntFlag(I2C_T* i2c, I2C_INT_FLAG_T flag);
void I2C_ClearIntFlag(I2C_T* i2c, uint32_t flag);

/** SMBus*/
void I2C_EnableSMBusAlert(I2C_T* i2c);
void I2C_DisableSMBusAlert(I2C_T* i2c);

void I2C_EnableSMBusHAEN(I2C_T* i2c);
void I2C_DisableSMBusHAEN(I2C_T* i2c);

void I2C_EnableSMBusDAEN(I2C_T* i2c);
void I2C_DisableSMBusDAEN(I2C_T* i2c);

void I2C_EnablePEC(I2C_T* i2c);
void I2C_DisablePEC(I2C_T* i2c);
void I2C_EnablePECRequest(I2C_T* i2c);
uint8_t I2C_ReadPEC(I2C_T* i2c);

/**@} end of group I2C_Fuctions*/
/**@} end of group I2C_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_I2C_H */
