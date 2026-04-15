#include "probe_engine.h"

#include <stdio.h>
#include <string.h>
#include "probe_pins.h"

#define PROBE_EVENT_QUEUE_SIZE 256U
#define EVENT_SRC_POLL 0U
#define EVENT_SRC_EXTI 1U

static uint8_t g_last_values[32];
static ProbeEvent g_events[PROBE_EVENT_QUEUE_SIZE];
static uint16_t g_ev_head = 0;
static uint16_t g_ev_tail = 0;
static uint32_t g_drop_count = 0;
static uint32_t g_depth_max = 0;
static uint32_t g_poll_event_count = 0;
static uint32_t g_exti_event_count = 0;

static uint16_t queue_depth(void)
{
    return (uint16_t)((g_ev_head + PROBE_EVENT_QUEUE_SIZE - g_ev_tail) % PROBE_EVENT_QUEUE_SIZE);
}

static void push_event(uint32_t tick_ms, uint32_t tick_us, uint8_t pin_index, uint8_t value, uint8_t source)
{
    uint16_t next = (uint16_t)((g_ev_head + 1U) % PROBE_EVENT_QUEUE_SIZE);
    if (next == g_ev_tail) {
        g_ev_tail = (uint16_t)((g_ev_tail + 1U) % PROBE_EVENT_QUEUE_SIZE);
        g_drop_count++;
    }

    g_events[g_ev_head].tick_ms = tick_ms;
    g_events[g_ev_head].tick_us = tick_us;
    g_events[g_ev_head].pin_index = pin_index;
    g_events[g_ev_head].value = value;
    g_events[g_ev_head].source = source;
    g_ev_head = next;

    uint16_t depth = queue_depth();
    if (depth > g_depth_max) {
        g_depth_max = depth;
    }
}

void ProbeEngine_Init(void)
{
    memset(g_last_values, 0, sizeof(g_last_values));
    g_ev_head = 0;
    g_ev_tail = 0;
    g_drop_count = 0;
    g_depth_max = 0;
    g_poll_event_count = 0;
    g_exti_event_count = 0;

    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (uint8_t i = 0; i < count; ++i) {
        g_last_values[i] = (uint8_t)ProbePins_Read(&table[i]);
    }
}

void ProbeEngine_Sample(uint32_t now_ms, uint32_t now_us)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (uint8_t i = 0; i < count; ++i) {
        uint8_t value = (uint8_t)ProbePins_Read(&table[i]);
        if (value != g_last_values[i]) {
            g_last_values[i] = value;
            push_event(now_ms, now_us, i, value, EVENT_SRC_POLL);
            g_poll_event_count++;
        }
    }
}

bool ProbeEngine_PopEvent(ProbeEvent *out)
{
    if (g_ev_head == g_ev_tail) {
        return false;
    }
    *out = g_events[g_ev_tail];
    g_ev_tail = (uint16_t)((g_ev_tail + 1U) % PROBE_EVENT_QUEUE_SIZE);
    return true;
}

uint8_t ProbeEngine_GetPinValue(uint8_t pin_index)
{
    return g_last_values[pin_index];
}

void ProbeEngine_BuildSnapshot(char *out, uint16_t max_len)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    uint16_t used = 0;

    used += (uint16_t)snprintf(out + used, max_len - used, "{\"type\":\"snapshot\",\"pins\":{");
    for (uint8_t i = 0; i < count; ++i) {
        used += (uint16_t)snprintf(
            out + used,
            max_len - used,
            "%s\"%s\":%u",
            (i == 0U) ? "" : ",",
            table[i].name,
            (unsigned)g_last_values[i]
        );
        if (used >= max_len) {
            break;
        }
    }
    (void)snprintf(out + ((used < max_len) ? used : (max_len - 1U)), (used < max_len) ? (max_len - used) : 1U, "}}");
}

void ProbeEngine_RecordExti(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, uint32_t now_ms, uint32_t now_us)
{
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (uint8_t i = 0; i < count; ++i) {
        if (table[i].pin == gpio_pin && table[i].port == gpio_port) {
            uint8_t value = (uint8_t)ProbePins_Read(&table[i]);
            if (value != g_last_values[i]) {
                g_last_values[i] = value;
            }
            push_event(now_ms, now_us, i, value, EVENT_SRC_EXTI);
            g_exti_event_count++;
            return;
        }
    }
}

void ProbeEngine_ResetStats(void)
{
    g_drop_count = 0;
    g_depth_max = queue_depth();
    g_poll_event_count = 0;
    g_exti_event_count = 0;
}

uint32_t ProbeEngine_GetDropCount(void)
{
    return g_drop_count;
}

uint32_t ProbeEngine_GetDepthMax(void)
{
    return g_depth_max;
}

uint32_t ProbeEngine_GetPollEventCount(void)
{
    return g_poll_event_count;
}

uint32_t ProbeEngine_GetExtiEventCount(void)
{
    return g_exti_event_count;
}
