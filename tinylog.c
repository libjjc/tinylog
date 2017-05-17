#include "logdef.h"
#include "tinylog.h"
#include "logadapter.h"
#include "loghelper.h"
#include "catagory.h"
#include <string.h>

struct catagory gl_logger_root;

void 
tlinit()
{
}

void 
tlshutdown()
{
}

struct catagory * 
root()
{
	return &gl_logger_root;
}

int
tlloadcfg(const char* config) {
	return 0;
}

void
tllog(int priority, const char* msg){

    struct catagory* _root = root();
    if (!_root) return;
    struct logmsg logmsg;
    if (priority <= TLL_EMERG){
        logmsg.priority = TLL_EMERG;
        logmsg.prioname = lscreate(EN_EMERG, strlen(EN_EMERG));
    } else if (priority <= TLL_FATAL){
        logmsg.priority = TLL_FATAL;
        logmsg.prioname = lscreate(EN_FATAL, strlen(EN_FATAL));
    } else if (priority <= TLL_ALERT){
        logmsg.priority = TLL_ALERT;
        logmsg.prioname = lscreate(EN_ALERT, strlen(EN_ALERT));
    } else if (priority <= TLL_ERROR){
        logmsg.priority = TLL_ERROR;
        logmsg.prioname = lscreate(EN_ERROR, strlen(EN_ERROR));
    } else if (priority <= TLL_WARN){
        logmsg.priority = TLL_WARN;
        logmsg.prioname = lscreate(EN_WARN, strlen(EN_WARN));
    } else if (priority <= TLL_NOTICE){
        logmsg.priority = TLL_NOTICE;
        logmsg.prioname = lscreate(EN_NOTICE, strlen(EN_NOTICE));
    } else if (priority <= TLL_INFO){
        logmsg.priority = TLL_INFO;
        logmsg.prioname = lscreate(EN_INFO, strlen(EN_INFO));
    } else if (priority <= TLL_DEBUG){
        logmsg.priority = TLL_DEBUG;
        logmsg.prioname = lscreate(EN_DEBUG, strlen(EN_DEBUG));
    } else if (priority <= TLL_NOTSET){
        logmsg.priority = TLL_NOTSET;
        logmsg.prioname = lscreate(EN_NOTSET, strlen(EN_NOTSET));
    } else return;

    logmsg.msg = lscreate(msg, strlen(msg));
    logmsg.timestamp = timestamp();

    capacityLogging(_root, &logmsg);
}

void 
tlemerg(const char * msg){
    tllog(TLL_EMERG, msg);
}

void 
tlfatal(const char * msg){
    tllog(TLL_FATAL, msg);
}

void 
tlalert(const char * msg){
    tllog(TLL_ALERT, msg);
}

void
tlerror(const char* msg){
    tllog(TLL_ERROR, msg);
}

void 
tlwarn(const char * msg){
    tllog(TLL_WARN, msg);
}

void 
tlnotice(const char * msg){
    tllog(TLL_NOTICE, msg);
}

void 
tlinfo(const char * msg){
    tllog(TLL_INFO, msg);
}

void 
tldebug(const char * msg){
    tllog(TLL_DEBUG, msg);
}

void 
tlnotset(const char * msg){
    tllog(TLL_NOTSET, msg);
}

