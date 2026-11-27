#include "fsm_manual.h"
#include "global.h"
#include "button.h"
#include "traffic_light.h"
#include "software_timer.h"
#include "lcd.h"
#include "ui.h"
#include <stdio.h>

/* Vào Manual: ánh xạ trạng thái Auto hiện tại sang trạng thái Manual tương đương,
 * đèn không đổi => không bao giờ có chuyển xanh/đỏ đột ngột hay 2 đường cùng xanh. */
void fsm_manual_enter(void)
{
    switch (auto_state) {
    case AUTO_G1_R2: manual_state = MAN_G1_R2; break;
    case AUTO_R1_G2: manual_state = MAN_R1_G2; break;
    case AUTO_Y1_R2: manual_state = MAN_Y1_R2; setTimer(TMR_STATE, MANUAL_YELLOW_MS); break;
    case AUTO_R1_Y2: manual_state = MAN_R1_Y2; setTimer(TMR_STATE, MANUAL_YELLOW_MS); break;
    default:         manual_state = MAN_G1_R2; break;
    }
    mode = MODE_MANUAL;
    button_clear_events();
}

/* Thoát về Auto: cũng ánh xạ tương đương, đèn giữ nguyên */
static void exit_to_auto(void)
{
    switch (manual_state) {
    case MAN_G1_R2: auto_state = AUTO_G1_R2; setTimer(TMR_STATE, time_green * 1000UL);  break;
    case MAN_R1_G2: auto_state = AUTO_R1_G2; setTimer(TMR_STATE, time_green * 1000UL);  break;
    case MAN_Y1_R2: auto_state = AUTO_Y1_R2; setTimer(TMR_STATE, time_yellow * 1000UL); break;
    case MAN_R1_Y2: auto_state = AUTO_R1_Y2; setTimer(TMR_STATE, time_yellow * 1000UL); break;
    }
    auto_screen = AUTO_SCREEN_MAIN;
    mode = MODE_AUTO;
    button_clear_events();
}

static void draw(void)
{
    char row[24];
    lcd_print_row(0, "MANUAL hold1=AUT");
    if (manual_state == MAN_Y1_R2 || manual_state == MAN_R1_Y2) {
        snprintf(row, sizeof row, "1:%s 2:%s  %02u", traffic_name(traffic_get(1)),
                 traffic_name(traffic_get(2)), (unsigned)getTimerRemainingSec(TMR_STATE));
        row[15] = 's'; row[16] = 0;
    } else {
        snprintf(row, sizeof row, "1:%s 2:%s", traffic_name(traffic_get(1)), traffic_name(traffic_get(2)));
    }
    lcd_print_row(1, row);
}

void fsm_manual_run(void)
{
    switch (manual_state) {
    case MAN_G1_R2: traffic_set_pair(LIGHT_GREEN, LIGHT_RED);    break;
    case MAN_Y1_R2: traffic_set_pair(LIGHT_YELLOW, LIGHT_RED);   break;
    case MAN_R1_G2: traffic_set_pair(LIGHT_RED, LIGHT_GREEN);    break;
    case MAN_R1_Y2: traffic_set_pair(LIGHT_RED, LIGHT_YELLOW);   break;
    }

    if (ui_popup_active()) { button_clear_events(); return; }

    uint8_t p1 = isButtonPressed(BUTTON_1), h1 = isButtonHeld(BUTTON_1);
    button_clear_events();

    if (h1) { exit_to_auto(); return; }

    switch (manual_state) {
    case MAN_G1_R2:
        if (p1) { manual_state = MAN_Y1_R2; setTimer(TMR_STATE, MANUAL_YELLOW_MS); }
        break;
    case MAN_R1_G2:
        if (p1) { manual_state = MAN_R1_Y2; setTimer(TMR_STATE, MANUAL_YELLOW_MS); }
        break;
    case MAN_Y1_R2:                       /* đèn vàng đệm: bỏ qua phím nhấn */
        if (isTimerExpired(TMR_STATE)) manual_state = MAN_R1_G2;
        break;
    case MAN_R1_Y2:
        if (isTimerExpired(TMR_STATE)) manual_state = MAN_G1_R2;
        break;
    }

    if (!ui_popup_active()) draw();
}
