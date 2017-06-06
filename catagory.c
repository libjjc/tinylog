
#include <malloc.h>
#include <string.h>
#include "logdef.h"
#include "catagory.h"
#include "logger.h"

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
    cg->countLoggers = 0;
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
    cg->countLoggers = 0;
    cg->countChildren = 0;
    return cg;
}



struct _catagory*
get_create_catagory(const struct _catagory* parent,const char* name){
    struct _catagory* parent_cata = _find_catagory(&gl_logger_root,name);
    struct _catagory* cata = NULL;
    if (!parent_cata){
        parent_cata = &gl_logger_root;
    } else{
        parent_cata = _find_catagory(&gl_logger_root, name);
        if (!parent_cata){
            parent_cata = createCatagory(&gl_logger_root, TLL_NOTSET, name);
        }
    }
    if (!parent_cata) return NULL;
    cata = _find_catagory(&gl_logger_root, name);
    if (!cata){
        cata = createCatagory(parent_cata, TLL_NOTSET, name);
    }
    return cata;
}

void 
freeCatagory(struct _catagory* cg){
    if (!cg){
        if(cg->name)lsfree(cg->name);
        for (int i = 0; i < cg->countLoggers; i++){
            logger_logging logger = cg->loggers[i];
			logger_free free = _get_logger_free(logger);
			if (free) {
				free(logger);
			}
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
_find_catagory(struct _catagory* parent, const char* name){
    struct _catagory* cg = parent ? parent : &gl_logger_root;
    struct _catagory* result = NULL;
    if (cg && cg->name && !lscmp(cg->name, name)){
        return cg;
    }
    for (int i = 0; i < cg->countChildren; i++){
        if (result = _find_catagory(cg->children[i], name)){
            return result;
        }
    }
    return result;
}

int
_add_logger(struct _catagory* cg, logger_logging logger){
    if (!cg || !logger) return -1;
    cg->loggers[cg->countLoggers] = logger;
    cg->countLoggers++;
    return 0;
}

int
_remove_logger(struct _catagory* cg, logger_logging ada){
    if (!cg || !ada) return -1;
    int index = 0;
    for (; index <= cg->countLoggers; index++){
        if (cg->loggers[index] == ada){
            break;
        }
    }
    if (index >= cg->countLoggers) return -1;

    char* s = (char*)&cg->loggers[0];
    char* e = (char*)&cg->loggers[MAX_CATAGORY_loggerS];
    char* p = (char*)&cg->loggers[index];
    if (memmove_s(p, s - p, p + sizeof(struct _logger*), s - p - 1)){
        return -1;
    }
    cg->countLoggers--;

    return 0;
}

int
_replace_logger(struct _catagory* cg, _callback_ptr old, _callback_ptr logger){
    for (int i = 0; i < cg->countLoggers; i++){
        if (old == cg->loggers[i]){
            cg->loggers[i] = logger;
            _set_logger_catagory(logger,cg);
            return 0;
        }
    }
    for (int i = 0; i < cg->countLoggers; i++){
        if (!_replace_logger(cg->children[i], old, logger)){
            return 0;
        }
    }
    return -1;
}

bool
_has_logger(struct _catagory* cg, logger_logging ada){
    if (!cg || !ada) return false;
    int index = 0;
    for (; index <= cg->countLoggers; index++){
        if (cg->loggers[index] == ada){
            break;
        }
    }
    if (index >= cg->countLoggers) return false;
    return true;
}

int
_catagory_logging(struct _catagory* cata, struct _log_msg* msg){
    if (!cata || !msg) return -1;
    int ret = 0;
    for (int i = 0; i < cata->countLoggers; i++){
        logger_logging adloggerer = cata->loggers[i];
        ret &= adloggerer(adloggerer, msg);
    }
    for (int i = 0; i < cata->countChildren;i++){
        ret &= _catagory_logging(cata->children[i], msg);
    }
    return ret;
}

bool
has_logger_recursive(struct _catagory* cq, struct _logger* logger){
    return false;
}

_callback_ptr
find_logger(struct _catagory* cq, const char* loggername){
    logger_logging logger = NULL;
    for (int i = 0; i < cq->countLoggers; i++){
        logger = cq->loggers[i];
        if (!lscmp(_get_logger_name(logger),loggername)) return logger;
    }
    for (int i = 0; i < cq->countChildren; i++){
        logger = find_logger(cq->children[i], loggername);
        if (logger) return logger;
    }
    return logger;
}