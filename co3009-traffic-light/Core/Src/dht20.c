#include "dht20.h"
#include "main.h"
#include "scheduler.h"

extern I2C_HandleTypeDef hi2c1;

#define DHT20_ADDR  (0x38 << 1)

static int16_t temp_x10, humi_x10;
static uint8_t valid = 0;

static uint8_t crc8(const uint8_t *d, uint8_t n)
{
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < n; i++) {
        crc ^= d[i];
        for (uint8_t b = 0; b < 8; b++)
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
    }
    return crc;
}

static void dht20_read_result(void)      /* one-shot task */
{
    uint8_t d[7];
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDR, d, 7, 20) != HAL_OK) return;
    if (d[0] & 0x80) return;             /* còn bận */
    if (crc8(d, 6) != d[6]) return;

    uint32_t raw_h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
    uint32_t raw_t = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];
    humi_x10 = (int16_t)((raw_h * 1000UL) >> 20);
    temp_x10 = (int16_t)(((raw_t * 2000UL) >> 20) - 500);
    valid = 1;
}

void dht20_init(void)
{
    HAL_Delay(100);                      /* DHT20 cần >= 100 ms sau khi cấp nguồn */
}

void dht20_task(void)
{
    uint8_t cmd[3] = {0xAC, 0x33, 0x00}; /* trigger measurement */
    if (HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDR, cmd, 3, 20) == HAL_OK)
        SCH_Add_Task(dht20_read_result, 10, 0);   /* one-shot sau 100 ms (>= 80 ms) */
}

uint8_t dht20_is_valid(void)      { return valid; }
int16_t dht20_get_temp_x10(void)  { return temp_x10; }
int16_t dht20_get_humi_x10(void)  { return humi_x10; }
