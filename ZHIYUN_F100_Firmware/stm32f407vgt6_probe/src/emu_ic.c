#include "emu_ic.h"

#include <string.h>
#include "inject_engine.h"
#include "probe_pins.h"
#include "stm32f4xx_hal.h"

typedef enum {
    PL5500_MODE_OFF = 0,
    PL5500_MODE_TRICKLE,
    PL5500_MODE_FAST,
    PL5500_MODE_DONE,
} Pl5500Mode;

typedef struct {
    const ProbePinDef *pin;
    Pl5500Mode mode;
    uint8_t enabled;
    uint8_t level;
    uint32_t next_toggle_ms;
} Pl5500Emu;

typedef struct {
    const ProbePinDef *en_pin;
    const ProbePinDef *pwm_pin;
    const ProbePinDef *adim_pin;
    uint8_t bound;
    uint8_t enabled;

    uint32_t pwm_period_us;
    uint32_t pwm_high_us;
    uint32_t pwm_next_us;
    uint8_t pwm_level;

    uint32_t adim_period_us;
    uint32_t adim_high_us;
    uint32_t adim_next_us;
    uint8_t adim_level;
} Mp3398Emu;

static Pl5500Emu g_pl5500;
static Mp3398Emu g_mp3398;

static EmuIcResult check_injectable_pin(const char *pin_name, const ProbePinDef **out)
{
    const ProbePinDef *pin = ProbePins_FindByName(pin_name);
    if (pin == NULL || pin->mode != PROBE_PIN_INJECTABLE || pin->analog_only) {
        return EMU_IC_ERR_PIN;
    }
    *out = pin;
    return EMU_IC_OK;
}

static bool parse_pl5500_mode(const char *mode_name, Pl5500Mode *out)
{
    if (strcmp(mode_name, "OFF") == 0) {
        *out = PL5500_MODE_OFF;
        return true;
    }
    if (strcmp(mode_name, "TRICKLE") == 0) {
        *out = PL5500_MODE_TRICKLE;
        return true;
    }
    if (strcmp(mode_name, "FAST") == 0) {
        *out = PL5500_MODE_FAST;
        return true;
    }
    if (strcmp(mode_name, "DONE") == 0) {
        *out = PL5500_MODE_DONE;
        return true;
    }
    return false;
}

void EmuIc_Init(void)
{
    memset(&g_pl5500, 0, sizeof(g_pl5500));
    memset(&g_mp3398, 0, sizeof(g_mp3398));
    g_mp3398.pwm_period_us = 10000U;
    g_mp3398.pwm_high_us = 5000U;
    g_mp3398.adim_period_us = 10000U;
    g_mp3398.adim_high_us = 5000U;
}

EmuIcResult EmuIc_Pl5500StatSet(const char *pin_name, const char *mode_name)
{
    if (pin_name == NULL || mode_name == NULL) {
        return EMU_IC_ERR_ARGS;
    }

    Pl5500Mode mode = PL5500_MODE_OFF;
    if (!parse_pl5500_mode(mode_name, &mode)) {
        return EMU_IC_ERR_ARGS;
    }

    const ProbePinDef *pin = NULL;
    EmuIcResult rc = check_injectable_pin(pin_name, &pin);
    if (rc != EMU_IC_OK) {
        return rc;
    }

    g_pl5500.pin = pin;
    g_pl5500.mode = mode;
    g_pl5500.enabled = (mode == PL5500_MODE_OFF) ? 0U : 1U;
    g_pl5500.level = 0U;
    g_pl5500.next_toggle_ms = HAL_GetTick();

    if (mode == PL5500_MODE_OFF) {
        (void)InjectEngine_Release(pin->name);
        return EMU_IC_OK;
    }
    if (mode == PL5500_MODE_FAST) {
        (void)InjectEngine_Set(pin->name, 0U, 0U);
    } else if (mode == PL5500_MODE_DONE) {
        (void)InjectEngine_Set(pin->name, 1U, 0U);
    } else {
        g_pl5500.level = 1U;
        (void)InjectEngine_Set(pin->name, g_pl5500.level, 0U);
    }
    return EMU_IC_OK;
}

EmuIcResult EmuIc_Mp3398Bind(const char *en_pin, const char *pwm_pin, const char *adim_pin)
{
    if (en_pin == NULL || pwm_pin == NULL || adim_pin == NULL) {
        return EMU_IC_ERR_ARGS;
    }
    EmuIcResult rc = check_injectable_pin(en_pin, &g_mp3398.en_pin);
    if (rc != EMU_IC_OK) {
        return rc;
    }
    rc = check_injectable_pin(pwm_pin, &g_mp3398.pwm_pin);
    if (rc != EMU_IC_OK) {
        return rc;
    }
    rc = check_injectable_pin(adim_pin, &g_mp3398.adim_pin);
    if (rc != EMU_IC_OK) {
        return rc;
    }

    g_mp3398.bound = 1U;
    g_mp3398.enabled = 0U;
    (void)InjectEngine_Set(g_mp3398.en_pin->name, 0U, 0U);
    (void)InjectEngine_Set(g_mp3398.pwm_pin->name, 0U, 0U);
    (void)InjectEngine_Set(g_mp3398.adim_pin->name, 0U, 0U);
    return EMU_IC_OK;
}

static EmuIcResult set_wave_cfg(uint32_t *period_us, uint32_t *high_us, uint32_t freq_hz, uint8_t duty_pct)
{
    if (freq_hz == 0U || duty_pct > 100U) {
        return EMU_IC_ERR_ARGS;
    }
    uint32_t period = 1000000U / freq_hz;
    if (period == 0U) {
        period = 1U;
    }
    *period_us = period;
    *high_us = (uint32_t)((period * duty_pct) / 100U);
    if (duty_pct != 0U && *high_us == 0U) {
        *high_us = 1U;
    }
    return EMU_IC_OK;
}

EmuIcResult EmuIc_Mp3398ConfigPwm(uint32_t freq_hz, uint8_t duty_pct)
{
    return set_wave_cfg(&g_mp3398.pwm_period_us, &g_mp3398.pwm_high_us, freq_hz, duty_pct);
}

EmuIcResult EmuIc_Mp3398ConfigAdim(uint32_t freq_hz, uint8_t duty_pct)
{
    return set_wave_cfg(&g_mp3398.adim_period_us, &g_mp3398.adim_high_us, freq_hz, duty_pct);
}

EmuIcResult EmuIc_Mp3398Enable(uint8_t enable)
{
    if (g_mp3398.bound == 0U) {
        return EMU_IC_ERR_PIN;
    }
    g_mp3398.enabled = enable ? 1U : 0U;
    if (g_mp3398.enabled == 0U) {
        (void)InjectEngine_Set(g_mp3398.en_pin->name, 0U, 0U);
        (void)InjectEngine_Set(g_mp3398.pwm_pin->name, 0U, 0U);
        (void)InjectEngine_Set(g_mp3398.adim_pin->name, 0U, 0U);
        return EMU_IC_OK;
    }

    uint32_t now_us = 0U;
    g_mp3398.pwm_next_us = now_us + 1U;
    g_mp3398.adim_next_us = now_us + 1U;
    g_mp3398.pwm_level = 0U;
    g_mp3398.adim_level = 0U;
    (void)InjectEngine_Set(g_mp3398.en_pin->name, 1U, 0U);
    return EMU_IC_OK;
}

void EmuIc_Tick(uint32_t now_ms, uint32_t now_us)
{
    if (g_pl5500.enabled != 0U && g_pl5500.mode == PL5500_MODE_TRICKLE && g_pl5500.pin != NULL) {
        if ((int32_t)(now_ms - g_pl5500.next_toggle_ms) >= 0) {
            g_pl5500.level = (uint8_t)(g_pl5500.level ? 0U : 1U);
            (void)InjectEngine_Set(g_pl5500.pin->name, g_pl5500.level, 0U);
            g_pl5500.next_toggle_ms = now_ms + 300U;
        }
    }

    if (g_mp3398.enabled == 0U || g_mp3398.bound == 0U) {
        return;
    }

    if ((int32_t)(now_us - g_mp3398.pwm_next_us) >= 0) {
        g_mp3398.pwm_level = (uint8_t)(g_mp3398.pwm_level ? 0U : 1U);
        (void)InjectEngine_Set(g_mp3398.pwm_pin->name, g_mp3398.pwm_level, 0U);
        g_mp3398.pwm_next_us = now_us + (g_mp3398.pwm_level ? g_mp3398.pwm_high_us : (g_mp3398.pwm_period_us - g_mp3398.pwm_high_us));
        if (g_mp3398.pwm_next_us == now_us) {
            g_mp3398.pwm_next_us = now_us + 1U;
        }
    }

    if ((int32_t)(now_us - g_mp3398.adim_next_us) >= 0) {
        g_mp3398.adim_level = (uint8_t)(g_mp3398.adim_level ? 0U : 1U);
        (void)InjectEngine_Set(g_mp3398.adim_pin->name, g_mp3398.adim_level, 0U);
        g_mp3398.adim_next_us = now_us + (g_mp3398.adim_level ? g_mp3398.adim_high_us : (g_mp3398.adim_period_us - g_mp3398.adim_high_us));
        if (g_mp3398.adim_next_us == now_us) {
            g_mp3398.adim_next_us = now_us + 1U;
        }
    }
}

const char *EmuIc_HelpText(void)
{
    return "EMU_PL5500_STAT <pin> <OFF|TRICKLE|FAST|DONE>; EMU_MP3398_BIND <en_pin> <pwm_pin> <adim_pin>; EMU_MP3398_PWM <freq_hz> <duty_pct>; EMU_MP3398_ADIM <freq_hz> <duty_pct>; EMU_MP3398_ENABLE <0|1>";
}
