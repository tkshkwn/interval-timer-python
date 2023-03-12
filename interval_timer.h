/**
 * @file interval_timer.h
 * @author tkshkwn (nekotk37@gmail.com)
 * @brief interval_timer.h 
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdint.h>

#define INTERVAL_TIMER_OK 0
#define INTERVAL_TIMER_ERROR 1

typedef struct interval_timer *interval_timer_t;

interval_timer_t interval_timer_construct(void);
int interval_timer_settime(interval_timer_t itimer, int64_t interval_ms);
void interval_timer_wait(interval_timer_t itimer);
int interval_timer_destruct(interval_timer_t itimer);
