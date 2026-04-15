#ifndef __PROBE_ADC_H
#define __PROBE_ADC_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t tick_ms;
    uint32_t tick_us;
    const char *pin_name;
    uint16_t value;
    uint8_t is_trigger;
} ProbeAdcEvent;

void ProbeAdc_Init(void);
bool ProbeAdc_ReadPin(const char *pin_name, uint16_t *out_value);
bool ProbeAdc_StreamStart(uint32_t interval_ms);
void ProbeAdc_StreamStop(void);
void ProbeAdc_Tick(uint32_t now_ms, uint32_t now_us);
bool ProbeAdc_PopEvent(ProbeAdcEvent *out);
bool ProbeAdc_SetTrigger(const char *pin_name, uint16_t low, uint16_t high, uint8_t enable);
uint32_t ProbeAdc_GetSampleCount(void);
uint32_t ProbeAdc_GetTriggerCount(void);
uint32_t ProbeAdc_GetDropCount(void);

#endif
