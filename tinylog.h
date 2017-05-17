#ifndef TINY_LOG_HH
#define TINY_LOG_HH
#include "logstr.h"
#include <stdbool.h>


void 
tlinit();

void
tlshutdown();

struct catagory*
root();

int 
tlloadcfg(const char* config);

void
tllog(int priority, const char* msg);

void
tlemerg(const char* msg);

void
tlfatal(const char* msg);

void
tlalert(const char* msg);

void
tlerror(const char* msg);

void
tlwarn(const char* msg);

void
tlnotice(const char* msg);

void
tlinfo(const char* msg);

void
tldebug(const char* msg);

void
tlnotset(const char* msg);

#endif//TINY_LOG_HH