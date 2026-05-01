/*
 * STM32F407 USB CDC 最小测试固件
 *
 * 功能：
 *   - PA0 LED：上电后慢闪（500ms），USB 枚举成功后快闪（200ms）
 *   - USB CDC：枚举成功后每秒发送一行计数文本
 *   - 收到任意数据后原样回显（echo）
 *
 * 接线：USB-C → 电脑，烧录后运行 `ls /dev/cu.*` 检查是否出现新设备
 */
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"

/* LED: PA0，低电平点亮（原理图：VCC3 → LED → R3 → PA0） */
#define LED_PORT  GPIOA
#define LED_PIN   GPIO_PIN_0

static void LED_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin   = LED_PIN;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Pull  = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &g);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET); /* 初始灭 */
}

/* 时钟：HSE 8MHz → PLL → 168MHz，PLLQ=7 → USB 48MHz */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState       = RCC_HSE_ON;
    osc.PLL.PLLState   = RCC_PLL_ON;
    osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM       = 8;
    osc.PLL.PLLN       = 336;
    osc.PLL.PLLP       = RCC_PLLP_DIV2;
    osc.PLL.PLLQ       = 7;
    /* HSE 失败时 LED 以 50ms 快闪死循环，区别于 USB 枚举失败的 500ms 慢闪 */
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        for (;;) {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            HAL_Delay(50);
        }
    }

    clk.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                       | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

int main(void)
{
    HAL_Init();
    LED_Init();              /* 先初始化 LED，以便 SystemClock_Config 失败时能用 LED 报错 */
    SystemClock_Config();
    MX_USB_DEVICE_Init();

    uint32_t last_led    = 0;
    uint32_t last_tx     = 0;
    uint32_t counter     = 0;
    uint8_t  last_conn   = 0;

    /* echo 缓冲：收到的字节攒够一包或遇到换行再回显 */
    uint8_t  echo_buf[64];
    uint8_t  echo_len = 0;

    for (;;) {
        uint32_t now       = HAL_GetTick();
        uint8_t  connected = CDC_IsConnected() ? 1U : 0U;

        /* 刚连上时发一条欢迎消息 */
        if (connected && !last_conn) {
            CDC_TransmitLine("=== STM32F407 USB CDC TEST ===");
            CDC_TransmitLine("LED blink: 200ms=connected, 500ms=no-host");
            CDC_TransmitLine("Send any text to echo back.");
        }
        last_conn = connected;

        /* LED 闪烁 */
        uint32_t interval = connected ? 200U : 500U;
        if ((now - last_led) >= interval) {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            last_led = now;
        }

        /* 每秒发送一行计数，确认数据流正常 */
        if (connected && (now - last_tx) >= 1000U) {
            char buf[56];
            snprintf(buf, sizeof(buf), "tick=%-8lu count=%lu",
                     (unsigned long)now, (unsigned long)counter);
            CDC_TransmitLine(buf);
            counter++;
            last_tx = now;
        }

        /* Echo：收到数据原样回显 */
        uint8_t byte;
        while (CDC_ReadByte(&byte)) {
            echo_buf[echo_len++] = byte;
            /* 遇到换行或缓冲满时发送 */
            if (byte == '\n' || echo_len >= sizeof(echo_buf) - 1U) {
                CDC_TransmitRaw(echo_buf, echo_len);
                echo_len = 0;
            }
        }

        CDC_Task();
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

void Error_Handler(void)
{
    __disable_irq();
    for (;;) {}
}
