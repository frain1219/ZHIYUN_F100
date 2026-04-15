#include "phase6/phase6_ui_event_router.h"

static const Phase6_UiDescriptor g_phase6_ui_descriptor_pool[] = {
    {2U, PHASE6_UI_EVENT_TAG_MODE_SWITCH, 0U, 0x10U},
    {2U, PHASE6_UI_EVENT_TAG_ADC_UPDATE,  1U, 0x20U},
    {2U, PHASE6_UI_EVENT_TAG_ENCODER,     2U, 0x30U},
    {3U, PHASE6_UI_EVENT_TAG_MODE_SWITCH, 3U, 0x11U},
    {3U, PHASE6_UI_EVENT_TAG_ADC_UPDATE,  4U, 0x21U},
    {3U, PHASE6_UI_EVENT_TAG_ENCODER,     5U, 0x31U},
    {2U, PHASE6_UI_EVENT_TAG_USB_ATTACH,  6U, 0x40U},
    {3U, PHASE6_UI_EVENT_TAG_USB_ATTACH,  7U, 0x41U}
};

static unsigned short Phase6_UiEventRouter_Compose(unsigned short tag, unsigned short param)
{
    return (unsigned short)(((tag & 0x00FFU) << 8) | (param & 0x00FFU));
}

void Phase6_UiEventRouter_Init(Phase6_UiEventRouter *router, unsigned short initial_mode)
{
    if (router == 0) {
        return;
    }

    router->last_event_code = Phase6_UiEventRouter_Compose(PHASE6_UI_EVENT_TAG_MODE_SWITCH, initial_mode);
    router->dispatch_count = 0U;
    router->pending = 0U;
}

void Phase6_UiEventRouter_PushModeSwitch(Phase6_UiEventRouter *router, unsigned short mode)
{
    if (router == 0) {
        return;
    }

    router->last_event_code = Phase6_UiEventRouter_Compose(PHASE6_UI_EVENT_TAG_MODE_SWITCH, mode);
    router->pending = 1U;
}

void Phase6_UiEventRouter_PushAdcUpdate(Phase6_UiEventRouter *router, unsigned short adc_level)
{
    if (router == 0) {
        return;
    }

    router->last_event_code = Phase6_UiEventRouter_Compose(PHASE6_UI_EVENT_TAG_ADC_UPDATE, (unsigned short)(adc_level >> 4));
    router->pending = 1U;
}

void Phase6_UiEventRouter_PushUsbAttach(Phase6_UiEventRouter *router)
{
    if (router == 0) {
        return;
    }

    router->last_event_code = Phase6_UiEventRouter_Compose(PHASE6_UI_EVENT_TAG_USB_ATTACH, 1U);
    router->pending = 1U;
}

void Phase6_UiEventRouter_PushEncoder(Phase6_UiEventRouter *router, int encoder_value)
{
    unsigned short param;

    if (router == 0) {
        return;
    }

    if (encoder_value < 0) {
        param = (unsigned short)((-encoder_value) & 0x00FF);
    } else {
        param = (unsigned short)(encoder_value & 0x00FF);
    }
    router->last_event_code = Phase6_UiEventRouter_Compose(PHASE6_UI_EVENT_TAG_ENCODER, param);
    router->pending = 1U;
}

unsigned char Phase6_UiEventRouter_Consume(Phase6_UiEventRouter *router, unsigned short *out_event_code)
{
    if (router == 0) {
        return 0U;
    }

    if (router->pending == 0U) {
        return 0U;
    }

    router->pending = 0U;
    router->dispatch_count++;
    if (out_event_code != 0) {
        *out_event_code = router->last_event_code;
    }
    return 1U;
}

unsigned int Phase6_UiEventRouter_ResolveDescriptorIndex(unsigned short event_code, unsigned short mode_code)
{
    unsigned short event_tag;
    unsigned int i;

    event_tag = (unsigned short)((event_code >> 8) & 0xFFU);
    for (i = 0U; i < (sizeof(g_phase6_ui_descriptor_pool) / sizeof(g_phase6_ui_descriptor_pool[0])); ++i) {
        if ((g_phase6_ui_descriptor_pool[i].event_tag == event_tag) &&
            (g_phase6_ui_descriptor_pool[i].mode_code == mode_code)) {
            return i;
        }
    }
    return 0U;
}

const Phase6_UiDescriptor *Phase6_UiEventRouter_GetDescriptor(unsigned int index)
{
    if (index >= (sizeof(g_phase6_ui_descriptor_pool) / sizeof(g_phase6_ui_descriptor_pool[0]))) {
        return &g_phase6_ui_descriptor_pool[0];
    }
    return &g_phase6_ui_descriptor_pool[index];
}

unsigned int Phase6_UiEventRouter_GetDescriptorCount(void)
{
    return (unsigned int)(sizeof(g_phase6_ui_descriptor_pool) / sizeof(g_phase6_ui_descriptor_pool[0]));
}
