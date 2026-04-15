#ifndef __INJECT_ENGINE_H
#define __INJECT_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    INJECT_OK = 0,
    INJECT_ERR_PIN_NOT_FOUND = -1,
    INJECT_ERR_NOT_INJECTABLE = -2,
    INJECT_ERR_FORBIDDEN = -3,
    INJECT_ERR_BUSY = -4,
    INJECT_ERR_BAD_ARGS = -5,
} InjectResult;

typedef struct {
    uint8_t open_drain;
    uint8_t pull_up;
    uint8_t pull_down;
    uint8_t speed_high;
} InjectPinCfg;

void InjectEngine_Init(void);
void InjectEngine_Tick(uint32_t now_ms, uint32_t now_us);

InjectResult InjectEngine_Set(const char *pin_name, uint8_t level, uint32_t hold_ms);
InjectResult InjectEngine_PulseUs(const char *pin_name, uint32_t pulse_us);
InjectResult InjectEngine_Release(const char *pin_name);
InjectResult InjectEngine_PinCfg(const char *pin_name, const InjectPinCfg *cfg);
InjectResult InjectEngine_PlayScript(const char *script);
InjectResult InjectEngine_AbortScript(void);
InjectResult InjectEngine_RunScene(const char *scene_name);
const char *InjectEngine_ListScenes(void);

#endif
