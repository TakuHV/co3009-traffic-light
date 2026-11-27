#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

/* ---- Thông số cấu hình ---- */
#define DEFAULT_RED_S       5
#define DEFAULT_YELLOW_S    2
#define DEFAULT_GREEN_S     3
#define TIME_MIN_S          1      /* Min khi tuning (Ideas.pdf chưa nêu) */
#define TIME_MAX_S          99
#define MANUAL_YELLOW_MS    3000   /* đèn vàng đệm ở Manual (text = 3s, sơ đồ ghi 2s) */
#define FSM_PERIOD_TICKS    5      /* FSM chạy mỗi 50 ms */

typedef enum { MODE_AUTO = 0, MODE_MANUAL, MODE_TUNING } SystemMode_t;
typedef enum { AUTO_R1_G2 = 0, AUTO_R1_Y2, AUTO_G1_R2, AUTO_Y1_R2 } AutoState_t;
typedef enum { MAN_G1_R2 = 0, MAN_Y1_R2, MAN_R1_G2, MAN_R1_Y2 } ManualState_t;
typedef enum { AUTO_SCREEN_MAIN = 0, AUTO_SCREEN_ENV } AutoScreen_t;

extern SystemMode_t  mode;
extern AutoState_t   auto_state;
extern ManualState_t manual_state;
extern AutoScreen_t  auto_screen;
extern uint8_t time_red, time_yellow, time_green;   /* đơn vị: giây */

void global_init(void);
void global_reset_times(void);

#endif
