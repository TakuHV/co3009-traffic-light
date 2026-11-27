#include "traffic_light.h"
#include "main.h"

/* Bảng tổ hợp {A, B}. GIẢ ĐỊNH - sửa theo bảng đo ở Bước 3.2 */
static const uint8_t code[4][2] = {
    [LIGHT_OFF]    = {0, 0},
    [LIGHT_RED]    = {0, 1},
    [LIGHT_GREEN]  = {1, 0},
    [LIGHT_YELLOW] = {1, 1},
};

static TrafficColor_t cur[3];

void traffic_set(uint8_t road, TrafficColor_t c)
{
    if (c > LIGHT_YELLOW) return;
    if (road == 1) {
        HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, code[c][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, code[c][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } else if (road == 2) {
        HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, code[c][0] ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, code[c][1] ? GPIO_PIN_SET : GPIO_PIN_RESET);
    } else return;
    cur[road] = c;
}

void traffic_set_pair(TrafficColor_t c1, TrafficColor_t c2)
{
    if (c1 == LIGHT_GREEN && c2 == LIGHT_GREEN) {   /* an toàn: không bao giờ 2 xanh */
        c1 = LIGHT_RED;
        c2 = LIGHT_RED;
    }
    traffic_set(1, c1);
    traffic_set(2, c2);
}

void traffic_init(void)  { traffic_set_pair(LIGHT_RED, LIGHT_RED); }

TrafficColor_t traffic_get(uint8_t road) { return (road == 1 || road == 2) ? cur[road] : LIGHT_OFF; }

const char *traffic_name(TrafficColor_t c)
{
    switch (c) {
    case LIGHT_RED:    return "RED";
    case LIGHT_GREEN:  return "GRN";
    case LIGHT_YELLOW: return "YEL";
    default:           return "OFF";
    }
}
