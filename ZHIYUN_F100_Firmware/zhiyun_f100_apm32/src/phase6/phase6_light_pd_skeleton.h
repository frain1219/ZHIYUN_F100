#ifndef PHASE6_LIGHT_PD_SKELETON_H
#define PHASE6_LIGHT_PD_SKELETON_H

/*
 * Phase 6 behavior skeleton.
 * Naming follows converged Ghidra symbols from 05-MMIO-v1 and task book handoff.
 * This header defines only control-flow level interfaces; no undocumented MMIO writes.
 */

typedef enum {
    PHASE6_UI_MODE_CCT = 2,
    PHASE6_UI_MODE_HSI = 3
} Phase6_UiMode;

typedef enum {
    PHASE6_STAGE_IDLE = 0,
    PHASE6_STAGE_PD_ATTACH = 1,
    PHASE6_STAGE_INPUT_SCAN = 2,
    PHASE6_STAGE_PD_GPIO = 3,
    PHASE6_STAGE_PD_ATTACH_HELPER = 4,
    PHASE6_STAGE_PROTOCOL_EXPAND = 5,
    PHASE6_STAGE_ENCODER_DISPATCH = 6,
    PHASE6_STAGE_LIGHT_APPLY = 7,
    PHASE6_STAGE_UI_MAIN = 8,
    PHASE6_STAGE_UI_RENDER = 9,
    PHASE6_STAGE_UI_SPI_FLUSH = 10
} Phase6_StageId;

typedef struct {
    int encoder_delta;
    unsigned short adc_level;
    unsigned char has_adc_update;
    Phase6_UiMode requested_mode;
    unsigned char has_mode_request;
    unsigned char usb_inserted;
} Phase6_InputEvent;

void PD_PowerAndProtocol_Block(void);

void PD_AttachSequence_StateMachine(void);
void Input_5Channel_TableStateMachine(void);
void PD_GPIO_StateMachine(void);
void PD_HelperSeq_3000tick_ThenPc13Gpio(void);
void ProtocolLanes_BitExpandTick(void);
void Encoder_DispatchDelta_FromTMR4(void);
void EncoderGated_LightOutputApplyChain(void);
void UI_OLED_MainStateMachine(void);
void UI_ModeRender_PostProcess(void);
void UI_SPI1_PumpEightFramebufferSlices(void);

void CCT_Slew_TableSplit_TMR1Shadow(void);
void LightOutput_PendingHSI_Dispatch(void);
void HSI_FloatMath_ComputeRGBTriple(void);
void RGBTriple_PushPattern_TMR2Gated(void);
void TMR1_PeriodHalfword_Write(unsigned short value);
void TMR1_CompareHalfword_Write(unsigned short value);
void Phase6_MainLoop_RequestSpiPump(void);

typedef struct {
    unsigned int scheduler_ticks;
    unsigned int last_stage_id;
    unsigned int input_scan_count;
    unsigned int usb_attached;
    unsigned int ui_mode_shadow;
    unsigned int ui_last_event_code;
    unsigned int ui_event_dispatch_count;
    unsigned int pd_attach_count;
    unsigned int pd_attached_loop_count;
    unsigned int pd_gpio_count;
    unsigned int pd_attach_window_count;
    unsigned int protocol_lane_expand_count;
    unsigned int light_apply_count;
    unsigned int cct_apply_count;
    unsigned int hsi_apply_count;
    unsigned int dimming_update_count;
    unsigned int encoder_accum_shadow;
    unsigned int adc_level_shadow;
    unsigned int tmr1_period_shadow;
    unsigned int tmr1_compare_shadow;
    unsigned int hsi_rgb_shadow;
    unsigned int ui_main_count;
    unsigned int ui_render_count;
    unsigned int ui_flush_count;
    unsigned int ui_framebuffer_checksum;
    unsigned int ui_descriptor_index;
    unsigned int spi_flush_bytes;
    unsigned int spi_flush_slices;
    unsigned int spi_flush_trigger_count;
    unsigned int spi_flush_skip_count;
    unsigned int tmr2_dma_push_count;
    unsigned int tmr2_dma_last_words;
    unsigned int tmr3_pwm_apply_count;
    unsigned int tmr3_cc2_shadow;
    unsigned int dma1_ch1_transfer_count;
    unsigned int mainloop_spi_pump_request_count;
    unsigned int mainloop_spi_pump_consumed_count;
    unsigned int main_boot_phase_counter8;
    unsigned int main_iwdg_reload_count;
    unsigned int systick_entry_count;
    unsigned int pd_call_ticks;
} Phase6_LightPd_TraceSnapshot;

void Phase6_LightPd_ReadTraceSnapshot(Phase6_LightPd_TraceSnapshot *out_snapshot);
void Phase6_LightPd_SetInputEvent(const Phase6_InputEvent *event);
void Phase6_LightPd_InjectUsbInsert(void);

#endif /* PHASE6_LIGHT_PD_SKELETON_H */

