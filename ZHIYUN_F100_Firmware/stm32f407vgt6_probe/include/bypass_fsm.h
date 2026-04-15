#ifndef __BYPASS_FSM_H
#define __BYPASS_FSM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t trigger_count;
    uint32_t success_count;
    uint32_t timeout_count;
    uint32_t active_cycles;
} BypassFsmStats;

void BypassFsm_Init(void);
void BypassFsm_Tick(uint32_t now_ms);
void BypassFsm_ResetStats(void);
void BypassFsm_GetStats(BypassFsmStats *out);
bool BypassFsm_IsEnabled(void);
bool BypassFsm_HandleCommand(const char *args, char *out, uint16_t out_size);

#endif
