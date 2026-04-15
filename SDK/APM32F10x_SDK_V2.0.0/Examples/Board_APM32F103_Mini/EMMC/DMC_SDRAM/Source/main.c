/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.2
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

/** @addtogroup Examples
  @{
  */

/** @addtogroup DMC_SDRAM
  @{
  */

/** @defgroup DMC_SDRAM_Macros Macros
  @{
*/

/* printf message */
#define MESSAGE0    "*******************************"
#define MESSAGE1    "* APM32F103xCS DMC SDRAM Demo "
#define MESSAGE2    "* SystemCoreClock      :"
#define MESSAGE3    "*  8 bit W/R Err Count :"
#define MESSAGE4    "* 16 bit W/R Err Count :"
#define MESSAGE5    "* 32 bit W/R Err Count :"

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group DMC_SDRAM_Macros*/

/** @defgroup DMC_SDRAM_Variables Variables
  @{
*/

volatile uint32_t tick = 0;

/**@} end of group DMC_SDRAM_Variables*/

/** @defgroup DMC_SDRAM_Structures Structures
  @{
  */

/**
 * @brief    W/R Err Count structure definition
 */
typedef struct
{
    uint32_t    ByteCount;
    uint32_t    HalfWordCount;
    uint32_t    WordCount;
} Err_Count_T;

/**@} end of group DMC_SDRAM_Structures*/

/** @defgroup DMC_SDRAM_Functions Functions
  @{
*/

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
    USART_Config_T USART_ConfigStruct;
    Err_Count_T Err;

    /* System clocks configuration */
    SystemClockConfig();

    /* Sets System clock frequency */
    SystemCoreClockConfig(3, 0);//!< 3*8 = 24MHz
//    SystemCoreClockConfig(6, 1);//!< 6*8 = 48Mhz
//    SystemCoreClockConfig(9, 2);//!< 9*8 = 72Mhz
//    SystemCoreClockConfig(12, 3);//!< 12*8 = 96Mhz

    /* Configures LED GPIO */
    BOARD_LEDInit(LED2);
    BOARD_LEDOff(LED2);

    BOARD_LEDInit(LED3);
    BOARD_LEDOff(LED3);

    /* Configures COM port. */
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
    BOARD_COMInit(COM1, &USART_ConfigStruct);

    /* printf message */
    printf("\r\n");
    printf("%s \r\n", MESSAGE0);
    printf("%s \r\n", MESSAGE1);
    printf("%s %d MHz\r\n", MESSAGE2, (unsigned int)RCM_ReadSYSCLKFreq()/1000000);

    /* SDRAM init */
    SDRAM_Init();

    /* SDRAM byte write/read test */
    Err.ByteCount = SDRAM_Byte_Test();
    printf("%s %d\r\n", MESSAGE3, (unsigned int)Err.ByteCount);

    /* SDRAM halfword write/read test */
    Err.HalfWordCount = SDRAM_HalfWord_Test();
    printf("%s %d\r\n", MESSAGE4, (unsigned int)Err.HalfWordCount);

    /* SDRAM word write/read test */
    Err.WordCount = SDRAM_Word_Test();
    printf("%s %d\r\n", MESSAGE5, (unsigned int)Err.WordCount);

    printf("%s \r\n", MESSAGE0);

    /* System Tick init */
    SysTick_Config(RCM_ReadSYSCLKFreq() / 1000);

    while (1)
    {
        Delay();

        BOARD_LEDToggle(LED2);

        /* The SDRAM test failed */
        if ((Err.ByteCount != 0) || (Err.HalfWordCount != 0) || ((Err.WordCount != 0)))
        {
            BOARD_LEDToggle(LED3);
        }
    }
}

/*!
 * @brief       Sets System clock frequency
 *
 * @param       pllMf  : PLL multiplication factor
 *                       This parameter can be RCM_PLLMF_x where x can be a value from 2 to 16.
 *
 * @param       latency: the FMC Latency value.
 *
 * @retval      None
 *
 */
void SystemCoreClockConfig(uint32_t pllMf, uint32_t latency)
{
    __IO uint32_t i;

    if (!(RCM->CTRL_B.HSERDYFLG))
    {
        RCM->CTRL_B.HSEEN= BIT_SET;

        for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
        {
            if (RCM->CTRL_B.HSERDYFLG)
            {
                break;
            }
        }
    }

    if (RCM->CTRL_B.HSERDYFLG)
    {
        /* HSelect HSE as system clock source */
        RCM->CFG_B.SCLKSEL = 0x01;

        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash latency wait state */
        FMC->CTRL1_B.WS = latency;

        /* HCLK = SYSCLK */
        RCM->CFG_B.AHBPSC= 0X00;
        /* PCLK2 = HCLK */
        RCM->CFG_B.APB2PSC= 0;
        /* PCLK1 = HCLK / 2 */
        RCM->CFG_B.APB1PSC = 4;

        /* Disable PLL */
        RCM->CTRL_B.PLL1EN = 0;

        /* PLL: HSE * PLLm */
        RCM->CFG_B.PLL1SRCSEL = 1;
        RCM->CFG_B.PLL1MULCFG = (pllMf - 2);

        /* Enable PLL */
        RCM->CTRL_B.PLL1EN = 1;

        /* Wait PLL Ready */
        while (RCM->CTRL_B.PLL1RDYFLG == BIT_RESET);

        /* Select PLL as system clock source */
        RCM->CFG_B.SCLKSEL = 2;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG_B.SCLKSELSTS!= 0x02);
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;

    while (tick < 500);
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


/**@} end of group DMC_SDRAM_Functions */
/**@} end of group DMC_SDRAM */
/**@} end of group Examples */
