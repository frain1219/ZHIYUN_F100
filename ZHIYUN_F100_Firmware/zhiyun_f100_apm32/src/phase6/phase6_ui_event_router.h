#ifndef PHASE6_UI_EVENT_ROUTER_H
#define PHASE6_UI_EVENT_ROUTER_H

typedef struct {
    unsigned short last_event_code;
    unsigned int dispatch_count;
    unsigned char pending;
} Phase6_UiEventRouter;

typedef struct {
    unsigned short mode_code;
    unsigned short event_tag;
    unsigned char page;
    unsigned char style;
} Phase6_UiDescriptor;

enum {
    PHASE6_UI_EVENT_TAG_MODE_SWITCH = 0x11,
    PHASE6_UI_EVENT_TAG_ADC_UPDATE = 0x22,
    PHASE6_UI_EVENT_TAG_USB_ATTACH = 0x33,
    PHASE6_UI_EVENT_TAG_ENCODER = 0x44
};

void Phase6_UiEventRouter_Init(Phase6_UiEventRouter *router, unsigned short initial_mode);
void Phase6_UiEventRouter_PushModeSwitch(Phase6_UiEventRouter *router, unsigned short mode);
void Phase6_UiEventRouter_PushAdcUpdate(Phase6_UiEventRouter *router, unsigned short adc_level);
void Phase6_UiEventRouter_PushUsbAttach(Phase6_UiEventRouter *router);
void Phase6_UiEventRouter_PushEncoder(Phase6_UiEventRouter *router, int encoder_value);
unsigned char Phase6_UiEventRouter_Consume(Phase6_UiEventRouter *router, unsigned short *out_event_code);
unsigned int Phase6_UiEventRouter_ResolveDescriptorIndex(unsigned short event_code, unsigned short mode_code);
const Phase6_UiDescriptor *Phase6_UiEventRouter_GetDescriptor(unsigned int index);
unsigned int Phase6_UiEventRouter_GetDescriptorCount(void);

#endif /* PHASE6_UI_EVENT_ROUTER_H */
