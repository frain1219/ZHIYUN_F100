# STM32F407 Probe Capability Matrix

## Scope

This matrix is focused on reverse engineering support for the F103 production firmware.

## Implemented

- `PING` basic link check
- `GET_STATUS` digital pin snapshot
- `SELF_CHECK` software-only readiness summary (pin roles, EXTI watch list)
- `GET_CAPS` capability discovery with implemented/not_implemented lists
- `GET_STATS` runtime counters and drop visibility
- `RESET_STATS` clear counters
- `HELP` protocol quick help, including emulation subcommands
- `SUBSCRIBE ON|OFF` event streaming
- `LIST_PINS` pin table dump
- `GPIO_SET <pin> <0|1> [hold_ms]` controlled digital drive
- `GPIO_RELEASE <pin>` return to high impedance input
- `GPIO_PULSE <pin> <pulse_us>` microsecond pulse scheduling
- `PIN_CFG <pin> <pp|od> <none|up|down> <low|high>` safe output profile
- `SEQ_PLAY <script>` script playback, format: `pin,level,delay_us;...`
- `SEQ_ABORT` stop active script
- `SCENE_LIST` list built-in reverse scenes
- `SCENE_RUN <scene>` start built-in scene
- `SCENE_STOP` stop scene/script playback
- `ADC_READ <PF3|PF4|PF5|PF6>` one-shot ADC read
- `ADC_STREAM_START [interval_ms]` periodic ADC events
- `ADC_STREAM_STOP` stop ADC streaming
- `ADC_TRIGGER <pin> <low> <high> <on|off>` threshold trigger events
- `EMU_PL5500_STAT <pin> <OFF|TRICKLE|FAST|DONE>` emulate PL5500 STAT output logic
- `EMU_MP3398_BIND <en_pin> <pwm_pin> <adim_pin>` bind MP3398 logical pins
- `EMU_MP3398_PWM <freq_hz> <duty_pct>` configure PWM waveform
- `EMU_MP3398_ADIM <freq_hz> <duty_pct>` configure ADIM waveform
- `EMU_MP3398_ENABLE <0|1>` enable or disable MP3398 emulation
- `I2C_SLAVE_EMU CFG <addr>` enable protocol-level I2C slave emulation
- `I2C_SLAVE_EMU REG_WRITE <reg> <val>` set register map value
- `I2C_SLAVE_EMU REG_READ <reg>` read register map value
- `I2C_SLAVE_EMU XFER <r|w> <reg> [val]` execute one emulated transaction
- `I2C_SLAVE_EMU STATUS|OFF` query/disable emulation
- `SPI_SLAVE_EMU CFG <cs_pin> <mode0-3> <resp_bytes_csv>` enable protocol-level SPI slave emulation
- `SPI_SLAVE_EMU XFER <tx_bytes_csv>` execute one emulated transaction
- `SPI_SLAVE_EMU STATUS|OFF` query/disable emulation
- `BYPASS_FSM ON|OFF|STATUS|RESET` automatic sideboard-missing bypass state machine
- `OLED_SNIFF_START|STOP` enable/disable OLED sniff capture
- `OLED_SNIFF_STATUS` report bus type and capture counters
- `OLED_SNIFF_DUMP edge|txn|bytes [limit]` export capture slices for offline decode

## Not Implemented

- None in current published command set.

## Limitations

- EXTI event attribution is port-aware for current watch set, but EXTI line-routing constraints still apply; line 1 conflict means not all line-1 candidates can be armed simultaneously.
- ADC stream currently uses software polling (not DMA circular mode yet).
- Under severe host-side backpressure USB CDC can still drop frames; drops are observable through stats.
- MP3398 emulation is logic-level only; it is not equivalent to a real LED power stage.
- I2C/SPI emulation is protocol-level only and does not model physical signal timing.
- OLED sniff currently relies on EXTI edge capture and software decoding; under very high edge rates samples may drop.

## Key Stats Fields

- `tx_drop_count`: protocol-level send failure count
- `event_drop_count`: digital event queue overwrite count
- `event_queue_depth_max`: max observed digital queue depth
- `usb_tx_queue_depth` / `usb_tx_queue_depth_max`: live and peak USB TX queue usage
- `usb_tx_drop_count`: USB TX queue overflow count
- `event_poll_count` / `event_exti_count`: source split of digital events
- `adc_sample_count` / `adc_trigger_count`: ADC activity counters
- `emu_i2c_req_count` / `emu_i2c_hit_count` / `emu_i2c_timeout_count` / `emu_i2c_drop_count`: I2C emulation request and ring-buffer counters
- `emu_spi_req_count` / `emu_spi_hit_count` / `emu_spi_timeout_count` / `emu_spi_drop_count`: SPI emulation request and ring-buffer counters
- `bypass_trigger_count` / `bypass_success_count` / `bypass_timeout_count` / `bypass_active_cycles`: sideboard bypass FSM counters
- `oled_edge_count` / `oled_txn_count` / `oled_byte_count` / `oled_drop_count`: OLED sniff capture counters
