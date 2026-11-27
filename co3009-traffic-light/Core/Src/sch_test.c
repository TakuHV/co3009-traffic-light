/* File TEST TẠM THỜI cho scheduler - chép vào Core/Src. KHÔNG nộp / không merge vào main.
 * Kết quả in ra UART2 (ST-Link Virtual COM, 115200 8N1). */
#include "main.h"
#include "scheduler.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
static uint32_t id_a;

static void say(const char *s) { HAL_UART_Transmit(&huart2, (uint8_t *)s, strlen(s), 100); }

static void tog_a(void) { HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);        say("A "); }   /* 500 ms  */
static void tog_b(void) { HAL_GPIO_TogglePin(LED_PC9_GPIO_Port, LED_PC9_Pin); say("B "); }   /* 1000 ms */

static void os0(void)   { say("\r\n[one-shot delay=0 tick]\r\n"); }
static void os50(void)  { say("\r\n[one-shot delay=50 tick = 0.5 s]\r\n"); }
static void os120(void) { say("\r\n[one-shot delay=120 tick = 1.2 s]\r\n"); }
static void os300(void) { say("\r\n[one-shot delay=300 tick = 3.0 s]\r\n"); }
static void del_a(void) { say(SCH_Delete_Task(id_a) ? "\r\n[DA XOA task A]\r\n" : "\r\n[XOA THAT BAI]\r\n"); }

void sch_test_setup(void)       /* gọi thay cho 5 dòng SCH_Add_Task trong main.c */
{
    id_a = SCH_Add_Task(tog_a, 0, 50);       /* chu kỳ 500 ms  */
    SCH_Add_Task(tog_b, 0, 100);             /* chu kỳ 1000 ms */
    /* thêm theo thứ tự lộn xộn để thử chèn đầu / giữa / cuối danh sách */
    SCH_Add_Task(os300, 300, 0);
    SCH_Add_Task(os50,   50, 0);
    SCH_Add_Task(os120, 120, 0);
    SCH_Add_Task(os0,     0, 0);
    SCH_Add_Task(del_a, 1000, 0);            /* sau 10 s xoá task A */
}
