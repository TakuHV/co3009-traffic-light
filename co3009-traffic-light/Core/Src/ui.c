#include "ui.h"
#include "lcd.h"
#include "software_timer.h"
#include <string.h>

static char l0[17], l1[17];
static uint8_t on = 0;

void ui_popup(const char *line0, const char *line1, uint32_t ms)
{
    strncpy(l0, line0, 16); l0[16] = 0;
    strncpy(l1, line1, 16); l1[16] = 0;
    setTimer(TMR_POPUP, ms);
    on = 1;
}

uint8_t ui_popup_active(void)
{
    if (on && isTimerExpired(TMR_POPUP)) on = 0;
    return on;
}

void ui_popup_draw(void)
{
    if (ui_popup_active()) {
        lcd_print_row(0, l0);
        lcd_print_row(1, l1);
    }
}
