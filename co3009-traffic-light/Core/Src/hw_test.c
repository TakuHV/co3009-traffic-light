/* File TEST TẠM THỜI - chép vào Core/Src. Không dùng trong bản nộp cuối.
 * Kết quả in ra UART2 (ST-Link Virtual COM, 115200 8N1). */
#include "main.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>

#define HW_TEST  1      /* 1 = màu đèn | 2 = nút nhấn | 3 = I2C scan | 4 = LCD */

extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef  hi2c1;

static void say(const char *s) { HAL_UART_Transmit(&huart2, (uint8_t *)s, strlen(s), 100); }

static void test_lights(void)
{
    char m[80];
    say("\r\n=== TEST DEN: moi to hop giu 4 giay ===\r\n");
    for (;;) {
        for (int i = 0; i < 4; i++) {
            int a = (i >> 1) & 1, b = i & 1;
            HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, a);
            HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, b);
            HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, a);
            HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, b);
            snprintf(m, sizeof m, "A=%d B=%d -> ghi mau cua den 1 va den 2\r\n", a, b);
            say(m);
            HAL_Delay(4000);
        }
    }
}

static void test_buttons(void)
{
    char m[80];
    uint8_t last = 0xFF;
    say("\r\n=== TEST NUT: dong dau = trang thai khi KHONG nhan ===\r\n");
    for (;;) {
        uint8_t v = 0;
        v |= HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin) << 0;
        v |= HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin) << 1;
        v |= HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin) << 2;
        v |= HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin) << 3;
        if (v != last) {
            last = v;
            snprintf(m, sizeof m, "BTN1=%d BTN2=%d BTN3=%d BTN4=%d\r\n",
                     v & 1, (v >> 1) & 1, (v >> 2) & 1, (v >> 3) & 1);
            say(m);
        }
        HAL_Delay(20);
    }
}

static void test_i2c_scan(void)
{
    char m[48];
    for (;;) {
        say("\r\n=== I2C SCAN ===\r\n");
        int n = 0;
        for (uint8_t addr = 1; addr < 128; addr++) {
            if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) == HAL_OK) {
                snprintf(m, sizeof m, "Thay thiet bi: 0x%02X\r\n", addr);
                say(m);
                n++;
            }
        }
        snprintf(m, sizeof m, "Tong: %d thiet bi\r\n", n);
        say(m);
        HAL_Delay(3000);
    }
}

static void test_lcd(void)
{
    char m[24];
    uint32_t s = 0;
    lcd_init();
    for (;;) {
        snprintf(m, sizeof m, "LCD OK 0x%02X", LCD_I2C_ADDR_7BIT);
        lcd_print_row(0, m);
        snprintf(m, sizeof m, "Count: %lu", (unsigned long)s++);
        lcd_print_row(1, m);
        HAL_Delay(1000);
    }
}

void hw_test_run(void)      /* không bao giờ return */
{
#if   HW_TEST == 1
    test_lights();
#elif HW_TEST == 2
    test_buttons();
#elif HW_TEST == 3
    test_i2c_scan();
#else
    test_lcd();
#endif
}
