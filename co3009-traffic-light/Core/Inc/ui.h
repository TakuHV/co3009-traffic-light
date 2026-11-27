#ifndef INC_UI_H_
#define INC_UI_H_

#include <stdint.h>

void    ui_popup(const char *line0, const char *line1, uint32_t ms);
uint8_t ui_popup_active(void);
void    ui_popup_draw(void);

#endif
