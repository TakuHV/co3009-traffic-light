#include "lcd.h"
#include "main.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define LCD_ADDR  (LCD_I2C_ADDR_7BIT << 1)
#define LCD_BL    0x08
#define LCD_EN    0x04
#define LCD_RS    0x01

static char shadow[LCD_ROWS][LCD_COLS];
static const uint8_t row_offset[4] = {0x00, 0x40, 0x14, 0x54};

static HAL_StatusTypeDef lcd_nibble(uint8_t n)
{
    uint8_t b[2] = { (uint8_t)(n | LCD_BL | LCD_EN), (uint8_t)(n | LCD_BL) };
    return HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, b, 2, 10);
}

static HAL_StatusTypeDef lcd_write(uint8_t data, uint8_t rs)
{
    uint8_t hi = data & 0xF0, lo = (uint8_t)(data << 4);
    uint8_t b[4] = {
        (uint8_t)(hi | LCD_BL | LCD_EN | rs), (uint8_t)(hi | LCD_BL | rs),
        (uint8_t)(lo | LCD_BL | LCD_EN | rs), (uint8_t)(lo | LCD_BL | rs)
    };
    return HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, b, 4, 10);
}

void lcd_init(void)
{
    HAL_Delay(50);
    lcd_nibble(0x30); HAL_Delay(5);
    lcd_nibble(0x30); HAL_Delay(1);
    lcd_nibble(0x30); HAL_Delay(1);
    lcd_nibble(0x20); HAL_Delay(1);      /* chuyển sang 4-bit */
    lcd_write(0x28, 0);                  /* 4-bit, 2 dòng, 5x8 */
    lcd_write(0x08, 0);                  /* tắt hiển thị */
    lcd_write(0x01, 0); HAL_Delay(2);    /* xoá màn hình */
    lcd_write(0x06, 0);                  /* tự tăng con trỏ */
    lcd_write(0x0C, 0);                  /* bật hiển thị, ẩn con trỏ */
    memset(shadow, 0, sizeof(shadow));   /* ép lần in đầu ghi toàn bộ */
}

void lcd_print_row(uint8_t row, const char *s)
{
    if (row >= LCD_ROWS) return;
    char line[LCD_COLS];
    uint8_t i = 0;
    for (; i < LCD_COLS && s[i]; i++) line[i] = s[i];
    for (; i < LCD_COLS; i++) line[i] = ' ';

    int8_t pos = -1;                     /* cột con trỏ hiện tại, -1 = chưa biết */
    for (uint8_t c = 0; c < LCD_COLS; c++) {
        if (line[c] == shadow[row][c]) { pos = -1; continue; }
        if (pos != (int8_t)c) {
            if (lcd_write((uint8_t)(0x80 | (row_offset[row] + c)), 0) != HAL_OK) return;
        }
        if (lcd_write((uint8_t)line[c], LCD_RS) != HAL_OK) return;
        shadow[row][c] = line[c];
        pos = (int8_t)(c + 1);
    }
}
