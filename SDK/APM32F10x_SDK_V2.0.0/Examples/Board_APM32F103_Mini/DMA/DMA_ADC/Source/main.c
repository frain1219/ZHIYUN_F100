/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
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
#include <stdio.h>
/** @addtogroup Examples
  @{
  */

/** @addtogroup DMA_ADC
  @{
  */

/** @defgroup DMA_ADC_Macros Macros
  @{
*/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group DMA_ADC_Macros*/

/** @defgroup DMA_ADC_Variables Variables
  @{
*/
uint32_t systick = 0;
/* use to generate analog signal for DAC*/
uint16_t dacData = 0;

/**@} end of group DMA_ADC_Variables */

/** @defgroup DMA_ADC_Functions Functions
  @{
*/

/* Delay */
void Delay(uint32_t count);
/* DMA init */
void DMA_Init(uint32_t *Buf);
/* ADC init */
void ADC_Init(void);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    USART_Config_T usartConfigStruct;

    /* System clocks configuration */
    SystemClockConfig();

    /* Configures LED2 and LED3 */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /* USART configuration */
    USART_ConfigStructInit(&usartConfigStruct);
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;

    /* COM1 init*/
    BOARD_COMInit(COM1, &usartConfigStruct);

    /* DMA value from ADC*/
    uint32_t DMA_ConvertedValue = 0;
    /* ADC convert to volatage*/
    float ADC_ConvertedValue = 0;

    /* DMA init*/
    DMA_Init(&DMA_ConvertedValue);

    /* ADC init*/
    ADC_Init();

    /* Configure the SysTick to generate a time base equal to 1 ms */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1);
    }
    while (1)
    {
        if (DMA_ReadStatusFlag(DMA1_FLAG_TC1) == SET)
        {
            ADC_ConvertedValue = 3.3 * DMA_ConvertedValue / 4096;
            printf("\r\n");
            printf("ADC REGDATA = 0x%04X \r\n", (unsigned int)DMA_ConvertedValue);
            printf("Volatage    = %f V \r\n", ADC_ConvertedValue);

            Delay(1000);
            DMA_ClearStatusFlag(DMA1_FLAG_TC1);
        }
    }
}
/*!
 * @brief     DMA Init
 *
 * @param     Buf:   memory addr
 *
 * @retval    None
 */
void DMA_Init(uint32_t *Buf)
{
    DMA_Config_T dmaconfig;
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);

    dmaconfig.bufferSize         = 1;
    dmaconfig.dir                = DMA_DIR_PERIPHERAL_SRC;
    dmaconfig.loopMode           = DMA_MODE_CIRCULAR;
    dmaconfig.memoryBaseAddr     = (uint32_t)Buf;
    dmaconfig.memoryDataSize     = DMA_MEMORY_DATA_SIZE_HALFWORD;
    dmaconfig.peripheralBaseAddr = (uint32_t)&ADC1->REGDATA;
    dmaconfig.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_HALFWORD;
    dmaconfig.memoryInc          = DMA_MEMORY_INC_DISABLE;
    dmaconfig.peripheralInc      = DMA_PERIPHERAL_INC_DISABLE;
    dmaconfig.priority           = DMA_PRIORITY_HIGH;
    dmaconfig.M2M                = DMA_M2MEN_DISABLE;

    DMA_Config(DMA1_Channel1, &dmaconfig);

    DMA_EnableInterrupt(DMA1_Channel1, DMA_INT_TC);
    DMA_Enable(DMA1_Channel1);
}

/*!
 * @brief     ADC Init
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_Init(void)
{
    GPIO_Config_T   gpioConfig;
    ADC_Config_T    adcConfig;

    /* Enable GPIOA clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);

    /* ADC channel 0 configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_ANALOG;
    gpioConfig.speed     = GPIO_SPEED_50MHz;
    gpioConfig.pin     = GPIO_PIN_0;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset(ADC1);
    ADC_ConfigStructInit(&adcConfig);
    adcConfig.continuosConvMode  = ENABLE;
    adcConfig.dataAlign          = ADC_DATA_ALIGN_RIGHT;
    adcConfig.externalTrigConv   = ADC_EXT_TRIG_CONV_None;
    adcConfig.scanConvMode       = DISABLE;
    ADC_Config(ADC1, &adcConfig);

    /* ADC channel 10 Convert configuration */
    ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_239CYCLES5);

    ADC_EnableDMA(ADC1);

    /* Enable ADC */
    ADC_Enable(ADC1);

    /* ADC start conversion */
    ADC_EnableSoftwareStartConv(ADC1);
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
 */
void Delay(uint32_t count)
{
    systick = count;

    while (systick);
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
int fputc(int ch, FILE *f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
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

    /* wait for the data to be send  */
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
int _write(int file, char *ptr, int len)
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

/**@} end of group DMA_ADC_Functions */
/**@} end of group DMA_ADC */
/**@} end of group Examples */
