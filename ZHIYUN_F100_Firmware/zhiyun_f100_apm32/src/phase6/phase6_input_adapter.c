#include "phase6/phase6_input_adapter.h"
#ifndef PHASE6_HOST_TEST
#include "apm32f10x.h"
#endif

/*
 * Register map (APM32F103CBT6 / STM32F1 compatible offsets)
 * - TMR4 CNT:   0x40000824
 * - ADC1 DR:    0x4001244C
 * - GPIOC IDR:  0x40011008 (PC13 sampled as attach hint)
 */
#define PHASE6_PC13_MASK (1UL << 13)
#define PHASE6_APB2EN_AFIOEN (1UL << 0)
#define PHASE6_APB2EN_IOPCEN (1UL << 4)
#define PHASE6_APB2EN_ADC1EN (1UL << 9)
#define PHASE6_APB1EN_TMR4EN (1UL << 2)
#define PHASE6_ADC_CTRL2_ADON (1UL << 0)

#define PHASE6_USB_DEBOUNCE_CONFIRM_COUNT 3U
#define PHASE6_ENCODER_DEADBAND 1
#define PHASE6_ENCODER_MAX_STEP 32
#define PHASE6_ADC_CHANGE_THRESHOLD 16U
#define PHASE6_MODE_SWITCH_CONFIRM_COUNT 8U
#define PHASE6_MODE_HSI_ON_THRESHOLD 2800U
#define PHASE6_MODE_CCT_ON_THRESHOLD 2200U

static volatile unsigned short g_phase6_last_tmr4_cnt;
static volatile unsigned char g_phase6_tmr4_inited;
static volatile unsigned char g_phase6_usb_state;
static volatile unsigned char g_phase6_usb_debounce_count;
static volatile unsigned int g_phase6_adc_filtered;
static volatile unsigned short g_phase6_adc_last_reported;
static volatile unsigned int g_phase6_mode_switch_counter;
static volatile Phase6_UiMode g_phase6_mode_hint;

static int Phase6_ClampEncoderDelta(signed short delta)
{
    int value;

    value = (int)delta;
    if (value > PHASE6_ENCODER_MAX_STEP) {
        value = PHASE6_ENCODER_MAX_STEP;
    } else if (value < -PHASE6_ENCODER_MAX_STEP) {
        value = -PHASE6_ENCODER_MAX_STEP;
    }

    if ((value <= PHASE6_ENCODER_DEADBAND) && (value >= -PHASE6_ENCODER_DEADBAND)) {
        return 0;
    }

    return value;
}

static unsigned char Phase6_UpdateUsbDebounce(unsigned char usb_inserted_raw)
{
    if (usb_inserted_raw == g_phase6_usb_state) {
        g_phase6_usb_debounce_count = 0U;
        return 0U;
    }

    if (g_phase6_usb_debounce_count < 255U) {
        g_phase6_usb_debounce_count++;
    }

    if (g_phase6_usb_debounce_count >= PHASE6_USB_DEBOUNCE_CONFIRM_COUNT) {
        g_phase6_usb_state = usb_inserted_raw;
        g_phase6_usb_debounce_count = 0U;
        if (usb_inserted_raw != 0U) {
            return 1U;
        }
    }

    return 0U;
}

static unsigned short Phase6_FilterAdc12Bit(unsigned short raw)
{
    unsigned int filtered;

    /*
     * 1/8 IIR low-pass:
     * filtered = (prev*7 + raw) / 8
     */
    filtered = ((g_phase6_adc_filtered * 7U) + (unsigned int)raw) >> 3;
    g_phase6_adc_filtered = filtered;
    return (unsigned short)(filtered & 0x0FFFU);
}

static unsigned char Phase6_EvaluateModeSwitch(unsigned short adc_norm, Phase6_InputEvent *event)
{
    Phase6_UiMode target_mode;

    target_mode = g_phase6_mode_hint;
    if (g_phase6_mode_hint == PHASE6_UI_MODE_CCT) {
        if (adc_norm >= PHASE6_MODE_HSI_ON_THRESHOLD) {
            target_mode = PHASE6_UI_MODE_HSI;
        }
    } else {
        if (adc_norm <= PHASE6_MODE_CCT_ON_THRESHOLD) {
            target_mode = PHASE6_UI_MODE_CCT;
        }
    }

    if (target_mode != g_phase6_mode_hint) {
        g_phase6_mode_switch_counter++;
        if (g_phase6_mode_switch_counter >= PHASE6_MODE_SWITCH_CONFIRM_COUNT) {
            g_phase6_mode_hint = target_mode;
            g_phase6_mode_switch_counter = 0U;
            event->requested_mode = g_phase6_mode_hint;
            event->has_mode_request = 1U;
            return 1U;
        }
    } else {
        g_phase6_mode_switch_counter = 0U;
    }

    return 0U;
}

void Phase6_InputAdapter_Init(void)
{
    /*
     * Minimal hardware prep:
     * - Enable AFIO/GPIOC/ADC1/TMR4 clocks
     * - Configure PB3/PB11 as output-low for PD GPIO state machine sink
     * - Keep PC13 as pull-up input for attach hint read
     * - Ensure ADC1 is powered so DR reads are stable when ADC pipeline is active
     */
    RCM->APB2CLKEN |= (PHASE6_APB2EN_AFIOEN | PHASE6_APB2EN_IOPCEN | PHASE6_APB2EN_ADC1EN);
    RCM->APB1CLKEN |= PHASE6_APB1EN_TMR4EN;
    ADC1->CTRL2 |= PHASE6_ADC_CTRL2_ADON;

    /* PC13 config nibble in GPIOC_CRH[23:20]: input pull-up/down (1000b), then ODR13=1 pull-up. */
    GPIOC->CFGHIG = (GPIOC->CFGHIG & ~(0xFUL << 20)) | (0x8UL << 20);
    GPIOC->ODATA |= PHASE6_PC13_MASK;
    /* PB3 (CRL[15:12]) and PB11 (CRH[15:12]): output push-pull 2MHz (0010b), default low. */
    GPIOB->CFGLOW = (GPIOB->CFGLOW & ~(0xFUL << 12)) | (0x2UL << 12);
    GPIOB->CFGHIG = (GPIOB->CFGHIG & ~(0xFUL << 12)) | (0x2UL << 12);
    GPIOB->BC = (1UL << 3) | (1UL << 11);

    (void)GPIOC->CFGLOW;
    g_phase6_last_tmr4_cnt = TMR4->CNT;
    g_phase6_usb_state = ((GPIOC->IDATA & PHASE6_PC13_MASK) == 0U) ? 1U : 0U;
    g_phase6_usb_debounce_count = 0U;
    g_phase6_adc_filtered = (unsigned int)(ADC1->REGDATA & 0x0FFFU);
    g_phase6_adc_last_reported = (unsigned short)(g_phase6_adc_filtered & 0x0FFFU);
    g_phase6_mode_switch_counter = 0U;
    g_phase6_mode_hint = PHASE6_UI_MODE_CCT;
    g_phase6_tmr4_inited = 1U;
}

unsigned char Phase6_InputAdapter_Poll(Phase6_InputEvent *out_event)
{
    unsigned short curr_cnt;
    unsigned short adc_raw;
    unsigned int gpioc_idr;
    unsigned char usb_inserted_raw;
    unsigned char usb_inserted_edge;
    unsigned short adc_norm;
    signed short delta;
    unsigned char changed;

    if (out_event == 0) {
        return 0U;
    }

    curr_cnt = TMR4->CNT;
    adc_raw = ADC1->REGDATA & 0x0FFFU;
    gpioc_idr = GPIOC->IDATA;
    usb_inserted_raw = ((gpioc_idr & PHASE6_PC13_MASK) == 0U) ? 1U : 0U;
    usb_inserted_edge = Phase6_UpdateUsbDebounce(usb_inserted_raw);
    adc_norm = Phase6_FilterAdc12Bit(adc_raw);

    if (g_phase6_tmr4_inited == 0U) {
        g_phase6_last_tmr4_cnt = curr_cnt;
        g_phase6_tmr4_inited = 1U;
    }

    delta = (signed short)(curr_cnt - g_phase6_last_tmr4_cnt);
    g_phase6_last_tmr4_cnt = curr_cnt;

    out_event->encoder_delta = Phase6_ClampEncoderDelta(delta);
    out_event->adc_level = adc_norm;
    out_event->has_adc_update = 0U;
    out_event->requested_mode = PHASE6_UI_MODE_CCT;
    out_event->has_mode_request = 0U;
    out_event->usb_inserted = 0U;

    changed = 0U;
    if (out_event->encoder_delta != 0) {
        changed = 1U;
    }

    if ((adc_norm > g_phase6_adc_last_reported) &&
        ((adc_norm - g_phase6_adc_last_reported) >= PHASE6_ADC_CHANGE_THRESHOLD)) {
        changed = 1U;
        out_event->has_adc_update = 1U;
        g_phase6_adc_last_reported = adc_norm;
    } else if ((g_phase6_adc_last_reported > adc_norm) &&
               ((g_phase6_adc_last_reported - adc_norm) >= PHASE6_ADC_CHANGE_THRESHOLD)) {
        changed = 1U;
        out_event->has_adc_update = 1U;
        g_phase6_adc_last_reported = adc_norm;
    }

    if (Phase6_EvaluateModeSwitch(adc_norm, out_event) != 0U) {
        changed = 1U;
    }

    if (usb_inserted_edge != 0U) {
        out_event->usb_inserted = 1U;
        changed = 1U;
    }

    return changed;
}

unsigned int Phase6_InputAdapter_ReadStableAdc(void)
{
    return g_phase6_adc_filtered;
}
