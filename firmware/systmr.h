#ifndef _SYSTMR_H_
#define _SYSTMR_H_

typedef systmr_t int32_t;
typedef systick_t int64_t;

void systmr_init(void);
systmr_t systmr_nowMillis();
systmr_t systmr_nowMicros();

#endif