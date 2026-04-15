/*!
 * @file        apm32l0xx_comp.h
 *
 * @brief       This file contains all the functions prototypes for the COMP firmware library
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

#ifndef  __APM32L0XX_COMP_H
#define  __APM32L0XX_COMP_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup COMP_Driver COMP Driver
  @{
*/

/** @addtogroup COMP_Enumerations Enumerations
  @{
*/

/**
 * @brief    COMP_Selection
 */
typedef enum
{
    COMP_SELECT_COMP1,  //!< Comparator 1
    COMP_SELECT_COMP2   //!< Comparator 2
}COMP_SELECT_T;

/**
 * @brief    COMP Inverting Input
 */
typedef enum
{
    COMP_INVERTING_INPUT_VREFINT,     //!< VREFINT
    COMP_INVERTING_INPUT_PA0_PA2,     //!< Comparator1 : PA0  Comparator2 : PA2
    COMP_INVERTING_INPUT_DAC1,        //!< PA4 with DAC_OUT1 if enabled
    COMP_INVERTING_INPUT_DAC2,        //!< PA5 with DAC_OUT2 if enabled
    COMP_INVERTING_INPUT2_1_4VREFINT, //!< 1/4 of VREFINT (only for Comparator 2)
    COMP_INVERTING_INPUT2_1_2VREFINT, //!< 1/2 of VREFINT (only for Comparator 2)
    COMP_INVERTING_INPUT2_3_4VREFINT, //!< 3/4 of VREFINT (only for Comparator 2)
    COMP_INVERTING_INPUT2_PB3         //!< PB3            (only for Comparator 2)
}COMP_INVERTING_INPUT_T;

/**
 * @brief    COMP Speed Mode Select (only for Comparator2)
 */
typedef enum
{
    COMP_SPEED_LOW,  //!< Low speed
    COMP_SPEED_FAST  //!< High speed
}COMP_SPEED_T;

/**
 * @brief    COMP Output Polarity
 */
typedef enum
{
    COMP_OUTPUTPOL_NONINVERTED, //!< output is not inverted
    COMP_OUTPUTPOL_INVERTED     //!< output is inverted
}COMP_OUTPUTPOL_T;

/**
 * @brief    COMP Output Propagate 1
 */
typedef enum
{
    COMP_OUTPUTPROPAGATE1_GATE,   //!< Comparator 1 or 2 output gated
    COMP_OUTPUTPROPAGATE1_LPTMR1  //!< Comparator 1 or 2 output sent to LPTIM input 1
}COMP_OUTPUTPROPAGATE1_T;

/**
 * @brief    COMP Output Propagate 2 (only for Comparator 2)
 */
typedef enum
{
    COMP_OUTPUTPROPAGATE2_GATE,   //!< Comparator 2 output gated
    COMP_OUTPUTPROPAGATE2_LPTMR1  //!< Comparator 2 output sent to LPTIM input 2
}COMP_OUTPUTPROPAGATE2_T;

/**
 * @brief    COMP input Connection (only for Comparator 2)
 */
typedef enum
{
    COMP_INPUT_CONNECT_PA3,
    COMP_INPUT_CONNECT_PB4,
    COMP_INPUT_CONNECT_PB5,
    COMP_INPUT_CONNECT_PB6,
    COMP_INPUT_CONNECT_PB7
}COMP_INPUT_CONNECT_T;

/**
 * @brief    COMP window mode selection (only for Comparator 1)
 */
typedef enum
{
    COMP_WINDOW_PA1,     //!< Plus input of comparator 1 connected to PA1
    COMP_WINDOW_SHORTED  //!<  Plus input of comparator 1 shorted with Plus input of comparator 2
}COMP_WINDOW_T;

/**@} end of group COMP_Enumerations*/

/** @addtogroup COMP_Structure Data Structure
  @{
*/

/**
 * @brief    COMP Config structure definition
 */
typedef struct
{
    COMP_SPEED_T             speedSelecct;      //!< Comparator speed mode select     (only for Comparator 2)
    COMP_WINDOW_T            windowSelect;      //!< Comparator window mode selection (only for Comparator 1)
    COMP_OUTPUTPOL_T         outputPol;         //!< Comparator output polarity
    COMP_INPUT_CONNECT_T     inputConnect;      //!< Comparator connection            (only for Comparator 2)
    COMP_INVERTING_INPUT_T   invertingInput;    //!< Comparator inverting input selection
    COMP_OUTPUTPROPAGATE1_T  outputPropagate1;  //!< Comparator output Propagate
    COMP_OUTPUTPROPAGATE2_T  outputPropagate2;  //!< Comparator output Propagate      (only for Comparator 2)
}COMP_Config_T;

/**@} end of group COMP_Structure*/

/** @addtogroup COMP_Fuctions Fuctions
  @{
*/

/** COMP configuration **/
void COMP_Reset(COMP_SELECT_T compSelect);
void COMP_Config(COMP_SELECT_T compSelect, COMP_Config_T* compConfig);
void COMP_ConfigStructInit(COMP_SELECT_T compSelect, COMP_Config_T* compConfig);
void COMP_Enable(COMP_SELECT_T compSelect);
void COMP_Disable(COMP_SELECT_T compSelect);
uint32_t COMP_ReadOutPutLevel(COMP_SELECT_T compSelect);

/** COMP configuration locking **/
void COMP_ConfigLOCK(COMP_SELECT_T compSelect);

/**@} end of group COMP_Fuctions*/
/**@} end of group COMP_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0XX_COMP_H */
