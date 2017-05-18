
#ifdef _WIN
#include <sys/timeb.h>
#elif defined(_LINUX)
#include <sys/time.h>
#endif

#include "logmsg.h"


void
timestamp(struct _log_ts* ts){
#ifdef _WIN
    struct timeb tb;
    ftime(&tb);
    ts->sec = tb.time;
    ts->usec = tb.millitm;
#elif defined(_LINUX)
    struct timeval tv;
    gettimeofday(&tv, 0);
    ts->sec = tv.tv_sec;
    ts->usec = tv.tv_usec;
#endif
}
