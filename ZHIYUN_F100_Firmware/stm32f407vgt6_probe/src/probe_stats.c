#include "probe_stats.h"

#include <string.h>

static ProbeStats g_stats;

void ProbeStats_Reset(void)
{
    memset(&g_stats, 0, sizeof(g_stats));
}

void ProbeStats_IncCmd(void)
{
    g_stats.cmd_count++;
}

void ProbeStats_IncTxDrop(void)
{
    g_stats.tx_drop_count++;
}

void ProbeStats_SetEvent(uint32_t drop_count, uint32_t depth_max, uint32_t poll_count, uint32_t exti_count)
{
    g_stats.event_drop_count = drop_count;
    if (depth_max > g_stats.event_depth_max) {
        g_stats.event_depth_max = depth_max;
    }
    g_stats.poll_event_count = poll_count;
    g_stats.exti_event_count = exti_count;
}

void ProbeStats_SetAdc(uint32_t sample_count, uint32_t trigger_count)
{
    g_stats.adc_sample_count = sample_count;
    g_stats.adc_trigger_count = trigger_count;
}

void ProbeStats_SetEmu(
    uint32_t i2c_req,
    uint32_t i2c_hit,
    uint32_t i2c_timeout,
    uint32_t i2c_drop,
    uint32_t spi_req,
    uint32_t spi_hit,
    uint32_t spi_timeout,
    uint32_t spi_drop
)
{
    g_stats.emu_i2c_req_count = i2c_req;
    g_stats.emu_i2c_hit_count = i2c_hit;
    g_stats.emu_i2c_timeout_count = i2c_timeout;
    g_stats.emu_i2c_drop_count = i2c_drop;
    g_stats.emu_spi_req_count = spi_req;
    g_stats.emu_spi_hit_count = spi_hit;
    g_stats.emu_spi_timeout_count = spi_timeout;
    g_stats.emu_spi_drop_count = spi_drop;
}

void ProbeStats_SetBypass(
    uint32_t trigger_count,
    uint32_t success_count,
    uint32_t timeout_count,
    uint32_t active_cycles
)
{
    g_stats.bypass_trigger_count = trigger_count;
    g_stats.bypass_success_count = success_count;
    g_stats.bypass_timeout_count = timeout_count;
    g_stats.bypass_active_cycles = active_cycles;
}

void ProbeStats_SetOled(
    uint32_t edge_count,
    uint32_t txn_count,
    uint32_t byte_count,
    uint32_t drop_count
)
{
    g_stats.oled_edge_count = edge_count;
    g_stats.oled_txn_count = txn_count;
    g_stats.oled_byte_count = byte_count;
    g_stats.oled_drop_count = drop_count;
}

void ProbeStats_Get(ProbeStats *out)
{
    if (out != NULL) {
        *out = g_stats;
    }
}
