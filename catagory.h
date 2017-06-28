#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include <stdbool.h>
#include "logdef.h"
#include "logstr.h"

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_loggerS 16

struct _logger;
struct _catagory {
    int priority;
    int countLoggers;
    int countChildren;
    ls_t name;
    struct _catagory* parent;
    struct _catagory* children[MAX_CATAGORY_CHILDREN];
    struct _logger* loggers[MAX_CATAGORY_loggerS];
    char eoc[];
};

typedef bool(*filter)(int priorityMsg, int priorityAda);

bool 
_bigger_filter(int priorityMsg, int priorityAda);

bool 
_smaller_filter(int priorityMsg, int priorityAda);

bool 
_equal_filter(int priorityMsg, int priorityAda);

struct _catagory*
_create_catagory(struct _catagory* parent,int priority,const char* name);

struct _catagory*
_create_null_catagory(const char* name);

struct _catagory*
_get_create_catagory(const struct _catagory* parent,const char* name);

void
_free_catagory(struct _catagory* cg);

int
_add_catagory(struct _catagory* parent, struct _catagory* child);

int
_remove_catagory(struct _catagory* parent, struct _catagory* child);

struct _catagory*
_find_catagory(struct _catagory* parent, const char* name);

int
_add_logger(struct _catagory* cg, struct _logger* ada);

int
_remove_logger(struct _catagory* cg, struct _logger* ada);

int
_replace_logger(struct _catagory* cg, struct _logger* old,struct _logger* logger);

bool
_has_logger(struct _catagory* cq, struct _logger* ada);

bool
_has_logger_recursive(struct _catagory* cq, struct _logger* logger);

struct _logger*
_find_logger(struct _catagory* cq, const char* loggername);

int
_catagory_logging(struct _catagory* cata, struct _log_msg* msg);

#endif//LOG_CATAGORY_HH
