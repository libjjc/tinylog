#include <stdio.h>
#ifdef _WIN
#include <sys/timeb.h>
#elif defined(_LINUX)
#include <sys/time.h>
#endif

#include "tick.h"

static struct _log_ts _gl_ts;

#ifdef _WIN
int 
_init_global_ts(){
    timestamp(&_gl_ts);
    return 0;
}
typedef int func();
#pragma data_seg(".CRT$XIU")
static func * before = { _init_global_ts };
#pragma data_seg(".CRT$XPU")
#pragma data_seg()
#elif defined(_LINUX)
__attribute((constructor))void
_init_global_ts(){
    timestamp(&_gl_ts);
}
#endif


void
timestamp(struct _log_ts* ts){
#ifdef _WIN
    struct timeb tb;
    ftime(&tb);
    ts->sec = tb.time;
    ts->usec = tb.millitm*1000;
#elif defined(_LINUX)
    struct timeval tv;
    gettimeofday(&tv, 0);
    ts->sec = tv.tv_sec;
    ts->usec = tv.tv_usec;
#endif
}

void
tsprocess(struct _log_ts* ts){
    ts->sec = _gl_ts.sec;
    ts->usec = _gl_ts.usec;
}

void
tsepoch(struct _log_ts* ts){
    timestamp(ts);
}

long
delta_us(const struct _log_ts* far, const struct _log_ts* near){
    if (!far || !near)return 0;
    return near->usec - far->usec;
}

long
delta_ms(const struct _log_ts* far, const struct _log_ts* near){
    if (!far || !near)return 0;
    return (near->usec - far->usec) / 1000;
}

long
delta_s(const struct _log_ts* far, const struct _log_ts* near){
    if (!far || !near)return 0;
    return (long)(near->sec - far->sec);
}

long
detla_us_epoch(const struct _log_ts* ts){
    return delta_us(&_gl_ts, ts);
}

long
delta_ms_epoch(const struct _log_ts* ts){
    return delta_ms(&_gl_ts, ts);
}

long
delta_s_epoch(const struct _log_ts* ts){
    return delta_s(&_gl_ts, ts);
}

int
delta_tm(const struct _log_ts* far, const struct _log_ts* near, struct tm* t){
    if (!far || !near || !t) return 0;
    time_t time = near->sec - far->sec;
    return localtime_r(&time,t);
}

long
tick(){
    return clock();
}


