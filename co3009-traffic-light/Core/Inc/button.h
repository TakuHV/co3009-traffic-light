#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

typedef enum { BUTTON_1 = 0, BUTTON_2, BUTTON_3, BUTTON_4, NUM_BUTTONS } ButtonId_t;

#define BUTTON_HOLD_MS   1000

void    button_init(void);
void    button_scan(void);                 /* task chu kỳ 1 tick (10 ms) */
uint8_t isButtonPressed(uint8_t i);        /* nhấn ngắn (phát hiện khi nhả), đọc xong tự xoá */
uint8_t isButtonHeld(uint8_t i);           /* giữ >= 1 s (phát sinh 1 lần), đọc xong tự xoá */
void    button_clear_events(void);

#endif
