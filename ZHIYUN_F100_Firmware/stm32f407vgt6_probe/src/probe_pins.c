#include "probe_pins.h"

#include <string.h>

static const ProbePinDef g_probe_pins[] = {
    {"PE0", GPIOE, GPIO_PIN_0, PROBE_PIN_MONITOR_ONLY, false, "F103 PC13 status"},
    {"PE1", GPIOE, GPIO_PIN_1, PROBE_PIN_INJECTABLE, false, "F103 PB11 PD GPIO"},
    {"PE2", GPIOE, GPIO_PIN_2, PROBE_PIN_INJECTABLE, false, "F103 PB3 VS_EN"},
    {"PE5", GPIOE, GPIO_PIN_5, PROBE_PIN_INJECTABLE, false, "F103 PA0 control"},
    {"PE6", GPIOE, GPIO_PIN_6, PROBE_PIN_INJECTABLE, false, "F103 PA1 input"},
    {"PE7", GPIOE, GPIO_PIN_7, PROBE_PIN_MONITOR_ONLY, false, "F103 PA2 pwm"},
    {"PC0", GPIOC, GPIO_PIN_0, PROBE_PIN_INJECTABLE, false, "F103 PB4 key (remap from PG2)"},
    {"PC1", GPIOC, GPIO_PIN_1, PROBE_PIN_MONITOR_ONLY, false, "F103 PB5 MO_PWM (remap from PG5)"},
    {"PC2", GPIOC, GPIO_PIN_2, PROBE_PIN_MONITOR_ONLY, false, "F103 PB6 encoder A (remap from PG6)"},
    {"PC3", GPIOC, GPIO_PIN_3, PROBE_PIN_MONITOR_ONLY, false, "F103 PB7 encoder B (remap from PG7)"},
    {"PD2", GPIOD, GPIO_PIN_2, PROBE_PIN_INJECTABLE, false, "F103 PB12 aux (remap from PF7)"},
    {"PD3", GPIOD, GPIO_PIN_3, PROBE_PIN_INJECTABLE, false, "F103 PB13 aux/stat (remap from PF8)"},
    {"PD4", GPIOD, GPIO_PIN_4, PROBE_PIN_INJECTABLE, false, "F103 PB14 MP3398 EN (remap from PF9)"},
    {"PD5", GPIOD, GPIO_PIN_5, PROBE_PIN_INJECTABLE, false, "F103 PB15 MP3398 ADIM (remap from PF10)"},
    {"PD6", GPIOD, GPIO_PIN_6, PROBE_PIN_INJECTABLE, false, "F103 PA8 W_PWM (remap from PF11)"},
    {"PD7", GPIOD, GPIO_PIN_7, PROBE_PIN_INJECTABLE, false, "F103 PA9 C_PWM (remap from PF12)"},
    {"PE8", GPIOE, GPIO_PIN_8, PROBE_PIN_MONITOR_ONLY, false, "F103 PA5 OLED_SCK"},
    {"PE9", GPIOE, GPIO_PIN_9, PROBE_PIN_MONITOR_ONLY, false, "F103 PA7 OLED_MOSI"},
    {"PE10", GPIOE, GPIO_PIN_10, PROBE_PIN_MONITOR_ONLY, false, "F103 PB10 OLED_CS"},
    {"PE11", GPIOE, GPIO_PIN_11, PROBE_PIN_MONITOR_ONLY, false, "F103 PB2 OLED_RES"},
    {"PE12", GPIOE, GPIO_PIN_12, PROBE_PIN_MONITOR_ONLY, false, "F103 PA6 OLED_SEL/DC (candidate, see docs)"},
    {"PA0", GPIOA, GPIO_PIN_0, PROBE_PIN_MONITOR_ONLY, true, "F103 PA3 ADC (remap from PF3)"},
    {"PA1", GPIOA, GPIO_PIN_1, PROBE_PIN_MONITOR_ONLY, true, "F103 PA4 ADC (remap from PF4)"},
    {"PA2", GPIOA, GPIO_PIN_2, PROBE_PIN_MONITOR_ONLY, true, "F103 PB0 ADC (remap from PF5)"},
    {"PA3", GPIOA, GPIO_PIN_3, PROBE_PIN_MONITOR_ONLY, true, "F103 PB1 ADC (remap from PF6)"},
    {"PC4", GPIOC, GPIO_PIN_4, PROBE_PIN_FORBIDDEN_DRIVE, false, "F103 SWDIO (remap from PF14)"},
    {"PC5", GPIOC, GPIO_PIN_5, PROBE_PIN_FORBIDDEN_DRIVE, false, "F103 SWCLK (remap from PF15)"},
    {"PB8", GPIOB, GPIO_PIN_8, PROBE_PIN_INJECTABLE, false, "BYPASS_FSM injection (remap from PF1)"},
};

static void ProbePins_EnableGpioClocks(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
}

const ProbePinDef *ProbePins_GetTable(size_t *count)
{
    if (count != NULL) {
        *count = sizeof(g_probe_pins) / sizeof(g_probe_pins[0]);
    }
    return g_probe_pins;
}

const ProbePinDef *ProbePins_FindByName(const char *name)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(table[i].name, name) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

uint8_t ProbePins_IndexOf(const ProbePinDef *pin)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (uint8_t i = 0; i < count; ++i) {
        if (&table[i] == pin) {
            return i;
        }
    }
    return 0xFFU;
}

void ProbePins_SetInput(const ProbePinDef *pin)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = pin->pin;
    g.Pull = GPIO_NOPULL;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    g.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(pin->port, &g);
}

void ProbePins_SetAnalog(const ProbePinDef *pin)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = pin->pin;
    g.Pull = GPIO_NOPULL;
    g.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(pin->port, &g);
}

void ProbePins_SetOutput(const ProbePinDef *pin, GPIO_PinState level)
{
    ProbePins_SetOutputWithCfg(pin, level, NULL);
}

void ProbePins_SetOutputWithCfg(const ProbePinDef *pin, GPIO_PinState level, const ProbePinDriveCfg *cfg)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = pin->pin;
    g.Mode = (cfg != NULL && cfg->open_drain != 0U) ? GPIO_MODE_OUTPUT_OD : GPIO_MODE_OUTPUT_PP;
    if (cfg != NULL && cfg->pull_up != 0U) {
        g.Pull = GPIO_PULLUP;
    } else if (cfg != NULL && cfg->pull_down != 0U) {
        g.Pull = GPIO_PULLDOWN;
    } else {
        g.Pull = GPIO_NOPULL;
    }
    g.Speed = (cfg != NULL && cfg->speed_high != 0U) ? GPIO_SPEED_FREQ_HIGH : GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(pin->port, &g);
    HAL_GPIO_WritePin(pin->port, pin->pin, level);
}

GPIO_PinState ProbePins_Read(const ProbePinDef *pin)
{
    return HAL_GPIO_ReadPin(pin->port, pin->pin);
}

void ProbePins_InitAllSafe(void)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    ProbePins_EnableGpioClocks();

    for (size_t i = 0; i < count; ++i) {
        if (table[i].analog_only) {
            ProbePins_SetAnalog(&table[i]);
        } else {
            ProbePins_SetInput(&table[i]);
        }
    }
}

bool ProbePins_EnableInterrupt(const ProbePinDef *pin)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = pin->pin;
    g.Pull = GPIO_NOPULL;
    g.Mode = GPIO_MODE_IT_RISING_FALLING;
    HAL_GPIO_Init(pin->port, &g);
    return true;
}
