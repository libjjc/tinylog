#include "tinylog.h"
#include "logadapter.h"
#include <string.h>

void 
tlinit()
{
}

void 
tlshutdown()
{
}

struct adapter * 
root()
{
	return NULL;
}

void
tllog(int priority, const char* msg){

    struct adapter* _root = root();
    if (!_root) return;
    struct logmsg logmsg;
    if (priority <= TLL_EMERG){
        logmsg.priority = TLL_EMERG;
    } else if (priority <= TLL_FATAL){
        logmsg.priority = TLL_FATAL;
    } else if (priority <= TLL_ALERT){
        logmsg.priority = TLL_ALERT;
    } else if (priority <= TLL_ERROR){
        logmsg.priority = TLL_ERROR;
    } else if (priority <= TLL_WARN){
        logmsg.priority = TLL_WARN;
    } else if (priority <= TLL_NOTICE){
        logmsg.priority = TLL_NOTICE;
    } else if (priority <= TLL_INFO){
        logmsg.priority = TLL_INFO;
    } else if (priority <= TLL_DEBUG){
        logmsg.priority = TLL_DEBUG;
    } else if (priority <= TLL_NOTSET){
        logmsg.priority = TLL_NOTSET;
    } else return;

    logmsg.msg = lscreate(msg, strlen(msg));

    if (!_root->handle)return;

    (*_root->handler)(msg);

}

void 
tlemerg(const char * msg){
    tllog(TLL_EMERG, msg);
}

void 
tlfatal(const char * msg){
    tllog(TLL_FATAL, msg);
}

void 
tlalert(const char * msg){
    tllog(TLL_ALERT, msg);
}

void
tlerror(const char* msg){
    tllog(TLL_ERROR, msg);
}

void 
tlwarn(const char * msg){
    tllog(TLL_WARN, msg);
}

void 
tlnotice(const char * msg){
    tllog(TLL_NOTICE, msg);
}

void 
tlinfo(const char * msg){
    tllog(TLL_INFO, msg);
}

void 
tldebug(const char * msg){
    tllog(TLL_DEBUG, msg);
}

void 
tlnotset(const char * msg){
    tllog(TLL_NOTSET, msg);
}

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

struct catagory*
createCatagory(struct catagory* parent, int priority, const char* name){
    struct catagory* cg;
    cg = (struct catagory*)malloc(sizeof(struct catagory) + 1);
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

struct catagory*
createNullCatagory(){
    struct catagory* cg;
    cg = (struct catagory*)malloc(sizeof(struct catagory) + 1);
    if (!cg) return NULL;
    cg->parent = NULL;
    cg->priority = TLL_NOTSET;
    cg->name = NULL;
    cg->countAdapters = 0;
    cg->countChildren = 0;
    return cg;
}

void 
freeCatagory(struct catagory* cg){
    if (!cg){
        if(cg->name)lsfree(cg->name);
        for (int i = 0; i < cg->countAdapters; i++){
            //freeAdapter(cg->adapters[i]);
        }
        for (int i = 0; i < cg->countChildren; i++){
            freeCatagory(cg->children[i]);
        }
        free(cg);
    }
}

int
addCatagory(struct catagory* parent, struct catagory* child){
    if (parent&&child){
        parent->children[parent->countChildren] = child;
        parent->countChildren++;
        return 0;
    }
    return -1;
}

int
removeCatagory(struct catagory* parent, struct catagory* child){
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
    if (memmove_s(p, s - p, p + sizeof(struct catagory*), s - p - 1)){
        return -1;
    }
    parent->countChildren--;

    return 0;
}

struct catagory*
findCatagory(struct catagory* parent, const char* name){
    struct catagory* cg = NULL;
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
addAdapter(struct catagory* cg, struct adapter* ada){
    if (!cg || !ada) return -1;
    cg->adapters[cg->countAdapters] = ada;
    cg->countAdapters++;
    return 0;
}

int
removeAdapter(struct catagory* cg, struct adapter* ada){
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
    if (memmove_s(p, s - p, p + sizeof(struct adapter*), s - p - 1)){
        return -1;
    }
    cg->countAdapters--;

    return 0;
}

bool
hasAdapter(struct catagory* cg, struct adapter* ada){
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

