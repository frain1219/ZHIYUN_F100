#ifndef PHASE6_STARTUP_CHAIN_H
#define PHASE6_STARTUP_CHAIN_H

typedef enum {
    PHASE6_STARTUP_STEP_IDLE = 0,
    PHASE6_STARTUP_STEP_CLOCK_TREE = 1,
    PHASE6_STARTUP_STEP_MAGIC55AA = 2,
    PHASE6_STARTUP_STEP_IWDT_INIT = 3,
    PHASE6_STARTUP_STEP_GPIOB = 4,
    PHASE6_STARTUP_STEP_DMA1_CLOCK = 5,
    PHASE6_STARTUP_STEP_SPI1_STARTUP_FRAME = 6,
    PHASE6_STARTUP_STEP_USB_CONTEXT = 7,
    PHASE6_STARTUP_STEP_USB_EP0_DEFAULTS = 8,
    PHASE6_STARTUP_STEP_USB_DESC_POST_PWM = 9,
    PHASE6_STARTUP_STEP_DONE = 10,
    PHASE6_STARTUP_STEP_AHB_SRAM = 11
} Phase6_StartupStepId;

typedef struct {
    unsigned int completed;
    unsigned int step_count;
    unsigned int last_step_id;
    unsigned int magic55aa_value;
    unsigned int magic55aa_matched;
    unsigned int iwdt_started;
    unsigned int iwdt_reload_shadow;
    unsigned int clock_hsi_ready;
    unsigned int clock_pll_ready;
    unsigned int clock_pll_selected;
    unsigned int fmc_latency_shadow;
    unsigned int sysclk_hz_shadow;
    unsigned int systick_reload_shadow;
} Phase6_StartupTrace;

void Phase6_StartupChain_Reset(void);
void Phase6_StartupChain_Run(void);
void Phase6_StartupChain_ReadTrace(Phase6_StartupTrace *out_trace);
void Phase6_StartupChain_SetBootMagicWord(unsigned short magic_word);
void SPI1_StartupSequence(void);
void Phase6_SPI1_EnsureReady(void);
void Phase6_SPI1_SetCs(unsigned char high);
unsigned int Phase6_SPI1_Transmit(const unsigned char *data, unsigned int len);

#endif /* PHASE6_STARTUP_CHAIN_H */

