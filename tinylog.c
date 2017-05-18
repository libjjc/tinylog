#include "logdef.h"
#include "tinylog.h"
#include "adapter.h"
#include "helper.h"
#include "catagory.h"
#include "logmsg.h"
#include <string.h>

struct _catagory gl_logger_root;

void
tlinit()
{
}

void
tlshutdown()
{
}

struct _catagory *
    root()
{
    return &gl_logger_root;
}

int
tlloadcfg(const char* config) {
    return 0;
}

void
tllog(int priority, const char* _msg, ...){

    struct _catagory* _root = root();
    if (!_root) return;
    struct _log_msg msg;
    if (priority <= TLL_EMERG){
        msg.prior = TLL_EMERG;
        msg.s_prior = lscreate(EN_EMERG, strlen(EN_EMERG));
    } else if (priority <= TLL_FATAL){
        msg.prior = TLL_FATAL;
        msg.s_prior = lscreate(EN_FATAL, strlen(EN_FATAL));
    } else if (priority <= TLL_ALERT){
        msg.prior = TLL_ALERT;
        msg.s_prior = lscreate(EN_ALERT, strlen(EN_ALERT));
    } else if (priority <= TLL_ERROR){
        msg.prior = TLL_ERROR;
        msg.s_prior = lscreate(EN_ERROR, strlen(EN_ERROR));
    } else if (priority <= TLL_WARN){
        msg.prior = TLL_WARN;
        msg.s_prior = lscreate(EN_WARN, strlen(EN_WARN));
    } else if (priority <= TLL_NOTICE){
        msg.prior = TLL_NOTICE;
        msg.s_prior = lscreate(EN_NOTICE, strlen(EN_NOTICE));
    } else if (priority <= TLL_INFO){
        msg.prior = TLL_INFO;
        msg.s_prior = lscreate(EN_INFO, strlen(EN_INFO));
    } else if (priority <= TLL_DEBUG){
        msg.prior = TLL_DEBUG;
        msg.s_prior = lscreate(EN_DEBUG, strlen(EN_DEBUG));
    } else if (priority <= TLL_NOTSET){
        msg.prior = TLL_NOTSET;
        msg.s_prior = lscreate(EN_NOTSET, strlen(EN_NOTSET));
    } else return;

    msg.msg = lscreate(_msg, strlen(_msg));
    msg.ts.sec = 0;

    capacityLogging(_root, &msg);

    lsfree(msg.cagy);
    lsfree(msg.msg);
    lsfree(msg.s_prior);
}

void
tlemerg(const char * msg, ...){
    tllog(TLL_EMERG, msg);
}

void
tlfatal(const char * msg, ...){
    tllog(TLL_FATAL, msg);
}

void
tlalert(const char * msg, ...){
    tllog(TLL_ALERT, msg);
}

void
tlerror(const char* msg, ...){
    tllog(TLL_ERROR, msg);
}

void
tlwarn(const char * msg, ...){
    tllog(TLL_WARN, msg);
}

void
tlnotice(const char * msg, ...){
    tllog(TLL_NOTICE, msg);
}

void
tlinfo(const char * msg, ...){
    tllog(TLL_INFO, msg);
}

void
tldebug(const char * msg, ...){
    tllog(TLL_DEBUG, msg);
}

void
tlnotset(const char * msg, ...){
    tllog(TLL_NOTSET, msg);
}

