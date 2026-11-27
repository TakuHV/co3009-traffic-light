#ifndef INC_FSM_AUTOMATIC_H_
#define INC_FSM_AUTOMATIC_H_

void fsm_automatic_init(void);
void fsm_automatic_lights(void);   /* chu kỳ đèn tự động (cũng chạy nền khi ở Tuning) */
void fsm_automatic_run(void);      /* đèn + giao diện/phím của màn hình Auto */

#endif
