#ifndef __PROBE_ENGINE_H
#define __PROBE_ENGINE_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct {
    uint32_t tick_ms;
    uint32_t tick_us;
    uint8_t pin_index;
    uint8_t value;
    uint8_t source;
} ProbeEvent;

void ProbeEngine_Init(void);
void ProbeEngine_Sample(uint32_t now_ms, uint32_t now_us);
bool ProbeEngine_PopEvent(ProbeEvent *out);
uint8_t ProbeEngine_GetPinValue(uint8_t pin_index);
void ProbeEngine_BuildSnapshot(char *out, uint16_t max_len);
void ProbeEngine_RecordExti(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, uint32_t now_ms, uint32_t now_us);
void ProbeEngine_ResetStats(void);
uint32_t ProbeEngine_GetDropCount(void);
uint32_t ProbeEngine_GetDepthMax(void);
uint32_t ProbeEngine_GetPollEventCount(void);
uint32_t ProbeEngine_GetExtiEventCount(void);

#endif
