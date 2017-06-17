#ifndef TINYLOG_LOG_MSG_HH
#define TINYLOG_LOG_MSG_HH

#include <time.h>

#include "logdef.h"
#include "logstr.h"
#include "tick.h"


struct _log_msg {
    int prior;
    ls_t s_prior;
    ls_t catagory;
    ls_t msg;
    struct _log_ts ts;
};






#endif//TINYLOG_LOG_MSG_HH
