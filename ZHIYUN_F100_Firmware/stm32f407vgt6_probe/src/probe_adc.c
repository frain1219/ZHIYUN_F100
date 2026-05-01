#include "probe_adc.h"

#include <string.h>
#include "probe_pins.h"

#define ADC_EVENT_QUEUE_SIZE 64U

typedef struct {
    const char *name;
    uint32_t channel;
    uint16_t last;
    uint8_t trigger_enable;
    uint16_t trigger_low;
    uint16_t trigger_high;
} ProbeAdcPin;

static ADC_HandleTypeDef g_hadc1;
static ProbeAdcPin g_adc_pins[] = {
    {"PA0", ADC_CHANNEL_0, 0U, 0U, 0U, 4095U},
    {"PA1", ADC_CHANNEL_1, 0U, 0U, 0U, 4095U},
    {"PA2", ADC_CHANNEL_2, 0U, 0U, 0U, 4095U},
    {"PA3", ADC_CHANNEL_3, 0U, 0U, 0U, 4095U},
};

static ProbeAdcEvent g_events[ADC_EVENT_QUEUE_SIZE];
static uint8_t g_ev_head = 0;
static uint8_t g_ev_tail = 0;
static uint8_t g_stream_enabled = 0;
static uint32_t g_stream_interval_ms = 20U;
static uint32_t g_last_stream_ms = 0U;
static uint32_t g_sample_count = 0U;
static uint32_t g_trigger_count = 0U;
static uint32_t g_drop_count = 0U;

static ProbeAdcPin *find_adc_pin(const char *name)
{
    for (uint8_t i = 0; i < (uint8_t)(sizeof(g_adc_pins) / sizeof(g_adc_pins[0])); ++i) {
        if (strcmp(name, g_adc_pins[i].name) == 0) {
            return &g_adc_pins[i];
        }
    }
    return NULL;
}

static void queue_event(uint32_t now_ms, uint32_t now_us, const char *pin_name, uint16_t value, uint8_t is_trigger)
{
    uint8_t next = (uint8_t)((g_ev_head + 1U) % ADC_EVENT_QUEUE_SIZE);
    if (next == g_ev_tail) {
        g_ev_tail = (uint8_t)((g_ev_tail + 1U) % ADC_EVENT_QUEUE_SIZE);
        g_drop_count++;
    }
    g_events[g_ev_head].tick_ms = now_ms;
    g_events[g_ev_head].tick_us = now_us;
    g_events[g_ev_head].pin_name = pin_name;
    g_events[g_ev_head].value = value;
    g_events[g_ev_head].is_trigger = is_trigger;
    g_ev_head = next;
}

void ProbeAdc_Init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    g_hadc1.Instance = ADC1;
    g_hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    g_hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    g_hadc1.Init.ScanConvMode = DISABLE;
    g_hadc1.Init.ContinuousConvMode = DISABLE;
    g_hadc1.Init.DiscontinuousConvMode = DISABLE;
    g_hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_hadc1.Init.NbrOfConversion = 1;
    g_hadc1.Init.DMAContinuousRequests = DISABLE;
    g_hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    (void)HAL_ADC_Init(&g_hadc1);
}

bool ProbeAdc_ReadPin(const char *pin_name, uint16_t *out_value)
{
    ProbeAdcPin *p = find_adc_pin(pin_name);
    if (p == NULL || out_value == NULL) {
        return false;
    }

    ADC_ChannelConfTypeDef cfg = {0};
    cfg.Channel = p->channel;
    cfg.Rank = 1;
    cfg.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    if (HAL_ADC_ConfigChannel(&g_hadc1, &cfg) != HAL_OK) {
        return false;
    }
    if (HAL_ADC_Start(&g_hadc1) != HAL_OK) {
        return false;
    }
    if (HAL_ADC_PollForConversion(&g_hadc1, 2U) != HAL_OK) {
        (void)HAL_ADC_Stop(&g_hadc1);
        return false;
    }
    *out_value = (uint16_t)HAL_ADC_GetValue(&g_hadc1);
    p->last = *out_value;
    (void)HAL_ADC_Stop(&g_hadc1);
    return true;
}

bool ProbeAdc_StreamStart(uint32_t interval_ms)
{
    if (interval_ms == 0U) {
        return false;
    }
    g_stream_enabled = 1U;
    g_stream_interval_ms = interval_ms;
    g_last_stream_ms = HAL_GetTick();
    return true;
}

void ProbeAdc_StreamStop(void)
{
    g_stream_enabled = 0U;
}

void ProbeAdc_Tick(uint32_t now_ms, uint32_t now_us)
{
    if (g_stream_enabled == 0U) {
        return;
    }
    if ((now_ms - g_last_stream_ms) < g_stream_interval_ms) {
        return;
    }
    g_last_stream_ms = now_ms;

    for (uint8_t i = 0; i < (uint8_t)(sizeof(g_adc_pins) / sizeof(g_adc_pins[0])); ++i) {
        uint16_t v = 0U;
        if (!ProbeAdc_ReadPin(g_adc_pins[i].name, &v)) {
            continue;
        }
        g_sample_count++;
        queue_event(now_ms, now_us, g_adc_pins[i].name, v, 0U);
        if (g_adc_pins[i].trigger_enable != 0U && (v < g_adc_pins[i].trigger_low || v > g_adc_pins[i].trigger_high)) {
            g_trigger_count++;
            queue_event(now_ms, now_us, g_adc_pins[i].name, v, 1U);
        }
    }
}

bool ProbeAdc_PopEvent(ProbeAdcEvent *out)
{
    if (out == NULL || g_ev_head == g_ev_tail) {
        return false;
    }
    *out = g_events[g_ev_tail];
    g_ev_tail = (uint8_t)((g_ev_tail + 1U) % ADC_EVENT_QUEUE_SIZE);
    return true;
}

bool ProbeAdc_SetTrigger(const char *pin_name, uint16_t low, uint16_t high, uint8_t enable)
{
    ProbeAdcPin *p = find_adc_pin(pin_name);
    if (p == NULL || low > high) {
        return false;
    }
    p->trigger_low = low;
    p->trigger_high = high;
    p->trigger_enable = enable ? 1U : 0U;
    return true;
}

uint32_t ProbeAdc_GetSampleCount(void)
{
    return g_sample_count;
}

uint32_t ProbeAdc_GetTriggerCount(void)
{
    return g_trigger_count;
}

uint32_t ProbeAdc_GetDropCount(void)
{
    return g_drop_count;
}
