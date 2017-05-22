#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include <stdbool.h>
#include "logdef.h"
#include "logstr.h"

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_ADAPTERS 16

struct _adapter;

struct _catagory {
    int priority;
    int countAdapters;
    int countChildren;
    ls_t name;
    struct _catagory* parent;
    struct _catagory* children[MAX_CATAGORY_CHILDREN];
    struct _adapter* adapters[MAX_CATAGORY_ADAPTERS];
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
get_catagory_create(const char* parent,const char* name);

void
freeCatagory(struct _catagory* cg);

int
addCatagory(struct _catagory* parent, struct _catagory* child);

int
removeCatagory(struct _catagory* parent, struct _catagory* child);

struct _catagory*
findCatagory(struct _catagory* parent, const char* name);

int
addAdapter(struct _catagory* cg, struct _adapter* ada);

int
removeAdapter(struct _catagory* cg, struct _adapter* ada);

bool
hasAdapter(struct _catagory* cq, struct _adapter* ada);

bool
has_apt_recursive(struct _catagory* cq, struct _adapter* apt);

struct _adapter*
find_adapter(struct _catagory* cq, const char* aptname);

int
capacityLogging(struct _catagory* cata, struct _log_msg* msg);

#endif//LOG_CATAGORY_HH
