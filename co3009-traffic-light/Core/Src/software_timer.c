#include "software_timer.h"
#include "scheduler.h"

static uint32_t timer_counter[NUM_TIMERS];
uint8_t timer_flag[NUM_TIMERS];

void setTimer(uint8_t idx, uint32_t ms)
{
    if (idx >= NUM_TIMERS) return;
    timer_counter[idx] = (ms + SCH_TICK_MS - 1) / SCH_TICK_MS;
    timer_flag[idx] = (timer_counter[idx] == 0) ? 1 : 0;
}

uint8_t isTimerExpired(uint8_t idx)
{
    return (idx < NUM_TIMERS) ? timer_flag[idx] : 0;
}

uint32_t getTimerRemainingSec(uint8_t idx)
{
    if (idx >= NUM_TIMERS) return 0;
    return (timer_counter[idx] * SCH_TICK_MS + 999) / 1000;
}

void timer_run(void)
{
    for (uint8_t i = 0; i < NUM_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) timer_flag[i] = 1;
        }
    }
}
