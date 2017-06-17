#ifndef TINY_LOG_HH
#define TINY_LOG_HH
#include <stdbool.h>

#include "logstr.h"
#include "config.h"

void 
_tlinit();

void
_tlopen();

void
_tlshutdown();

struct _catagory*
_root();

int 
_tlloadcfg(const char* config);

void
_tllog(int priority, const char* msg, ...);

void
_tlemerg(const char* msg, ...);

void
_tlfatal(const char* msg, ...);

void
_tlalert(const char* msg, ...);

void
_tlerror(const char* msg, ...);

void
_tlwarn(const char* msg, ...);

void
_tlnotice(const char* msg, ...);

void
_tlinfo(const char* msg, ...);

void
_tldebug(const char* msg, ...);

void
_tlnotset(const char* msg, ...);

#endif//TINY_LOG_HH