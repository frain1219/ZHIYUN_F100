/*!
 * @file        apm32l0xx_gpio.c
 *
 * @brief       This file contains all the functions for the GPIO peripheral
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

#include "apm32l0xx_gpio.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup GPIO_Fuctions Fuctions
  @{
*/

/*!
 * @brief       Reset GPIO peripheral registers to their default reset values
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @retval      None
 */
void GPIO_Reset(GPIO_T* port)
{
    if (port == GPIOA)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_A);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_A);
    }
    else if (port == GPIOB)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_B);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_B);
    }
    else if (port == GPIOC)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_C);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_C);
    }
    else if (port == GPIOD)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_D);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_D);
    }
    else if (port == GPIOE)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_E);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_E);
    }
    else if(port == GPIOH)
    {
        RCM_EnableGPIOReset(RCM_GPIO_CLOCK_H);
        RCM_DisableGPIOReset(RCM_GPIO_CLOCK_H);
    }
}

/*!
 * @brief       Config the GPIO peripheral according to the specified parameters in the gpioConfig
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       gpioConfig: Pointer to a GPIO_Config_T structure that
 *                          contains the configuration information for the specified GPIO peripheral
 *
 * @retval      None
 */
void GPIO_Config(GPIO_T* port, GPIO_Config_T* gpioConfig)
{
    uint32_t i;
    uint32_t bit;

    for (i = 0; i < 16; i++)
    {
        bit = (uint32_t)1 << i;

        if (!(gpioConfig->pin & bit))
        {
            continue;
        }

        if ((gpioConfig->mode == GPIO_MODE_OUT) || (gpioConfig->mode == GPIO_MODE_AF))
        {
            /** speed */
            port->OSSEL &= ~((0x03) << (i * 2));
            port->OSSEL |= ((uint32_t)(gpioConfig->speed) << (i * 2));

            /** Output mode configuration */
            port->OMODE &= ~(((uint16_t)gpioConfig->outtype) << ((uint16_t)i));
            port->OMODE |= (uint16_t)(((uint16_t)gpioConfig->outtype) << ((uint16_t)i));
        }

        /** input/output mode */
        port->MODE  &= ~(0x03 << (i * 2));
        port->MODE |= (((uint32_t)gpioConfig->mode) << (i * 2));

        /** Pull-up Pull down resistor configuration */
        port->PUPD &= ~(0x03 << ((uint16_t)i * 2));
        port->PUPD |= (((uint32_t)gpioConfig->pupd) << (i * 2));
    }
}

/*!
 * @brief       Fills each GPIO_Config_T member with its default value
 *
 * @param       gpioConfig: Pointer to a GPIO_Config_T structure which will be initialized
 *
 * @retval      None
 */
void GPIO_ConfigStructInit(GPIO_Config_T* gpioConfig)
{
    gpioConfig->pin     = GPIO_PIN_ALL;
    gpioConfig->mode    = GPIO_MODE_IN;
    gpioConfig->outtype = GPIO_OUT_TYPE_PP;
    gpioConfig->speed   = GPIO_SPEED_LOW;
    gpioConfig->pupd    = GPIO_PUPD_NO;
}

/*!
 * @brief       Locks GPIO Pins configuration registers
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin: specifies the port bit to be locked
 *
 * @retval      None
 */
void GPIO_ConfigPinLock(GPIO_T* port, GPIO_PIN_T pin)
{
    uint32_t val = 0x00010000;

    val  |= pin;
    /** Set LOCK bit */
    port->LOCK = val ;
    /** Reset LOCK bit */
    port->LOCK = pin;
    /** Set LOCK bit */
    port->LOCK = val;
    /** Read LOCK bit*/
    val = port->LOCK;
    /** Read LOCK bit*/
    val = port->LOCK;
}

/*!
 * @brief       Reads the specified input port pin
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin: specifies pin to read
 *
 * @retval      The input port pin value
 */
uint8_t GPIO_ReadInputBit(GPIO_T* port, uint16_t pin)
{
    uint8_t ret;

    ret = (port->IDATA & pin) ?  BIT_SET : BIT_RESET;

    return ret;
}

/*!
 * @brief       Reads the specified GPIO input data port
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @retval      GPIO input data port value
 */
uint16_t GPIO_ReadInputPort(GPIO_T* port)
{
    return ((uint16_t)port->IDATA);
}

/*!
 * @brief       Reads the specified output data port bit
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin: specifies pin to read
 *
 * @retval      The output port pin value
 */
uint8_t GPIO_ReadOutputBit(GPIO_T* port, uint16_t pin)
{
    uint8_t ret;

    ret = (port->ODATA & pin) ? BIT_SET : BIT_RESET;

    return ret;
}

/*!
 * @brief       Reads the specified GPIO output data port
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @retval      output data port value
 */
uint16_t GPIO_ReadOutputPort(GPIO_T* port)
{
    return ((uint16_t)port->ODATA);
}

/*!
 * @brief       Sets the selected data port bits
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin:    specifies the port bits to be written
 *
 * @retval      None
 */
void GPIO_SetBit(GPIO_T* port, uint16_t pin)
{
    port->BSC = (uint32_t)pin;
}

/*!
 * @brief       Clears the selected data port bits
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin:    specifies the port bits to be written
 *
 * @retval      None
 */
void GPIO_ClearBit(GPIO_T* port, uint16_t pin)
{
    port->BR = (uint32_t)pin;
}

/*!
 * @brief       Sets or clears the selected data port bit
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pin:    specifies the port bits to be written
 *
 * @param       bitVal
 *
 * @retval      None
 */
void GPIO_WriteBitValue(GPIO_T* port, GPIO_PIN_T pin, GPIO_BSRET_T bitVal)
{
    if (bitVal != GPIO_BIT_RESET)
    {
        port->BSC = pin;
    }
    else
    {
        port->BR = pin ;
    }
}

/*!
 * @brief       Writes data to the specified GPIO data port
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       portVal:  Write value to the port output data register
 *
 * @retval      None
 */
void GPIO_WriteOutputPort(GPIO_T* port, uint16_t portValue)
{
    port->ODATA = (uint32_t)portValue;
}

/*!
 * @brief       Changes the mapping of the specified pin
 *
 * @param       port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param       pinSource:  Specifies the pin for the Alternate function.
 *              This parameter can be values between 0 and 15.
 *
 * @param       afPin: Selects the pin to used as Alternate function.
 *
 * @retval      None
 */
void GPIO_ConfigPinAF(GPIO_T* port, GPIO_PIN_SOURCE_T pinSource, GPIO_AF_T afPin)
{
    uint32_t temp  = 0x00;
    uint32_t temp1 = 0x00;

    if (pinSource <= 0x07)
    {
        temp = (uint8_t)afPin << ((uint32_t)pinSource * 4);
        port->ALFL &= ~((uint32_t)0xf << ((uint32_t)pinSource * 4));
        port->ALFL |=  temp;
    }
    else
    {
        temp1 =  (uint8_t)afPin << ((uint32_t)(pinSource & 0x07) * 4);
        port->ALFH &= ~((uint32_t)0xf << (((uint32_t)pinSource & 0x07) * 4));
        port->ALFH |=  temp1;
    }
}

/*!
 * @brief  Toggles the specified GPIO pins.
 *
 * @param  port: GPIO peripheral.It can be GPIOA/GPIOB/GPIOC/GPIOD/GPIOE/GPIOH
 *
 * @param  pin Specifies the pins to be toggled.
 *
 * @retval None
 */
void GPIO_TogglePin(GPIO_T* port, GPIO_PIN_T pin)
{
    uint32_t oDATA;

    oDATA = port->ODATA;
    port->BSC = ((oDATA & pin) << 16U) | (~oDATA & pin);
}

/**@} end of group GPIO_Fuctions*/
/**@} end of group GPIO_Driver*/
/**@} end of group Peripherals_Library*/
