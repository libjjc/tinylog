#ifndef TINY_LOG_HH
#define TINY_LOG_HH

struct adapter;

void 
tlinit();

void
tlshutdown();

struct adapter*
root();

#define TLL_EMERG		0
#define TLL_FATAL		100
#define TLL_ALERT		200
#define TLL_ERROR		300
#define TLL_WARN		400
#define TLL_NOTICE		500
#define TLL_INFO		600
#define TLL_DEBUG		700
#define TLL_NOTSET		800

void
tllog(int priority, const char* msg);

void
tlemerg(const char* msg);

void
tlfatal(const char* msg);

void
tlalert(const char* msg);

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