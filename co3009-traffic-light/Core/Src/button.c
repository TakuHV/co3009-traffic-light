#include "button.h"
#include "main.h"
#include "scheduler.h"

/* 1: tự dò mức logic lúc nhả (giả định khi khởi động KHÔNG nút nào đang bị nhấn)
 * 0: dùng BTN_PRESSED_LEVEL bên dưới (điền theo kết quả Bước 3.3) */
#define BTN_AUTO_POLARITY   1
#define BTN_PRESSED_LEVEL   GPIO_PIN_RESET
#define HOLD_TICKS          (BUTTON_HOLD_MS / SCH_TICK_MS)

static GPIO_TypeDef * const btn_port[NUM_BUTTONS] =
    { BTN_1_GPIO_Port, BTN_2_GPIO_Port, BTN_3_GPIO_Port, BTN_4_GPIO_Port };
static const uint16_t btn_pin[NUM_BUTTONS] =
    { BTN_1_Pin, BTN_2_Pin, BTN_3_Pin, BTN_4_Pin };

static GPIO_PinState active_level[NUM_BUTTONS];
static uint8_t s0[NUM_BUTTONS], s1[NUM_BUTTONS], s2[NUM_BUTTONS];
static uint8_t stable[NUM_BUTTONS], long_fired[NUM_BUTTONS];
static uint16_t hold_cnt[NUM_BUTTONS];
static uint8_t ev_press[NUM_BUTTONS], ev_hold[NUM_BUTTONS];

void button_init(void)
{
#if BTN_AUTO_POLARITY
    HAL_Delay(20);
    for (uint8_t i = 0; i < NUM_BUTTONS; i++)
        active_level[i] = (HAL_GPIO_ReadPin(btn_port[i], btn_pin[i]) == GPIO_PIN_SET)
                          ? GPIO_PIN_RESET : GPIO_PIN_SET;
#else
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) active_level[i] = BTN_PRESSED_LEVEL;
#endif
    button_clear_events();
}

void button_scan(void)
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        s2[i] = s1[i];
        s1[i] = s0[i];
        s0[i] = (HAL_GPIO_ReadPin(btn_port[i], btn_pin[i]) == active_level[i]);

        if (s0[i] != s1[i] || s1[i] != s2[i]) continue;      /* chưa ổn định (chống dội) */

        if (stable[i] != s0[i]) {                            /* cạnh thay đổi */
            stable[i] = s0[i];
            if (stable[i]) {
                hold_cnt[i] = 0;
            } else {                                         /* vừa nhả */
                if (!long_fired[i]) ev_press[i] = 1;
                long_fired[i] = 0;
                hold_cnt[i] = 0;
            }
        } else if (stable[i] && !long_fired[i]) {
            if (++hold_cnt[i] >= HOLD_TICKS) {
                long_fired[i] = 1;
                ev_hold[i] = 1;
            }
        }
    }
}

uint8_t isButtonPressed(uint8_t i)
{
    if (i >= NUM_BUTTONS || !ev_press[i]) return 0;
    ev_press[i] = 0;
    return 1;
}

uint8_t isButtonHeld(uint8_t i)
{
    if (i >= NUM_BUTTONS || !ev_hold[i]) return 0;
    ev_hold[i] = 0;
    return 1;
}

void button_clear_events(void)
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) { ev_press[i] = 0; ev_hold[i] = 0; }
}
