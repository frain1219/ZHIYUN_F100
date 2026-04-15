#ifndef __PROBE_PINS_H
#define __PROBE_PINS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum {
    PROBE_PIN_MONITOR_ONLY = 0,
    PROBE_PIN_INJECTABLE = 1,
    PROBE_PIN_FORBIDDEN_DRIVE = 2,
} ProbePinMode;

typedef struct {
    const char *name;
    GPIO_TypeDef *port;
    uint16_t pin;
    ProbePinMode mode;
    bool analog_only;
    const char *note;
} ProbePinDef;

typedef struct {
    uint8_t open_drain;
    uint8_t pull_up;
    uint8_t pull_down;
    uint8_t speed_high;
} ProbePinDriveCfg;

const ProbePinDef *ProbePins_GetTable(size_t *count);
const ProbePinDef *ProbePins_FindByName(const char *name);
uint8_t ProbePins_IndexOf(const ProbePinDef *pin);

void ProbePins_InitAllSafe(void);
void ProbePins_SetInput(const ProbePinDef *pin);
void ProbePins_SetOutput(const ProbePinDef *pin, GPIO_PinState level);
void ProbePins_SetOutputWithCfg(const ProbePinDef *pin, GPIO_PinState level, const ProbePinDriveCfg *cfg);
void ProbePins_SetAnalog(const ProbePinDef *pin);
GPIO_PinState ProbePins_Read(const ProbePinDef *pin);
bool ProbePins_EnableInterrupt(const ProbePinDef *pin);

#endif
