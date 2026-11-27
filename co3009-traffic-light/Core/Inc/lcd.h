#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>

#define LCD_I2C_ADDR_7BIT  0x27     /* GIẢ ĐỊNH - sửa theo kết quả I2C scan (hay gặp 0x27 / 0x3F) */
#define LCD_COLS           16
#define LCD_ROWS           2

void lcd_init(void);                              /* dùng HAL_Delay, chỉ gọi trước khi chạy scheduler */
void lcd_print_row(uint8_t row, const char *s);   /* tự đệm khoảng trắng, chỉ ghi ký tự thay đổi */

#endif
