#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include <stdbool.h>
#include "logdef.h"
#include "logstr.h"

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_ADAPTERS 16

struct _catagory {
    int priority;
    int countAdapters;
    int countChildren;
    ls_t name;
    struct _catagory* parent;
    struct _catagory* children[MAX_CATAGORY_CHILDREN];
    _callback_ptr adapters[MAX_CATAGORY_ADAPTERS];
    char eoc[];
};

typedef bool(*filter)(int priorityMsg, int priorityAda);

bool 
bigger_filter(int priorityMsg, int priorityAda);

bool 
smaller_filter(int priorityMsg, int priorityAda);

bool 
equal_filter(int priorityMsg, int priorityAda);

struct _catagory*
createCatagory(struct _catagory* parent,int priority,const char* name);

struct _catagory*
createNullCatagory(const char* name);

struct _catagory*
get_create_catagory(const struct _catagory* parent,const char* name);

void
freeCatagory(struct _catagory* cg);

int
addCatagory(struct _catagory* parent, struct _catagory* child);

int
removeCatagory(struct _catagory* parent, struct _catagory* child);

struct _catagory*
findCatagory(struct _catagory* parent, const char* name);

int
addAdapter(struct _catagory* cg, _callback_ptr ada);

int
removeAdapter(struct _catagory* cg, _callback_ptr ada);

int
_replace_adapter(struct _catagory* cg, _callback_ptr old,_callback_ptr apt);

bool
hasAdapter(struct _catagory* cq, _callback_ptr ada);

bool
has_apt_recursive(struct _catagory* cq, _callback_ptr apt);

_callback_ptr
find_adapter(struct _catagory* cq, const char* aptname);

int
capacityLogging(struct _catagory* cata, struct _log_msg* msg);

#endif//LOG_CATAGORY_HH
