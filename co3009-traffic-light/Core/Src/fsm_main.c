#include "fsm_main.h"
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_tuning.h"
#include "global.h"
#include "ui.h"

void fsm_main_init(void) { fsm_automatic_init(); }

void fsm_main_run(void)
{
    switch (mode) {
    case MODE_AUTO:   fsm_automatic_run(); break;
    case MODE_MANUAL: fsm_manual_run();    break;
    case MODE_TUNING: fsm_automatic_lights(); fsm_tuning_run(); break;   /* đèn Auto vẫn chạy nền */
    default:          mode = MODE_AUTO;    break;
    }
    ui_popup_draw();
}
