#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include <stdint.h>

typedef enum { LIGHT_OFF = 0, LIGHT_RED, LIGHT_GREEN, LIGHT_YELLOW } TrafficColor_t;

void           traffic_init(void);
void           traffic_set(uint8_t road, TrafficColor_t c);              /* road = 1 hoặc 2 */
void           traffic_set_pair(TrafficColor_t c1, TrafficColor_t c2);   /* đặt cả 2 đường, chặn 2 xanh */
TrafficColor_t traffic_get(uint8_t road);
const char    *traffic_name(TrafficColor_t c);                           /* "RED" "GRN" "YEL" "OFF" */

#endif
