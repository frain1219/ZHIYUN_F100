#include "phase6/phase6_light_pd_skeleton.h"
#include "phase6/phase6_startup_chain.h"
#include "phase6/phase6_ui_event_router.h"
#ifndef PHASE6_HOST_TEST
#include "apm32f10x.h"
#endif

extern volatile unsigned int g_phase6_scheduler_ticks;
extern volatile unsigned int g_phase6_systick_entry_count;

#define PHASE6_PD_ATTACH_HELPER_TICK_WINDOW 3000U
#define PHASE6_CCT_PERIOD_BASE 1200U
#define PHASE6_CCT_PERIOD_SPAN 2600U
#define PHASE6_CCT_COMPARE_MIN 24U
#define PHASE6_CCT_COMPARE_MARGIN 24U
#define PHASE6_HSI_MIX_BIAS 32U
#define PHASE6_DIMMING_ACTIVE_TICKS 4U
#define PHASE6_OLED_SLICE_COUNT 8U
#define PHASE6_OLED_SLICE_BYTES 96U
#define PHASE6_OLED_PREFIX_BYTES 3U
/*
 * Measured in PD block invocations (g_phase6_call_ticks), not wall-clock ms.
 * OEM ties UI/SPI cadence to PD_PeriodicDispatchFromSysTick (/20); host harness
 * feeds 20 SysTick-equivalent steps per RunOnce so one PD == one prior RunOnce.
 */
#define PHASE6_OLED_FORCE_REFRESH_TICKS 16U
#define PHASE6_TMR2_DMA_PATTERN_WORDS 16U

#define PHASE6_APB2EN_AFIOEN (1UL << 0)
#define PHASE6_APB2EN_IOPAEN (1UL << 2)
#define PHASE6_APB2EN_IOPBEN (1UL << 3)
#define PHASE6_APB2EN_TMR1EN (1UL << 11)
#define PHASE6_APB1EN_TMR2EN (1UL << 0)
#define PHASE6_APB1EN_TMR3EN (1UL << 1)
#define PHASE6_AHBEN_DMA1EN (1UL << 0)
#define PHASE6_TMR_CTRL1_CNTEN (1U << 0)
#define PHASE6_TMR_CTRL1_AUTORLDEN (1U << 7)
#define PHASE6_TMR_CCM_OCM_PWM1 0x6U
#define PHASE6_TMR_CCM_OC1PE (1U << 3)
#define PHASE6_TMR_CCM_OC3PE (1U << 3)
#define PHASE6_TMR_CCM_OC2PE (1U << 11)
#define PHASE6_TMR_CCEN_CC1EN (1U << 0)
#define PHASE6_TMR_CCEN_CC3EN (1U << 8)
#define PHASE6_TMR_CCEN_CC2EN (1U << 4)
#define PHASE6_DMA_CHCFG_EN (1UL << 0)
#define PHASE6_DMA_CHCFG_DIR (1UL << 4)
#define PHASE6_DMA_CHCFG_MINC (1UL << 7)
#define PHASE6_DMA_CHCFG_PSIZE_16BIT (1UL << 8)
#define PHASE6_DMA_CHCFG_MSIZE_16BIT (1UL << 10)
#define PHASE6_DMA_CHCFG_PL_HIGH (0x2UL << 12)

/*
 * Phase 6 first-pass skeleton:
 * - keep original call order from 05-MMIO-v1 section 2/3
 * - do not perform undocumented peripheral writes
 * - provide traceable placeholders for behavior acceptance checklist
 */

static volatile unsigned int g_phase6_call_ticks;
static volatile unsigned int g_phase6_last_stage;
static volatile unsigned int g_phase6_input_scan_count;
static volatile unsigned int g_phase6_pd_attach_count;
static volatile unsigned int g_phase6_pd_attached_loop_count;
static volatile unsigned int g_phase6_pd_gpio_count;
static volatile unsigned int g_phase6_pd_attach_window_count;
static volatile unsigned int g_phase6_protocol_lane_expand_count;
static volatile unsigned int g_phase6_light_apply_count;
static volatile unsigned int g_phase6_cct_apply_count;
static volatile unsigned int g_phase6_hsi_apply_count;
static volatile unsigned int g_phase6_dimming_update_count;
static volatile unsigned int g_phase6_ui_main_count;
static volatile unsigned int g_phase6_ui_render_count;
static volatile unsigned int g_phase6_ui_flush_count;
static volatile unsigned int g_phase6_usb_insert_event_count;
static volatile unsigned int g_phase6_usb_attached;
static volatile int g_phase6_encoder_accum;
static volatile int g_phase6_last_encoder_delta;
static volatile unsigned short g_phase6_adc_level;
static volatile unsigned short g_phase6_tmr1_period_shadow;
static volatile unsigned short g_phase6_tmr1_compare_shadow;
static volatile unsigned char g_phase6_hsi_rgb_shadow[3];
static volatile Phase6_UiMode g_phase6_mode;
static volatile Phase6_InputEvent g_phase6_pending_input;
static volatile unsigned char g_phase6_has_pending_input;
static volatile unsigned short g_phase6_ui_mode_word;
static volatile unsigned short g_phase6_ui_last_event_code;
static volatile unsigned int g_phase6_ui_event_dispatch_count;
static volatile unsigned int g_phase6_ui_descriptor_index;
static volatile unsigned int g_phase6_ui_framebuffer_checksum;
static volatile unsigned int g_phase6_spi_flush_bytes;
static volatile unsigned int g_phase6_spi_flush_slices;
static volatile unsigned int g_phase6_spi_flush_trigger_count;
static volatile unsigned int g_phase6_spi_flush_skip_count;
volatile unsigned int g_phase6_mainloop_spi_pump_request_count;
volatile unsigned int g_phase6_mainloop_spi_pump_consumed_count;
volatile unsigned char g_phase6_mainloop_spi_pump_pending;
static volatile unsigned int g_phase6_tmr2_dma_push_count;
static volatile unsigned int g_phase6_tmr2_dma_last_words;
static volatile unsigned int g_phase6_tmr3_pwm_apply_count;
static volatile unsigned int g_phase6_tmr3_cc2_shadow;
static volatile unsigned int g_phase6_dma1_ch1_transfer_count;
static Phase6_UiEventRouter g_phase6_ui_router;
static volatile unsigned char g_phase6_ui_router_inited;
static volatile unsigned int g_phase6_last_attach_window_systick;
static volatile unsigned int g_phase6_dimming_pending_ticks;
static volatile unsigned int g_phase6_tmr1_pwm_inited;
static volatile unsigned int g_phase6_tmr3_pwm_inited;
static volatile unsigned int g_phase6_dma1_ch1_inited;
static volatile unsigned int g_phase6_oled_last_flush_tick;
static volatile unsigned char g_phase6_pd_pb3_state;
static volatile unsigned char g_phase6_pd_pb11_state;
static volatile unsigned char g_phase6_ui_frame_dirty;
static unsigned char g_phase6_oled_framebuffer[PHASE6_OLED_SLICE_COUNT][PHASE6_OLED_SLICE_BYTES];
static unsigned short g_phase6_tmr2_dma_pattern[PHASE6_TMR2_DMA_PATTERN_WORDS];

static void Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15(void);
static unsigned char PD_ReadGPIOC13_IdrBit(void);
static void PD_WriteGPIOB3_BSRR_BRR(unsigned char high);
static void PD_WriteGPIOB11_BSRR_BRR(unsigned char high);
static void Phase6_PD_SetOutputs(unsigned char pb3_high, unsigned char pb11_high);
static void Phase6_Input_ApplyPendingEvent(void);
static unsigned int Phase6_UI_FramebufferChecksum(void);
static void Phase6_UI_RenderFramebuffer(unsigned short event_code);
static unsigned int Phase6_SPI1_FlushFramebufferSlices(void);
static void Init_TMR3_PWM_FanPB5(void);
static void Init_DMA1Ch1_TMR2CcrDMA(void);

void Phase6_MainLoop_RequestSpiPump(void)
{
    g_phase6_mainloop_spi_pump_request_count++;
    g_phase6_mainloop_spi_pump_pending = 1U;
}

static unsigned short Phase6_ClampU16(unsigned int value, unsigned short min_v, unsigned short max_v)
{
    if (value < (unsigned int)min_v) {
        return min_v;
    }

    if (value > (unsigned int)max_v) {
        return max_v;
    }

    return (unsigned short)value;
}

static unsigned int Phase6_PackRgbShadow(void)
{
    unsigned int packed;

    packed = ((unsigned int)g_phase6_hsi_rgb_shadow[0] << 16);
    packed |= ((unsigned int)g_phase6_hsi_rgb_shadow[1] << 8);
    packed |= (unsigned int)g_phase6_hsi_rgb_shadow[2];
    return packed;
}

static unsigned int Phase6_UI_FramebufferChecksum(void)
{
    unsigned int row;
    unsigned int col;
    unsigned int checksum;

    checksum = 0U;
    for (row = 0U; row < PHASE6_OLED_SLICE_COUNT; ++row) {
        for (col = 0U; col < PHASE6_OLED_SLICE_BYTES; ++col) {
            checksum += (unsigned int)g_phase6_oled_framebuffer[row][col];
            checksum = (checksum << 3) | (checksum >> 29);
        }
    }
    return checksum;
}

static void Phase6_UI_RenderFramebuffer(unsigned short event_code)
{
    const Phase6_UiDescriptor *descriptor;
    unsigned int row;
    unsigned int col;
    unsigned char mode_seed;
    unsigned char event_seed;
    unsigned short adc_seed;

    mode_seed = (unsigned char)(g_phase6_ui_mode_word & 0xFFU);
    event_seed = (unsigned char)((event_code >> 8) & 0xFFU);
    adc_seed = g_phase6_adc_level;
    g_phase6_ui_descriptor_index = Phase6_UiEventRouter_ResolveDescriptorIndex(event_code, g_phase6_ui_mode_word);
    descriptor = Phase6_UiEventRouter_GetDescriptor(g_phase6_ui_descriptor_index);

    for (row = 0U; row < PHASE6_OLED_SLICE_COUNT; ++row) {
        for (col = 0U; col < PHASE6_OLED_SLICE_BYTES; ++col) {
            unsigned int base;
            base = (unsigned int)(row * 7U + col * 3U);
            base += (unsigned int)(mode_seed * 11U);
            base += (unsigned int)(event_seed * 5U);
            base += (unsigned int)((adc_seed >> 5) & 0x3FU);
            base += (unsigned int)(descriptor->page * 13U + descriptor->style);
            if (g_phase6_mode == PHASE6_UI_MODE_HSI) {
                base ^= (unsigned int)g_phase6_hsi_rgb_shadow[row % 3U];
            } else {
                base ^= (unsigned int)(g_phase6_tmr1_compare_shadow & 0xFFU);
            }
            g_phase6_oled_framebuffer[row][col] = (unsigned char)(base & 0xFFU);
        }
    }
    g_phase6_ui_framebuffer_checksum = Phase6_UI_FramebufferChecksum();
}

static unsigned int Phase6_SPI1_FlushFramebufferSlices(void)
{
    unsigned int row;
    unsigned int bytes;

    bytes = 0U;
    Phase6_SPI1_EnsureReady();
    for (row = 0U; row < PHASE6_OLED_SLICE_COUNT; ++row) {
        unsigned char prefix[PHASE6_OLED_PREFIX_BYTES];
        prefix[0] = (unsigned char)(0xB0U + row);
        prefix[1] = 0x00U;
        prefix[2] = 0x10U;
        Phase6_SPI1_SetCs(0U);
        bytes += Phase6_SPI1_Transmit(prefix, PHASE6_OLED_PREFIX_BYTES);
        bytes += Phase6_SPI1_Transmit(g_phase6_oled_framebuffer[row], PHASE6_OLED_SLICE_BYTES);
        Phase6_SPI1_SetCs(1U);
    }
    return bytes;
}

static void Init_TMR3_PWM_FanPB5(void)
{
    if (g_phase6_tmr3_pwm_inited != 0U) {
        return;
    }

#ifndef PHASE6_HOST_TEST
    RCM->APB2CLKEN |= (PHASE6_APB2EN_AFIOEN | PHASE6_APB2EN_IOPBEN);
    RCM->APB1CLKEN |= PHASE6_APB1EN_TMR3EN;

    /* PB5 -> TMR3 CH2 AF push-pull. */
    GPIOB->CFGLOW = (GPIOB->CFGLOW & ~(0xFUL << 20)) | (0xAUL << 20);

    TMR3->CTRL1 = PHASE6_TMR_CTRL1_AUTORLDEN;
    TMR3->PSC = 71U; /* 1 MHz timer clock at 72 MHz SYSCLK. */
    TMR3->AUTORLD = 999U; /* 1 kHz fan PWM baseline. */
    TMR3->CC2 = 0U;
    TMR3->CCM1 =
        (unsigned short)((TMR3->CCM1 & ~(0xFFU << 8)) |
                         ((PHASE6_TMR_CCM_OCM_PWM1 << 12) | PHASE6_TMR_CCM_OC2PE));
    TMR3->CCEN = (unsigned short)(TMR3->CCEN | PHASE6_TMR_CCEN_CC2EN);
    TMR3->CEG = 1U;
    TMR3->CTRL1 = (unsigned short)(PHASE6_TMR_CTRL1_AUTORLDEN | PHASE6_TMR_CTRL1_CNTEN);
#endif
    g_phase6_tmr3_cc2_shadow = 0U;
    g_phase6_tmr3_pwm_inited = 1U;
}

static void Init_DMA1Ch1_TMR2CcrDMA(void)
{
    if (g_phase6_dma1_ch1_inited != 0U) {
        return;
    }

#ifndef PHASE6_HOST_TEST
    RCM->AHBCLKEN |= PHASE6_AHBEN_DMA1EN;
    RCM->APB1CLKEN |= PHASE6_APB1EN_TMR2EN;

    DMA1_Channel1->CHCFG = 0U;
    DMA1_Channel1->CHPADDR = (uint32_t)(&TMR2->CC3);
    DMA1_Channel1->CHMADDR = (uint32_t)(g_phase6_tmr2_dma_pattern);
    DMA1_Channel1->CHNDATA = PHASE6_TMR2_DMA_PATTERN_WORDS;
    DMA1_Channel1->CHCFG =
        PHASE6_DMA_CHCFG_DIR |
        PHASE6_DMA_CHCFG_MINC |
        PHASE6_DMA_CHCFG_PSIZE_16BIT |
        PHASE6_DMA_CHCFG_MSIZE_16BIT |
        PHASE6_DMA_CHCFG_PL_HIGH;
#endif
    g_phase6_dma1_ch1_inited = 1U;
}

void Phase6_LightPd_SetInputEvent(const Phase6_InputEvent *event)
{
    if (event == 0) {
        return;
    }

    g_phase6_pending_input = *event;
    g_phase6_has_pending_input = 1U;
}

void Phase6_LightPd_InjectUsbInsert(void)
{
    Phase6_InputEvent event;

    event.encoder_delta = 0;
    event.adc_level = g_phase6_adc_level;
    event.has_adc_update = 0U;
    event.requested_mode = g_phase6_mode;
    event.has_mode_request = 0U;
    event.usb_inserted = 1U;
    Phase6_LightPd_SetInputEvent(&event);
}

void TMR1_PeriodHalfword_Write(unsigned short value)
{
    g_phase6_tmr1_period_shadow = value;
#ifndef PHASE6_HOST_TEST
    TMR1->AUTORLD = value;
    TMR1->CEG = 1U;
#endif
}

void TMR1_CompareHalfword_Write(unsigned short value)
{
    g_phase6_tmr1_compare_shadow = value;
#ifndef PHASE6_HOST_TEST
    TMR1->CC1 = value;
    TMR1->CC3 = value;
#endif
}

static void Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15(void)
{
    if (g_phase6_tmr1_pwm_inited != 0U) {
        return;
    }

    /*
     * Anchor:
     * - 05 §5.3.1 points to Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15 @ 0x0800DD28.
     * - Phase 6 first pass keeps BDTR untouched (05 currently records no BDTR xref).
     * TODO(待证实): compare preload/enable ordering vs OEM (Renode/GDB/逆向；OI-004 不等待示波器)。
     */
#ifndef PHASE6_HOST_TEST
    RCM->APB2CLKEN |=
        (PHASE6_APB2EN_AFIOEN | PHASE6_APB2EN_IOPAEN | PHASE6_APB2EN_IOPBEN | PHASE6_APB2EN_TMR1EN);

    /* PA8/PA9 AF push-pull (mode=10, cnf=10), PB15 AF push-pull (mode=10, cnf=10). */
    GPIOA->CFGHIG =
        (GPIOA->CFGHIG & ~((0xFUL << 0) | (0xFUL << 4))) |
        (0xAUL << 0) | (0xAUL << 4);
    GPIOB->CFGHIG =
        (GPIOB->CFGHIG & ~(0xFUL << 28)) |
        (0xAUL << 28);

    TMR1->CTRL1 = PHASE6_TMR_CTRL1_AUTORLDEN;
    TMR1->SMCTRL = 0U;
    TMR1->PSC = 0U;
    TMR1->CNT = 0U;
    TMR1->AUTORLD = PHASE6_CCT_PERIOD_BASE;
    TMR1->CC1 = PHASE6_CCT_COMPARE_MIN;
    TMR1->CC3 = PHASE6_CCT_COMPARE_MIN;
    TMR1->CCM1 =
        (unsigned short)((PHASE6_TMR_CCM_OCM_PWM1 << 4) | PHASE6_TMR_CCM_OC1PE);
    TMR1->CCM2 =
        (unsigned short)((PHASE6_TMR_CCM_OCM_PWM1 << 4) | PHASE6_TMR_CCM_OC3PE);
    TMR1->CCEN = (PHASE6_TMR_CCEN_CC1EN | PHASE6_TMR_CCEN_CC3EN);
    TMR1->CEG = 1U;
    TMR1->CTRL1 =
        (unsigned short)(PHASE6_TMR_CTRL1_AUTORLDEN | PHASE6_TMR_CTRL1_CNTEN);
#endif

    g_phase6_tmr1_period_shadow = PHASE6_CCT_PERIOD_BASE;
    g_phase6_tmr1_compare_shadow = PHASE6_CCT_COMPARE_MIN;
    g_phase6_tmr1_pwm_inited = 1U;
}

static unsigned char PD_ReadGPIOC13_IdrBit(void)
{
#ifndef PHASE6_HOST_TEST
    return (unsigned char)((GPIOC->IDATA >> 13) & 0x1U);
#else
    return (g_phase6_usb_attached != 0U) ? 0U : 1U;
#endif
}

static void PD_WriteGPIOB3_BSRR_BRR(unsigned char high)
{
#ifndef PHASE6_HOST_TEST
    if (high != 0U) {
        GPIOB->BSC = (1UL << 3);
    } else {
        GPIOB->BC = (1UL << 3);
    }
#endif
    g_phase6_pd_pb3_state = (high != 0U) ? 1U : 0U;
}

static void PD_WriteGPIOB11_BSRR_BRR(unsigned char high)
{
#ifndef PHASE6_HOST_TEST
    if (high != 0U) {
        GPIOB->BSC = (1UL << 11);
    } else {
        GPIOB->BC = (1UL << 11);
    }
#endif
    g_phase6_pd_pb11_state = (high != 0U) ? 1U : 0U;
}

static void Phase6_PD_SetOutputs(unsigned char pb3_high, unsigned char pb11_high)
{
    PD_WriteGPIOB3_BSRR_BRR(pb3_high);
    PD_WriteGPIOB11_BSRR_BRR(pb11_high);
}

static void Phase6_Input_ApplyPendingEvent(void)
{
    unsigned char input_changes;

    input_changes = 0U;
    g_phase6_last_encoder_delta = g_phase6_pending_input.encoder_delta;
    g_phase6_encoder_accum += g_phase6_pending_input.encoder_delta;
    if (g_phase6_pending_input.encoder_delta != 0) {
        input_changes = 1U;
    }

    if (g_phase6_pending_input.has_mode_request != 0U) {
        g_phase6_mode = g_phase6_pending_input.requested_mode;
        g_phase6_ui_mode_word = (unsigned short)g_phase6_mode;
        Phase6_UiEventRouter_PushModeSwitch((Phase6_UiEventRouter *)&g_phase6_ui_router, g_phase6_ui_mode_word);
        input_changes = 1U;
    }

    if ((g_phase6_pending_input.has_adc_update != 0U) &&
        (g_phase6_pending_input.adc_level != g_phase6_adc_level)) {
        g_phase6_adc_level = g_phase6_pending_input.adc_level;
        g_phase6_dimming_update_count++;
        g_phase6_dimming_pending_ticks = PHASE6_DIMMING_ACTIVE_TICKS;
        Phase6_UiEventRouter_PushAdcUpdate((Phase6_UiEventRouter *)&g_phase6_ui_router, g_phase6_adc_level);
        input_changes = 1U;
    }

    if (g_phase6_pending_input.usb_inserted != 0U) {
        Phase6_UiEventRouter_PushUsbAttach((Phase6_UiEventRouter *)&g_phase6_ui_router);
        input_changes = 1U;
    }

    if (input_changes != 0U) {
        g_phase6_ui_frame_dirty = 1U;
    }
}

void PD_AttachSequence_StateMachine(void)
{
    g_phase6_pd_attach_count++;
    if ((g_phase6_has_pending_input != 0U) && (g_phase6_pending_input.usb_inserted != 0U)) {
        /* 05-MMIO-v1 path intent: attach hint arrives from input lane and gates PD branches. */
        g_phase6_usb_attached = 1U;
        g_phase6_usb_insert_event_count++;
    }

    if (g_phase6_usb_attached != 0U) {
        g_phase6_pd_attached_loop_count++;
    }

    g_phase6_last_stage = PHASE6_STAGE_PD_ATTACH;
}

void Input_5Channel_TableStateMachine(void)
{
    g_phase6_input_scan_count++;
    g_phase6_last_encoder_delta = 0;

    if (g_phase6_has_pending_input != 0U) {
        Phase6_Input_ApplyPendingEvent();
    }

    if (g_phase6_last_encoder_delta != 0) {
        unsigned short encoder_step;

        if (g_phase6_last_encoder_delta > 0) {
            encoder_step = (unsigned short)(g_phase6_last_encoder_delta & 0x00FFU);
        } else {
            encoder_step = (unsigned short)((-g_phase6_last_encoder_delta) & 0x00FFU);
        }
        Phase6_UiEventRouter_PushEncoder((Phase6_UiEventRouter *)&g_phase6_ui_router, (int)encoder_step);
    }

    g_phase6_last_stage = PHASE6_STAGE_INPUT_SCAN;
}

void PD_GPIO_StateMachine(void)
{
    if (g_phase6_usb_attached != 0U) {
        unsigned char pc13_is_high;

        pc13_is_high = PD_ReadGPIOC13_IdrBit();
        /*
         * Reference (04 §3.1/§3.2):
         * - PB3 and PB11 are independent GPIOB BSRR/BRR lines used by PD state path.
         * - PC13 low is treated as attach hint.
         * - Original symbol: PD_GPIO_StateMachine @ 0x0800C4E0.
         * 待证实（G3 实机）：PC13/PB3/PB11 与诱骗充电时序逐沿对应关系。
         */
        if (pc13_is_high == 0U) {
            Phase6_PD_SetOutputs(1U, (g_phase6_protocol_lane_expand_count > 0U) ? 1U : 0U);
        } else {
            Phase6_PD_SetOutputs(0U, 0U);
        }
        g_phase6_pd_gpio_count++;
    } else {
        Phase6_PD_SetOutputs(0U, 0U);
    }

    g_phase6_last_stage = PHASE6_STAGE_PD_GPIO;
}

void PD_HelperSeq_3000tick_ThenPc13Gpio(void)
{
    if (g_phase6_usb_attached != 0U) {
        /*
         * Tick window is anchored to SysTick IRQ entries.
         * OEM PD periodic path is dispatched from SysTick (/20), so using
         * g_phase6_systick_entry_count avoids inflating helper windows by 20x.
         */
        if ((g_phase6_systick_entry_count - g_phase6_last_attach_window_systick) >=
            PHASE6_PD_ATTACH_HELPER_TICK_WINDOW) {
            g_phase6_pd_attach_window_count++;
            g_phase6_last_attach_window_systick = g_phase6_systick_entry_count;
            PD_WriteGPIOB11_BSRR_BRR(1U);
        }
    }

    g_phase6_last_stage = PHASE6_STAGE_PD_ATTACH_HELPER;
}

void ProtocolLanes_BitExpandTick(void)
{
    if (g_phase6_usb_attached != 0U) {
        g_phase6_protocol_lane_expand_count++;
    }

    g_phase6_last_stage = PHASE6_STAGE_PROTOCOL_EXPAND;
}

void Encoder_DispatchDelta_FromTMR4(void)
{
    if (g_phase6_last_encoder_delta != 0) {
        g_phase6_dimming_pending_ticks = PHASE6_DIMMING_ACTIVE_TICKS;
    }
    g_phase6_last_stage = PHASE6_STAGE_ENCODER_DISPATCH;
}

void CCT_Slew_TableSplit_TMR1Shadow(void)
{
    unsigned int adc_norm;
    int encoder_norm;
    unsigned int period;
    unsigned int compare;
    unsigned int compare_max;

    /*
     * Reference anchors:
     * - CCT path and TMR1 compare/period shadow updates in 03 §0.3 / 05 §5.1.
     * - First-pass math is intentionally conservative and traceable.
     *   TODO(待证实): replace coefficients with measured mapping（可选仪器补强；第一期以逆推为主）。
     *   验证建议：`03`/`05` 表 + Renode 寄存器影子；OI-004 不将示波器列为门禁。
     */
    adc_norm = (unsigned int)(g_phase6_adc_level & 0x0FFFU);
    encoder_norm = g_phase6_encoder_accum;
    if (encoder_norm < -255) {
        encoder_norm = -255;
    } else if (encoder_norm > 255) {
        encoder_norm = 255;
    }

    period = PHASE6_CCT_PERIOD_BASE + ((adc_norm * PHASE6_CCT_PERIOD_SPAN) / 4095U);
    compare = (period / 8U) + ((adc_norm * period) / (4095U * 2U));
    compare += (unsigned int)(encoder_norm + 255) / 4U;

    period = (unsigned int)Phase6_ClampU16(period, 600U, 4095U);
    compare_max = period - PHASE6_CCT_COMPARE_MARGIN;
    compare = (unsigned int)Phase6_ClampU16(compare, PHASE6_CCT_COMPARE_MIN, (unsigned short)compare_max);

    TMR1_PeriodHalfword_Write(period);
    TMR1_CompareHalfword_Write(compare);
    g_phase6_cct_apply_count++;
}

void HSI_FloatMath_ComputeRGBTriple(void)
{
    unsigned int hue_seed;
    unsigned int sat_seed;
    unsigned int val_seed;

    /*
     * First-pass HSI projection:
     * keep deterministic RGB triple generation for acceptance replay.
     * TODO(待证实): replace with measured HSI/FX transfer once verified.
     * 验证建议：实机固定 HSI 模式输入，记录 RGB 占空/光学输出联合映射。
     */
    hue_seed = ((unsigned int)g_phase6_encoder_accum + (unsigned int)g_phase6_adc_level) & 0x00FFU;
    sat_seed = ((unsigned int)g_phase6_adc_level >> 4) & 0x00FFU;
    val_seed = ((unsigned int)(g_phase6_encoder_accum + 512) >> 2) & 0x00FFU;
    g_phase6_hsi_rgb_shadow[0] = (unsigned char)hue_seed;
    g_phase6_hsi_rgb_shadow[1] = (unsigned char)((255U - hue_seed + sat_seed) >> 1);
    g_phase6_hsi_rgb_shadow[2] = (unsigned char)((val_seed + PHASE6_HSI_MIX_BIAS) & 0x00FFU);
}

void RGBTriple_PushPattern_TMR2Gated(void)
{
    unsigned int i;
    unsigned short base_r;
    unsigned short base_g;
    unsigned short base_b;

    base_r = (unsigned short)g_phase6_hsi_rgb_shadow[0];
    base_g = (unsigned short)g_phase6_hsi_rgb_shadow[1];
    base_b = (unsigned short)g_phase6_hsi_rgb_shadow[2];
    for (i = 0U; i < PHASE6_TMR2_DMA_PATTERN_WORDS; ++i) {
        unsigned short mix;
        mix = (unsigned short)((base_r * (i + 1U) + base_g * (PHASE6_TMR2_DMA_PATTERN_WORDS - i) + base_b) & 0x03FFU);
        g_phase6_tmr2_dma_pattern[i] = mix;
    }

    Init_TMR3_PWM_FanPB5();
    Init_DMA1Ch1_TMR2CcrDMA();

#ifndef PHASE6_HOST_TEST
    RCM->APB1CLKEN |= PHASE6_APB1EN_TMR2EN | PHASE6_APB1EN_TMR3EN;
    /* Tie fan PWM to RGB energy envelope: conservative, deterministic slope. */
    g_phase6_tmr3_cc2_shadow = (unsigned int)((base_r + base_g + base_b) / 3U);
    if (g_phase6_tmr3_cc2_shadow > 999U) {
        g_phase6_tmr3_cc2_shadow = 999U;
    }
    TMR3->CC2 = (uint16_t)g_phase6_tmr3_cc2_shadow;

    DMA1_Channel1->CHCFG &= ~PHASE6_DMA_CHCFG_EN;
    DMA1_Channel1->CHMADDR = (uint32_t)(g_phase6_tmr2_dma_pattern);
    DMA1_Channel1->CHNDATA = PHASE6_TMR2_DMA_PATTERN_WORDS;
    DMA1_Channel1->CHCFG |= PHASE6_DMA_CHCFG_EN;
    TMR2->CC3 = g_phase6_tmr2_dma_pattern[0];
#endif
    g_phase6_tmr2_dma_push_count++;
    g_phase6_tmr2_dma_last_words = PHASE6_TMR2_DMA_PATTERN_WORDS;
    g_phase6_tmr3_pwm_apply_count++;
    g_phase6_dma1_ch1_transfer_count++;
    g_phase6_hsi_apply_count++;
}

void LightOutput_PendingHSI_Dispatch(void)
{
    HSI_FloatMath_ComputeRGBTriple();
    RGBTriple_PushPattern_TMR2Gated();
}

void EncoderGated_LightOutputApplyChain(void)
{
    unsigned char should_apply;

    should_apply = 0U;
    if (g_phase6_encoder_accum != 0) {
        should_apply = 1U;
    }
    if (g_phase6_dimming_pending_ticks > 0U) {
        should_apply = 1U;
        g_phase6_dimming_pending_ticks--;
    }

    if (should_apply != 0U) {
        g_phase6_light_apply_count++;

        if (g_phase6_mode == PHASE6_UI_MODE_HSI) {
            LightOutput_PendingHSI_Dispatch();
        } else {
            CCT_Slew_TableSplit_TMR1Shadow();
        }
    }

    g_phase6_last_stage = PHASE6_STAGE_LIGHT_APPLY;
}

void UI_OLED_MainStateMachine(void)
{
    unsigned short event_code;

    g_phase6_ui_main_count++;
    if (Phase6_UiEventRouter_Consume((Phase6_UiEventRouter *)&g_phase6_ui_router, &event_code) != 0U) {
        g_phase6_ui_last_event_code = event_code;
        g_phase6_ui_event_dispatch_count = g_phase6_ui_router.dispatch_count;
        Phase6_UI_RenderFramebuffer(event_code);
        g_phase6_ui_frame_dirty = 1U;
    }
    g_phase6_last_stage = PHASE6_STAGE_UI_MAIN;
}

void UI_ModeRender_PostProcess(void)
{
    if ((g_phase6_ui_event_dispatch_count == 0U) &&
        (((g_phase6_call_ticks & 0x7U) == 0U) ||
         (g_phase6_ui_frame_dirty != 0U) ||
         (g_phase6_ui_framebuffer_checksum == 0U))) {
        Phase6_UI_RenderFramebuffer(g_phase6_ui_last_event_code);
        g_phase6_ui_frame_dirty = 1U;
    }
    g_phase6_ui_framebuffer_checksum = Phase6_UI_FramebufferChecksum();
    g_phase6_ui_render_count++;
    g_phase6_last_stage = PHASE6_STAGE_UI_RENDER;
}

void UI_SPI1_PumpEightFramebufferSlices(void)
{
    unsigned char force_refresh;
    unsigned char mainloop_request;

    /*
     * Dirty / main-loop request / periodic skip share one decision:
     * force_refresh uses PD-call tick delta (OEM: PD runs every 20 SysTick IRQs).
     */
    force_refresh = (unsigned char)((g_phase6_call_ticks - g_phase6_oled_last_flush_tick) >=
                                    PHASE6_OLED_FORCE_REFRESH_TICKS);
    mainloop_request = g_phase6_mainloop_spi_pump_pending;
    if ((g_phase6_ui_frame_dirty != 0U) || (force_refresh != 0U) || (mainloop_request != 0U)) {
        /*
         * Observability anchor:
         * Count trigger on branch entry, so GDB snapshots can prove cadence branch
         * selection even when SPI TX wait loops stall before full flush completion.
         */
        g_phase6_spi_flush_trigger_count++;
        g_phase6_spi_flush_bytes += Phase6_SPI1_FlushFramebufferSlices();
        g_phase6_spi_flush_slices += PHASE6_OLED_SLICE_COUNT;
        g_phase6_ui_flush_count++;
        g_phase6_oled_last_flush_tick = g_phase6_call_ticks;
        g_phase6_ui_frame_dirty = 0U;
        if (mainloop_request != 0U) {
            g_phase6_mainloop_spi_pump_consumed_count++;
            g_phase6_mainloop_spi_pump_pending = 0U;
        }
    } else {
        g_phase6_spi_flush_skip_count++;
    }
    g_phase6_has_pending_input = 0U;
    g_phase6_last_stage = PHASE6_STAGE_UI_SPI_FLUSH;
}

void PD_PowerAndProtocol_Block(void)
{
    if (g_phase6_ui_router_inited == 0U) {
        Init_TMR1_PWM_WarmColdADIM_PA8PA9_PB15();
        Phase6_UiEventRouter_Init((Phase6_UiEventRouter *)&g_phase6_ui_router, (unsigned short)PHASE6_UI_MODE_CCT);
        g_phase6_ui_router_inited = 1U;
        g_phase6_ui_frame_dirty = 1U;
        g_phase6_oled_last_flush_tick = 0U;
        g_phase6_last_stage = PHASE6_STAGE_IDLE;
    }

    if ((g_phase6_mode != PHASE6_UI_MODE_HSI) && (g_phase6_mode != PHASE6_UI_MODE_CCT)) {
        g_phase6_mode = PHASE6_UI_MODE_CCT;
        g_phase6_ui_mode_word = (unsigned short)PHASE6_UI_MODE_CCT;
    }

    g_phase6_call_ticks++;

    PD_AttachSequence_StateMachine();
    Input_5Channel_TableStateMachine();
    PD_GPIO_StateMachine();
    PD_HelperSeq_3000tick_ThenPc13Gpio();
    ProtocolLanes_BitExpandTick();
    Encoder_DispatchDelta_FromTMR4();
    EncoderGated_LightOutputApplyChain();
    UI_OLED_MainStateMachine();
    UI_ModeRender_PostProcess();
    UI_SPI1_PumpEightFramebufferSlices();
}

void Phase6_LightPd_ReadTraceSnapshot(Phase6_LightPd_TraceSnapshot *out_snapshot)
{
    if (out_snapshot == 0) {
        return;
    }

    out_snapshot->scheduler_ticks = g_phase6_scheduler_ticks;
    out_snapshot->last_stage_id = g_phase6_last_stage;
    out_snapshot->input_scan_count = g_phase6_input_scan_count;
    out_snapshot->usb_attached = g_phase6_usb_attached;
    out_snapshot->ui_mode_shadow = g_phase6_ui_mode_word;
    out_snapshot->ui_last_event_code = g_phase6_ui_last_event_code;
    out_snapshot->ui_event_dispatch_count = g_phase6_ui_event_dispatch_count;
    out_snapshot->pd_attach_count = g_phase6_pd_attach_count;
    out_snapshot->pd_attached_loop_count = g_phase6_pd_attached_loop_count;
    out_snapshot->pd_gpio_count = g_phase6_pd_gpio_count;
    out_snapshot->pd_attach_window_count = g_phase6_pd_attach_window_count;
    out_snapshot->protocol_lane_expand_count = g_phase6_protocol_lane_expand_count;
    out_snapshot->light_apply_count = g_phase6_light_apply_count;
    out_snapshot->cct_apply_count = g_phase6_cct_apply_count;
    out_snapshot->hsi_apply_count = g_phase6_hsi_apply_count;
    out_snapshot->dimming_update_count = g_phase6_dimming_update_count;
    out_snapshot->encoder_accum_shadow = (unsigned int)(g_phase6_encoder_accum & 0x0000FFFFU);
    out_snapshot->adc_level_shadow = g_phase6_adc_level;
    out_snapshot->tmr1_period_shadow = g_phase6_tmr1_period_shadow;
    out_snapshot->tmr1_compare_shadow = g_phase6_tmr1_compare_shadow;
    out_snapshot->hsi_rgb_shadow = Phase6_PackRgbShadow();
    out_snapshot->ui_main_count = g_phase6_ui_main_count;
    out_snapshot->ui_render_count = g_phase6_ui_render_count;
    out_snapshot->ui_flush_count = g_phase6_ui_flush_count;
    out_snapshot->ui_framebuffer_checksum = g_phase6_ui_framebuffer_checksum;
    out_snapshot->ui_descriptor_index = g_phase6_ui_descriptor_index;
    out_snapshot->spi_flush_bytes = g_phase6_spi_flush_bytes;
    out_snapshot->spi_flush_slices = g_phase6_spi_flush_slices;
    out_snapshot->spi_flush_trigger_count = g_phase6_spi_flush_trigger_count;
    out_snapshot->spi_flush_skip_count = g_phase6_spi_flush_skip_count;
    out_snapshot->mainloop_spi_pump_request_count = g_phase6_mainloop_spi_pump_request_count;
    out_snapshot->mainloop_spi_pump_consumed_count = g_phase6_mainloop_spi_pump_consumed_count;
    out_snapshot->tmr2_dma_push_count = g_phase6_tmr2_dma_push_count;
    out_snapshot->tmr2_dma_last_words = g_phase6_tmr2_dma_last_words;
    out_snapshot->tmr3_pwm_apply_count = g_phase6_tmr3_pwm_apply_count;
    out_snapshot->tmr3_cc2_shadow = g_phase6_tmr3_cc2_shadow;
    out_snapshot->dma1_ch1_transfer_count = g_phase6_dma1_ch1_transfer_count;
    out_snapshot->systick_entry_count = g_phase6_systick_entry_count;
    out_snapshot->pd_call_ticks = g_phase6_call_ticks;
}

