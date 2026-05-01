/**
 * F407 侧车固件：
 * 1) 默认高阻监听 F103 关键线
 * 2) 允许白名单注入（模拟按键/控制线）
 * 3) USB CDC 与 macOS 实时交互
 */
#include "stm32f4xx_hal.h"

#include "bypass_fsm.h"
#include "bridge_protocol.h"
#include "emu_bus.h"
#include "emu_ic.h"
#include "inject_engine.h"
#include "oled_sniffer.h"
#include "probe_adc.h"
#include "probe_engine.h"
#include "probe_pins.h"
#include "probe_time.h"
#include "probe_stats.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

void Error_Handler(void);

#define LED0_GPIO_PORT GPIOA
#define LED0_PIN GPIO_PIN_0

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

static void LED0_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin = LED0_PIN;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED0_GPIO_PORT, &g);
}

static void Exti_Init(void)
{
    const ProbePinDef *pin = NULL;
    pin = ProbePins_FindByName("PE0");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    /* PF1 not bonded in LQFP100 — no EXTI1 setup */
    pin = ProbePins_FindByName("PG2");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PG5");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PG6");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PG7");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PE8");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PE9");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PE10");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PE11");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }
    pin = ProbePins_FindByName("PE12");
    if (pin != NULL) {
        (void)ProbePins_EnableInterrupt(pin);
    }

    HAL_NVIC_SetPriority(EXTI0_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    /* EXTI1 not enabled: PF1 not bonded in LQFP100 */
    HAL_NVIC_SetPriority(EXTI2_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    LED0_Init();
    ProbePins_InitAllSafe();
    ProbeTime_Init();
    ProbeStats_Reset();
    ProbeEngine_Init();
    InjectEngine_Init();
    BypassFsm_Init();
    OledSniffer_Init();
    EmuBus_Init();
    EmuIc_Init();
    ProbeAdc_Init();
    Exti_Init();
    MX_USB_DEVICE_Init();
    BridgeProtocol_Init();

    uint32_t last_led = 0;
    uint32_t last_sample = 0;
    uint8_t last_cdc_connected = 0;

    for (;;) {
        uint32_t now = HAL_GetTick();
        uint32_t now_us = ProbeTime_GetUs();

        uint8_t cdc_now = CDC_IsConnected() ? 1U : 0U;
        if (cdc_now && !last_cdc_connected) {
            CDC_TransmitLine("{\"type\":\"welcome\",\"fw\":\"f407_probe\",\"msg\":\"connected, send HELP for commands\"}");
        }
        last_cdc_connected = cdc_now;

        uint32_t led_interval = cdc_now ? 200U : 500U;
        if ((now - last_led) >= led_interval) {
            HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_PIN);
            last_led = now;
        }
        if ((now - last_sample) >= 1U) {
            ProbeEngine_Sample(now, now_us);
            last_sample = now;
        }

        ProbeAdc_Tick(now, now_us);
        InjectEngine_Tick(now, now_us);
        BypassFsm_Tick(now);
        EmuBus_Tick(now);
        EmuIc_Tick(now, now_us);
        BridgeProtocol_Task();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void OTG_FS_IRQHandler(void)
{
    extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/* EXTI1_IRQHandler omitted: PF1 not bonded in LQFP100 */

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    GPIO_TypeDef *port = NULL;
    switch (GPIO_Pin) {
    case GPIO_PIN_0:
        port = GPIOE; /* PE0 */
        break;
    /* GPIO_PIN_1 (PF1) not bonded in LQFP100 — no case */
    case GPIO_PIN_2:
        port = GPIOG; /* PG2 */
        break;
    case GPIO_PIN_5:
    case GPIO_PIN_6:
    case GPIO_PIN_7:
        port = GPIOG; /* PG5/PG6/PG7 */
        break;
    case GPIO_PIN_8:
    case GPIO_PIN_9:
    case GPIO_PIN_10:
    case GPIO_PIN_11:
    case GPIO_PIN_12:
        port = GPIOE; /* PE8/PE9/PE10/PE11/PE12 */
        break;
    default:
        break;
    }
    if (port != NULL) {
        uint32_t now_ms = HAL_GetTick();
        uint32_t now_us = ProbeTime_GetUs();
        ProbeEngine_RecordExti(port, GPIO_Pin, now_ms, now_us);
        if (port == GPIOE) {
            uint8_t v = (uint8_t)((HAL_GPIO_ReadPin(port, GPIO_Pin) == GPIO_PIN_SET) ? 1U : 0U);
            if (GPIO_Pin == GPIO_PIN_8) {
                OledSniffer_RecordEdge(OLED_LINE_SCK, v, now_us);
            } else if (GPIO_Pin == GPIO_PIN_9) {
                OledSniffer_RecordEdge(OLED_LINE_MOSI, v, now_us);
            } else if (GPIO_Pin == GPIO_PIN_10) {
                OledSniffer_RecordEdge(OLED_LINE_CS, v, now_us);
            } else if (GPIO_Pin == GPIO_PIN_11) {
                OledSniffer_RecordEdge(OLED_LINE_RES, v, now_us);
            } else if (GPIO_Pin == GPIO_PIN_12) {
                OledSniffer_RecordEdge(OLED_LINE_DC, v, now_us);
            }
        }
    }
}
