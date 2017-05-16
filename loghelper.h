#ifndef LOG_HELPER_HH
#define LOG_HELPER_HH

#include <time.h>
#include "logstr.h"

int
getTimeStemp();

ls_t
getTimeFormat(const char* fmt);

ls_t
getTimeDefault();

#endif//LOG_HELPER_HH
