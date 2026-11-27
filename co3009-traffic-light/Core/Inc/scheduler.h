#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_TICK_MS     10      /* chu kỳ ngắt TIM2 */
#define SCH_MAX_TASKS   16

typedef struct {
    void (*pTask)(void);   /* con trỏ hàm task */
    uint32_t Delay;        /* số tick còn lại, TƯƠNG ĐỐI so với task đứng trước */
    uint32_t Period;       /* chu kỳ (tick), 0 = one-shot */
    uint8_t  RunMe;        /* cờ: task sẵn sàng chạy */
    uint32_t TaskID;
} sTask;

void     SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD); /* trả 0 nếu đầy */
uint8_t  SCH_Delete_Task(uint32_t taskID);                                    /* 1 = đã xoá */
void     SCH_Update(void);          /* gọi trong ngắt timer, O(1) */
void     SCH_Dispatch_Tasks(void);  /* gọi trong while(1) */

#endif
