#include "probe_time.h"

#include "stm32f4xx.h"

void ProbeTime_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t ProbeTime_GetUs(void)
{
    return (uint32_t)(DWT->CYCCNT / (SystemCoreClock / 1000000U));
}
