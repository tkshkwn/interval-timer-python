/**
 * @file interval_timer.c
 * @author tkshkwn (nekotk37@gmail.com)
 * @brief interval_timer.c
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifdef __linux
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#elif _WIN32
#include <windows.h>
#endif

#include "interval_timer.h"


struct interval_timer {
#ifdef __linux
    int32_t tfd;
#elif _WIN32
    HANDLE htimer;
#endif
};


#ifdef __linux
static interval_timer_t interval_timer_construct_linux(void);
static int interval_timer_settime_linux(interval_timer_t itimer, int64_t interval_ms);
static void interval_timer_wait_linux(interval_timer_t itimer);
static int interval_timer_destruct_linux(interval_timer_t itimer);
#elif _WIN32
static interval_timer_t interval_timer_construct_win(void);
static int interval_timer_settime_win(interval_timer_t itimer, int64_t interval_ms);
static void interval_timer_wait_win(interval_timer_t itimer);
static int interval_timer_destruct_win(interval_timer_t itimer);
#endif


interval_timer_t interval_timer_construct(void)
{
#ifdef __linux
    return interval_timer_construct_linux();
#elif _WIN32
    return interval_timer_construct_win();
#endif
}

int interval_timer_settime(interval_timer_t itimer, int64_t interval_ms)
{
#ifdef __linux
    return interval_timer_settime_linux(itimer, interval_ms);
#elif _WIN32
    return interval_timer_settime_win(itimer, interval_ms);
#endif
}

void interval_timer_wait(interval_timer_t itimer)
{
#ifdef __linux
    interval_timer_wait_linux(itimer);
#elif _WIN32
    interval_timer_wait_win(itimer);
#endif
}

int interval_timer_destruct(interval_timer_t itimer)
{
#ifdef __linux
    return interval_timer_destruct_linux(itimer);
#elif _WIN32
    return interval_timer_destruct_win(itimer);
#endif
}

#ifdef __linux
static interval_timer_t interval_timer_construct_linux(void)
{
    interval_timer_t itimer;
    int32_t tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd == -1) {
        return NULL;
    }
    itimer = (interval_timer_t)malloc(sizeof(struct interval_timer));
    if (itimer == NULL) {
        return NULL;
    }
    itimer->tfd = tfd;
    return itimer;
}

static int interval_timer_settime_linux(interval_timer_t itimer, int64_t interval_ms)
{
    struct itimerspec its;
    uint32_t interval_sec = interval_ms / 1000;
    uint64_t interval_nsec = (interval_ms - (interval_sec * 1000)) * 1000000;
    its.it_interval.tv_sec = interval_sec;
    its.it_interval.tv_nsec = interval_nsec;
    its.it_value.tv_sec = interval_sec;
    its.it_value.tv_nsec = interval_nsec;
    timerfd_settime(itimer->tfd, 0, &its, 0);
    return INTERVAL_TIMER_OK;
}

static void interval_timer_wait_linux(interval_timer_t itimer)
{
    static uint64_t buf;
    read(itimer->tfd, &buf, sizeof(uint64_t));
}

static int interval_timer_destruct_linux(interval_timer_t itimer)
{
    close(itimer->tfd);
    free(itimer);
    return INTERVAL_TIMER_OK;
}
#endif  // __linux

#ifdef _WIN32
static interval_timer_t interval_timer_construct_win(void)
{
    interval_timer_t itimer = NULL;
    HANDLE htimer;
    htimer = CreateWaitableTimerW(NULL, FALSE, NULL);
    if (htimer == NULL) {
        /* error */
        return NULL;
    }
    itimer = (interval_timer_t)malloc(sizeof(struct interval_timer));
    if (itimer == NULL) {
        return NULL;
    }
    itimer->htimer = htimer;
    if (!CancelWaitableTimer(itimer->htimer)) {
        return NULL;
    }
    return itimer;
}

static int interval_timer_settime_win(interval_timer_t itimer, int64_t interval_ms)
{
    LARGE_INTEGER duetime;
    duetime.QuadPart = -10000 * (LONG)interval_ms;
    if (!SetWaitableTimer(itimer->htimer, &duetime, interval_ms, NULL, NULL, 0)) {
        /* error */
        return INTERVAL_TIMER_ERROR;
    }
    return INTERVAL_TIMER_OK;
}

static void interval_timer_wait_win(const interval_timer_t itimer)
{
    if (WaitForSingleObject(itimer->htimer, INFINITE) != WAIT_OBJECT_0) {
        /* error */
        return;
    }
}

static int interval_timer_destruct_win(interval_timer_t itimer)
{
    if (!CloseHandle(itimer->htimer)) {
        return INTERVAL_TIMER_ERROR;
    }
    free(itimer);
    return INTERVAL_TIMER_OK;
}
#endif  // _WIN32
