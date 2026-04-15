#include "bridge_protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bypass_fsm.h"
#include "emu_bus.h"
#include "emu_ic.h"
#include "inject_engine.h"
#include "oled_sniffer.h"
#include "probe_adc.h"
#include "probe_engine.h"
#include "probe_pins.h"
#include "probe_stats.h"
#include "probe_time.h"
#include "usbd_cdc_if.h"

#define LINE_BUF_SIZE 192U

static char g_line_buf[LINE_BUF_SIZE];
static uint16_t g_line_len = 0;
static uint8_t g_subscribe = 0;
static uint32_t g_last_snapshot_ms = 0;

static void stats_sync(void)
{
    EmuBusStats emu_stats = {0};
    BypassFsmStats bypass_stats = {0};
    OledSnifferStats oled_stats = {0};
    EmuBus_GetStats(&emu_stats);
    BypassFsm_GetStats(&bypass_stats);
    OledSniffer_GetStats(&oled_stats);

    ProbeStats_SetEvent(
        ProbeEngine_GetDropCount(),
        ProbeEngine_GetDepthMax(),
        ProbeEngine_GetPollEventCount(),
        ProbeEngine_GetExtiEventCount()
    );
    ProbeStats_SetAdc(
        ProbeAdc_GetSampleCount(),
        ProbeAdc_GetTriggerCount()
    );
    ProbeStats_SetEmu(
        emu_stats.i2c_req_count,
        emu_stats.i2c_hit_count,
        emu_stats.i2c_timeout_count,
        emu_stats.i2c_drop_count,
        emu_stats.spi_req_count,
        emu_stats.spi_hit_count,
        emu_stats.spi_timeout_count,
        emu_stats.spi_drop_count
    );
    ProbeStats_SetBypass(
        bypass_stats.trigger_count,
        bypass_stats.success_count,
        bypass_stats.timeout_count,
        bypass_stats.active_cycles
    );
    ProbeStats_SetOled(
        oled_stats.edge_count,
        oled_stats.txn_count,
        oled_stats.byte_count,
        oled_stats.drop_count
    );
}

static void send_line_checked(const char *line)
{
    if (!CDC_TransmitLine(line)) {
        ProbeStats_IncTxDrop();
    }
}

static void send_ok(const char *cmd, const char *msg)
{
    char out[200];
    (void)snprintf(out, sizeof(out), "{\"type\":\"ok\",\"cmd\":\"%s\",\"implemented\":1,\"msg\":\"%s\"}", cmd, msg);
    send_line_checked(out);
}

static void send_err(const char *cmd, const char *reason)
{
    char out[220];
    (void)snprintf(out, sizeof(out), "{\"type\":\"err\",\"cmd\":\"%s\",\"implemented\":1,\"reason\":\"%s\"}", cmd, reason);
    send_line_checked(out);
}

static void send_pin_list(void)
{
    char out[512];
    size_t count = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    uint16_t used = 0;
    used += (uint16_t)snprintf(out + used, sizeof(out) - used, "{\"type\":\"pins\",\"list\":[");
    for (size_t i = 0; i < count; ++i) {
        used += (uint16_t)snprintf(
            out + used,
            sizeof(out) - used,
            "%s{\"name\":\"%s\",\"mode\":%u,\"analog\":%u}",
            (i == 0) ? "" : ",",
            table[i].name,
            (unsigned)table[i].mode,
            table[i].analog_only ? 1U : 0U
        );
        if (used >= sizeof(out)) {
            break;
        }
    }
    (void)snprintf(out + ((used < sizeof(out)) ? used : (sizeof(out) - 1U)),
                   (used < sizeof(out)) ? (sizeof(out) - used) : 1U,
                   "]}");
    send_line_checked(out);
}

static void send_caps(void)
{
    send_line_checked("{\"type\":\"caps\",\"implemented\":[\"PING\",\"GET_STATUS\",\"SELF_CHECK\",\"SUBSCRIBE\",\"GPIO_SET\",\"GPIO_RELEASE\",\"GPIO_PULSE\",\"PIN_CFG\",\"SEQ_PLAY\",\"SEQ_ABORT\",\"SCENE_LIST\",\"SCENE_RUN\",\"SCENE_STOP\",\"ADC_READ\",\"ADC_STREAM_START\",\"ADC_STREAM_STOP\",\"ADC_TRIGGER\",\"EMU_PL5500_STAT\",\"EMU_MP3398_BIND\",\"EMU_MP3398_PWM\",\"EMU_MP3398_ADIM\",\"EMU_MP3398_ENABLE\",\"I2C_SLAVE_EMU\",\"SPI_SLAVE_EMU\",\"BYPASS_FSM\",\"OLED_SNIFF_START\",\"OLED_SNIFF_STOP\",\"OLED_SNIFF_STATUS\",\"OLED_SNIFF_DUMP\",\"LIST_PINS\",\"GET_CAPS\",\"GET_STATS\",\"RESET_STATS\",\"HELP\"],\"not_implemented\":[],\"limitations\":[\"EXTI line conflict limits concurrent line1 capture\",\"ADC stream uses software polling\",\"USB CDC can still drop under extreme host backpressure\",\"MP3398 waveform uses software toggling and is not a power-stage equivalent\",\"I2C/SPI emulation is protocol-level and not physical bus timing equivalent\"]}" );
}

static void send_stats(void)
{
    ProbeStats stats;
    stats_sync();
    ProbeStats_Get(&stats);

    char out[760];
    (void)snprintf(
        out,
        sizeof(out),
        "{\"type\":\"stats\",\"cmd_count\":%lu,\"tx_drop_count\":%lu,\"event_drop_count\":%lu,\"event_queue_depth_max\":%lu,\"event_poll_count\":%lu,\"event_exti_count\":%lu,\"adc_sample_count\":%lu,\"adc_trigger_count\":%lu,\"emu_i2c_req_count\":%lu,\"emu_i2c_hit_count\":%lu,\"emu_i2c_timeout_count\":%lu,\"emu_i2c_drop_count\":%lu,\"emu_spi_req_count\":%lu,\"emu_spi_hit_count\":%lu,\"emu_spi_timeout_count\":%lu,\"emu_spi_drop_count\":%lu,\"bypass_trigger_count\":%lu,\"bypass_success_count\":%lu,\"bypass_timeout_count\":%lu,\"bypass_active_cycles\":%lu,\"oled_edge_count\":%lu,\"oled_txn_count\":%lu,\"oled_byte_count\":%lu,\"oled_drop_count\":%lu,\"usb_tx_queue_depth\":%u,\"usb_tx_queue_depth_max\":%u,\"usb_tx_drop_count\":%lu}",
        (unsigned long)stats.cmd_count,
        (unsigned long)stats.tx_drop_count,
        (unsigned long)stats.event_drop_count,
        (unsigned long)stats.event_depth_max,
        (unsigned long)stats.poll_event_count,
        (unsigned long)stats.exti_event_count,
        (unsigned long)stats.adc_sample_count,
        (unsigned long)stats.adc_trigger_count,
        (unsigned long)stats.emu_i2c_req_count,
        (unsigned long)stats.emu_i2c_hit_count,
        (unsigned long)stats.emu_i2c_timeout_count,
        (unsigned long)stats.emu_i2c_drop_count,
        (unsigned long)stats.emu_spi_req_count,
        (unsigned long)stats.emu_spi_hit_count,
        (unsigned long)stats.emu_spi_timeout_count,
        (unsigned long)stats.emu_spi_drop_count,
        (unsigned long)stats.bypass_trigger_count,
        (unsigned long)stats.bypass_success_count,
        (unsigned long)stats.bypass_timeout_count,
        (unsigned long)stats.bypass_active_cycles,
        (unsigned long)stats.oled_edge_count,
        (unsigned long)stats.oled_txn_count,
        (unsigned long)stats.oled_byte_count,
        (unsigned long)stats.oled_drop_count,
        (unsigned)CDC_GetTxQueueDepth(),
        (unsigned)CDC_GetTxQueueDepthMax(),
        (unsigned long)CDC_GetTxQueueDropCount()
    );
    send_line_checked(out);
}

static void send_self_check(void)
{
    size_t count = 0;
    size_t injectable = 0;
    size_t monitor_only = 0;
    size_t forbidden = 0;
    size_t analog_only = 0;
    const ProbePinDef *table = ProbePins_GetTable(&count);
    for (size_t i = 0; i < count; ++i) {
        if (table[i].mode == PROBE_PIN_INJECTABLE) {
            injectable++;
        } else if (table[i].mode == PROBE_PIN_MONITOR_ONLY) {
            monitor_only++;
        } else if (table[i].mode == PROBE_PIN_FORBIDDEN_DRIVE) {
            forbidden++;
        }
        if (table[i].analog_only) {
            analog_only++;
        }
    }

    char out[560];
    (void)snprintf(
        out,
        sizeof(out),
        "{\"type\":\"self_check\",\"fw\":\"f407_probe\",\"pins_total\":%u,\"pins_injectable\":%u,\"pins_monitor_only\":%u,\"pins_forbidden_drive\":%u,\"pins_analog_only\":%u,\"exti_watch\":[\"PE0\",\"PF1\",\"PG2\",\"PG5\",\"PG6\",\"PG7\",\"PE8\",\"PE9\",\"PE10\",\"PE11\",\"PE12\"],\"note\":\"software-layer check only; no physical wiring asserted\"}",
        (unsigned)count,
        (unsigned)injectable,
        (unsigned)monitor_only,
        (unsigned)forbidden,
        (unsigned)analog_only
    );
    send_line_checked(out);
}

static void handle_i2c_emu(char *args)
{
    char out[280];
    if (!EmuBus_HandleI2cCommand(args, out, sizeof(out))) {
        send_err("I2C_SLAVE_EMU", "bad args, try HELP");
        return;
    }
    send_line_checked(out);
}

static void handle_spi_emu(char *args)
{
    char out[280];
    if (!EmuBus_HandleSpiCommand(args, out, sizeof(out))) {
        send_err("SPI_SLAVE_EMU", "bad args, try HELP");
        return;
    }
    send_line_checked(out);
}

static void handle_bypass_fsm(char *args)
{
    char out[280];
    if (!BypassFsm_HandleCommand(args, out, sizeof(out))) {
        send_err("BYPASS_FSM", "bad args, try: ON|OFF|STATUS|RESET");
        return;
    }
    send_line_checked(out);
}

static void handle_oled_dump(char *args)
{
    char out[900];
    if (!OledSniffer_Dump(args, out, sizeof(out))) {
        send_err("OLED_SNIFF_DUMP", "bad args, try: edge|txn|bytes [limit]");
        return;
    }
    send_line_checked(out);
}

static void handle_gpio_set(char *args)
{
    char *pin = strtok(args, " ");
    char *level_s = strtok(NULL, " ");
    char *hold_s = strtok(NULL, " ");

    if (pin == NULL || level_s == NULL) {
        send_err("GPIO_SET", "need pin level [hold_ms]");
        return;
    }

    uint8_t level = (uint8_t)strtoul(level_s, NULL, 0);
    uint32_t hold_ms = (hold_s != NULL) ? (uint32_t)strtoul(hold_s, NULL, 0) : 0U;

    InjectResult rc = InjectEngine_Set(pin, level ? 1U : 0U, hold_ms);
    if (rc == INJECT_OK) {
        send_ok("GPIO_SET", "applied");
    } else {
        send_err("GPIO_SET", "apply failed");
    }
}

static void handle_gpio_release(char *args)
{
    char *pin = strtok(args, " ");
    if (pin == NULL) {
        send_err("GPIO_RELEASE", "need pin");
        return;
    }
    InjectResult rc = InjectEngine_Release(pin);
    if (rc == INJECT_OK) {
        send_ok("GPIO_RELEASE", "released");
    } else {
        send_err("GPIO_RELEASE", "release failed");
    }
}

static void handle_gpio_pulse(char *args)
{
    char *pin = strtok(args, " ");
    char *us_s = strtok(NULL, " ");
    if (pin == NULL || us_s == NULL) {
        send_err("GPIO_PULSE", "need pin pulse_us");
        return;
    }
    uint32_t pulse_us = (uint32_t)strtoul(us_s, NULL, 0);
    InjectResult rc = InjectEngine_PulseUs(pin, pulse_us);
    if (rc == INJECT_OK) {
        send_ok("GPIO_PULSE", "scheduled");
    } else {
        send_err("GPIO_PULSE", "pulse failed");
    }
}

static void handle_pin_cfg(char *args)
{
    char *pin = strtok(args, " ");
    char *mode_s = strtok(NULL, " ");
    char *pull_s = strtok(NULL, " ");
    char *speed_s = strtok(NULL, " ");
    if (pin == NULL || mode_s == NULL || pull_s == NULL || speed_s == NULL) {
        send_err("PIN_CFG", "need pin mode(pp|od) pull(none|up|down) speed(low|high)");
        return;
    }

    InjectPinCfg cfg = {0};
    cfg.open_drain = (uint8_t)(strcmp(mode_s, "od") == 0);
    cfg.pull_up = (uint8_t)(strcmp(pull_s, "up") == 0);
    cfg.pull_down = (uint8_t)(strcmp(pull_s, "down") == 0);
    cfg.speed_high = (uint8_t)(strcmp(speed_s, "high") == 0);
    InjectResult rc = InjectEngine_PinCfg(pin, &cfg);
    if (rc == INJECT_OK) {
        send_ok("PIN_CFG", "configured");
    } else {
        send_err("PIN_CFG", "config failed");
    }
}

static void handle_seq_play(char *args)
{
    InjectResult rc = InjectEngine_PlayScript(args);
    if (rc == INJECT_OK) {
        send_ok("SEQ_PLAY", "running");
    } else if (rc == INJECT_ERR_BUSY) {
        send_err("SEQ_PLAY", "busy");
    } else {
        send_err("SEQ_PLAY", "bad script");
    }
}

static void handle_scene_run(char *args)
{
    char *scene = strtok(args, " ");
    if (scene == NULL) {
        send_err("SCENE_RUN", "need scene name");
        return;
    }
    InjectResult rc = InjectEngine_RunScene(scene);
    if (rc == INJECT_OK) {
        send_ok("SCENE_RUN", "running");
    } else {
        send_err("SCENE_RUN", "invalid scene");
    }
}

static void handle_adc_read(char *args)
{
    char *pin = strtok(args, " ");
    if (pin == NULL) {
        send_err("ADC_READ", "need pin");
        return;
    }
    uint16_t v = 0;
    if (!ProbeAdc_ReadPin(pin, &v)) {
        send_err("ADC_READ", "read failed");
        return;
    }
    char out[128];
    (void)snprintf(out, sizeof(out), "{\"type\":\"adc_read\",\"pin\":\"%s\",\"value\":%u}", pin, (unsigned)v);
    send_line_checked(out);
}

static void handle_adc_stream(char *cmd, char *args)
{
    if (strcmp(cmd, "ADC_STREAM_START") == 0) {
        uint32_t interval = 20U;
        if (args != NULL) {
            interval = (uint32_t)strtoul(args, NULL, 0);
        }
        if (ProbeAdc_StreamStart(interval)) {
            send_ok("ADC_STREAM_START", "started");
        } else {
            send_err("ADC_STREAM_START", "invalid interval");
        }
    } else {
        ProbeAdc_StreamStop();
        send_ok("ADC_STREAM_STOP", "stopped");
    }
}

static void handle_adc_trigger(char *args)
{
    char *pin = strtok(args, " ");
    char *low_s = strtok(NULL, " ");
    char *high_s = strtok(NULL, " ");
    char *en_s = strtok(NULL, " ");
    if (pin == NULL || low_s == NULL || high_s == NULL || en_s == NULL) {
        send_err("ADC_TRIGGER", "need pin low high on|off");
        return;
    }
    uint16_t low = (uint16_t)strtoul(low_s, NULL, 0);
    uint16_t high = (uint16_t)strtoul(high_s, NULL, 0);
    uint8_t en = (uint8_t)((strcmp(en_s, "on") == 0 || strcmp(en_s, "1") == 0) ? 1U : 0U);
    if (ProbeAdc_SetTrigger(pin, low, high, en)) {
        send_ok("ADC_TRIGGER", "configured");
    } else {
        send_err("ADC_TRIGGER", "invalid args");
    }
}

static void handle_emu_pl5500(char *args)
{
    char *pin = strtok(args, " ");
    char *mode = strtok(NULL, " ");
    if (pin == NULL || mode == NULL) {
        send_err("EMU_PL5500_STAT", "need pin mode(OFF|TRICKLE|FAST|DONE)");
        return;
    }
    EmuIcResult rc = EmuIc_Pl5500StatSet(pin, mode);
    if (rc == EMU_IC_OK) {
        send_ok("EMU_PL5500_STAT", "configured");
    } else {
        send_err("EMU_PL5500_STAT", "invalid pin or mode");
    }
}

static void handle_emu_mp3398_bind(char *args)
{
    char *en = strtok(args, " ");
    char *pwm = strtok(NULL, " ");
    char *adim = strtok(NULL, " ");
    if (en == NULL || pwm == NULL || adim == NULL) {
        send_err("EMU_MP3398_BIND", "need en_pin pwm_pin adim_pin");
        return;
    }
    EmuIcResult rc = EmuIc_Mp3398Bind(en, pwm, adim);
    if (rc == EMU_IC_OK) {
        send_ok("EMU_MP3398_BIND", "bound");
    } else {
        send_err("EMU_MP3398_BIND", "invalid pin");
    }
}

static void handle_emu_mp3398_wave(char *cmd, char *args)
{
    char *freq_s = strtok(args, " ");
    char *duty_s = strtok(NULL, " ");
    if (freq_s == NULL || duty_s == NULL) {
        send_err(cmd, "need freq_hz duty_pct");
        return;
    }
    uint32_t freq = (uint32_t)strtoul(freq_s, NULL, 0);
    uint8_t duty = (uint8_t)strtoul(duty_s, NULL, 0);
    EmuIcResult rc = (strcmp(cmd, "EMU_MP3398_PWM") == 0)
        ? EmuIc_Mp3398ConfigPwm(freq, duty)
        : EmuIc_Mp3398ConfigAdim(freq, duty);
    if (rc == EMU_IC_OK) {
        send_ok(cmd, "configured");
    } else {
        send_err(cmd, "invalid freq or duty");
    }
}

static void handle_emu_mp3398_enable(char *args)
{
    char *en_s = strtok(args, " ");
    if (en_s == NULL) {
        send_err("EMU_MP3398_ENABLE", "need 0 or 1");
        return;
    }
    uint8_t en = (uint8_t)(strtoul(en_s, NULL, 0) ? 1U : 0U);
    EmuIcResult rc = EmuIc_Mp3398Enable(en);
    if (rc == EMU_IC_OK) {
        send_ok("EMU_MP3398_ENABLE", en ? "on" : "off");
    } else {
        send_err("EMU_MP3398_ENABLE", "need bind first");
    }
}

static void handle_line(char *line)
{
    char *cmd = strtok(line, " ");
    char *args = strtok(NULL, "");

    if (cmd == NULL) {
        return;
    }
    ProbeStats_IncCmd();

    if (strcmp(cmd, "PING") == 0) {
        send_line_checked("{\"type\":\"pong\",\"implemented\":1}");
    } else if (strcmp(cmd, "GET_STATUS") == 0) {
        char snap[512];
        ProbeEngine_BuildSnapshot(snap, sizeof(snap));
        send_line_checked(snap);
    } else if (strcmp(cmd, "SELF_CHECK") == 0) {
        send_self_check();
    } else if (strcmp(cmd, "GET_CAPS") == 0) {
        send_caps();
    } else if (strcmp(cmd, "GET_STATS") == 0) {
        send_stats();
    } else if (strcmp(cmd, "RESET_STATS") == 0) {
        ProbeStats_Reset();
        ProbeEngine_ResetStats();
        CDC_ResetTxStats();
        EmuBus_ResetStats();
        BypassFsm_ResetStats();
        OledSniffer_Reset();
        send_ok("RESET_STATS", "reset");
    } else if (strcmp(cmd, "SUBSCRIBE") == 0) {
        if (args != NULL && (strcmp(args, "ON") == 0 || strcmp(args, "1") == 0)) {
            g_subscribe = 1U;
            send_ok("SUBSCRIBE", "on");
        } else {
            g_subscribe = 0U;
            send_ok("SUBSCRIBE", "off");
        }
    } else if (strcmp(cmd, "GPIO_SET") == 0) {
        if (args == NULL) {
            send_err("GPIO_SET", "need args");
        } else {
            handle_gpio_set(args);
        }
    } else if (strcmp(cmd, "GPIO_RELEASE") == 0) {
        if (args == NULL) {
            send_err("GPIO_RELEASE", "need args");
        } else {
            handle_gpio_release(args);
        }
    } else if (strcmp(cmd, "GPIO_PULSE") == 0) {
        if (args == NULL) {
            send_err("GPIO_PULSE", "need args");
        } else {
            handle_gpio_pulse(args);
        }
    } else if (strcmp(cmd, "PIN_CFG") == 0) {
        if (args == NULL) {
            send_err("PIN_CFG", "need args");
        } else {
            handle_pin_cfg(args);
        }
    } else if (strcmp(cmd, "SEQ_PLAY") == 0) {
        if (args == NULL) {
            send_err("SEQ_PLAY", "need script");
        } else {
            handle_seq_play(args);
        }
    } else if (strcmp(cmd, "SEQ_ABORT") == 0) {
        (void)InjectEngine_AbortScript();
        send_ok("SEQ_ABORT", "aborted");
    } else if (strcmp(cmd, "SCENE_LIST") == 0) {
        char out[160];
        (void)snprintf(out, sizeof(out), "{\"type\":\"scenes\",\"list\":\"%s\"}", InjectEngine_ListScenes());
        send_line_checked(out);
    } else if (strcmp(cmd, "SCENE_RUN") == 0) {
        if (args == NULL) {
            send_err("SCENE_RUN", "need scene");
        } else {
            handle_scene_run(args);
        }
    } else if (strcmp(cmd, "SCENE_STOP") == 0) {
        (void)InjectEngine_AbortScript();
        send_ok("SCENE_STOP", "stopped");
    } else if (strcmp(cmd, "ADC_READ") == 0) {
        if (args == NULL) {
            send_err("ADC_READ", "need pin");
        } else {
            handle_adc_read(args);
        }
    } else if (strcmp(cmd, "ADC_STREAM_START") == 0 || strcmp(cmd, "ADC_STREAM_STOP") == 0) {
        handle_adc_stream(cmd, args);
    } else if (strcmp(cmd, "ADC_TRIGGER") == 0) {
        if (args == NULL) {
            send_err("ADC_TRIGGER", "need args");
        } else {
            handle_adc_trigger(args);
        }
    } else if (strcmp(cmd, "EMU_PL5500_STAT") == 0) {
        if (args == NULL) {
            send_err("EMU_PL5500_STAT", "need args");
        } else {
            handle_emu_pl5500(args);
        }
    } else if (strcmp(cmd, "EMU_MP3398_BIND") == 0) {
        if (args == NULL) {
            send_err("EMU_MP3398_BIND", "need args");
        } else {
            handle_emu_mp3398_bind(args);
        }
    } else if (strcmp(cmd, "EMU_MP3398_PWM") == 0 || strcmp(cmd, "EMU_MP3398_ADIM") == 0) {
        if (args == NULL) {
            send_err(cmd, "need args");
        } else {
            handle_emu_mp3398_wave(cmd, args);
        }
    } else if (strcmp(cmd, "EMU_MP3398_ENABLE") == 0) {
        if (args == NULL) {
            send_err("EMU_MP3398_ENABLE", "need args");
        } else {
            handle_emu_mp3398_enable(args);
        }
    } else if (strcmp(cmd, "LIST_PINS") == 0) {
        send_pin_list();
    } else if (strcmp(cmd, "HELP") == 0) {
        char out[980];
        (void)snprintf(out, sizeof(out), "{\"type\":\"help\",\"cmds\":\"PING GET_STATUS SELF_CHECK GET_CAPS GET_STATS RESET_STATS SUBSCRIBE GPIO_SET GPIO_RELEASE GPIO_PULSE PIN_CFG SEQ_PLAY SEQ_ABORT SCENE_LIST SCENE_RUN SCENE_STOP ADC_READ ADC_STREAM_START ADC_STREAM_STOP ADC_TRIGGER EMU_PL5500_STAT EMU_MP3398_BIND EMU_MP3398_PWM EMU_MP3398_ADIM EMU_MP3398_ENABLE I2C_SLAVE_EMU SPI_SLAVE_EMU BYPASS_FSM OLED_SNIFF_START OLED_SNIFF_STOP OLED_SNIFF_STATUS OLED_SNIFF_DUMP LIST_PINS\",\"emu_ic\":\"%s\",\"emu_i2c\":\"%s\",\"emu_spi\":\"%s\",\"bypass\":\"BYPASS_FSM ON|OFF|STATUS|RESET\",\"oled\":\"OLED_SNIFF_START|STOP|STATUS|DUMP edge|txn|bytes [limit]\"}", EmuIc_HelpText(), EmuBus_I2cHelp(), EmuBus_SpiHelp());
        send_line_checked(out);
    } else if (strcmp(cmd, "I2C_SLAVE_EMU") == 0) {
        if (args == NULL) {
            send_err("I2C_SLAVE_EMU", "need args");
        } else {
            handle_i2c_emu(args);
        }
    } else if (strcmp(cmd, "SPI_SLAVE_EMU") == 0) {
        if (args == NULL) {
            send_err("SPI_SLAVE_EMU", "need args");
        } else {
            handle_spi_emu(args);
        }
    } else if (strcmp(cmd, "BYPASS_FSM") == 0) {
        if (args == NULL) {
            send_err("BYPASS_FSM", "need args");
        } else {
            handle_bypass_fsm(args);
        }
    } else if (strcmp(cmd, "OLED_SNIFF_START") == 0) {
        OledSniffer_SetEnabled(1U);
        send_ok("OLED_SNIFF_START", "started");
    } else if (strcmp(cmd, "OLED_SNIFF_STOP") == 0) {
        OledSniffer_SetEnabled(0U);
        send_ok("OLED_SNIFF_STOP", "stopped");
    } else if (strcmp(cmd, "OLED_SNIFF_STATUS") == 0) {
        OledSnifferStats stats = {0};
        char out[240];
        OledSniffer_GetStats(&stats);
        (void)snprintf(out,
                       sizeof(out),
                       "{\"type\":\"oled_status\",\"enabled\":%u,\"bus_type\":\"%s\",\"edge_count\":%lu,\"txn_count\":%lu,\"byte_count\":%lu,\"drop_count\":%lu}",
                       (unsigned)(OledSniffer_IsEnabled() ? 1U : 0U),
                       OledSniffer_GetBusType(),
                       (unsigned long)stats.edge_count,
                       (unsigned long)stats.txn_count,
                       (unsigned long)stats.byte_count,
                       (unsigned long)stats.drop_count);
        send_line_checked(out);
    } else if (strcmp(cmd, "OLED_SNIFF_DUMP") == 0) {
        if (args == NULL) {
            send_err("OLED_SNIFF_DUMP", "need args");
        } else {
            handle_oled_dump(args);
        }
    } else {
        send_err(cmd, "unknown command");
    }
}

void BridgeProtocol_Init(void)
{
    g_line_len = 0;
    g_subscribe = 0;
    g_last_snapshot_ms = 0;
}

void BridgeProtocol_Task(void)
{
    uint8_t ch = 0;
    while (CDC_ReadByte(&ch)) {
        if (ch == '\r' || ch == '\n') {
            if (g_line_len > 0U) {
                g_line_buf[g_line_len] = '\0';
                handle_line(g_line_buf);
                g_line_len = 0U;
            }
            continue;
        }

        if (g_line_len < (LINE_BUF_SIZE - 1U)) {
            g_line_buf[g_line_len++] = (char)ch;
        } else {
            g_line_len = 0U;
            send_err("LINE", "line too long");
        }
    }

    if (g_subscribe != 0U) {
        ProbeEvent ev;
        if (ProbeEngine_PopEvent(&ev)) {
            char out[220];
            size_t count = 0;
            const ProbePinDef *table = ProbePins_GetTable(&count);
            const char *name = (ev.pin_index < count) ? table[ev.pin_index].name : "UNK";
            (void)snprintf(
                out,
                sizeof(out),
                "{\"type\":\"event\",\"t_ms\":%lu,\"t_us\":%lu,\"pin\":\"%s\",\"v\":%u,\"src\":%u}",
                (unsigned long)ev.tick_ms,
                (unsigned long)ev.tick_us,
                name,
                (unsigned)ev.value,
                (unsigned)ev.source
            );
            send_line_checked(out);
        }

        ProbeAdcEvent adc_ev;
        if (ProbeAdc_PopEvent(&adc_ev)) {
            char out[220];
            (void)snprintf(
                out,
                sizeof(out),
                "{\"type\":\"adc_event\",\"t_ms\":%lu,\"t_us\":%lu,\"pin\":\"%s\",\"value\":%u,\"trigger\":%u}",
                (unsigned long)adc_ev.tick_ms,
                (unsigned long)adc_ev.tick_us,
                adc_ev.pin_name,
                (unsigned)adc_ev.value,
                (unsigned)adc_ev.is_trigger
            );
            send_line_checked(out);
        }

        uint32_t now = HAL_GetTick();
        if ((now - g_last_snapshot_ms) >= 500U) {
            char snap[512];
            ProbeEngine_BuildSnapshot(snap, sizeof(snap));
            send_line_checked(snap);
            g_last_snapshot_ms = now;
        }
    }

    stats_sync();
    CDC_Task();
}
