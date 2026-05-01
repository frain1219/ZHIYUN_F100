#include "bypass_fsm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inject_engine.h"
#include "probe_pins.h"

typedef enum {
    FSM_IDLE = 0,
    FSM_DRIVE_HIGH,
    FSM_DRIVE_LOW
} FsmState;

static uint8_t g_enabled = 0U;
static FsmState g_state = FSM_IDLE;
static uint32_t g_state_deadline_ms = 0U;
static uint32_t g_last_trigger_ms = 0U;
static uint8_t g_last_pc13 = 0U;
static uint8_t g_last_pb3 = 0U;
static uint8_t g_observed_transition = 0U;
static BypassFsmStats g_stats;

static const ProbePinDef *g_pin_pc13 = NULL; /* PE0 */
static const ProbePinDef *g_pin_pb3 = NULL;  /* PE2 */

static void reset_runtime(void)
{
    g_state = FSM_IDLE;
    g_state_deadline_ms = 0U;
    g_observed_transition = 0U;
}

void BypassFsm_Init(void)
{
    g_pin_pc13 = ProbePins_FindByName("PE0");
    g_pin_pb3 = ProbePins_FindByName("PE2");
    g_enabled = 0U;
    g_last_trigger_ms = 0U;
    g_last_pc13 = 0U;
    g_last_pb3 = 0U;
    memset(&g_stats, 0, sizeof(g_stats));
    reset_runtime();
}

void BypassFsm_ResetStats(void)
{
    memset(&g_stats, 0, sizeof(g_stats));
}

void BypassFsm_GetStats(BypassFsmStats *out)
{
    if (out != NULL) {
        *out = g_stats;
    }
}

bool BypassFsm_IsEnabled(void)
{
    return g_enabled != 0U;
}

static uint8_t read_pin(const ProbePinDef *pin)
{
    if (pin == NULL) {
        return 0U;
    }
    return (uint8_t)((ProbePins_Read(pin) == GPIO_PIN_SET) ? 1U : 0U);
}

void BypassFsm_Tick(uint32_t now_ms)
{
    uint8_t pc13 = read_pin(g_pin_pc13);
    uint8_t pb3 = read_pin(g_pin_pb3);

    if (g_enabled == 0U) {
        g_last_pc13 = pc13;
        g_last_pb3 = pb3;
        return;
    }

    g_stats.active_cycles++;

    if (pc13 != g_last_pc13 || pb3 != g_last_pb3) {
        g_observed_transition = 1U;
    }
    g_last_pc13 = pc13;
    g_last_pb3 = pb3;

    switch (g_state) {
    case FSM_IDLE:
        /*
         * Heuristic trigger:
         * - PB3 remains low (likely attach path not established)
         * - minimum interval to avoid overdriving shared line
         */
        if (pb3 == 0U && (now_ms - g_last_trigger_ms) >= 120U) {
            if (InjectEngine_Set("PE1", 1U, 0U) == INJECT_OK) {
                g_stats.trigger_count++;
                g_state = FSM_DRIVE_HIGH;
                g_state_deadline_ms = now_ms + 8U;
                g_last_trigger_ms = now_ms;
                g_observed_transition = 0U;
            }
        }
        break;
    case FSM_DRIVE_HIGH:
        if ((int32_t)(now_ms - g_state_deadline_ms) >= 0) {
            if (InjectEngine_Set("PE1", 0U, 0U) == INJECT_OK) {
                g_state = FSM_DRIVE_LOW;
                g_state_deadline_ms = now_ms + 8U;
            } else {
                reset_runtime();
                g_stats.timeout_count++;
            }
        }
        break;
    case FSM_DRIVE_LOW:
        if ((int32_t)(now_ms - g_state_deadline_ms) >= 0) {
            (void)InjectEngine_Release("PE1");
            if (g_observed_transition != 0U) {
                g_stats.success_count++;
            } else {
                g_stats.timeout_count++;
            }
            reset_runtime();
        }
        break;
    default:
        reset_runtime();
        break;
    }
}

bool BypassFsm_HandleCommand(const char *args, char *out, uint16_t out_size)
{
    if (args == NULL || out == NULL || out_size == 0U) {
        return false;
    }

    char local[80];
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

    if (strcmp(sub, "ON") == 0) {
        g_enabled = 1U;
        reset_runtime();
        g_last_trigger_ms = 0U;
        (void)snprintf(out, out_size, "{\"type\":\"bypass_fsm\",\"implemented\":1,\"enabled\":1}");
        return true;
    }

    if (strcmp(sub, "OFF") == 0) {
        g_enabled = 0U;
        (void)InjectEngine_Release("PB8");
        reset_runtime();
        (void)snprintf(out, out_size, "{\"type\":\"bypass_fsm\",\"implemented\":1,\"enabled\":0}");
        return true;
    }

    if (strcmp(sub, "RESET") == 0) {
        BypassFsm_ResetStats();
        (void)snprintf(out, out_size, "{\"type\":\"bypass_fsm\",\"implemented\":1,\"reset\":1}");
        return true;
    }

    if (strcmp(sub, "STATUS") == 0) {
        (void)snprintf(out,
                       out_size,
                       "{\"type\":\"bypass_fsm\",\"implemented\":1,\"enabled\":%u,\"state\":%u,"
                       "\"trigger_count\":%lu,\"success_count\":%lu,\"timeout_count\":%lu,"
                       "\"active_cycles\":%lu}",
                       (unsigned)g_enabled,
                       (unsigned)g_state,
                       (unsigned long)g_stats.trigger_count,
                       (unsigned long)g_stats.success_count,
                       (unsigned long)g_stats.timeout_count,
                       (unsigned long)g_stats.active_cycles);
        return true;
    }

    return false;
}
