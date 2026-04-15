#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#include <stdbool.h>
#include <stdint.h>
#include "usbd_cdc.h"

extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

bool CDC_ReadByte(uint8_t *out);
bool CDC_TransmitLine(const char *line);
bool CDC_TransmitRaw(const uint8_t *data, uint16_t len);
void CDC_Task(void);
uint16_t CDC_GetTxQueueDepth(void);
uint16_t CDC_GetTxQueueDepthMax(void);
uint32_t CDC_GetTxQueueDropCount(void);
void CDC_ResetTxStats(void);

#endif
