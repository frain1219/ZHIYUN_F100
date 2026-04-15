#ifndef __EMU_BUS_H
#define __EMU_BUS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t i2c_req_count;
    uint32_t i2c_hit_count;
    uint32_t i2c_timeout_count;
    uint32_t i2c_drop_count;
    uint32_t spi_req_count;
    uint32_t spi_hit_count;
    uint32_t spi_timeout_count;
    uint32_t spi_drop_count;
} EmuBusStats;

void EmuBus_Init(void);
void EmuBus_Tick(uint32_t now_ms);
void EmuBus_ResetStats(void);
void EmuBus_GetStats(EmuBusStats *out);

bool EmuBus_HandleI2cCommand(const char *args, char *out, uint16_t out_size);
bool EmuBus_HandleSpiCommand(const char *args, char *out, uint16_t out_size);

const char *EmuBus_I2cHelp(void);
const char *EmuBus_SpiHelp(void);

#endif
