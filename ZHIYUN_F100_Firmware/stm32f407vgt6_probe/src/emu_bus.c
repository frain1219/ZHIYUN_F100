#include "emu_bus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define I2C_TRACE_DEPTH 32U
#define SPI_TRACE_DEPTH 32U
#define SPI_SCRIPT_MAX 64U

typedef struct {
    uint32_t t_ms;
    uint8_t op; /* 0=write 1=read */
    uint8_t reg;
    uint8_t tx;
    uint8_t rx;
} I2cTrace;

typedef struct {
    uint32_t t_ms;
    uint8_t len;
    uint8_t tx0;
    uint8_t rx0;
} SpiTrace;

typedef struct {
    uint8_t enabled;
    uint8_t addr;
    uint8_t regs[256];
    I2cTrace traces[I2C_TRACE_DEPTH];
    uint8_t head;
    uint8_t count;
} I2cState;

typedef struct {
    uint8_t enabled;
    uint8_t mode;
    char cs_pin[12];
    uint8_t script[SPI_SCRIPT_MAX];
    uint8_t script_len;
    uint8_t script_pos;
    SpiTrace traces[SPI_TRACE_DEPTH];
    uint8_t head;
    uint8_t count;
} SpiState;

static I2cState g_i2c;
static SpiState g_spi;
static EmuBusStats g_stats;
static uint32_t g_now_ms = 0U;

static void trace_i2c(uint8_t op, uint8_t reg, uint8_t tx, uint8_t rx)
{
    if (g_i2c.count >= I2C_TRACE_DEPTH) {
        g_stats.i2c_drop_count++;
        g_i2c.head = (uint8_t)((g_i2c.head + 1U) % I2C_TRACE_DEPTH);
        g_i2c.count = (uint8_t)(I2C_TRACE_DEPTH - 1U);
    }
    uint8_t idx = (uint8_t)((g_i2c.head + g_i2c.count) % I2C_TRACE_DEPTH);
    g_i2c.traces[idx].t_ms = g_now_ms;
    g_i2c.traces[idx].op = op;
    g_i2c.traces[idx].reg = reg;
    g_i2c.traces[idx].tx = tx;
    g_i2c.traces[idx].rx = rx;
    g_i2c.count++;
}

static void trace_spi(uint8_t len, uint8_t tx0, uint8_t rx0)
{
    if (g_spi.count >= SPI_TRACE_DEPTH) {
        g_stats.spi_drop_count++;
        g_spi.head = (uint8_t)((g_spi.head + 1U) % SPI_TRACE_DEPTH);
        g_spi.count = (uint8_t)(SPI_TRACE_DEPTH - 1U);
    }
    uint8_t idx = (uint8_t)((g_spi.head + g_spi.count) % SPI_TRACE_DEPTH);
    g_spi.traces[idx].t_ms = g_now_ms;
    g_spi.traces[idx].len = len;
    g_spi.traces[idx].tx0 = tx0;
    g_spi.traces[idx].rx0 = rx0;
    g_spi.count++;
}

static bool parse_u8(const char *s, uint8_t *out)
{
    if (s == NULL || out == NULL) {
        return false;
    }
    char *end = NULL;
    unsigned long v = strtoul(s, &end, 0);
    if (end == s || *end != '\0' || v > 0xFFUL) {
        return false;
    }
    *out = (uint8_t)v;
    return true;
}

static bool parse_spi_script(const char *s)
{
    if (s == NULL || *s == '\0') {
        return false;
    }
    char local[220];
    size_t len = strlen(s);
    if (len >= sizeof(local)) {
        return false;
    }
    memcpy(local, s, len + 1U);

    uint8_t tmp[SPI_SCRIPT_MAX];
    uint8_t n = 0;
    char *save = NULL;
    char *tok = strtok_r(local, ",:|", &save);
    while (tok != NULL) {
        if (n >= SPI_SCRIPT_MAX) {
            return false;
        }
        uint8_t v = 0;
        if (!parse_u8(tok, &v)) {
            return false;
        }
        tmp[n++] = v;
        tok = strtok_r(NULL, ",:|", &save);
    }
    if (n == 0U) {
        return false;
    }
    memcpy(g_spi.script, tmp, n);
    g_spi.script_len = n;
    g_spi.script_pos = 0U;
    return true;
}

void EmuBus_Init(void)
{
    memset(&g_i2c, 0, sizeof(g_i2c));
    memset(&g_spi, 0, sizeof(g_spi));
    memset(&g_stats, 0, sizeof(g_stats));
}

void EmuBus_Tick(uint32_t now_ms)
{
    g_now_ms = now_ms;
}

void EmuBus_ResetStats(void)
{
    memset(&g_stats, 0, sizeof(g_stats));
}

void EmuBus_GetStats(EmuBusStats *out)
{
    if (out != NULL) {
        *out = g_stats;
    }
}

bool EmuBus_HandleI2cCommand(const char *args, char *out, uint16_t out_size)
{
    if (args == NULL || out == NULL || out_size == 0U) {
        return false;
    }

    char local[180];
    size_t len = strlen(args);
    if (len >= sizeof(local)) {
        return false;
    }
    memcpy(local, args, len + 1U);

    char *save = NULL;
    char *sub = strtok_r(local, " ", &save);
    if (sub == NULL) {
        return false;
    }

    if (strcmp(sub, "OFF") == 0) {
        g_i2c.enabled = 0U;
        (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"enabled\":0}");
        return true;
    }

    if (strcmp(sub, "CFG") == 0) {
        char *addr_s = strtok_r(NULL, " ", &save);
        uint8_t addr = 0;
        if (!parse_u8(addr_s, &addr)) {
            return false;
        }
        g_i2c.addr = addr;
        g_i2c.enabled = 1U;
        (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"enabled\":1,\"addr\":%u}", (unsigned)addr);
        return true;
    }

    if (strcmp(sub, "REG_WRITE") == 0) {
        uint8_t reg = 0;
        uint8_t val = 0;
        if (!parse_u8(strtok_r(NULL, " ", &save), &reg) || !parse_u8(strtok_r(NULL, " ", &save), &val)) {
            return false;
        }
        g_i2c.regs[reg] = val;
        (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"reg\":%u,\"value\":%u}", (unsigned)reg, (unsigned)val);
        return true;
    }

    if (strcmp(sub, "REG_READ") == 0) {
        uint8_t reg = 0;
        if (!parse_u8(strtok_r(NULL, " ", &save), &reg)) {
            return false;
        }
        (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"reg\":%u,\"value\":%u}", (unsigned)reg, (unsigned)g_i2c.regs[reg]);
        return true;
    }

    if (strcmp(sub, "XFER") == 0) {
        g_stats.i2c_req_count++;
        char *rw = strtok_r(NULL, " ", &save);
        uint8_t reg = 0;
        if (rw == NULL || !parse_u8(strtok_r(NULL, " ", &save), &reg)) {
            return false;
        }
        if (g_i2c.enabled == 0U) {
            g_stats.i2c_timeout_count++;
            (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"timeout\":1}");
            return true;
        }

        if (strcmp(rw, "w") == 0) {
            uint8_t val = 0;
            if (!parse_u8(strtok_r(NULL, " ", &save), &val)) {
                return false;
            }
            g_i2c.regs[reg] = val;
            g_stats.i2c_hit_count++;
            trace_i2c(0U, reg, val, val);
            (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"op\":\"w\",\"reg\":%u,\"ack\":1}", (unsigned)reg);
            return true;
        }
        if (strcmp(rw, "r") == 0) {
            uint8_t v = g_i2c.regs[reg];
            g_stats.i2c_hit_count++;
            trace_i2c(1U, reg, 0U, v);
            (void)snprintf(out, out_size, "{\"type\":\"i2c_emu\",\"implemented\":1,\"op\":\"r\",\"reg\":%u,\"value\":%u}", (unsigned)reg, (unsigned)v);
            return true;
        }
        return false;
    }

    if (strcmp(sub, "STATUS") == 0) {
        (void)snprintf(out,
                       out_size,
                       "{\"type\":\"i2c_emu\",\"implemented\":1,\"enabled\":%u,\"addr\":%u,\"trace_depth\":%u}",
                       (unsigned)g_i2c.enabled,
                       (unsigned)g_i2c.addr,
                       (unsigned)g_i2c.count);
        return true;
    }

    return false;
}

bool EmuBus_HandleSpiCommand(const char *args, char *out, uint16_t out_size)
{
    if (args == NULL || out == NULL || out_size == 0U) {
        return false;
    }

    char local[180];
    size_t len = strlen(args);
    if (len >= sizeof(local)) {
        return false;
    }
    memcpy(local, args, len + 1U);

    char *save = NULL;
    char *sub = strtok_r(local, " ", &save);
    if (sub == NULL) {
        return false;
    }

    if (strcmp(sub, "OFF") == 0) {
        g_spi.enabled = 0U;
        (void)snprintf(out, out_size, "{\"type\":\"spi_emu\",\"implemented\":1,\"enabled\":0}");
        return true;
    }

    if (strcmp(sub, "CFG") == 0) {
        char *cs = strtok_r(NULL, " ", &save);
        char *mode_s = strtok_r(NULL, " ", &save);
        char *script_s = strtok_r(NULL, "", &save);
        uint8_t mode = 0;
        if (cs == NULL || !parse_u8(mode_s, &mode) || mode > 3U || !parse_spi_script(script_s)) {
            return false;
        }
        (void)snprintf(g_spi.cs_pin, sizeof(g_spi.cs_pin), "%s", cs);
        g_spi.mode = mode;
        g_spi.enabled = 1U;
        (void)snprintf(out,
                       out_size,
                       "{\"type\":\"spi_emu\",\"implemented\":1,\"enabled\":1,\"cs\":\"%s\",\"mode\":%u,\"script_len\":%u}",
                       g_spi.cs_pin,
                       (unsigned)mode,
                       (unsigned)g_spi.script_len);
        return true;
    }

    if (strcmp(sub, "XFER") == 0) {
        g_stats.spi_req_count++;
        char *tx_s = strtok_r(NULL, "", &save);
        if (tx_s == NULL) {
            return false;
        }
        if (g_spi.enabled == 0U || g_spi.script_len == 0U) {
            g_stats.spi_timeout_count++;
            (void)snprintf(out, out_size, "{\"type\":\"spi_emu\",\"implemented\":1,\"timeout\":1}");
            return true;
        }

        char tx_copy[160];
        size_t tlen = strlen(tx_s);
        if (tlen >= sizeof(tx_copy)) {
            return false;
        }
        memcpy(tx_copy, tx_s, tlen + 1U);

        uint8_t tx0 = 0U;
        uint8_t rx0 = 0U;
        uint8_t n = 0U;
        char rx_list[160];
        rx_list[0] = '\0';
        char *tok_save = NULL;
        char *tok = strtok_r(tx_copy, ",:|", &tok_save);
        while (tok != NULL) {
            uint8_t txb = 0;
            if (!parse_u8(tok, &txb)) {
                return false;
            }
            uint8_t rxb = g_spi.script[g_spi.script_pos];
            g_spi.script_pos = (uint8_t)((g_spi.script_pos + 1U) % g_spi.script_len);
            if (n == 0U) {
                tx0 = txb;
                rx0 = rxb;
            }
            char frag[12];
            (void)snprintf(frag, sizeof(frag), "%s%u", (n == 0U) ? "" : ",", (unsigned)rxb);
            if ((strlen(rx_list) + strlen(frag) + 1U) >= sizeof(rx_list)) {
                return false;
            }
            strcat(rx_list, frag);
            n++;
            tok = strtok_r(NULL, ",:|", &tok_save);
        }
        if (n == 0U) {
            return false;
        }
        g_stats.spi_hit_count++;
        trace_spi(n, tx0, rx0);
        (void)snprintf(out, out_size, "{\"type\":\"spi_emu\",\"implemented\":1,\"rx\":[%s],\"n\":%u}", rx_list, (unsigned)n);
        return true;
    }

    if (strcmp(sub, "STATUS") == 0) {
        (void)snprintf(out,
                       out_size,
                       "{\"type\":\"spi_emu\",\"implemented\":1,\"enabled\":%u,\"cs\":\"%s\",\"mode\":%u,\"trace_depth\":%u}",
                       (unsigned)g_spi.enabled,
                       g_spi.cs_pin,
                       (unsigned)g_spi.mode,
                       (unsigned)g_spi.count);
        return true;
    }

    return false;
}

const char *EmuBus_I2cHelp(void)
{
    return "I2C_SLAVE_EMU CFG <addr>; OFF; STATUS; REG_WRITE <reg> <val>; REG_READ <reg>; XFER <r|w> <reg> [val]";
}

const char *EmuBus_SpiHelp(void)
{
    return "SPI_SLAVE_EMU CFG <cs_pin> <mode0-3> <resp_bytes_csv>; OFF; STATUS; XFER <tx_bytes_csv>";
}
