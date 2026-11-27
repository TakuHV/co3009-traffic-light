#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define TMR_STATE   0     /* thời gian của trạng thái đèn hiện tại */
#define TMR_POPUP   1     /* thông báo tạm trên LCD */
#define NUM_TIMERS  4

extern uint8_t timer_flag[NUM_TIMERS];

void     setTimer(uint8_t idx, uint32_t ms);
uint8_t  isTimerExpired(uint8_t idx);
uint32_t getTimerRemainingSec(uint8_t idx);   /* làm tròn lên */
void     timer_run(void);                     /* task chu kỳ 1 tick (10 ms) */

#endif
