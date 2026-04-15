#include <assert.h>
#include "phase6/phase6_ui_event_router.h"

int main(void)
{
    Phase6_UiEventRouter router;
    const Phase6_UiDescriptor *descriptor;
    unsigned short event_code;
    unsigned char consumed;

    Phase6_UiEventRouter_Init(&router, 2U);
    assert(router.dispatch_count == 0U);
    assert(router.pending == 0U);

    Phase6_UiEventRouter_PushModeSwitch(&router, 3U);
    consumed = Phase6_UiEventRouter_Consume(&router, &event_code);
    assert(consumed == 1U);
    assert((event_code >> 8) == PHASE6_UI_EVENT_TAG_MODE_SWITCH);
    assert((event_code & 0xFFU) == 3U);
    descriptor = Phase6_UiEventRouter_GetDescriptor(
        Phase6_UiEventRouter_ResolveDescriptorIndex(event_code, (unsigned short)(event_code & 0xFFU)));
    assert(descriptor->mode_code == 3U);
    assert(descriptor->event_tag == PHASE6_UI_EVENT_TAG_MODE_SWITCH);

    Phase6_UiEventRouter_PushAdcUpdate(&router, 0x0A80U);
    consumed = Phase6_UiEventRouter_Consume(&router, &event_code);
    assert(consumed == 1U);
    assert((event_code >> 8) == PHASE6_UI_EVENT_TAG_ADC_UPDATE);
    descriptor = Phase6_UiEventRouter_GetDescriptor(
        Phase6_UiEventRouter_ResolveDescriptorIndex(event_code, 2U));
    assert(descriptor->event_tag == PHASE6_UI_EVENT_TAG_ADC_UPDATE);

    Phase6_UiEventRouter_PushUsbAttach(&router);
    consumed = Phase6_UiEventRouter_Consume(&router, &event_code);
    assert(consumed == 1U);
    assert((event_code >> 8) == PHASE6_UI_EVENT_TAG_USB_ATTACH);
    assert((event_code & 0xFFU) == 1U);

    Phase6_UiEventRouter_PushEncoder(&router, -9);
    consumed = Phase6_UiEventRouter_Consume(&router, &event_code);
    assert(consumed == 1U);
    assert((event_code >> 8) == PHASE6_UI_EVENT_TAG_ENCODER);
    assert((event_code & 0xFFU) == 9U);

    consumed = Phase6_UiEventRouter_Consume(&router, &event_code);
    assert(consumed == 0U);
    assert(router.dispatch_count == 4U);
    assert(Phase6_UiEventRouter_GetDescriptorCount() >= 8U);

    return 0;
}
