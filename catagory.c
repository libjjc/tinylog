
#include <malloc.h>
#include <string.h>
#include "logdef.h"
#include "catagory.h"
#include "adapter.h"

extern struct _catagory gl_logger_root;

bool
bigger_filter(int priorityMsg, int priorityAda){
    return priorityMsg > priorityAda;
}

bool
smaller_filter(int priorityMsg, int priorityAda){
    return priorityMsg < priorityAda;
}

bool
equal_filter(int priorityMsg, int priorityAda){
    return priorityMsg == priorityAda;
}

struct _catagory*
createCatagory(struct _catagory* parent, int priority, const char* name){
    struct _catagory* cg;
    cg = (struct _catagory*)malloc(sizeof(struct _catagory) + 1);
    if (!cg) return NULL;
    cg->parent = parent;
    cg->priority = priority;
    cg->name = lscreate(name, strlen(name));
    cg->countAdapters = 0;
    cg->countChildren = 0;
    if (parent){
        parent->children[parent->countChildren] = cg;
        parent->countChildren++;
    }
    return cg;
}

struct _catagory*
createNullCatagory(const char* name){
    struct _catagory* cg;
    cg = (struct _catagory*)malloc(sizeof(struct _catagory) + 1);
    if (!cg) return NULL;
    cg->parent = NULL;
    cg->priority = TLL_NOTSET;
    cg->name = lscreate(name,strlen(name));
    cg->countAdapters = 0;
    cg->countChildren = 0;
    return cg;
}

struct _catagory*
get_catagory_create(const char* parent,const char* name){
    struct _catagory* parent_cata = NULL;
    struct _catagory* cata = NULL;
    if (!parent_cata){
        parent_cata = &gl_logger_root;
    } else{
        parent_cata = findCatagory(&gl_logger_root, name);
        if (!parent_cata){
            parent_cata = createCatagory(&gl_logger_root, TLL_NOTSET, name);
        }
    }
    if (!parent_cata) return NULL;
    cata = findCatagory(&gl_logger_root, name);
    if (!cata){
        cata = createCatagory(parent_cata, TLL_NOTSET, name);
    }
    return cata;
}

void 
freeCatagory(struct _catagory* cg){
    if (!cg){
        if(cg->name)lsfree(cg->name);
        for (int i = 0; i < cg->countAdapters; i++){
            struct _adapter* ada = cg->adapters[i];
            (*ada->free)(ada);
        }
        for (int i = 0; i < cg->countChildren; i++){
            freeCatagory(cg->children[i]);
        }
        free(cg);
    }
}

int
addCatagory(struct _catagory* parent, struct _catagory* child){
    if (parent&&child){
        parent->children[parent->countChildren] = child;
        parent->countChildren++;
        return 0;
    }
    return -1;
}

int
removeCatagory(struct _catagory* parent, struct _catagory* child){
    if (!parent || !child) return -1;
    int index = 0;
    for (; index <= parent->countChildren; index++){
        if (parent->children[index] == child){
            break;
        }
    }
    if (index >= parent->countChildren) return -1;

    char* s = (char*)&parent->children[0];
    char* e = (char*)&parent->children[MAX_CATAGORY_CHILDREN];
    char* p = (char*)&parent->children[index];
    if (memmove_s(p, s - p, p + sizeof(struct _catagory*), s - p - 1)){
        return -1;
    }
    parent->countChildren--;

    return 0;
}

struct _catagory*
findCatagory(struct _catagory* parent, const char* name){
    struct _catagory* cg = NULL;
    if (!parent) return cg;
    for (int i = 0; i <= parent->countChildren; i++){
        if (!parent->children[i] && parent->children[i]->name && !lscmp(parent->children[i]->name, name)){
            cg = parent->children[i];
            break;
        }
    }
    return cg;
}

int
addAdapter(struct _catagory* cg, struct _adapter* ada){
    if (!cg || !ada) return -1;
    cg->adapters[cg->countAdapters] = ada;
    cg->countAdapters++;
    return 0;
}

int
removeAdapter(struct _catagory* cg, struct _adapter* ada){
    if (!cg || !ada) return -1;
    int index = 0;
    for (; index <= cg->countAdapters; index++){
        if (cg->adapters[index] == ada){
            break;
        }
    }
    if (index >= cg->countAdapters) return -1;

    char* s = (char*)&cg->adapters[0];
    char* e = (char*)&cg->adapters[MAX_CATAGORY_ADAPTERS];
    char* p = (char*)&cg->adapters[index];
    if (memmove_s(p, s - p, p + sizeof(struct _adapter*), s - p - 1)){
        return -1;
    }
    cg->countAdapters--;

    return 0;
}

bool
hasAdapter(struct _catagory* cg, struct _adapter* ada){
    if (!cg || !ada) return false;
    int index = 0;
    for (; index <= cg->countAdapters; index++){
        if (cg->adapters[index] == ada){
            break;
        }
    }
    if (index >= cg->countAdapters) return false;
    return true;
}

int
capacityLogging(struct _catagory* cata, struct _log_msg* msg){
    if (!cata || !msg) return -1;
    int ret = 0;
    for (int i = 0; i < cata->countAdapters; i++){
        struct _adapter* adapter = cata->adapters[i];
        ret &= (*adapter->accept)(adapter, msg);
    }
    for (int i = 0; i < cata->countChildren;i++){
        ret &= capacityLogging(cata->children[i], msg);
    }
    return ret;
}

bool
has_apt_recursive(struct _catagory* cq, struct _adapter* apt){
    return false;
}

struct _adapter*
find_adapter(struct _catagory* cq, const char* aptname){
    struct _adapter* apt = NULL;
    for (int i = 0; i < cq->countAdapters; i++){
        apt = cq->adapters[i];
        if (apt) return apt;
    }
    for (int i = 0; i < cq->countChildren; i++){
        apt = find_adapter(cq->children[i], aptname);
        if (apt) return apt;
    }
    return apt;
}