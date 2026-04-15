#ifndef __EMU_IC_H
#define __EMU_IC_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EMU_IC_OK = 0,
    EMU_IC_ERR_ARGS = -1,
    EMU_IC_ERR_PIN = -2,
} EmuIcResult;

void EmuIc_Init(void);
void EmuIc_Tick(uint32_t now_ms, uint32_t now_us);

EmuIcResult EmuIc_Pl5500StatSet(const char *pin_name, const char *mode_name);

EmuIcResult EmuIc_Mp3398Bind(const char *en_pin, const char *pwm_pin, const char *adim_pin);
EmuIcResult EmuIc_Mp3398ConfigPwm(uint32_t freq_hz, uint8_t duty_pct);
EmuIcResult EmuIc_Mp3398ConfigAdim(uint32_t freq_hz, uint8_t duty_pct);
EmuIcResult EmuIc_Mp3398Enable(uint8_t enable);

const char *EmuIc_HelpText(void);

#endif
