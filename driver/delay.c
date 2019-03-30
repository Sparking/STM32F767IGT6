#include "delay.h"

void delayus(uint16_t us)
{
    uint32_t base_time = SysTick->VAL;
    uint32_t us_interval_count = us * (SysTick->LOAD / 1000) * 2;
    uint32_t current_count = 0;
    uint32_t current_val;

    while (current_count < us_interval_count) {
        current_val = SysTick->VAL;
        if (current_val < base_time)
            current_count += base_time - current_val;
        else
            current_count += current_val - base_time;
        base_time = current_val;
    }
}

void delayms(uint32_t delay_time)
{
    uint32_t cnt;

    for (cnt = 0; cnt < delay_time - 1; cnt ++)
        delayus(1000);
}

#if (SYSTEM_SUPPORT_OS != 0U)

#define FAC_MS (1000 / configTICK_RATE_HZ)

void delayxms(uint32_t ms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        if (ms >= FAC_MS)
            vTaskDelay(ms / FAC_MS);
        ms %= FAC_MS;
    }
    delayus((uint16_t)(ms * 1000));
}

#endif

