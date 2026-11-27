#include "scheduler.h"
#include "main.h"          /* CMSIS: __get_PRIMASK / __disable_irq */
#include <string.h>

/* Cài đặt bằng MẢNG đã sắp theo thời gian chạy, Delay lưu dạng tương đối.
 * SCH_Update chỉ đụng phần tử đầu -> O(1). Việc nặng (tìm chỗ chèn) nằm ở SCH_Add_Task. */
static sTask    SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t  SCH_count = 0;
static uint32_t SCH_next_id = 1;

#define ENTER_CRITICAL()  uint32_t _primask = __get_PRIMASK(); __disable_irq()
#define EXIT_CRITICAL()   __set_PRIMASK(_primask)

/* gọi khi đang trong vùng găng */
static void remove_at(uint8_t idx)
{
    if ((uint8_t)(idx + 1) < SCH_count)
        SCH_tasks_G[idx + 1].Delay += SCH_tasks_G[idx].Delay;  /* trả lại delay cho task sau */
    for (uint8_t j = idx; (uint8_t)(j + 1) < SCH_count; j++)
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    SCH_count--;
}

static uint8_t insert_task(void (*pFunc)(void), uint32_t delay, uint32_t period, uint32_t id)
{
    ENTER_CRITICAL();
    if (SCH_count >= SCH_MAX_TASKS) { EXIT_CRITICAL(); return 0; }

    uint8_t  i = 0;
    uint32_t rem = delay;
    while (i < SCH_count && rem >= SCH_tasks_G[i].Delay) {   /* tìm điểm chèn (đầu/giữa/cuối) */
        rem -= SCH_tasks_G[i].Delay;
        i++;
    }
    for (uint8_t j = SCH_count; j > i; j--)
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];

    SCH_tasks_G[i].pTask  = pFunc;
    SCH_tasks_G[i].Delay  = rem;
    SCH_tasks_G[i].Period = period;
    SCH_tasks_G[i].RunMe  = 0;
    SCH_tasks_G[i].TaskID = id;
    SCH_count++;

    if ((uint8_t)(i + 1) < SCH_count)
        SCH_tasks_G[i + 1].Delay -= rem;                    /* task sau chỉ còn phần chênh lệch */
    EXIT_CRITICAL();
    return 1;
}

void SCH_Init(void)
{
    ENTER_CRITICAL();
    memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
    SCH_count = 0;
    SCH_next_id = 1;
    EXIT_CRITICAL();
}

uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD)
{
    uint32_t id = SCH_next_id;
    if (!insert_task(pFunc, DELAY, PERIOD, id)) return 0;
    SCH_next_id++;
    return id;
}

uint8_t SCH_Delete_Task(uint32_t taskID)
{
    ENTER_CRITICAL();
    for (uint8_t i = 0; i < SCH_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            remove_at(i);
            EXIT_CRITICAL();
            return 1;
        }
    }
    EXIT_CRITICAL();
    return 0;
}

void SCH_Update(void)   /* O(1) */
{
    if (SCH_count == 0) return;
    if (SCH_tasks_G[0].Delay > 0) SCH_tasks_G[0].Delay--;
    if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
}

void SCH_Dispatch_Tasks(void)
{
    for (;;) {
        sTask t;
        ENTER_CRITICAL();
        if (SCH_count == 0 || !(SCH_tasks_G[0].RunMe || SCH_tasks_G[0].Delay == 0)) {
            EXIT_CRITICAL();
            return;
        }
        t = SCH_tasks_G[0];
        remove_at(0);
        EXIT_CRITICAL();

        if (t.pTask) t.pTask();
        if (t.Period > 0)                                   /* periodic: thêm lại, cùng TaskID */
            insert_task(t.pTask, t.Period, t.Period, t.TaskID);
        /* Period == 0: one-shot, không thêm lại */
    }
}
