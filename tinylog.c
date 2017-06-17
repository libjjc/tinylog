#include <string.h>
#include "logdef.h"
#include "tinylog.h"
#include "logger.h"
#include "catagory.h"
#include "logmsg.h"
struct _catagory gl_logger_root;

void
_tlinit(){
    gl_logger_root.name = lscreate("root", 4);
}

int
_tl_local_reopen_(struct _catagory* catagory){
    int success = 0;
    if (!catagory) return success;
    for (int i = 0; i < catagory->countLoggers; i++){
        _logger_t logger = catagory->loggers[i];
        success |= _reopen_logger(logger);
    }
    for (int i = 0; i < catagory->countChildren; i++){
        success |= _tl_local_reopen_(catagory->children[i]);
    }
    return success;
}

void
_tlopen(){
    int result = _tl_local_reopen_(&gl_logger_root);
}

void
_tlshutdown(){
}

struct _catagory *
_root()
{
    return &gl_logger_root;
}

int
_tlloadcfg(const char* config) {
    return 0;
}

void
_tllog(int priority, const char* _msg, ...){

    struct _catagory* cgroot = _root();
    if (!cgroot) return;
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

    _catagory_logging(cgroot, &msg);

    lsfree(msg.catagory);
    lsfree(msg.msg);
    lsfree(msg.s_prior);
}

void
_tlemerg(const char * msg, ...){
    _tllog(TLL_EMERG, msg);
}

void
_tlfatal(const char * msg, ...){
    _tllog(TLL_FATAL, msg);
}

void
_tlalert(const char * msg, ...){
    _tllog(TLL_ALERT, msg);
}

void
_tlerror(const char* msg, ...){
    _tllog(TLL_ERROR, msg);
}

void
_tlwarn(const char * msg, ...){
    _tllog(TLL_WARN, msg);
}

void
tlnotice(const char * msg, ...){
    _tllog(TLL_NOTICE, msg);
}

void
_tlinfo(const char * msg, ...){
    _tllog(TLL_INFO, msg);
}

void
_tldebug(const char * msg, ...){
    _tllog(TLL_DEBUG, msg);
}

void
_tlnotset(const char * msg, ...){
    _tllog(TLL_NOTSET, msg);
}

