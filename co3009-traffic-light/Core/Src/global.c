#include "global.h"

SystemMode_t  mode = MODE_AUTO;
AutoState_t   auto_state = AUTO_R1_G2;
ManualState_t manual_state = MAN_G1_R2;
AutoScreen_t  auto_screen = AUTO_SCREEN_MAIN;
uint8_t time_red = DEFAULT_RED_S, time_yellow = DEFAULT_YELLOW_S, time_green = DEFAULT_GREEN_S;

void global_reset_times(void)
{
    time_red = DEFAULT_RED_S;
    time_yellow = DEFAULT_YELLOW_S;
    time_green = DEFAULT_GREEN_S;
}

void global_init(void)
{
    mode = MODE_AUTO;
    auto_state = AUTO_R1_G2;
    manual_state = MAN_G1_R2;
    auto_screen = AUTO_SCREEN_MAIN;
    global_reset_times();
}
