#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdint.h>

void    dht20_init(void);      /* HAL_Delay, chỉ gọi trước scheduler */
void    dht20_task(void);      /* task chu kỳ (2 s): gửi lệnh đo, rồi one-shot đọc kết quả sau 100 ms */
uint8_t dht20_is_valid(void);
int16_t dht20_get_temp_x10(void);   /* 275 = 27.5 C */
int16_t dht20_get_humi_x10(void);   /* 601 = 60.1 % */

#endif
