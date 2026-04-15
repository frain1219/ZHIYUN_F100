#include "oled_sniffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EDGE_DEPTH 512U
#define TXN_DEPTH 128U
#define BYTE_DEPTH 1024U

typedef struct {
    uint32_t t_us;
    uint8_t line;
    uint8_t level;
} EdgeEntry;

typedef struct {
    uint32_t t_start_us;
    uint32_t t_end_us;
    uint16_t byte_len;
} TxnEntry;

typedef struct {
    uint8_t enabled;
    const char *bus_type;
    OledSnifferStats stats;

    EdgeEntry edges[EDGE_DEPTH];
    uint16_t edge_head;
    uint16_t edge_count;

    TxnEntry txns[TXN_DEPTH];
    uint16_t txn_head;
    uint16_t txn_count;

    uint8_t bytes[BYTE_DEPTH];
    uint16_t byte_head;
    uint16_t byte_count;

    uint8_t spi_active;
    uint32_t spi_start_us;
    uint32_t spi_bytes_total_start;
    uint8_t last_mosi;
    uint8_t shift;
    uint8_t bit_count;
    uint8_t last_cs;
} SnifferState;

static SnifferState g;

static uint16_t ring_index(uint16_t head, uint16_t count, uint16_t depth, uint16_t i)
{
    uint16_t base = (uint16_t)((head + depth - count) % depth);
    return (uint16_t)((base + i) % depth);
}

static void push_edge(uint32_t t_us, uint8_t line, uint8_t level)
{
    if (g.edge_count >= EDGE_DEPTH) {
        g.stats.drop_count++;
        g.edge_head = (uint16_t)((g.edge_head + 1U) % EDGE_DEPTH);
        g.edge_count = (uint16_t)(EDGE_DEPTH - 1U);
    }
    uint16_t idx = (uint16_t)((g.edge_head + g.edge_count) % EDGE_DEPTH);
    g.edges[idx].t_us = t_us;
    g.edges[idx].line = line;
    g.edges[idx].level = level;
    g.edge_count++;
    g.stats.edge_count++;
}

static void push_byte(uint8_t b)
{
    if (g.byte_count >= BYTE_DEPTH) {
        g.stats.drop_count++;
        g.byte_head = (uint16_t)((g.byte_head + 1U) % BYTE_DEPTH);
        g.byte_count = (uint16_t)(BYTE_DEPTH - 1U);
    }
    uint16_t idx = (uint16_t)((g.byte_head + g.byte_count) % BYTE_DEPTH);
    g.bytes[idx] = b;
    g.byte_count++;
    g.stats.byte_count++;
}

static void push_txn(uint32_t t_start_us, uint32_t t_end_us, uint16_t len)
{
    if (g.txn_count >= TXN_DEPTH) {
        g.stats.drop_count++;
        g.txn_head = (uint16_t)((g.txn_head + 1U) % TXN_DEPTH);
        g.txn_count = (uint16_t)(TXN_DEPTH - 1U);
    }
    uint16_t idx = (uint16_t)((g.txn_head + g.txn_count) % TXN_DEPTH);
    g.txns[idx].t_start_us = t_start_us;
    g.txns[idx].t_end_us = t_end_us;
    g.txns[idx].byte_len = len;
    g.txn_count++;
    g.stats.txn_count++;
}

void OledSniffer_Init(void)
{
    memset(&g, 0, sizeof(g));
    g.bus_type = "unknown";
    g.last_cs = 1U;
}

void OledSniffer_Reset(void)
{
    uint8_t enabled = g.enabled;
    OledSniffer_Init();
    g.enabled = enabled;
}

void OledSniffer_SetEnabled(uint8_t enabled)
{
    g.enabled = enabled ? 1U : 0U;
    g.spi_active = 0U;
    g.shift = 0U;
    g.bit_count = 0U;
    g.last_cs = 1U;
}

bool OledSniffer_IsEnabled(void)
{
    return g.enabled != 0U;
}

void OledSniffer_RecordEdge(OledLine line, uint8_t level, uint32_t tick_us)
{
    if (g.enabled == 0U) {
        return;
    }

    push_edge(tick_us, (uint8_t)line, level ? 1U : 0U);

    if (line == OLED_LINE_MOSI) {
        g.last_mosi = level ? 1U : 0U;
        return;
    }

    if (line == OLED_LINE_CS) {
        uint8_t cs = level ? 1U : 0U;
        if (g.last_cs == 1U && cs == 0U) {
            g.spi_active = 1U;
            g.spi_start_us = tick_us;
            g.spi_bytes_total_start = g.stats.byte_count;
            g.shift = 0U;
            g.bit_count = 0U;
            g.bus_type = "spi";
        } else if (g.last_cs == 0U && cs == 1U && g.spi_active != 0U) {
            uint16_t len = (uint16_t)(g.stats.byte_count - g.spi_bytes_total_start);
            push_txn(g.spi_start_us, tick_us, len);
            g.spi_active = 0U;
        }
        g.last_cs = cs;
        return;
    }

    if (line == OLED_LINE_SCK && g.spi_active != 0U && level != 0U) {
        g.shift = (uint8_t)((g.shift << 1) | (g.last_mosi & 0x01U));
        g.bit_count++;
        if (g.bit_count >= 8U) {
            push_byte(g.shift);
            g.shift = 0U;
            g.bit_count = 0U;
        }
    }
}

void OledSniffer_GetStats(OledSnifferStats *out)
{
    if (out != NULL) {
        *out = g.stats;
    }
}

const char *OledSniffer_GetBusType(void)
{
    return g.bus_type;
}

static bool dump_edges(uint16_t limit, char *out, uint16_t out_size)
{
    uint16_t n = (limit < g.edge_count) ? limit : g.edge_count;
    uint16_t used = 0U;
    used += (uint16_t)snprintf(out + used, out_size - used, "{\"type\":\"oled_dump\",\"kind\":\"edge\",\"n\":%u,\"items\":[", (unsigned)n);
    for (uint16_t i = 0; i < n; ++i) {
        uint16_t idx = ring_index(g.edge_head, g.edge_count, EDGE_DEPTH, (uint16_t)(g.edge_count - n + i));
        const EdgeEntry *e = &g.edges[idx];
        used += (uint16_t)snprintf(out + used,
                                   out_size - used,
                                   "%s[%lu,%u,%u]",
                                   (i == 0U) ? "" : ",",
                                   (unsigned long)e->t_us,
                                   (unsigned)e->line,
                                   (unsigned)e->level);
        if (used >= out_size) {
            return false;
        }
    }
    (void)snprintf(out + used, out_size - used, "]}");
    return true;
}

static bool dump_txns(uint16_t limit, char *out, uint16_t out_size)
{
    uint16_t n = (limit < g.txn_count) ? limit : g.txn_count;
    uint16_t used = 0U;
    used += (uint16_t)snprintf(out + used, out_size - used, "{\"type\":\"oled_dump\",\"kind\":\"txn\",\"n\":%u,\"items\":[", (unsigned)n);
    for (uint16_t i = 0; i < n; ++i) {
        uint16_t idx = ring_index(g.txn_head, g.txn_count, TXN_DEPTH, (uint16_t)(g.txn_count - n + i));
        const TxnEntry *t = &g.txns[idx];
        used += (uint16_t)snprintf(out + used,
                                   out_size - used,
                                   "%s[%lu,%lu,%u]",
                                   (i == 0U) ? "" : ",",
                                   (unsigned long)t->t_start_us,
                                   (unsigned long)t->t_end_us,
                                   (unsigned)t->byte_len);
        if (used >= out_size) {
            return false;
        }
    }
    (void)snprintf(out + used, out_size - used, "]}");
    return true;
}

static bool dump_bytes(uint16_t limit, char *out, uint16_t out_size)
{
    uint16_t n = (limit < g.byte_count) ? limit : g.byte_count;
    uint16_t used = 0U;
    used += (uint16_t)snprintf(out + used, out_size - used, "{\"type\":\"oled_dump\",\"kind\":\"bytes\",\"n\":%u,\"items\":[", (unsigned)n);
    for (uint16_t i = 0; i < n; ++i) {
        uint16_t idx = ring_index(g.byte_head, g.byte_count, BYTE_DEPTH, (uint16_t)(g.byte_count - n + i));
        used += (uint16_t)snprintf(out + used, out_size - used, "%s%u", (i == 0U) ? "" : ",", (unsigned)g.bytes[idx]);
        if (used >= out_size) {
            return false;
        }
    }
    (void)snprintf(out + used, out_size - used, "]}");
    return true;
}

bool OledSniffer_Dump(const char *args, char *out, uint16_t out_size)
{
    if (args == NULL || out == NULL || out_size == 0U) {
        return false;
    }

    char local[64];
    size_t len = strlen(args);
    if (len >= sizeof(local)) {
        return false;
    }
    memcpy(local, args, len + 1U);

    char *save = NULL;
    char *kind = strtok_r(local, " ", &save);
    char *limit_s = strtok_r(NULL, " ", &save);
    uint16_t limit = 32U;
    if (limit_s != NULL) {
        unsigned long v = strtoul(limit_s, NULL, 0);
        if (v > 0UL && v < 1024UL) {
            limit = (uint16_t)v;
        }
    }

    if (kind == NULL) {
        return false;
    }
    if (strcmp(kind, "edge") == 0) {
        return dump_edges(limit, out, out_size);
    }
    if (strcmp(kind, "txn") == 0) {
        return dump_txns(limit, out, out_size);
    }
    if (strcmp(kind, "bytes") == 0) {
        return dump_bytes(limit, out, out_size);
    }
    return false;
}
