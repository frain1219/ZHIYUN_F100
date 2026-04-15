#ifndef PHASE6_ACCEPTANCE_H
#define PHASE6_ACCEPTANCE_H

#include "phase6/phase6_light_pd_skeleton.h"

typedef enum {
    PHASE6_ACCEPT_NOT_TESTED = 0,
    PHASE6_ACCEPT_PASS = 1,
    PHASE6_ACCEPT_FAIL = 2,
    PHASE6_ACCEPT_BLOCKED = 3
} Phase6_AcceptanceStatus;

typedef struct {
    Phase6_AcceptanceStatus bhv001_power_loop;
    Phase6_AcceptanceStatus bhv002_cct_path;
    Phase6_AcceptanceStatus bhv003_hsi_path;
    Phase6_AcceptanceStatus bhv004_dimming_path;
    Phase6_AcceptanceStatus bhv005_usb_pd_attach_path;
    unsigned int cct_apply_count;
    unsigned int hsi_apply_count;
    unsigned int dimming_update_count;
    unsigned int light_apply_count;
    unsigned int usb_attached;
    unsigned int ui_mode_shadow;
    unsigned int ui_last_event_code;
    unsigned int ui_event_dispatch_count;
    unsigned int usb_insert_event_count;
    unsigned int usb_upgrade_block_count;
    unsigned int usb_upgrade_enabled;
    unsigned int pd_attach_count;
    unsigned int pd_attached_loop_count;
    unsigned int pd_gpio_count;
    unsigned int pd_attach_window_count;
    unsigned int protocol_lane_expand_count;
    unsigned int encoder_accum_shadow;
    unsigned int adc_level_shadow;
    unsigned int tmr1_period_shadow;
    unsigned int tmr1_compare_shadow;
    unsigned int hsi_rgb_shadow;
    unsigned int ui_framebuffer_checksum;
    unsigned int ui_descriptor_index;
    unsigned int spi_flush_bytes;
    unsigned int spi_flush_slices;
    unsigned int spi_flush_trigger_count;
    unsigned int spi_flush_skip_count;
    unsigned int mainloop_spi_pump_request_count;
    unsigned int mainloop_spi_pump_consumed_count;
    unsigned int tmr2_dma_push_count;
    unsigned int tmr2_dma_last_words;
    unsigned int tmr3_pwm_apply_count;
    unsigned int tmr3_cc2_shadow;
    unsigned int dma1_ch1_transfer_count;
    unsigned int main_boot_phase_counter8;
    unsigned int main_iwdg_reload_count;
    unsigned int usb_upgrade_policy_guarded;
    unsigned int loop_sample_count;
    unsigned int last_observed_stage;
    unsigned int startup_chain_completed;
    unsigned int startup_step_count;
    unsigned int startup_last_step_id;
    unsigned int startup_magic55aa_value;
    unsigned int startup_magic55aa_matched;
    unsigned int startup_iwdt_started;
    unsigned int startup_iwdt_reload_shadow;
    unsigned int startup_clock_hsi_ready;
    unsigned int startup_clock_pll_ready;
    unsigned int startup_clock_pll_selected;
    unsigned int startup_fmc_latency_shadow;
    unsigned int startup_sysclk_hz_shadow;
    unsigned int startup_systick_reload_shadow;
    unsigned int usb_device_initialized;
    unsigned int usb_device_connected;
    unsigned int usb_device_context_ptr_shadow;
    unsigned int usb_device_ep0_max_packet_shadow;
    unsigned int usb_device_descriptors_ready;
    unsigned int usb_device_sof_ticks;
    unsigned int usb_ep0_setup_count;
    unsigned int usb_ep0_in_count;
    unsigned int usb_ep0_out_count;
    unsigned int usb_upgrade_guard_initialized;
    unsigned int usb_upgrade_preflight_ready;
    unsigned int usb_upgrade_preflight_fail_count;
    unsigned int usb_upgrade_flash_write_attempt_count;
} Phase6_AcceptanceReport;

void Phase6_Acceptance_Init(void);
void Phase6_Acceptance_OnLoopSample(const Phase6_LightPd_TraceSnapshot *trace);
void Phase6_Acceptance_NotifyUsbInserted(void);
void Phase6_Acceptance_ReadReport(Phase6_AcceptanceReport *out_report);

#endif /* PHASE6_ACCEPTANCE_H */
