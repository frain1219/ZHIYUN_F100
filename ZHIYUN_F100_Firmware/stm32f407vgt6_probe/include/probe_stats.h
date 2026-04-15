#ifndef __PROBE_STATS_H
#define __PROBE_STATS_H

#include <stdint.h>

typedef struct {
    uint32_t cmd_count;
    uint32_t tx_drop_count;
    uint32_t event_drop_count;
    uint32_t event_depth_max;
    uint32_t poll_event_count;
    uint32_t exti_event_count;
    uint32_t adc_sample_count;
    uint32_t adc_trigger_count;
    uint32_t emu_i2c_req_count;
    uint32_t emu_i2c_hit_count;
    uint32_t emu_i2c_timeout_count;
    uint32_t emu_i2c_drop_count;
    uint32_t emu_spi_req_count;
    uint32_t emu_spi_hit_count;
    uint32_t emu_spi_timeout_count;
    uint32_t emu_spi_drop_count;
    uint32_t bypass_trigger_count;
    uint32_t bypass_success_count;
    uint32_t bypass_timeout_count;
    uint32_t bypass_active_cycles;
    uint32_t oled_edge_count;
    uint32_t oled_txn_count;
    uint32_t oled_byte_count;
    uint32_t oled_drop_count;
} ProbeStats;

void ProbeStats_Reset(void);
void ProbeStats_IncCmd(void);
void ProbeStats_IncTxDrop(void);
void ProbeStats_SetEvent(uint32_t drop_count, uint32_t depth_max, uint32_t poll_count, uint32_t exti_count);
void ProbeStats_SetAdc(uint32_t sample_count, uint32_t trigger_count);
void ProbeStats_SetEmu(
    uint32_t i2c_req,
    uint32_t i2c_hit,
    uint32_t i2c_timeout,
    uint32_t i2c_drop,
    uint32_t spi_req,
    uint32_t spi_hit,
    uint32_t spi_timeout,
    uint32_t spi_drop
);
void ProbeStats_SetBypass(
    uint32_t trigger_count,
    uint32_t success_count,
    uint32_t timeout_count,
    uint32_t active_cycles
);
void ProbeStats_SetOled(
    uint32_t edge_count,
    uint32_t txn_count,
    uint32_t byte_count,
    uint32_t drop_count
);
void ProbeStats_Get(ProbeStats *out);

#endif
