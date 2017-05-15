#ifndef TINY_LOG_HH
#define TINY_LOG_HH
#include "logstr.h"
#include <stdbool.h>

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_ADAPTERS 16
struct adapter;
struct logmsg {
    int priority;
    int timestamp;
    ls_t prioname;
    ls_t msg;
};

struct catagory {
    int priority;
    int countAdapters;
    int countChildren;
    ls_t name;
    struct catagory* parent;
    struct catagory* children[MAX_CATAGORY_CHILDREN];
    struct adapter* adapters[MAX_CATAGORY_ADAPTERS];
    char eoc[];
};

void 
tlinit();

void
tlshutdown();

struct adapter*
root();

int tlloadcfg(const char* config);

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

typedef bool(*filter)(int priorityMsg, int priorityAda);

bool 
bigger_filter(int priorityMsg, int priorityAda);

bool 
smaller_filter(int priorityMsg, int priorityAda);

bool 
equal_filter(int priorityMsg, int priorityAda);

struct catagory*
createCatagory(struct catagory* parent,int priority,const char* name);

struct catagory*
createNullCatagory();

void
freeCatagory(struct catagory* cg);

int
addCatagory(struct catagory* parent, struct catagory* child);

int
removeCatagory(struct catagory* parent, struct catagory* child);

struct catagory*
findCatagory(struct catagory* parent, const char* name);

int
addAdapter(struct catagory* cg, struct adapter* ada);

int
removeAdapter(struct catagory* cg, struct adapter* ada);

bool
hasAdapter(struct catagory* cq, struct adapter* ada);



#endif//TINY_LOG_HH