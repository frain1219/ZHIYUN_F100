/*!
 * @file        i2c.c
 *
 * @brief       I2C Driver
 *
 * @version     V1.0.0
 *
 * @date        2025-4-23
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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
#include "i2c.h"
#include "uart.h"
#include "config.h"
#include <string.h>

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOI2C
  @{
*/

/** @defgroup USARTTOI2C_Macros Macros
  @{
*/

/* I2C Address Set */
#define APM32F402_I2C_ADDR    0x76
#define APM32F427_I2C_ADDR    0x7C
#define APM32F072_I2C_ADDR    0x82

#define I2C_SLAVE_ADDR      APM32F402_I2C_ADDR

/**@} end of group USARTTOI2C_Macros */

/** @defgroup USARTTOI2C_Variables Variables
  @{
*/

uint8_t g_i2cBuf[512];
uint32_t g_addr;

/**@} end of group USARTTOI2C_Variables */

/** @defgroup USARTTOI2C_Functions Functions
  @{
*/

/*!
 * @brief   I2C Init
 *
 * @param   speed: I2C Master speed
 *
 * @retval  None
 *
 */
void I2C_Init(uint32_t speed)
{
   GPIO_Config_T gpioConfigStruct;
   I2C_Config_T i2cConfigStruct;

   /* Enable I2C related Clock */
   RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOB | RCM_APB2_PERIPH_AFIO);
   RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_I2C1);

   /* Free I2C_SCL and I2C_SDA */
   gpioConfigStruct.mode = GPIO_MODE_AF_OD;
   gpioConfigStruct.speed = GPIO_SPEED_50MHz;
   gpioConfigStruct.pin = GPIO_PIN_6;
   GPIO_Config(GPIOB, &gpioConfigStruct);

   gpioConfigStruct.mode = GPIO_MODE_AF_OD;
   gpioConfigStruct.speed = GPIO_SPEED_50MHz;
   gpioConfigStruct.pin = GPIO_PIN_7;
   GPIO_Config(GPIOB, &gpioConfigStruct);

   /* Config I2C1 */
   I2C_Reset(I2C1);
   i2cConfigStruct.mode = I2C_MODE_I2C;
   i2cConfigStruct.dutyCycle = I2C_DUTYCYCLE_2;
   i2cConfigStruct.ackAddress = I2C_ACK_ADDRESS_7BIT;
   i2cConfigStruct.ownAddress1 = I2C_SLAVE_ADDR;
   i2cConfigStruct.ack = I2C_ACK_ENABLE;
   i2cConfigStruct.clockSpeed = speed;
   I2C_Config(I2C1,&i2cConfigStruct);

   /* Enable I2Cx */
   I2C_Enable(I2C1);
}

/*!
 * @brief   I2C Write data
 *
 * @param   dat: data address
 *
 * @param   len: data length
 *
 * @retval  None
 *
 */
void I2C_Write(uint8_t *dat, uint16_t len)
{
    while(I2C_ReadStatusFlag(I2C1, I2C_FLAG_BUSBSY) == SET);
    /* Send START condition */
    I2C_EnableGenerateStart(I2C1);
    /* EV5 */
    while(!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
    }

    /* Send address for write */
    I2C_Tx7BitAddress(I2C1, I2C_SLAVE_ADDR, I2C_DIRECTION_TX);
    /* EV6 */
    while(!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
    }

    while (len)
    {
        /* Send the current byte */
        I2C_TxData(I2C1, *dat);

        /* Point to the next byte to be written */
        dat++;
        len--;
        /* EV8 */
        while (!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
        {
        }
    }

    I2C_EnableGenerateStop(I2C1);
    while(I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET)
    {
    }
}


/*!
 * @brief   I2C Read data
 *
 * @param   dat: data address
 *
 * @param   len: data length
 *
 * @retval  None
 *
 */
void I2C_Read(uint8_t *dat, uint16_t len)
{
    uint16_t i = 0;

    while(I2C_ReadStatusFlag(I2C1, I2C_FLAG_BUSBSY) == SET);
    /* Send START condition */
    I2C_EnableGenerateStart(I2C1);
    /* EV5 */
    while(!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
    }

    /* Send address for write */
    I2C_Tx7BitAddress(I2C1, I2C_SLAVE_ADDR, I2C_DIRECTION_RX);
    /* EV6 */
    while(!I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
    }
    I2C_EnableAcknowledge(I2C1);

    for(;i < len;i++)
    {
        if(i == (len - 1))
        {
            I2C_DisableAcknowledge(I2C1);
            I2C_EnableGenerateStop(I2C1);
        }
        /* EV7 */
        while(I2C_ReadEventStatus(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == RESET)
        {
        }
        dat[i] = I2C_RxData(I2C1);
    }
    I2C_EnableAcknowledge(I2C1);
}

/**@} end of group USARTTOI2C_Functions */
/**@} end of group USARTTOI2C */
/**@} end of group Examples */

