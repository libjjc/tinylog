#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include <stdbool.h>
#include "logdef.h"
#include "logstr.h"

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_loggerS 16

struct _catagory {
    int priority;
    int countLoggers;
    int countChildren;
    ls_t name;
    struct _catagory* parent;
    struct _catagory* children[MAX_CATAGORY_CHILDREN];
    _callback_ptr loggers[MAX_CATAGORY_loggerS];
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
_find_catagory(struct _catagory* parent, const char* name);

int
_add_logger(struct _catagory* cg, _callback_ptr ada);

int
_remove_logger(struct _catagory* cg, _callback_ptr ada);

int
_replace_logger(struct _catagory* cg, _callback_ptr old,_callback_ptr logger);

bool
_has_logger(struct _catagory* cq, _callback_ptr ada);

bool
has_logger_recursive(struct _catagory* cq, _callback_ptr logger);

_callback_ptr
find_logger(struct _catagory* cq, const char* loggername);

int
_catagory_logging(struct _catagory* cata, struct _log_msg* msg);

#endif//LOG_CATAGORY_HH
