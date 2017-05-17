#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include "logstr.h"
#include <stdbool.h>

#define MAX_CATAGORY_CHILDREN 16
#define MAX_CATAGORY_ADAPTERS 16

struct adapter;

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

int
capacityLogging(struct catagory* cata, struct logmsg* msg);

#endif//LOG_CATAGORY_HH
