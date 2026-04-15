#include "phase6/phase6_startup_chain.h"
#include "phase6/phase6_usb_device.h"
#ifndef PHASE6_HOST_TEST
#include "apm32f10x.h"
#endif

#define PHASE6_APB2EN_AFIOEN (1UL << 0)
#define PHASE6_APB2EN_IOPAEN (1UL << 2)
#define PHASE6_APB2EN_IOPBEN (1UL << 3)
#define PHASE6_APB2EN_SPI1EN (1UL << 12)
#define PHASE6_AHBEN_DMA1EN (1UL << 0)
#define PHASE6_AHBEN_SRAMEN (1UL << 2)
#define PHASE6_AHBEN_FMCEN (1UL << 4)
#define PHASE6_SPI_FLAG_TXBE (1U << 1)
#define PHASE6_SPI_FLAG_RXBNE (1U << 0)
#define PHASE6_SPI_FLAG_BSY (1U << 7)
#define PHASE6_SPI_STARTUP_WAIT_SPIN_MAX 256U
#define PHASE6_RCM_READY_WAIT_MAX 100000U
#define PHASE6_SYSCLK_PLL_SEL 0x2U
#define PHASE6_BOOT_MAGIC55AA 0x55AAU
#define PHASE6_IWDT_KEY_UNLOCK 0x5555U
#define PHASE6_IWDT_KEY_RELOAD 0xAAAAU
#define PHASE6_IWDT_KEY_ENABLE 0xCCCCU
#define PHASE6_IWDT_PSC_DIV256 6U
#define PHASE6_IWDT_RELOAD_0X30D 0x30DU

static volatile Phase6_StartupTrace g_phase6_startup_trace;
static volatile unsigned int g_phase6_usb_context_shadow;
static volatile unsigned int g_phase6_usb_ep0_packet_len_shadow;
static volatile unsigned int g_phase6_usb_descriptor_init_shadow;
volatile unsigned int g_phase6_boot_magic_word_shadow;
volatile unsigned int g_phase6_boot_magic_matched;
volatile unsigned int g_phase6_iwdt_started;
volatile unsigned int g_phase6_iwdt_reload_shadow;
volatile unsigned int g_phase6_spi1_startup_tx_count;
volatile unsigned int g_phase6_spi1_startup_done;
volatile unsigned int g_phase6_spi1_startup_last_byte;
volatile unsigned int g_phase6_spi1_startup_cs_low_count;
volatile unsigned int g_phase6_spi1_startup_cs_high_count;
volatile unsigned int g_phase6_spi1_startup_wait_timeout_count;
static volatile unsigned int g_phase6_spi1_hw_inited;
static volatile unsigned int g_phase6_spi1_status_fallback_mode;
static volatile unsigned int g_phase6_clock_hsi_ready;
static volatile unsigned int g_phase6_clock_pll_ready;
static volatile unsigned int g_phase6_clock_pll_selected;
static volatile unsigned int g_phase6_fmc_latency_shadow;
static volatile unsigned int g_phase6_sysclk_hz_shadow;
static volatile unsigned int g_phase6_systick_reload_shadow;

static const unsigned char g_phase6_spi1_startup_frame_23b[23] = {
    0xAE, 0x00, 0x10, 0x40, 0x81, 0xFF, 0xA1, 0xA8,
    0x3F, 0xAD, 0x8B, 0xC8, 0xD3, 0x00, 0xDA, 0x12,
    0xD9, 0x22, 0xDB, 0x2B, 0xA4, 0xA6, 0xAF
};

static void Phase6_Startup_Mark(Phase6_StartupStepId step)
{
    g_phase6_startup_trace.last_step_id = (unsigned int)step;
    g_phase6_startup_trace.step_count++;
}

void Phase6_StartupChain_SetBootMagicWord(unsigned short magic_word)
{
    g_phase6_boot_magic_word_shadow = (unsigned int)magic_word;
}

static unsigned char CheckMagicWord55AA_Phase6Anchor(void)
{
    g_phase6_boot_magic_matched =
        (g_phase6_boot_magic_word_shadow == PHASE6_BOOT_MAGIC55AA) ? 1U : 0U;
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_MAGIC55AA);
    return (unsigned char)g_phase6_boot_magic_matched;
}

static void Iwdg_UnlockConfigurePrescaleReloadAndStart_Phase6Anchor(void)
{
#ifndef PHASE6_HOST_TEST
    /*
     * OEM anchor (05 §1):
     * KR 0x5555 -> PSC 6 -> RLR 0x30D -> KR 0xAAAA -> KR 0xCCCC.
     */
    IWDT->KEY = PHASE6_IWDT_KEY_UNLOCK;
    IWDT->PSC = PHASE6_IWDT_PSC_DIV256;
    IWDT->CNTRLD = PHASE6_IWDT_RELOAD_0X30D;
    IWDT->KEY = PHASE6_IWDT_KEY_RELOAD;
    IWDT->KEY = PHASE6_IWDT_KEY_ENABLE;
#endif
    g_phase6_iwdt_started = 1U;
    g_phase6_iwdt_reload_shadow++;
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_IWDT_INIT);
}

static void InitializeClock_PLLFlashSysTick_Phase6Anchor(void)
{
#ifndef PHASE6_HOST_TEST
    unsigned int spin;
    unsigned int systick_reload;

    /*
     * Replace mark-only startup placeholder with bounded register sequence:
     * HSI ready -> FMC latency/prefetch -> PLL ready -> SYSCLK select -> SysTick.
     * This tightens startup parity while staying inside frozen MMIO scope.
     */
    RCM->CTRL |= (1UL << 0); /* HSIEN */
    spin = 0U;
    while (((RCM->CTRL & (1UL << 1)) == 0U) && (spin < PHASE6_RCM_READY_WAIT_MAX)) {
        spin++;
    }
    g_phase6_clock_hsi_ready = (spin < PHASE6_RCM_READY_WAIT_MAX) ? 1U : 0U;

    RCM->AHBCLKEN |= PHASE6_AHBEN_FMCEN;
    FMC->CTRL1 = (FMC->CTRL1 & ~0x7UL) | 0x2UL; /* WS=2 */
    FMC->CTRL1 |= (1UL << 3) | (1UL << 4);      /* HCAEN + PBEN */
    g_phase6_fmc_latency_shadow = FMC->CTRL1 & 0x1FUL;

    /* PLL source HSI/2, MUL x9; APB1 /2 keeps peripheral clock conservative. */
    RCM->CFG &= ~((0xFUL << 4) | (0x7UL << 8) | (0x7UL << 11) | (1UL << 16) | (1UL << 17) | (0xFUL << 18));
    RCM->CFG |= (0x4UL << 8) | (0x4UL << 18);

    RCM->CTRL |= (1UL << 24); /* PLL1EN */
    spin = 0U;
    while (((RCM->CTRL & (1UL << 25)) == 0U) && (spin < PHASE6_RCM_READY_WAIT_MAX)) {
        spin++;
    }
    g_phase6_clock_pll_ready = (spin < PHASE6_RCM_READY_WAIT_MAX) ? 1U : 0U;

    if (g_phase6_clock_pll_ready != 0U) {
        RCM->CFG = (RCM->CFG & ~0x3UL) | PHASE6_SYSCLK_PLL_SEL;
        spin = 0U;
        while ((((RCM->CFG >> 2) & 0x3UL) != PHASE6_SYSCLK_PLL_SEL) && (spin < PHASE6_RCM_READY_WAIT_MAX)) {
            spin++;
        }
        g_phase6_clock_pll_selected = (spin < PHASE6_RCM_READY_WAIT_MAX) ? 1U : 0U;
    } else {
        g_phase6_clock_pll_selected = 0U;
    }

    SystemCoreClockUpdate();
    g_phase6_sysclk_hz_shadow = SystemCoreClock;
    if (g_phase6_sysclk_hz_shadow > 1000U) {
        systick_reload = (g_phase6_sysclk_hz_shadow / 1000U) - 1U;
        if (systick_reload <= 0x00FFFFFFU) {
            SysTick->LOAD = systick_reload;
            SysTick->VAL = 0U;
            SysTick->CTRL = (1UL << 2) | (1UL << 1) | 1UL;
            g_phase6_systick_reload_shadow = systick_reload;
        }
    }
#else
    g_phase6_clock_hsi_ready = 1U;
    g_phase6_clock_pll_ready = 1U;
    g_phase6_clock_pll_selected = 1U;
    g_phase6_fmc_latency_shadow = 0x12U;
    g_phase6_sysclk_hz_shadow = 8000000U;
    g_phase6_systick_reload_shadow = 7999U;
#endif
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_CLOCK_TREE);
}

static void InitializeGPIOB_ClockAndPinMode_Phase6Anchor(void)
{
#ifndef PHASE6_HOST_TEST
    RCM->APB2CLKEN |= (PHASE6_APB2EN_AFIOEN | PHASE6_APB2EN_IOPBEN);
#endif
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_GPIOB);
}

static void EnableAHBPeriphClock_DMA1_Phase6Anchor(void)
{
#ifndef PHASE6_HOST_TEST
    RCM->AHBCLKEN |= PHASE6_AHBEN_DMA1EN;
#endif
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_DMA1_CLOCK);
}

static void EnableAHBPeriphClock_SRAM_Phase6Anchor(void)
{
#ifndef PHASE6_HOST_TEST
    RCM->AHBCLKEN |= PHASE6_AHBEN_SRAMEN;
#endif
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_AHB_SRAM);
}

static void Phase6_SPI1_StartupHwInit(void)
{
#ifndef PHASE6_HOST_TEST
    if (g_phase6_spi1_hw_inited != 0U) {
        return;
    }

    RCM->APB2CLKEN |=
        (PHASE6_APB2EN_AFIOEN | PHASE6_APB2EN_IOPAEN | PHASE6_APB2EN_IOPBEN | PHASE6_APB2EN_SPI1EN);

    /* SPI1: PA5=SCK, PA6=MISO, PA7=MOSI; OLED CS: PB10 (active low). */
    GPIOA->CFGLOW =
        (GPIOA->CFGLOW & ~((0xFUL << 20) | (0xFUL << 24) | (0xFUL << 28))) |
        (0xBUL << 20) | (0x4UL << 24) | (0xBUL << 28);
    GPIOB->CFGHIG =
        (GPIOB->CFGHIG & ~(0xFUL << 8)) |
        (0x3UL << 8);
    GPIOB->BSC = (1UL << 10);

    SPI1->CTRL1 = 0U;
    SPI1->CTRL1 |= (1U << 2);          /* master */
    SPI1->CTRL1 |= (1U << 8) | (1U << 9); /* software NSS high */
    SPI1->CTRL1 |= (0x2U << 3);        /* baud div 8 */
    SPI1->CTRL1 |= (1U << 6);          /* SPI enable */
    g_phase6_spi1_hw_inited = 1U;
#endif
}

void Phase6_SPI1_EnsureReady(void)
{
    Phase6_SPI1_StartupHwInit();
}

void Phase6_SPI1_SetCs(unsigned char high)
{
#ifndef PHASE6_HOST_TEST
    if (high != 0U) {
        GPIOB->BSC = (1UL << 10);
        g_phase6_spi1_startup_cs_high_count++;
    } else {
        GPIOB->BC = (1UL << 10);
        g_phase6_spi1_startup_cs_low_count++;
    }
#else
    if (high != 0U) {
        g_phase6_spi1_startup_cs_high_count++;
    } else {
        g_phase6_spi1_startup_cs_low_count++;
    }
#endif
}

unsigned int Phase6_SPI1_Transmit(const unsigned char *data, unsigned int len)
{
    unsigned int i;

    if ((data == 0) || (len == 0U)) {
        return 0U;
    }

    Phase6_SPI1_EnsureReady();
    for (i = 0U; i < len; ++i) {
        unsigned char data_byte;

        data_byte = data[i];
#ifndef PHASE6_HOST_TEST
        if (g_phase6_spi1_status_fallback_mode == 0U) {
            unsigned int spin;
            spin = 0U;
            while (((SPI1->STS & PHASE6_SPI_FLAG_TXBE) == 0U) && (spin < PHASE6_SPI_STARTUP_WAIT_SPIN_MAX)) {
                spin++;
            }
            if (spin == PHASE6_SPI_STARTUP_WAIT_SPIN_MAX) {
                /*
                 * Renode 的 SPI STS 在部分版本中不会稳定反映 TX 就绪，
                 * 首次命中后降级为保守直发，避免把每字节都计入 timeout。
                 */
                g_phase6_spi1_startup_wait_timeout_count++;
                g_phase6_spi1_status_fallback_mode = 1U;
            }
        }
        SPI1->DATA = data_byte;
        if (g_phase6_spi1_status_fallback_mode == 0U) {
            unsigned int spin;
            spin = 0U;
            while (((SPI1->STS & PHASE6_SPI_FLAG_RXBNE) == 0U) && (spin < PHASE6_SPI_STARTUP_WAIT_SPIN_MAX)) {
                spin++;
            }
            if ((SPI1->STS & PHASE6_SPI_FLAG_RXBNE) != 0U) {
                (void)SPI1->DATA;
            } else {
                g_phase6_spi1_startup_wait_timeout_count++;
                g_phase6_spi1_status_fallback_mode = 1U;
            }
        }
#endif
        g_phase6_spi1_startup_tx_count++;
        g_phase6_spi1_startup_last_byte = (unsigned int)data_byte;
    }

#ifndef PHASE6_HOST_TEST
    if (g_phase6_spi1_status_fallback_mode == 0U) {
        unsigned int spin;
        spin = 0U;
        while (((SPI1->STS & PHASE6_SPI_FLAG_BSY) != 0U) && (spin < PHASE6_SPI_STARTUP_WAIT_SPIN_MAX)) {
            spin++;
        }
        if (spin == PHASE6_SPI_STARTUP_WAIT_SPIN_MAX) {
            g_phase6_spi1_startup_wait_timeout_count++;
            g_phase6_spi1_status_fallback_mode = 1U;
        }
    }
#endif
    return len;
}

void SPI1_StartupSequence(void)
{
    if (g_phase6_spi1_startup_done != 0U) {
        return;
    }

    Phase6_SPI1_SetCs(0U);
    (void)Phase6_SPI1_Transmit(g_phase6_spi1_startup_frame_23b, 23U);
    Phase6_SPI1_SetCs(1U);

    g_phase6_spi1_startup_done = 1U;
}

static void SetUsbDeviceContextPointer_Phase6Anchor(void)
{
    g_phase6_usb_context_shadow = 0x20004860U;
    Phase6_UsbDevice_SetContextPointer(g_phase6_usb_context_shadow);
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_USB_CONTEXT);
}

static void InitializeUsbControlEndpointDefaults_Phase6Anchor(void)
{
    g_phase6_usb_ep0_packet_len_shadow = 64U;
    Phase6_UsbDevice_SetEp0MaxPacket(g_phase6_usb_ep0_packet_len_shadow);
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_USB_EP0_DEFAULTS);
}

static void InitializeUsbDescriptorsAfterPwmInit_Phase6Anchor(void)
{
    g_phase6_usb_descriptor_init_shadow = 1U;
    Phase6_UsbDevice_SetDescriptorsReady(g_phase6_usb_descriptor_init_shadow);
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_USB_DESC_POST_PWM);
}

void Phase6_StartupChain_Reset(void)
{
    g_phase6_startup_trace.completed = 0U;
    g_phase6_startup_trace.step_count = 0U;
    g_phase6_startup_trace.last_step_id = (unsigned int)PHASE6_STARTUP_STEP_IDLE;
    g_phase6_usb_context_shadow = 0U;
    g_phase6_usb_ep0_packet_len_shadow = 0U;
    g_phase6_usb_descriptor_init_shadow = 0U;
    g_phase6_boot_magic_word_shadow = 0U;
    g_phase6_boot_magic_matched = 0U;
    g_phase6_iwdt_started = 0U;
    g_phase6_iwdt_reload_shadow = 0U;
    g_phase6_spi1_startup_tx_count = 0U;
    g_phase6_spi1_startup_done = 0U;
    g_phase6_spi1_startup_last_byte = 0U;
    g_phase6_spi1_startup_cs_low_count = 0U;
    g_phase6_spi1_startup_cs_high_count = 0U;
    g_phase6_spi1_startup_wait_timeout_count = 0U;
    g_phase6_spi1_hw_inited = 0U;
    g_phase6_spi1_status_fallback_mode = 0U;
    g_phase6_clock_hsi_ready = 0U;
    g_phase6_clock_pll_ready = 0U;
    g_phase6_clock_pll_selected = 0U;
    g_phase6_fmc_latency_shadow = 0U;
    g_phase6_sysclk_hz_shadow = 0U;
    g_phase6_systick_reload_shadow = 0U;
}

void Phase6_StartupChain_Run(void)
{
    if (g_phase6_startup_trace.completed != 0U) {
        return;
    }

    InitializeClock_PLLFlashSysTick_Phase6Anchor();
    EnableAHBPeriphClock_SRAM_Phase6Anchor();
    if (CheckMagicWord55AA_Phase6Anchor() == 0U) {
        Iwdg_UnlockConfigurePrescaleReloadAndStart_Phase6Anchor();
    }
    InitializeGPIOB_ClockAndPinMode_Phase6Anchor();
    EnableAHBPeriphClock_DMA1_Phase6Anchor();
    SPI1_StartupSequence();
    Phase6_Startup_Mark(PHASE6_STARTUP_STEP_SPI1_STARTUP_FRAME);
    SetUsbDeviceContextPointer_Phase6Anchor();
    InitializeUsbControlEndpointDefaults_Phase6Anchor();
    InitializeUsbDescriptorsAfterPwmInit_Phase6Anchor();
    Phase6_UsbDevice_CompleteInitFromContext();

    g_phase6_startup_trace.completed = 1U;
    g_phase6_startup_trace.last_step_id = (unsigned int)PHASE6_STARTUP_STEP_DONE;
}

void Phase6_StartupChain_ReadTrace(Phase6_StartupTrace *out_trace)
{
    if (out_trace == 0) {
        return;
    }

    out_trace->completed = g_phase6_startup_trace.completed;
    out_trace->step_count = g_phase6_startup_trace.step_count;
    out_trace->last_step_id = g_phase6_startup_trace.last_step_id;
    out_trace->magic55aa_value = g_phase6_boot_magic_word_shadow;
    out_trace->magic55aa_matched = g_phase6_boot_magic_matched;
    out_trace->iwdt_started = g_phase6_iwdt_started;
    out_trace->iwdt_reload_shadow = g_phase6_iwdt_reload_shadow;
    out_trace->clock_hsi_ready = g_phase6_clock_hsi_ready;
    out_trace->clock_pll_ready = g_phase6_clock_pll_ready;
    out_trace->clock_pll_selected = g_phase6_clock_pll_selected;
    out_trace->fmc_latency_shadow = g_phase6_fmc_latency_shadow;
    out_trace->sysclk_hz_shadow = g_phase6_sysclk_hz_shadow;
    out_trace->systick_reload_shadow = g_phase6_systick_reload_shadow;
}

