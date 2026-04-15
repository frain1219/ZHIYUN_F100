#ifndef PHASE6_USB_DEVICE_H
#define PHASE6_USB_DEVICE_H

typedef struct {
    unsigned int initialized;
    unsigned int connected;
    unsigned int context_ptr_shadow;
    unsigned int ep0_max_packet_shadow;
    unsigned int descriptors_ready;
    unsigned int sof_ticks;
    unsigned int ep0_setup_count;
    unsigned int ep0_in_count;
    unsigned int ep0_out_count;
} Phase6_UsbDeviceTrace;

void Phase6_UsbDevice_Init(void);
void Phase6_UsbDevice_SetContextPointer(unsigned int context_ptr);
void Phase6_UsbDevice_SetEp0MaxPacket(unsigned int ep0_max_packet);
void Phase6_UsbDevice_SetDescriptorsReady(unsigned int ready);
void Phase6_UsbDevice_CompleteInitFromContext(void);
void Phase6_UsbDevice_MainLoop_DataPump(unsigned char cable_attached);
void Phase6_UsbDevice_ReadTrace(Phase6_UsbDeviceTrace *out_trace);

#endif /* PHASE6_USB_DEVICE_H */
