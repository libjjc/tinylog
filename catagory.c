
#include <malloc.h>
#include <string.h>
#include "logdef.h"
#include "catagory.h"
#include "logger.h"
#include "logmsg.h"

extern struct _catagory gl_logger_root;

bool
_bigger_filter(int priorityMsg, int priorityAda){
    return priorityMsg > priorityAda;
}

bool
_smaller_filter(int priorityMsg, int priorityAda){
    return priorityMsg < priorityAda;
}

bool
_equal_filter(int priorityMsg, int priorityAda){
    return priorityMsg == priorityAda;
}

struct _catagory*
_create_catagory(struct _catagory* parent, int priority, const char* name){
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
_create_null_catagory(const char* name){
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
_get_create_catagory(const struct _catagory* parent,const char* name){
    struct _catagory* parent_cata = _find_catagory(&gl_logger_root,name);
    struct _catagory* cata = NULL;
    if (!parent_cata){
        parent_cata = &gl_logger_root;
    } else{
        parent_cata = _find_catagory(&gl_logger_root, name);
        if (!parent_cata){
            parent_cata = _create_catagory(&gl_logger_root, TLL_NOTSET, name);
        }
    }
    if (!parent_cata) return NULL;
    cata = _find_catagory(&gl_logger_root, name);
    if (!cata){
        cata = _create_catagory(parent_cata, TLL_NOTSET, name);
    }
    return cata;
}

void 
_free_catagory(struct _catagory* cg){
    if (!cg){
        if(cg->name)lsfree(cg->name);
        for (int i = 0; i < cg->countLoggers; i++){
            _logger_t logger = cg->loggers[i];
            _free_logger(logger);
        }
        for (int i = 0; i < cg->countChildren; i++){
            _free_catagory(cg->children[i]);
        }
        free(cg);
    }
}

int
_add_catagory(struct _catagory* parent, struct _catagory* child){
    if (parent&&child){
        parent->children[parent->countChildren] = child;
        parent->countChildren++;
        return 0;
    }
    return -1;
}

int
_remove_catagory(struct _catagory* parent, struct _catagory* child){
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
_add_logger(struct _catagory* cg, _logger_t logger){
    if (!cg || !logger) return -1;
    if (_has_logger(cg, logger)){
        return -1;
    }
    cg->loggers[cg->countLoggers] = logger;
    cg->countLoggers++;
    return 0;
}

int
_remove_logger(struct _catagory* cg, _logger_t ada){
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
            _set_logger_owner(logger,cg);
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
_has_logger(struct _catagory* cg, _logger_t ada){
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
    msg->catagory = lscreate(cata->name, lslen(cata->name));
    int ret = 0;
    for (int i = 0; i < cata->countLoggers; i++){
        _logger_t logger = cata->loggers[i];
        ret &= logger->logging(logger, msg);
        //ret &= logger(adloggerer, msg);
    }
    for (int i = 0; i < cata->countChildren;i++){
        ret &= _catagory_logging(cata->children[i], msg);
    }
    return ret;
}

bool
_has_logger_recursive(struct _catagory* cq, struct _logger* logger){
    return false;
}

_logger_t
_find_logger(struct _catagory* cq, const char* loggername){
    for (int i = 0; i < cq->countLoggers; i++){
        if (!lscmp(_get_logger_name(cq->loggers[i]), loggername)){
            return cq->loggers[i];
        }
    }
    for (int i = 0; i < cq->countChildren; i++){
        _logger_t logger = _find_logger(cq->children[i], loggername);
        if (logger) return logger;
    }
    return NULL;
}