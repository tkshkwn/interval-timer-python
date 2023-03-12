#include <stdio.h>
#include <time.h>

#ifdef __linux
#include <sys/timerfd.h>
#elif _WIN32
#include <windows.h>
#endif

#include "interval_timer.h"


struct interval_timer {
    HANDLE htimer;
    LARGE_INTEGER duetime;
    int64_t interval;
};

static interval_timer_t *interval_timer_construct_win(void);
static int interval_timer_settime_win(interval_timer_t itimer, int64_t interval);
static void interval_timer_wait_win(interval_timer_t itimer);
static int interval_timer_destruct_win(interval_timer_t itimer);


interval_timer_t *interval_timer_construct(void)
{
    #ifdef __linux
    #elif _WIN32
    return interval_timer_construct_win();
    #endif
}

int interval_timer_settime(interval_timer_t itimer, int64_t interval)
{
    #ifdef __linux
    #elif _WIN32
    return interval_timer_settime_win(itimer, interval);
    #endif
}

void interval_timer_wait(interval_timer_t itimer)
{
    #ifdef __linux
    #elif _WIN32
    interval_timer_wait_win(itimer);
    #endif
}

int interval_timer_destruct(interval_timer_t itimer)
{
    #ifdef __linux
    #elif _WIN32
    return interval_timer_destruct_win(itimer);
    #endif
}

static interval_timer_t *interval_timer_construct_win(void)
{
    interval_timer_t itimer = NULL;
    HANDLE htimer;
    htimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (htimer == NULL) {
        /* error */
        return NULL;
    }
    itimer = (interval_timer_t)malloc(sizeof(struct interval_timer));
    if (itimer == NULL) {
        return NULL;
    }
    itimer->htimer = htimer;
    itimer->duetime.QuadPart = 0;
    itimer->interval = 0;
    return itimer;
}

static int interval_timer_settime_win(interval_timer_t itimer, int64_t interval_ms)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    itimer->interval = interval_ms * 1000 * 10;
    itimer->duetime.LowPart = ft.dwLowDateTime;
    itimer->duetime.HighPart = ft.dwHighDateTime;
    (itimer->duetime).QuadPart += itimer->interval;
    if (!SetWaitableTimer(itimer->htimer, &(itimer->duetime), 0, NULL, NULL, 0)) {
        /* error */
    }
    return 0;
}

static void interval_timer_wait_win(const interval_timer_t itimer)
{
    if (WaitForSingleObject(itimer->htimer, INFINITE) != WAIT_OBJECT_0) {
        /* error */
        return;
    }
    (itimer->duetime).QuadPart += itimer->interval;
    if (!SetWaitableTimer(itimer->htimer, &(itimer->duetime), 0, NULL, NULL, 0)) {
        /* error */
    }
}

static int interval_timer_destruct_win(interval_timer_t itimer)
{
    if (!CloseHandle(itimer->htimer)) {
        return 1;
    }
    free(itimer);
    return 0;
}
