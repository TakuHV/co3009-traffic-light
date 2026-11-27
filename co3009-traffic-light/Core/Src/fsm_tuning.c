#include "fsm_tuning.h"
#include "global.h"
#include "button.h"
#include "lcd.h"
#include "ui.h"
#include <stdio.h>

typedef enum { TUNE_SELECT = 0, TUNE_EDIT } TuneLevel_t;
enum { IDX_R = 0, IDX_Y, IDX_G };

static TuneLevel_t level;
static uint8_t sel;          /* màu đang sửa */
static uint8_t edit;         /* giá trị đang sửa ở cấp 2 */
static uint8_t pend[3];      /* bộ thông số "chờ" (R, Y, G); chỉ ghi vào thông số thật khi R == G + Y */
static const char *const cname[3] = {"RED", "YEL", "GRN"};

void fsm_tuning_enter(void)
{
    level = TUNE_SELECT;
    pend[IDX_R] = time_red; pend[IDX_Y] = time_yellow; pend[IDX_G] = time_green;
    mode = MODE_TUNING;
    button_clear_events();
}

static uint8_t pend_valid(void)
{
    return (pend[IDX_R] == pend[IDX_G] + pend[IDX_Y]) && pend[IDX_R] <= TIME_MAX_S;
}

static void draw(void)
{
    char row[24];
    if (level == TUNE_SELECT) {
        snprintf(row, sizeof row, "R=%02u Y=%02u G=%02u", pend[IDX_R], pend[IDX_Y], pend[IDX_G]);
        lcd_print_row(0, row);
        lcd_print_row(1, "R   Y   G   BACK");
    } else {
        snprintf(row, sizeof row, "EDIT %s: %02u s", cname[sel], edit);
        lcd_print_row(0, row);
        lcd_print_row(1, "+   -  SAVE BACK");
    }
}

void fsm_tuning_run(void)
{
    if (ui_popup_active()) { button_clear_events(); return; }

    uint8_t p[NUM_BUTTONS];
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) p[i] = isButtonPressed(i);
    button_clear_events();

    if (level == TUNE_SELECT) {
        for (uint8_t i = 0; i < 3; i++) {
            if (p[i]) { sel = i; edit = pend[i]; level = TUNE_EDIT; break; }
        }
        if (p[BUTTON_4]) {                       /* Back -> Auto, bỏ các giá trị chờ chưa hợp lệ */
            auto_screen = AUTO_SCREEN_MAIN;
            mode = MODE_AUTO;
            button_clear_events();
            return;
        }
    } else {
        if (p[BUTTON_1]) edit = (edit >= TIME_MAX_S) ? TIME_MIN_S : edit + 1;   /* wrap-around */
        if (p[BUTTON_2]) edit = (edit <= TIME_MIN_S) ? TIME_MAX_S : edit - 1;
        if (p[BUTTON_3]) {                                                      /* Save */
            pend[sel] = edit;
            if (pend_valid()) {
                time_red = pend[IDX_R]; time_yellow = pend[IDX_Y]; time_green = pend[IDX_G];
                level = TUNE_SELECT;
                ui_popup("SAVED OK", "", 1000);
            } else {                                                            /* từ chối, ở lại cấp 2 */
                ui_popup("INVALID TIME!", "ERR: R != G+Y", 2000);
            }
        }
        if (p[BUTTON_4]) level = TUNE_SELECT;                                   /* Back -> cấp 1 */
    }

    if (!ui_popup_active()) draw();
}
