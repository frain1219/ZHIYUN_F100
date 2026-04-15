#include "inject_engine.h"

#include <stdlib.h>
#include <string.h>
#include "probe_pins.h"
#include "probe_time.h"

typedef struct {
    const ProbePinDef *pin;
    InjectPinCfg cfg;
    uint32_t release_deadline;
    uint32_t release_deadline_us;
    uint8_t active;
} InjectState;

typedef struct {
    const ProbePinDef *pin;
    uint8_t level;
    uint32_t delay_us;
} SeqStep;

#define MAX_SEQ_STEPS 32U

static InjectState g_states[32];
static uint8_t g_state_count = 0;
static SeqStep g_seq_steps[MAX_SEQ_STEPS];
static uint8_t g_seq_count = 0;
static uint8_t g_seq_index = 0;
static uint8_t g_seq_running = 0;
static uint32_t g_seq_deadline_us = 0;

static InjectState *find_state(const ProbePinDef *pin)
{
    for (uint8_t i = 0; i < g_state_count; ++i) {
        if (g_states[i].pin == pin) {
            return &g_states[i];
        }
    }

    if (g_state_count >= (uint8_t)(sizeof(g_states) / sizeof(g_states[0]))) {
        return NULL;
    }

    g_states[g_state_count].pin = pin;
    g_states[g_state_count].cfg.open_drain = 0U;
    g_states[g_state_count].cfg.pull_up = 0U;
    g_states[g_state_count].cfg.pull_down = 0U;
    g_states[g_state_count].cfg.speed_high = 0U;
    g_states[g_state_count].release_deadline = 0;
    g_states[g_state_count].release_deadline_us = 0;
    g_states[g_state_count].active = 0;
    ++g_state_count;
    return &g_states[g_state_count - 1U];
}

static bool can_drive(const ProbePinDef *pin, InjectResult *err)
{
    if (pin == NULL) {
        *err = INJECT_ERR_PIN_NOT_FOUND;
        return false;
    }
    if (pin->mode == PROBE_PIN_FORBIDDEN_DRIVE) {
        *err = INJECT_ERR_FORBIDDEN;
        return false;
    }
    if (pin->mode != PROBE_PIN_INJECTABLE || pin->analog_only) {
        *err = INJECT_ERR_NOT_INJECTABLE;
        return false;
    }
    return true;
}

void InjectEngine_Init(void)
{
    g_state_count = 0;
    g_seq_count = 0;
    g_seq_index = 0;
    g_seq_running = 0;
    g_seq_deadline_us = 0;
}

void InjectEngine_Tick(uint32_t now_ms, uint32_t now_us)
{
    for (uint8_t i = 0; i < g_state_count; ++i) {
        InjectState *st = &g_states[i];
        if (st->active && st->release_deadline_us > 0U) {
            if ((int32_t)(now_us - st->release_deadline_us) >= 0) {
                ProbePins_SetInput(st->pin);
                st->active = 0;
                st->release_deadline_us = 0;
                st->release_deadline = 0;
                continue;
            }
        }
        if (st->active && st->release_deadline > 0U) {
            if ((int32_t)(now_ms - st->release_deadline) >= 0) {
                ProbePins_SetInput(st->pin);
                st->active = 0;
                st->release_deadline = 0;
                st->release_deadline_us = 0;
            }
        }
    }

    if (g_seq_running != 0U && (int32_t)(now_us - g_seq_deadline_us) >= 0) {
        if (g_seq_index >= g_seq_count) {
            g_seq_running = 0U;
            g_seq_count = 0U;
            g_seq_index = 0U;
            return;
        }

        SeqStep *step = &g_seq_steps[g_seq_index];
        (void)InjectEngine_Set(step->pin->name, step->level, 0U);
        g_seq_deadline_us = now_us + step->delay_us;
        g_seq_index++;
    }
}

InjectResult InjectEngine_Set(const char *pin_name, uint8_t level, uint32_t hold_ms)
{
    InjectResult err = INJECT_OK;
    const ProbePinDef *pin = ProbePins_FindByName(pin_name);
    if (!can_drive(pin, &err)) {
        return err;
    }

    InjectState *st = find_state(pin);
    if (st == NULL) {
        return INJECT_ERR_NOT_INJECTABLE;
    }

    ProbePinDriveCfg drive = {
        .open_drain = st->cfg.open_drain,
        .pull_up = st->cfg.pull_up,
        .pull_down = st->cfg.pull_down,
        .speed_high = st->cfg.speed_high
    };
    ProbePins_SetOutputWithCfg(pin, (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET, &drive);
    st->active = 1U;
    st->release_deadline_us = 0U;
    if (hold_ms > 0U) {
        st->release_deadline = HAL_GetTick() + hold_ms;
    } else {
        st->release_deadline = 0U;
    }
    return INJECT_OK;
}

InjectResult InjectEngine_PulseUs(const char *pin_name, uint32_t pulse_us)
{
    InjectResult err = INJECT_OK;
    const ProbePinDef *pin = ProbePins_FindByName(pin_name);
    if (!can_drive(pin, &err)) {
        return err;
    }

    InjectState *st = find_state(pin);
    if (st == NULL) {
        return INJECT_ERR_NOT_INJECTABLE;
    }

    ProbePinDriveCfg drive = {
        .open_drain = st->cfg.open_drain,
        .pull_up = st->cfg.pull_up,
        .pull_down = st->cfg.pull_down,
        .speed_high = st->cfg.speed_high
    };
    ProbePins_SetOutputWithCfg(pin, GPIO_PIN_SET, &drive);
    st->active = 1U;
    st->release_deadline = 0U;
    st->release_deadline_us = ProbeTime_GetUs() + pulse_us;
    return INJECT_OK;
}

InjectResult InjectEngine_Release(const char *pin_name)
{
    const ProbePinDef *pin = ProbePins_FindByName(pin_name);
    if (pin == NULL) {
        return INJECT_ERR_PIN_NOT_FOUND;
    }

    ProbePins_SetInput(pin);
    InjectState *st = find_state(pin);
    if (st != NULL) {
        st->active = 0U;
        st->release_deadline = 0U;
        st->release_deadline_us = 0U;
    }
    return INJECT_OK;
}

InjectResult InjectEngine_PinCfg(const char *pin_name, const InjectPinCfg *cfg)
{
    InjectResult err = INJECT_OK;
    const ProbePinDef *pin = ProbePins_FindByName(pin_name);
    if (!can_drive(pin, &err)) {
        return err;
    }
    if (cfg == NULL) {
        return INJECT_ERR_BAD_ARGS;
    }

    InjectState *st = find_state(pin);
    if (st == NULL) {
        return INJECT_ERR_NOT_INJECTABLE;
    }
    st->cfg = *cfg;
    return INJECT_OK;
}

static InjectResult parse_script(char *buf)
{
    char *save_step = NULL;
    char *step = strtok_r(buf, ";", &save_step);
    g_seq_count = 0U;

    while (step != NULL) {
        if (g_seq_count >= MAX_SEQ_STEPS) {
            return INJECT_ERR_BAD_ARGS;
        }

        char *save_tok = NULL;
        char *pin_name = strtok_r(step, ",", &save_tok);
        char *level_s = strtok_r(NULL, ",", &save_tok);
        char *delay_s = strtok_r(NULL, ",", &save_tok);
        if (pin_name == NULL || level_s == NULL || delay_s == NULL) {
            return INJECT_ERR_BAD_ARGS;
        }

        InjectResult err = INJECT_OK;
        const ProbePinDef *pin = ProbePins_FindByName(pin_name);
        if (!can_drive(pin, &err)) {
            return err;
        }

        g_seq_steps[g_seq_count].pin = pin;
        g_seq_steps[g_seq_count].level = (uint8_t)(strtoul(level_s, NULL, 0) ? 1U : 0U);
        g_seq_steps[g_seq_count].delay_us = (uint32_t)strtoul(delay_s, NULL, 0);
        if (g_seq_steps[g_seq_count].delay_us == 0U) {
            g_seq_steps[g_seq_count].delay_us = 1U;
        }
        g_seq_count++;
        step = strtok_r(NULL, ";", &save_step);
    }
    return (g_seq_count == 0U) ? INJECT_ERR_BAD_ARGS : INJECT_OK;
}

InjectResult InjectEngine_PlayScript(const char *script)
{
    char local[256];
    if (script == NULL) {
        return INJECT_ERR_BAD_ARGS;
    }
    if (g_seq_running != 0U) {
        return INJECT_ERR_BUSY;
    }

    size_t len = strlen(script);
    if (len >= sizeof(local)) {
        return INJECT_ERR_BAD_ARGS;
    }
    memcpy(local, script, len + 1U);

    InjectResult rc = parse_script(local);
    if (rc != INJECT_OK) {
        g_seq_count = 0U;
        return rc;
    }

    g_seq_index = 0U;
    g_seq_running = 1U;
    g_seq_deadline_us = 0U;
    return INJECT_OK;
}

InjectResult InjectEngine_AbortScript(void)
{
    g_seq_running = 0U;
    g_seq_count = 0U;
    g_seq_index = 0U;
    g_seq_deadline_us = 0U;
    return INJECT_OK;
}

const char *InjectEngine_ListScenes(void)
{
    return "PD_ATTACH_BLIP KEY_RIGHT_TAP PA0_PULSE2";
}

InjectResult InjectEngine_RunScene(const char *scene_name)
{
    if (scene_name == NULL) {
        return INJECT_ERR_BAD_ARGS;
    }
    if (strcmp(scene_name, "PD_ATTACH_BLIP") == 0) {
        return InjectEngine_PlayScript("PF1,1,2000;PF1,0,2000");
    }
    if (strcmp(scene_name, "KEY_RIGHT_TAP") == 0) {
        return InjectEngine_PlayScript("PG2,1,30000;PG2,0,30000");
    }
    if (strcmp(scene_name, "PA0_PULSE2") == 0) {
        return InjectEngine_PlayScript("PE5,1,20000;PE5,0,20000;PE5,1,20000;PE5,0,20000");
    }
    return INJECT_ERR_BAD_ARGS;
}
