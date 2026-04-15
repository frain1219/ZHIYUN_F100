#include "phase6/phase6_acceptance.h"
#include "phase6/phase6_startup_chain.h"
#include "phase6/phase6_usb_device.h"
#include "phase6/phase6_usb_guard.h"

#define PHASE6_ACCEPT_BLOCKED_TIMEOUT_LOOPS 12000U

static volatile Phase6_AcceptanceReport g_phase6_acceptance_report;

void Phase6_Acceptance_Init(void)
{
    g_phase6_acceptance_report.bhv001_power_loop = PHASE6_ACCEPT_NOT_TESTED;
    g_phase6_acceptance_report.bhv002_cct_path = PHASE6_ACCEPT_NOT_TESTED;
    g_phase6_acceptance_report.bhv003_hsi_path = PHASE6_ACCEPT_NOT_TESTED;
    g_phase6_acceptance_report.bhv004_dimming_path = PHASE6_ACCEPT_NOT_TESTED;
    g_phase6_acceptance_report.bhv005_usb_pd_attach_path = PHASE6_ACCEPT_NOT_TESTED;
    g_phase6_acceptance_report.cct_apply_count = 0U;
    g_phase6_acceptance_report.hsi_apply_count = 0U;
    g_phase6_acceptance_report.dimming_update_count = 0U;
    g_phase6_acceptance_report.light_apply_count = 0U;
    g_phase6_acceptance_report.usb_attached = 0U;
    g_phase6_acceptance_report.ui_mode_shadow = 0U;
    g_phase6_acceptance_report.ui_last_event_code = 0U;
    g_phase6_acceptance_report.ui_event_dispatch_count = 0U;
    g_phase6_acceptance_report.usb_insert_event_count = 0U;
    g_phase6_acceptance_report.usb_upgrade_block_count = 0U;
    g_phase6_acceptance_report.usb_upgrade_enabled = 0U;
    g_phase6_acceptance_report.pd_attach_count = 0U;
    g_phase6_acceptance_report.pd_attached_loop_count = 0U;
    g_phase6_acceptance_report.pd_gpio_count = 0U;
    g_phase6_acceptance_report.pd_attach_window_count = 0U;
    g_phase6_acceptance_report.protocol_lane_expand_count = 0U;
    g_phase6_acceptance_report.encoder_accum_shadow = 0U;
    g_phase6_acceptance_report.adc_level_shadow = 0U;
    g_phase6_acceptance_report.tmr1_period_shadow = 0U;
    g_phase6_acceptance_report.tmr1_compare_shadow = 0U;
    g_phase6_acceptance_report.hsi_rgb_shadow = 0U;
    g_phase6_acceptance_report.ui_framebuffer_checksum = 0U;
    g_phase6_acceptance_report.ui_descriptor_index = 0U;
    g_phase6_acceptance_report.spi_flush_bytes = 0U;
    g_phase6_acceptance_report.spi_flush_slices = 0U;
    g_phase6_acceptance_report.spi_flush_trigger_count = 0U;
    g_phase6_acceptance_report.spi_flush_skip_count = 0U;
    g_phase6_acceptance_report.mainloop_spi_pump_request_count = 0U;
    g_phase6_acceptance_report.mainloop_spi_pump_consumed_count = 0U;
    g_phase6_acceptance_report.tmr2_dma_push_count = 0U;
    g_phase6_acceptance_report.tmr2_dma_last_words = 0U;
    g_phase6_acceptance_report.tmr3_pwm_apply_count = 0U;
    g_phase6_acceptance_report.tmr3_cc2_shadow = 0U;
    g_phase6_acceptance_report.dma1_ch1_transfer_count = 0U;
    g_phase6_acceptance_report.main_boot_phase_counter8 = 0U;
    g_phase6_acceptance_report.main_iwdg_reload_count = 0U;
    g_phase6_acceptance_report.usb_upgrade_policy_guarded = 0U;
    g_phase6_acceptance_report.loop_sample_count = 0U;
    g_phase6_acceptance_report.last_observed_stage = 0U;
    g_phase6_acceptance_report.startup_chain_completed = 0U;
    g_phase6_acceptance_report.startup_step_count = 0U;
    g_phase6_acceptance_report.startup_last_step_id = 0U;
    g_phase6_acceptance_report.startup_magic55aa_value = 0U;
    g_phase6_acceptance_report.startup_magic55aa_matched = 0U;
    g_phase6_acceptance_report.startup_iwdt_started = 0U;
    g_phase6_acceptance_report.startup_iwdt_reload_shadow = 0U;
    g_phase6_acceptance_report.startup_clock_hsi_ready = 0U;
    g_phase6_acceptance_report.startup_clock_pll_ready = 0U;
    g_phase6_acceptance_report.startup_clock_pll_selected = 0U;
    g_phase6_acceptance_report.startup_fmc_latency_shadow = 0U;
    g_phase6_acceptance_report.startup_sysclk_hz_shadow = 0U;
    g_phase6_acceptance_report.startup_systick_reload_shadow = 0U;
    g_phase6_acceptance_report.usb_device_initialized = 0U;
    g_phase6_acceptance_report.usb_device_connected = 0U;
    g_phase6_acceptance_report.usb_device_context_ptr_shadow = 0U;
    g_phase6_acceptance_report.usb_device_ep0_max_packet_shadow = 0U;
    g_phase6_acceptance_report.usb_device_descriptors_ready = 0U;
    g_phase6_acceptance_report.usb_device_sof_ticks = 0U;
    g_phase6_acceptance_report.usb_ep0_setup_count = 0U;
    g_phase6_acceptance_report.usb_ep0_in_count = 0U;
    g_phase6_acceptance_report.usb_ep0_out_count = 0U;
    g_phase6_acceptance_report.usb_upgrade_guard_initialized = 0U;
    g_phase6_acceptance_report.usb_upgrade_preflight_ready = 0U;
    g_phase6_acceptance_report.usb_upgrade_preflight_fail_count = 0U;
    g_phase6_acceptance_report.usb_upgrade_flash_write_attempt_count = 0U;
}

void Phase6_Acceptance_OnLoopSample(const Phase6_LightPd_TraceSnapshot *trace)
{
    Phase6_StartupTrace startup_trace;
    Phase6_UsbDeviceTrace usb_trace;
    Phase6_UsbUpgradeGuardTrace usb_guard_trace;

    if (trace == 0) {
        return;
    }

    Phase6_StartupChain_ReadTrace(&startup_trace);
    Phase6_UsbDevice_ReadTrace(&usb_trace);
    Phase6_USB_UpgradePath_ReadTrace(&usb_guard_trace);

    g_phase6_acceptance_report.loop_sample_count++;
    g_phase6_acceptance_report.last_observed_stage = trace->last_stage_id;
    g_phase6_acceptance_report.startup_chain_completed = startup_trace.completed;
    g_phase6_acceptance_report.startup_step_count = startup_trace.step_count;
    g_phase6_acceptance_report.startup_last_step_id = startup_trace.last_step_id;
    g_phase6_acceptance_report.startup_magic55aa_value = startup_trace.magic55aa_value;
    g_phase6_acceptance_report.startup_magic55aa_matched = startup_trace.magic55aa_matched;
    g_phase6_acceptance_report.startup_iwdt_started = startup_trace.iwdt_started;
    g_phase6_acceptance_report.startup_iwdt_reload_shadow = startup_trace.iwdt_reload_shadow;
    g_phase6_acceptance_report.startup_clock_hsi_ready = startup_trace.clock_hsi_ready;
    g_phase6_acceptance_report.startup_clock_pll_ready = startup_trace.clock_pll_ready;
    g_phase6_acceptance_report.startup_clock_pll_selected = startup_trace.clock_pll_selected;
    g_phase6_acceptance_report.startup_fmc_latency_shadow = startup_trace.fmc_latency_shadow;
    g_phase6_acceptance_report.startup_sysclk_hz_shadow = startup_trace.sysclk_hz_shadow;
    g_phase6_acceptance_report.startup_systick_reload_shadow = startup_trace.systick_reload_shadow;
    g_phase6_acceptance_report.cct_apply_count = trace->cct_apply_count;
    g_phase6_acceptance_report.hsi_apply_count = trace->hsi_apply_count;
    g_phase6_acceptance_report.dimming_update_count = trace->dimming_update_count;
    g_phase6_acceptance_report.light_apply_count = trace->light_apply_count;
    g_phase6_acceptance_report.usb_attached = trace->usb_attached;
    g_phase6_acceptance_report.ui_mode_shadow = trace->ui_mode_shadow;
    g_phase6_acceptance_report.ui_last_event_code = trace->ui_last_event_code;
    g_phase6_acceptance_report.ui_event_dispatch_count = trace->ui_event_dispatch_count;
    g_phase6_acceptance_report.pd_attach_count = trace->pd_attach_count;
    g_phase6_acceptance_report.pd_attached_loop_count = trace->pd_attached_loop_count;
    g_phase6_acceptance_report.pd_gpio_count = trace->pd_gpio_count;
    g_phase6_acceptance_report.pd_attach_window_count = trace->pd_attach_window_count;
    g_phase6_acceptance_report.protocol_lane_expand_count = trace->protocol_lane_expand_count;
    g_phase6_acceptance_report.encoder_accum_shadow = trace->encoder_accum_shadow;
    g_phase6_acceptance_report.adc_level_shadow = trace->adc_level_shadow;
    g_phase6_acceptance_report.tmr1_period_shadow = trace->tmr1_period_shadow;
    g_phase6_acceptance_report.tmr1_compare_shadow = trace->tmr1_compare_shadow;
    g_phase6_acceptance_report.hsi_rgb_shadow = trace->hsi_rgb_shadow;
    g_phase6_acceptance_report.ui_framebuffer_checksum = trace->ui_framebuffer_checksum;
    g_phase6_acceptance_report.ui_descriptor_index = trace->ui_descriptor_index;
    g_phase6_acceptance_report.spi_flush_bytes = trace->spi_flush_bytes;
    g_phase6_acceptance_report.spi_flush_slices = trace->spi_flush_slices;
    g_phase6_acceptance_report.spi_flush_trigger_count = trace->spi_flush_trigger_count;
    g_phase6_acceptance_report.spi_flush_skip_count = trace->spi_flush_skip_count;
    g_phase6_acceptance_report.mainloop_spi_pump_request_count = trace->mainloop_spi_pump_request_count;
    g_phase6_acceptance_report.mainloop_spi_pump_consumed_count = trace->mainloop_spi_pump_consumed_count;
    g_phase6_acceptance_report.tmr2_dma_push_count = trace->tmr2_dma_push_count;
    g_phase6_acceptance_report.tmr2_dma_last_words = trace->tmr2_dma_last_words;
    g_phase6_acceptance_report.tmr3_pwm_apply_count = trace->tmr3_pwm_apply_count;
    g_phase6_acceptance_report.tmr3_cc2_shadow = trace->tmr3_cc2_shadow;
    g_phase6_acceptance_report.dma1_ch1_transfer_count = trace->dma1_ch1_transfer_count;
    g_phase6_acceptance_report.main_boot_phase_counter8 = trace->main_boot_phase_counter8;
    g_phase6_acceptance_report.main_iwdg_reload_count = trace->main_iwdg_reload_count;
    g_phase6_acceptance_report.usb_device_initialized = usb_trace.initialized;
    g_phase6_acceptance_report.usb_device_connected = usb_trace.connected;
    g_phase6_acceptance_report.usb_device_context_ptr_shadow = usb_trace.context_ptr_shadow;
    g_phase6_acceptance_report.usb_device_ep0_max_packet_shadow = usb_trace.ep0_max_packet_shadow;
    g_phase6_acceptance_report.usb_device_descriptors_ready = usb_trace.descriptors_ready;
    g_phase6_acceptance_report.usb_device_sof_ticks = usb_trace.sof_ticks;
    g_phase6_acceptance_report.usb_ep0_setup_count = usb_trace.ep0_setup_count;
    g_phase6_acceptance_report.usb_ep0_in_count = usb_trace.ep0_in_count;
    g_phase6_acceptance_report.usb_ep0_out_count = usb_trace.ep0_out_count;
    g_phase6_acceptance_report.usb_upgrade_block_count = Phase6_USB_UpgradePath_ReadBlockedCount();
    g_phase6_acceptance_report.usb_upgrade_enabled = Phase6_USB_UpgradePath_ReadEnableFlag();
    g_phase6_acceptance_report.usb_upgrade_policy_guarded =
        (g_phase6_acceptance_report.usb_upgrade_enabled == 0U) &&
        (g_phase6_acceptance_report.usb_upgrade_block_count > 0U);
    g_phase6_acceptance_report.usb_upgrade_guard_initialized = usb_guard_trace.guard_initialized;
    g_phase6_acceptance_report.usb_upgrade_preflight_ready = usb_guard_trace.preflight_ready;
    g_phase6_acceptance_report.usb_upgrade_preflight_fail_count = usb_guard_trace.preflight_fail_count;
    g_phase6_acceptance_report.usb_upgrade_flash_write_attempt_count = usb_guard_trace.flash_write_attempt_count;

    /*
     * PH6-BHV-001:
     * Main loop should keep running and repeatedly complete scheduled chain.
     */
    if ((trace->scheduler_ticks > 0U) &&
        (startup_trace.completed != 0U) &&
        (startup_trace.clock_hsi_ready != 0U) &&
        (startup_trace.clock_pll_ready != 0U) &&
        (startup_trace.clock_pll_selected != 0U) &&
        (startup_trace.systick_reload_shadow > 0U) &&
        (trace->last_stage_id == (unsigned int)PHASE6_STAGE_UI_SPI_FLUSH) &&
        (trace->ui_main_count > 0U) &&
        (trace->ui_render_count > 0U) &&
        (trace->ui_flush_count > 0U) &&
        (trace->mainloop_spi_pump_request_count > 0U) &&
        (trace->mainloop_spi_pump_consumed_count > 0U) &&
        (trace->spi_flush_slices >= 8U) &&
        (trace->spi_flush_bytes >= (8U * (3U + 96U))) &&
        (trace->ui_framebuffer_checksum != 0U) &&
        (trace->main_iwdg_reload_count > 0U) &&
        (trace->main_boot_phase_counter8 < 8U)) {
        g_phase6_acceptance_report.bhv001_power_loop = PHASE6_ACCEPT_PASS;
    }

    /*
     * PH6-BHV-002 / 003 / 004:
     * First-pass software evidence:
     * - CCT path: cct_apply_count > 0
     * - HSI path: hsi_apply_count > 0
     * - Dimming path: dimming updates and light apply both observed
     */
    if ((trace->cct_apply_count > 0U) &&
        (trace->tmr1_period_shadow > trace->tmr1_compare_shadow) &&
        (trace->tmr1_compare_shadow > 0U)) {
        g_phase6_acceptance_report.bhv002_cct_path = PHASE6_ACCEPT_PASS;
    }

    if ((trace->hsi_apply_count > 0U) &&
        (trace->hsi_rgb_shadow != 0U) &&
        (trace->tmr2_dma_push_count > 0U) &&
        (trace->tmr2_dma_last_words >= 16U) &&
        (trace->tmr3_pwm_apply_count > 0U) &&
        (trace->dma1_ch1_transfer_count > 0U) &&
        (trace->tmr3_cc2_shadow <= 999U)) {
        g_phase6_acceptance_report.bhv003_hsi_path = PHASE6_ACCEPT_PASS;
    }

    if ((trace->light_apply_count > 0U) && (trace->dimming_update_count > 0U)) {
        g_phase6_acceptance_report.bhv004_dimming_path = PHASE6_ACCEPT_PASS;
    }

    /*
     * PH6-BHV-005:
     * Only mark pass after USB insertion event is observed and PD path keeps running.
     * This is a software trace gate, not a hardware-level proof.
     */
    if ((g_phase6_acceptance_report.usb_insert_event_count > 0U) &&
        (trace->usb_attached > 0U) &&
        (trace->pd_attach_count > 0U) &&
        (trace->pd_attached_loop_count > 0U) &&
        (trace->pd_gpio_count > 0U) &&
        (trace->pd_attach_window_count > 0U) &&
        (trace->protocol_lane_expand_count > 0U) &&
        (usb_trace.initialized > 0U) &&
        (usb_trace.connected > 0U) &&
        (usb_trace.context_ptr_shadow != 0U) &&
        (usb_trace.ep0_max_packet_shadow >= 8U) &&
        (usb_trace.descriptors_ready > 0U) &&
        (usb_trace.ep0_setup_count > 0U) &&
        (usb_trace.ep0_in_count > 0U) &&
        (g_phase6_acceptance_report.usb_upgrade_policy_guarded != 0U)) {
        g_phase6_acceptance_report.bhv005_usb_pd_attach_path = PHASE6_ACCEPT_PASS;
    }

    if (g_phase6_acceptance_report.usb_upgrade_enabled != 0U) {
        g_phase6_acceptance_report.bhv005_usb_pd_attach_path = PHASE6_ACCEPT_FAIL;
    }

    if (g_phase6_acceptance_report.loop_sample_count > PHASE6_ACCEPT_BLOCKED_TIMEOUT_LOOPS) {
        if (g_phase6_acceptance_report.bhv002_cct_path == PHASE6_ACCEPT_NOT_TESTED) {
            g_phase6_acceptance_report.bhv002_cct_path = PHASE6_ACCEPT_BLOCKED;
        }
        if (g_phase6_acceptance_report.bhv003_hsi_path == PHASE6_ACCEPT_NOT_TESTED) {
            g_phase6_acceptance_report.bhv003_hsi_path = PHASE6_ACCEPT_BLOCKED;
        }
        if (g_phase6_acceptance_report.bhv004_dimming_path == PHASE6_ACCEPT_NOT_TESTED) {
            g_phase6_acceptance_report.bhv004_dimming_path = PHASE6_ACCEPT_BLOCKED;
        }
        if (g_phase6_acceptance_report.bhv005_usb_pd_attach_path == PHASE6_ACCEPT_NOT_TESTED) {
            g_phase6_acceptance_report.bhv005_usb_pd_attach_path = PHASE6_ACCEPT_BLOCKED;
        }
    }
}

void Phase6_Acceptance_NotifyUsbInserted(void)
{
    g_phase6_acceptance_report.usb_insert_event_count++;
}

void Phase6_Acceptance_ReadReport(Phase6_AcceptanceReport *out_report)
{
    if (out_report == 0) {
        return;
    }

    out_report->bhv001_power_loop = g_phase6_acceptance_report.bhv001_power_loop;
    out_report->bhv002_cct_path = g_phase6_acceptance_report.bhv002_cct_path;
    out_report->bhv003_hsi_path = g_phase6_acceptance_report.bhv003_hsi_path;
    out_report->bhv004_dimming_path = g_phase6_acceptance_report.bhv004_dimming_path;
    out_report->bhv005_usb_pd_attach_path = g_phase6_acceptance_report.bhv005_usb_pd_attach_path;
    out_report->cct_apply_count = g_phase6_acceptance_report.cct_apply_count;
    out_report->hsi_apply_count = g_phase6_acceptance_report.hsi_apply_count;
    out_report->dimming_update_count = g_phase6_acceptance_report.dimming_update_count;
    out_report->light_apply_count = g_phase6_acceptance_report.light_apply_count;
    out_report->usb_attached = g_phase6_acceptance_report.usb_attached;
    out_report->ui_mode_shadow = g_phase6_acceptance_report.ui_mode_shadow;
    out_report->ui_last_event_code = g_phase6_acceptance_report.ui_last_event_code;
    out_report->ui_event_dispatch_count = g_phase6_acceptance_report.ui_event_dispatch_count;
    out_report->usb_insert_event_count = g_phase6_acceptance_report.usb_insert_event_count;
    out_report->usb_upgrade_block_count = g_phase6_acceptance_report.usb_upgrade_block_count;
    out_report->usb_upgrade_enabled = g_phase6_acceptance_report.usb_upgrade_enabled;
    out_report->pd_attach_count = g_phase6_acceptance_report.pd_attach_count;
    out_report->pd_attached_loop_count = g_phase6_acceptance_report.pd_attached_loop_count;
    out_report->pd_gpio_count = g_phase6_acceptance_report.pd_gpio_count;
    out_report->pd_attach_window_count = g_phase6_acceptance_report.pd_attach_window_count;
    out_report->protocol_lane_expand_count = g_phase6_acceptance_report.protocol_lane_expand_count;
    out_report->encoder_accum_shadow = g_phase6_acceptance_report.encoder_accum_shadow;
    out_report->adc_level_shadow = g_phase6_acceptance_report.adc_level_shadow;
    out_report->tmr1_period_shadow = g_phase6_acceptance_report.tmr1_period_shadow;
    out_report->tmr1_compare_shadow = g_phase6_acceptance_report.tmr1_compare_shadow;
    out_report->hsi_rgb_shadow = g_phase6_acceptance_report.hsi_rgb_shadow;
    out_report->ui_framebuffer_checksum = g_phase6_acceptance_report.ui_framebuffer_checksum;
    out_report->ui_descriptor_index = g_phase6_acceptance_report.ui_descriptor_index;
    out_report->spi_flush_bytes = g_phase6_acceptance_report.spi_flush_bytes;
    out_report->spi_flush_slices = g_phase6_acceptance_report.spi_flush_slices;
    out_report->spi_flush_trigger_count = g_phase6_acceptance_report.spi_flush_trigger_count;
    out_report->spi_flush_skip_count = g_phase6_acceptance_report.spi_flush_skip_count;
    out_report->mainloop_spi_pump_request_count = g_phase6_acceptance_report.mainloop_spi_pump_request_count;
    out_report->mainloop_spi_pump_consumed_count = g_phase6_acceptance_report.mainloop_spi_pump_consumed_count;
    out_report->tmr2_dma_push_count = g_phase6_acceptance_report.tmr2_dma_push_count;
    out_report->tmr2_dma_last_words = g_phase6_acceptance_report.tmr2_dma_last_words;
    out_report->tmr3_pwm_apply_count = g_phase6_acceptance_report.tmr3_pwm_apply_count;
    out_report->tmr3_cc2_shadow = g_phase6_acceptance_report.tmr3_cc2_shadow;
    out_report->dma1_ch1_transfer_count = g_phase6_acceptance_report.dma1_ch1_transfer_count;
    out_report->main_boot_phase_counter8 = g_phase6_acceptance_report.main_boot_phase_counter8;
    out_report->main_iwdg_reload_count = g_phase6_acceptance_report.main_iwdg_reload_count;
    out_report->usb_upgrade_policy_guarded = g_phase6_acceptance_report.usb_upgrade_policy_guarded;
    out_report->loop_sample_count = g_phase6_acceptance_report.loop_sample_count;
    out_report->last_observed_stage = g_phase6_acceptance_report.last_observed_stage;
    out_report->startup_chain_completed = g_phase6_acceptance_report.startup_chain_completed;
    out_report->startup_step_count = g_phase6_acceptance_report.startup_step_count;
    out_report->startup_last_step_id = g_phase6_acceptance_report.startup_last_step_id;
    out_report->startup_magic55aa_value = g_phase6_acceptance_report.startup_magic55aa_value;
    out_report->startup_magic55aa_matched = g_phase6_acceptance_report.startup_magic55aa_matched;
    out_report->startup_iwdt_started = g_phase6_acceptance_report.startup_iwdt_started;
    out_report->startup_iwdt_reload_shadow = g_phase6_acceptance_report.startup_iwdt_reload_shadow;
    out_report->startup_clock_hsi_ready = g_phase6_acceptance_report.startup_clock_hsi_ready;
    out_report->startup_clock_pll_ready = g_phase6_acceptance_report.startup_clock_pll_ready;
    out_report->startup_clock_pll_selected = g_phase6_acceptance_report.startup_clock_pll_selected;
    out_report->startup_fmc_latency_shadow = g_phase6_acceptance_report.startup_fmc_latency_shadow;
    out_report->startup_sysclk_hz_shadow = g_phase6_acceptance_report.startup_sysclk_hz_shadow;
    out_report->startup_systick_reload_shadow = g_phase6_acceptance_report.startup_systick_reload_shadow;
    out_report->usb_device_initialized = g_phase6_acceptance_report.usb_device_initialized;
    out_report->usb_device_connected = g_phase6_acceptance_report.usb_device_connected;
    out_report->usb_device_context_ptr_shadow = g_phase6_acceptance_report.usb_device_context_ptr_shadow;
    out_report->usb_device_ep0_max_packet_shadow = g_phase6_acceptance_report.usb_device_ep0_max_packet_shadow;
    out_report->usb_device_descriptors_ready = g_phase6_acceptance_report.usb_device_descriptors_ready;
    out_report->usb_device_sof_ticks = g_phase6_acceptance_report.usb_device_sof_ticks;
    out_report->usb_ep0_setup_count = g_phase6_acceptance_report.usb_ep0_setup_count;
    out_report->usb_ep0_in_count = g_phase6_acceptance_report.usb_ep0_in_count;
    out_report->usb_ep0_out_count = g_phase6_acceptance_report.usb_ep0_out_count;
    out_report->usb_upgrade_guard_initialized = g_phase6_acceptance_report.usb_upgrade_guard_initialized;
    out_report->usb_upgrade_preflight_ready = g_phase6_acceptance_report.usb_upgrade_preflight_ready;
    out_report->usb_upgrade_preflight_fail_count = g_phase6_acceptance_report.usb_upgrade_preflight_fail_count;
    out_report->usb_upgrade_flash_write_attempt_count = g_phase6_acceptance_report.usb_upgrade_flash_write_attempt_count;
}
