#include "usbd_cdc_if.h"

#include <string.h>
#include "usb_device.h"

#define CDC_RX_BUFFER_SIZE 256U
#define CDC_APP_RX_RING_SIZE 1024U
#define CDC_TX_QUEUE_SIZE 16U
#define CDC_TX_FRAME_MAX 256U

static int8_t CDC_Init(void);
static int8_t CDC_DeInit(void);
static int8_t CDC_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive(uint8_t *buf, uint32_t *len);
static int8_t CDC_TransmitCplt(uint8_t *buf, uint32_t *len, uint8_t epnum);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init,
    CDC_DeInit,
    CDC_Control,
    CDC_Receive,
    CDC_TransmitCplt
};

static uint8_t g_rx_buffer[CDC_RX_BUFFER_SIZE];
static uint8_t g_app_rx_ring[CDC_APP_RX_RING_SIZE];
static volatile uint16_t g_rx_head = 0;
static volatile uint16_t g_rx_tail = 0;

static volatile uint8_t g_tx_busy = 0;
static uint8_t g_tx_buffer[512];
static uint8_t g_tx_queue[CDC_TX_QUEUE_SIZE][CDC_TX_FRAME_MAX];
static uint16_t g_tx_len[CDC_TX_QUEUE_SIZE];
static volatile uint8_t g_tx_head = 0;
static volatile uint8_t g_tx_tail = 0;
static uint16_t g_tx_depth_max = 0;
static uint32_t g_tx_drop_count = 0;

static uint8_t tx_depth(void)
{
    return (uint8_t)((g_tx_head + CDC_TX_QUEUE_SIZE - g_tx_tail) % CDC_TX_QUEUE_SIZE);
}

static bool tx_enqueue(const uint8_t *data, uint16_t len)
{
    uint8_t next = (uint8_t)((g_tx_head + 1U) % CDC_TX_QUEUE_SIZE);
    if (next == g_tx_tail) {
        g_tx_drop_count++;
        return false;
    }

    if (len > CDC_TX_FRAME_MAX) {
        len = CDC_TX_FRAME_MAX;
    }
    memcpy(g_tx_queue[g_tx_head], data, len);
    g_tx_len[g_tx_head] = len;
    g_tx_head = next;

    uint8_t depth = tx_depth();
    if (depth > g_tx_depth_max) {
        g_tx_depth_max = depth;
    }
    return true;
}

static int8_t CDC_Init(void)
{
    g_tx_head = 0;
    g_tx_tail = 0;
    g_tx_busy = 0;
    g_tx_depth_max = 0;
    g_tx_drop_count = 0;
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, g_tx_buffer, 0U);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, g_rx_buffer);
    (void)USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return (int8_t)USBD_OK;
}

static int8_t CDC_DeInit(void)
{
    return (int8_t)USBD_OK;
}

static int8_t CDC_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    UNUSED(length);

    switch (cmd) {
        case CDC_SET_LINE_CODING:
        case CDC_GET_LINE_CODING:
        case CDC_SET_CONTROL_LINE_STATE:
        case CDC_SEND_BREAK:
        default:
            break;
    }
    UNUSED(pbuf);
    return (int8_t)USBD_OK;
}

static int8_t CDC_Receive(uint8_t *buf, uint32_t *len)
{
    for (uint32_t i = 0; i < *len; ++i) {
        uint16_t next = (uint16_t)((g_rx_head + 1U) % CDC_APP_RX_RING_SIZE);
        if (next == g_rx_tail) {
            g_rx_tail = (uint16_t)((g_rx_tail + 1U) % CDC_APP_RX_RING_SIZE);
        }
        g_app_rx_ring[g_rx_head] = buf[i];
        g_rx_head = next;
    }

    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, g_rx_buffer);
    (void)USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return (int8_t)USBD_OK;
}

static int8_t CDC_TransmitCplt(uint8_t *buf, uint32_t *len, uint8_t epnum)
{
    UNUSED(buf);
    UNUSED(len);
    UNUSED(epnum);
    g_tx_busy = 0U;
    return (int8_t)USBD_OK;
}

bool CDC_ReadByte(uint8_t *out)
{
    if (g_rx_head == g_rx_tail) {
        return false;
    }
    *out = g_app_rx_ring[g_rx_tail];
    g_rx_tail = (uint16_t)((g_rx_tail + 1U) % CDC_APP_RX_RING_SIZE);
    return true;
}

bool CDC_TransmitRaw(const uint8_t *data, uint16_t len)
{
    if (len == 0U || data == NULL) {
        return false;
    }
    return tx_enqueue(data, len);
}

bool CDC_TransmitLine(const char *line)
{
    uint8_t frame[CDC_TX_FRAME_MAX];
    if (line == NULL) {
        return false;
    }

    size_t len = strlen(line);
    if (len > sizeof(frame) - 2U) {
        len = sizeof(frame) - 2U;
    }
    memcpy(frame, line, len);
    frame[len++] = '\r';
    frame[len++] = '\n';

    return tx_enqueue(frame, (uint16_t)len);
}

void CDC_Task(void)
{
    if (g_tx_busy != 0U || g_tx_head == g_tx_tail) {
        return;
    }

    uint16_t len = g_tx_len[g_tx_tail];
    memcpy(g_tx_buffer, g_tx_queue[g_tx_tail], len);
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, g_tx_buffer, len);
    if (USBD_CDC_TransmitPacket(&hUsbDeviceFS) == USBD_OK) {
        g_tx_busy = 1U;
        g_tx_tail = (uint8_t)((g_tx_tail + 1U) % CDC_TX_QUEUE_SIZE);
    } else {
        g_tx_drop_count++;
        g_tx_tail = (uint8_t)((g_tx_tail + 1U) % CDC_TX_QUEUE_SIZE);
    }
}

uint16_t CDC_GetTxQueueDepth(void)
{
    return tx_depth();
}

uint16_t CDC_GetTxQueueDepthMax(void)
{
    return g_tx_depth_max;
}

uint32_t CDC_GetTxQueueDropCount(void)
{
    return g_tx_drop_count;
}

void CDC_ResetTxStats(void)
{
    g_tx_depth_max = 0;
    g_tx_drop_count = 0;
}
