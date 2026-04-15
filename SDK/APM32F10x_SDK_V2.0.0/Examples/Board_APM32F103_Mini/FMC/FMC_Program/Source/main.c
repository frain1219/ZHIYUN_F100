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

/** @addtogroup FMC_Program
  @{
  */

/** @defgroup FMC_Program_Macros Macros
  @{
*/

#if defined (APM32F103xC) || defined (APM32F103xD) || defined (APM32F103xE)
#define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
#define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif

#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08008000)
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800C000)

/**@} end of group FMC_Program_Macros*/

/** @defgroup FMC_Program_Enumerations Enumerations
  @{
*/

enum {FAILED, PASSED};

/**@} end of group FMC_Program_Enumerations */

/** @defgroup FMC_Program_Variables Variables
  @{
*/

volatile FMC_STATUS_T FLASHStatus = FMC_STATUS_COMPLETE;
volatile uint8_t MemoryProgramStatus = PASSED;

uint32_t EraseCounter = 0x00;
uint32_t Address      = 0x00;
uint32_t NbrOfPage    = 0x00;
uint32_t Data         = 0x3210ABCD;

/**@} end of group FMC_Program_Variables */

/** @defgroup FMC_Program_Functions Functions
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
    /* System clocks configuration */
    SystemClockConfig();

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);

    FMC_Unlock();

    NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;
    FMC_ClearStatusFlag((FMC_FLAG_T)(FMC_FLAG_OC | FMC_FLAG_PE | FMC_FLAG_WPE));
    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FMC_STATUS_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FMC_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    Address = BANK1_WRITE_START_ADDR;
    while((Address < BANK1_WRITE_END_ADDR) && (FLASHStatus == FMC_STATUS_COMPLETE))
    {
        FLASHStatus = FMC_ProgramWord(Address, Data);
        Address = Address + 4;
    }

    FMC_Lock();

    Address = BANK1_WRITE_START_ADDR;

    while((Address < BANK1_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
    {
        if((*(__IO uint32_t*) Address) != Data)
        {
            MemoryProgramStatus = FAILED;
        }
        Address += 4;
    }
    
    if(MemoryProgramStatus == FAILED)
    {
        BOARD_LEDOn(LED3);
    }
    else
    {
        BOARD_LEDOn(LED2);
    }

    while (1)
    {
    }
}

/**@} end of group FMC_Program_Functions */
/**@} end of group FMC_Program */
/**@} end of group Examples */
