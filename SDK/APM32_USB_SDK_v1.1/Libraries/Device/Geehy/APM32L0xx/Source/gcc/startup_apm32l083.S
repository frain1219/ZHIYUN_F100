/*!
 * @file       startup_apm32l083.S
 *
 * @brief      CMSIS Cortex-M0 PLUS based Core Device Startup File for Device startup_apm32l083
 *
 * @version    V1.0.0
 *
 * @date       2023-02-28
 *
 * @attention
 *
 *  Copyright (C) 2022-2023 Geehy Semiconductor
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

  .syntax unified
  .cpu cortex-m0plus
  .fpu softvfp
  .thumb

.global g_apm32_Vectors
.global Default_Handler

.word _start_address_init_data
.word _start_address_data
.word _end_address_data
.word _start_address_bss
.word _end_address_bss

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function

// Reset handler routine
Reset_Handler:
// User Stack and Heap initialization
  ldr   r0, =_end_stack
  mov   sp, r0

  ldr r0, =_start_address_data
  ldr r1, =_end_address_data
  ldr r2, =_start_address_init_data
  movs r3, #0
  b L_loop0_0

L_loop0:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

L_loop0_0:
  adds r4, r0, r3
  cmp r4, r1
  bcc L_loop0

  ldr r2, =_start_address_bss
  ldr r4, =_end_address_bss
  movs r3, #0
  b L_loop1

L_loop2:
  str  r3, [r2]
  adds r2, r2, #4

L_loop1:
  cmp r2, r4
  bcc L_loop2

  bl  SystemInit
  bl __libc_init_array
  bl main

L_loop3:
    b L_loop3

.size Reset_Handler, .-Reset_Handler

// This is the code that gets called when the processor receives an unexpected interrupt.
    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
L_Loop_infinite:
  b L_Loop_infinite
  .size Default_Handler, .-Default_Handler

// The minimal vector table for a Cortex M0 Plus.
   .section .apm32_isr_vector,"a",%progbits
  .type g_apm32_Vectors, %object
  .size g_apm32_Vectors, .-g_apm32_Vectors

// Vector Table Mapped to Address 0 at Reset
g_apm32_Vectors:

  .word _end_stack                          // Top of Stack
  .word Reset_Handler                       // Reset Handler
  .word NMI_Handler                         // NMI Handler
  .word HardFault_Handler                   // Hard Fault Handler
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word 0                                   // Reserved
  .word SVC_Handler                         // SVCall Handler
  .word 0                                   // Debug Monitor Handler
  .word 0                                   // Reserved
  .word PendSV_Handler                      // PendSV Handler
  .word SysTick_Handler                     // SysTick Handler
  .word WWDT_IRQHandler                     // Window Watchdog
  .word PVD_VDDIO2_IRQHandler               // PVD and VDDIO2 through EINT Line detect
  .word RTC_IRQHandler                      // RTC through EINT Line
  .word FLASH_IRQHandler                    // Flash
  .word RCM_CRS_IRQHandler                  // RCM and CRS
  .word EINT0_1_IRQHandler                  // EINT Line 0..1
  .word EINT2_3_IRQHandler                  // EINT Line 2..3
  .word EINT4_15_IRQHandler                 // EINT Line 4..15
  .word TSC_IRQHandler                      // TSC
  .word DMA_CH1_IRQHandler                  // DMA Channel 1
  .word DMA_CH2_3_IRQHandler                // DMA Channel 2 and DMA1 Channel 3
  .word DMA_CH4_5_6_7_IRQHandler            // DMA Channel 4, Channel 5, Channel 6 and Channel 7
  .word ADC1_COMP_IRQHandler                // ADC1 COMP1 and COMP2
  .word LPTMR_IRQHandler                    // LPTMR through EINT Line
  .word USART4_5IRQHandler                  // USART4 and USART5
  .word TMR2_IRQHandler                     // TMR2
  .word TMR3_IRQHandler                     // TMR3
  .word TMR6_DAC_IRQHandler                 // TMR6 and DAC
  .word TMR7_IRQHandler                     // TMR7
  .word 0                                   // Reserved
  .word TMR21_IRQHandler                    // TMR21
  .word I2C3_IRQHandler                     // I2C3
  .word TMR22_IRQHandler                    // TMR22
  .word I2C1_IRQHandler                     // I2C1
  .word I2C2_IRQHandler                     // I2C2
  .word SPI1_IRQHandler                     // SPI1
  .word SPI2_IRQHandler                     // SPI2
  .word USART1_IRQHandler                   // USART1
  .word USART2_IRQHandler                   // USART2
  .word LPUART_AES_RNG_IRQHandler           // LPUART AES and RNG
  .word LCD_IRQHandler                      // LCD
  .word USBD_IRQHandler                     // USBD

// Default exception/interrupt handler

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak WWDT_IRQHandler
  .thumb_set WWDT_IRQHandler,Default_Handler

  .weak PVD_VDDIO2_IRQHandler
  .thumb_set PVD_VDDIO2_IRQHandler,Default_Handler

  .weak RTC_IRQHandler
  .thumb_set RTC_IRQHandler,Default_Handler

  .weak FLASH_IRQHandler
  .thumb_set FLASH_IRQHandler,Default_Handler

  .weak RCM_CRS_IRQHandler
  .thumb_set RCM_CRS_IRQHandler,Default_Handler

  .weak EINT0_1_IRQHandler
  .thumb_set EINT0_1_IRQHandler,Default_Handler

  .weak EINT2_3_IRQHandler
  .thumb_set EINT2_3_IRQHandler,Default_Handler

  .weak EINT4_15_IRQHandler
  .thumb_set EINT4_15_IRQHandler,Default_Handler

  .weak TSC_IRQHandler
  .thumb_set TSC_IRQHandler,Default_Handler

  .weak DMA_CH1_IRQHandler
  .thumb_set DMA_CH1_IRQHandler,Default_Handler

  .weak DMA_CH2_3_IRQHandler
  .thumb_set DMA_CH2_3_IRQHandler,Default_Handler

  .weak DMA_CH4_5_6_7_IRQHandler
  .thumb_set DMA_CH4_5_6_7_IRQHandler,Default_Handler

  .weak ADC1_COMP_IRQHandler
  .thumb_set ADC1_COMP_IRQHandler,Default_Handler

  .weak LPTMR_IRQHandler
  .thumb_set LPTMR_IRQHandler,Default_Handler

  .weak USART4_5IRQHandler
  .thumb_set USART4_5IRQHandler,Default_Handler

  .weak TMR2_IRQHandler
  .thumb_set TMR2_IRQHandler,Default_Handler

  .weak TMR3_IRQHandler
  .thumb_set TMR3_IRQHandler,Default_Handler

  .weak TMR6_DAC_IRQHandler
  .thumb_set TMR6_DAC_IRQHandler,Default_Handler

  .weak TMR7_IRQHandler
  .thumb_set TMR7_IRQHandler,Default_Handler

  .weak TMR21_IRQHandler
  .thumb_set TMR21_IRQHandler,Default_Handler

  .weak I2C3_IRQHandler
  .thumb_set I2C3_IRQHandler,Default_Handler

  .weak TMR22_IRQHandler
  .thumb_set TMR22_IRQHandler,Default_Handler

  .weak I2C1_IRQHandler
  .thumb_set I2C1_IRQHandler,Default_Handler

  .weak I2C2_IRQHandler
  .thumb_set I2C2_IRQHandler,Default_Handler

  .weak SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak SPI2_IRQHandler
  .thumb_set SPI2_IRQHandler,Default_Handler

  .weak USART1_IRQHandler
  .thumb_set USART1_IRQHandler,Default_Handler

  .weak USART2_IRQHandler
  .thumb_set USART2_IRQHandler,Default_Handler

  .weak LPUART_AES_RNG_IRQHandler
  .thumb_set LPUART_AES_RNG_IRQHandler,Default_Handler

  .weak LCD_IRQHandler
  .thumb_set LCD_IRQHandler,Default_Handler

  .weak USBD_IRQHandler
  .thumb_set USBD_IRQHandler,Default_Handler
