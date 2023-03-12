#include <stdio.h>
#include <time.h>

#ifdef __linux
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#elif _WIN32
#include <windows.h>
#endif

#include "interval_timer.h"


struct interval_timer {
    int64_t interval;
#ifdef __linux
	int32_t tfd;
#elif _WIN32
    HANDLE htimer;
    LARGE_INTEGER duetime;
#endif
};


#ifdef __linux
static interval_timer_t interval_timer_construct_linux(void);
static int interval_timer_settime_linux(interval_timer_t itimer, int64_t interval);
static void interval_timer_wait_linux(interval_timer_t itimer);
static int interval_timer_destruct_linux(interval_timer_t itimer);
#elif _WIN32
static interval_timer_t interval_timer_construct_win(void);
static int interval_timer_settime_win(interval_timer_t itimer, int64_t interval);
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

int interval_timer_settime(interval_timer_t itimer, int64_t interval)
{
#ifdef __linux
    return interval_timer_settime_linux(itimer, interval);
#elif _WIN32
    return interval_timer_settime_win(itimer, interval);
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
    itimer->interval = 0;
	itimer->tfd = tfd;
	return itimer;
}

static int interval_timer_settime_linux(interval_timer_t itimer, int64_t interval)
{
	struct itimerspec its;
	uint32_t interval_sec = interval / 1000;
	uint64_t interval_nsec = (interval - (interval_sec * 1000)) * 1000000;
	its.it_interval.tv_sec = interval_sec;
	its.it_interval.tv_nsec = interval_nsec;
	its.it_value.tv_sec = interval_sec;
	its.it_value.tv_nsec = interval_nsec;
	timerfd_settime(itimer->tfd, 0, &its, 0);
	return 0;
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
	return 0;
}
#endif  // __linux

#ifdef _WIN32
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
    if (!CancelWaitableTimer(self->itimer)) {
        return NULL;
    }
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
#endif  // _WIN32
