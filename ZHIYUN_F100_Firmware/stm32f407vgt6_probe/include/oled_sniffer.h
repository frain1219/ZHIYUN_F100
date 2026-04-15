#ifndef __OLED_SNIFFER_H
#define __OLED_SNIFFER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    OLED_LINE_SCK = 0,
    OLED_LINE_MOSI = 1,
    OLED_LINE_CS = 2,
    OLED_LINE_RES = 3,
    OLED_LINE_DC = 4
} OledLine;

typedef struct {
    uint32_t edge_count;
    uint32_t txn_count;
    uint32_t byte_count;
    uint32_t drop_count;
} OledSnifferStats;

void OledSniffer_Init(void);
void OledSniffer_Reset(void);
void OledSniffer_SetEnabled(uint8_t enabled);
bool OledSniffer_IsEnabled(void);
void OledSniffer_RecordEdge(OledLine line, uint8_t level, uint32_t tick_us);
void OledSniffer_GetStats(OledSnifferStats *out);
const char *OledSniffer_GetBusType(void);
bool OledSniffer_Dump(const char *args, char *out, uint16_t out_size);

#endif
