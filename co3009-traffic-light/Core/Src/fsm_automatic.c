#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_tuning.h"
#include "global.h"
#include "button.h"
#include "traffic_light.h"
#include "software_timer.h"
#include "lcd.h"
#include "dht20.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>

void fsm_automatic_init(void)
{
    auto_state = AUTO_R1_G2;
    auto_screen = AUTO_SCREEN_MAIN;
    setTimer(TMR_STATE, time_green * 1000UL);
}

void fsm_automatic_lights(void)
{
    switch (auto_state) {
    case AUTO_R1_G2:
        traffic_set_pair(LIGHT_RED, LIGHT_GREEN);
        if (isTimerExpired(TMR_STATE)) { auto_state = AUTO_R1_Y2; setTimer(TMR_STATE, time_yellow * 1000UL); }
        break;
    case AUTO_R1_Y2:
        traffic_set_pair(LIGHT_RED, LIGHT_YELLOW);
        if (isTimerExpired(TMR_STATE)) { auto_state = AUTO_G1_R2; setTimer(TMR_STATE, time_green * 1000UL); }
        break;
    case AUTO_G1_R2:
        traffic_set_pair(LIGHT_GREEN, LIGHT_RED);
        if (isTimerExpired(TMR_STATE)) { auto_state = AUTO_Y1_R2; setTimer(TMR_STATE, time_yellow * 1000UL); }
        break;
    case AUTO_Y1_R2:
        traffic_set_pair(LIGHT_YELLOW, LIGHT_RED);
        if (isTimerExpired(TMR_STATE)) { auto_state = AUTO_R1_G2; setTimer(TMR_STATE, time_green * 1000UL); }
        break;
    default:
        auto_state = AUTO_R1_G2;
        setTimer(TMR_STATE, time_green * 1000UL);
        break;
    }
}

static void draw_main(void)
{
    char row[24];
    lcd_print_row(0, "MAN LCD TUN RST ");
    snprintf(row, sizeof row, "1:%s 2:%s  %02u", traffic_name(traffic_get(1)),
             traffic_name(traffic_get(2)), (unsigned)getTimerRemainingSec(TMR_STATE));
    row[15] = 's'; row[16] = 0;
    lcd_print_row(1, row);
}

static void draw_env(void)
{
    char row[24];
    if (dht20_is_valid()) {
        int16_t t = dht20_get_temp_x10(), h = dht20_get_humi_x10();
        snprintf(row, sizeof row, "Temp: %s%d.%d C", (t < 0) ? "-" : "", abs(t) / 10, abs(t) % 10);
        lcd_print_row(0, row);
        snprintf(row, sizeof row, "Humi: %d.%d %% [2]", h / 10, h % 10);
        lcd_print_row(1, row);
    } else {
        lcd_print_row(0, "Temp: --.- C");
        lcd_print_row(1, "Humi: --.- % [2]");
    }
}

void fsm_automatic_run(void)
{
    fsm_automatic_lights();

    if (ui_popup_active()) { button_clear_events(); return; }

    uint8_t p1 = isButtonPressed(BUTTON_1), p2 = isButtonPressed(BUTTON_2);
    uint8_t p3 = isButtonPressed(BUTTON_3), h4 = isButtonHeld(BUTTON_4);
    button_clear_events();

    if (auto_screen == AUTO_SCREEN_MAIN) {
        if (p1)      { fsm_manual_enter(); return; }
        else if (p2) { auto_screen = AUTO_SCREEN_ENV; }
        else if (p3) { fsm_tuning_enter(); return; }
        else if (h4) {
            char l1[24];
            global_reset_times();
            snprintf(l1, sizeof l1, "R%u Y%u G%u", time_red, time_yellow, time_green);
            ui_popup("DEFAULT TIMES", l1, 1500);
        }
    } else if (p2) {
        auto_screen = AUTO_SCREEN_MAIN;
    }

    if (ui_popup_active()) return;
    if (auto_screen == AUTO_SCREEN_MAIN) draw_main(); else draw_env();
}
