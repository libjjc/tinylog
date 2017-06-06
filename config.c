#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "logdef.h"
#include "logstr.h"
#include "catagory.h"
#include "logger.h"
#include "layout.h"
#include "tinylog.h"
#include "dict.h"

#define DICT_SET_KV(d,k,v) dtset(d,k,v);

int
process_word(ls_t word, dict_t d);

int
logconfigureline(ls_t line, dict_t d);

int
process_root(dict_t d, struct _log_script_tree* tree);

typedef int(*property_configure)(void* owner, int argc, char** argv);

int
process_catagory(dict_t d, struct _log_script_tree* tree);

int
process_logger(dict_t d, struct _log_script_tree* tree);

int
process_layout(dict_t d, struct _log_script_tree* tree);

int
_property_set_priority(struct _catagory* c, int argc, char** argv);

int
_property_set_cataname(struct _catagory* c, int argc, char** argv);

int
_property_set_filelogger(struct _catagory* c, int argc, char** argv);

int
_property_set_rfilelogger(struct _catagory* c, int argc, char** argv);

int
_property_set_consolelogger(struct _catagory* c, int argc, char** argv);

int
_property_set_layout(logger_logging logger, int argc, char** argv);

int
_property_create_basiclayout(logger_logging logger, int argc, char** argv);

int
_property_create_patternlayout(logger_logging logger, int argc, char** argv);

int
get_or_create_catagory(struct _catagory* c, char* name, dict_t d);

int
add_catagory_child(struct _catagory* c, char* child, dict_t d);


int
get_priority(const ls_t prior){
    return TLL_NOTSET;
}

typedef int(*parse)(void*, const char* param,dict_t d);
struct _configure_node {
    void* node;
    parse handle;
};

int 
logconfigure(const char* file){
    if (!file) return -1;

    int fd = _open(file, O_RDONLY, O_TEXT | O_SEQUENTIAL);

    if (!fd) return -1;

    int flen = _filelength(fd);

    if (flen == (long)-1) return -1;

    ls_t buffer = lscreate(NULL, flen);

    if (!buffer) return -1;

    if (!_read(fd, buffer, flen))return -1;

    int ret = 0;

    ls_t sentence = lscreate(NULL, 256);

    dict_t dict = dtcreate(&oksvstrimpl);
    DICT_SET_KV(dict, TINYLOG_PROPERTY_PRIORITY, _property_set_priority);
    DICT_SET_KV(dict, TINYLOG_PROPERTY_FILE_logger, _property_set_filelogger);
    DICT_SET_KV(dict, TINYLOG_PROPERTY_RFILE_logger, _property_set_rfilelogger);
    DICT_SET_KV(dict, TINYLOG_PROPERTY_CONSOLE_logger, _property_set_consolelogger);

    const char* p = buffer;
    const char* f = buffer;
    while (*p){
        if (';' == *p){
            sentence = lscatlen(sentence, f, p - f);
            sentence = lstrimstr(sentence," \r\n");
            f = p;
            f++;

            if (logconfigureline(sentence,dict)){
                lsfree(sentence);
                return -1;
            }
            lsclear(sentence);
        }

        p++;
    }

    lsfree(buffer);

    return ret;
}

int 
process_word(ls_t word,dict_t d){
    dt_entity_t e = dtfind(d, word);
    if (!e) return -2;
    struct _configure_node* node = e->ptv;
    if (node->handle){
        if (node->handle(node->node, word, d)) return -1;
    }
    return 0;
}

int
logconfigureline(ls_t line,dict_t d){
    if (!line) return -1;
    struct _log_script_tree tree;
    int size = 2;
    ls_t* lstree = (ls_t*)malloc(sizeof(ls_t*)*size);
    lstree = lssplit(line, "=", lstree, &size);
    if (2 != size) return -2;
    tree._left.count = tree._right.count = 4;

    tree._left.words = lssplit(lstree[0], ". \r\n", NULL, &tree._left.count);
    tree._right.words = lssplit(lstree[1], ", \r\n", NULL, &tree._right.count);
    int ret = 0;
    if (0 == strcmp(TINYLOG_TYPE_ROOT, tree._left.words[0])){
        ret = process_root(d,&tree);
    } else if (0 == strcmp(TINYLOG_TYPE_CATAGORY, tree._left.words[0])){
        ret = process_catagory(d, &tree);
    } else if (0 == strcmp(TINYLOG_TYPE_logger, tree._left.words[0])){
        ret = process_logger(d, &tree);
    } else{
        ret = -1;
    }
    for (int i = 0; i < tree._left.count; i++){
        lsfree(tree._left.words[i]);
    }
    for (int i = 0; i < tree._right.count; i++){
        lsfree(tree._right.words[i]);
    }
    free(tree._left.words);
    free(tree._right.words);
    return ret;
}

int
process_root(dict_t d, struct _log_script_tree* tree){
    struct _catagory* c = root();
    if (tree->_left.count == 1){
        _property_set_cataname(c, tree->_right.count, &tree->_right.words[0]);
    } else{
        for (int i = 1; i < tree->_left.count; i++){
            c = get_create_catagory(c, tree->_left.words[i]);
            _property_set_priority(c, tree->_right.count, &tree->_right.words[0]);
        }
    }
    return 0;
}

typedef int(*property_configure)(void* owner, int argc, char** argv);

int
process_catagory(dict_t d, struct _log_script_tree* tree){
    struct _catagory* c = _find_catagory(root(), tree->_left.words[1]);
    
    dt_entity_t e = dtfind(d, tree->_left.words[2]);
    if (!e) return -1;
    if (((property_configure)e->val.ptv)(c, tree->_right.count, &tree->_right.words[0])){
        return -1;
    }
    return 0;
}

int
process_logger(dict_t d, struct _log_script_tree* tree){
    dt_entity_t e = dtfind(d, tree->_left.words[1]);
    if (!e) return -1;
    logger_logging logger = (logger_logging)e->ptv;
    e = dtfind(d, tree->_left.words[2]);
    if (!e)return -1;
    if (((property_configure)e->ptv)(logger, tree->_right.count, &tree->_right.words[0])){
        return -1;
    }
    return 0;
}

int
process_layout(dict_t d, struct _log_script_tree* tree){
    dt_entity_t e = dtfind(d, tree->_left.words[1]);
    if (!e) return -1;
    logger_logging logger = (logger_logging)e->ptv;
    e = dtfind(d, tree->_left.words[2]);
    if (!e)return -1;
    if (((property_configure)e->ptv)(logger,tree->_right.count , &tree->_right.words[0])){
        return -1;
    }
    return 0;
}

int
_property_set_priority(struct _catagory* c, int argc , char** argv){
    c->priority = atoi(argv[0]);
    return 0;
}

int 
_property_set_cataname(struct _catagory* c, int argc , char** argv){
    if (c->name){
        c->name = lscpy(c->name, argv[0]);
    } else{
        c->name = lscreate(argv[0], strlen(argv[0]));
    }
    return c->name ? 0 : -1;
}

int
_property_set_filelogger(struct _catagory* c, int argc, char** argv){
    if (argc < 2) return -2;
    logger_logging logger = 0;
    switch (argc){
    case 2:
        logger = _create_file_logger(argv[0], c, argv[1], DEFAULT_LOGFILE_MAXSIZE);
        break;
    case 3:
        logger = _create_file_logger(argv[0], c, argv[1], atoi(argv[2]));
        break;
    default:
        return -1;
    }
    return 0;
}

int
_property_set_rfilelogger(struct _catagory* c, int argc, char** argv){
    if (argc < 2) return -2;
    logger_logging logger = 0;
    switch (argc){
    case 2:
        logger = _create_rfile_logger(argv[0], c, argv[1], DEFAULT_LOGFILE_RSIZE,DEFAULT_LOGFILE_RCOUNT);
        break;
    case 3:
        logger = _create_rfile_logger(argv[0], c, argv[1], atoi(argv[2]),DEFAULT_LOGFILE_RCOUNT);
        break;
    case 4:
        logger = _create_rfile_logger(argv[0], c, argv[1], atoi(argv[2]), atoi(argv[3]));
    default:
        return -1;
    }
    return 0;
}

int
_property_set_consolelogger(struct _catagory* c, int argc, char** argv){
    if (1 != argc) return -1;
    //logger_logging logger = _create_console_logger(argv[0], c);
    //return logger ? 0 : -1;
    return 0;
}

int
_property_set_layout(struct _logger* logger, int argc, char** argv){
    if (argc < 1)return -1;
    if (0 == strcmp(argv[0], TINYLOG_TYPE_LAYOUT_BASIC)){
        return _property_create_basiclayout(logger, argc, argv);
    } else if (0 == strcmp(argv[0], TINYLOG_TYPE_LAYOUT_PATTERN)){
        return _property_create_patternlayout(logger, argc, argv);
    }
    return -1;
}

int
_property_create_basiclayout(struct _logger* logger, int argc, char** argv){
    if (!create_base_layout(logger)){
        return -1;
    }
    return 0;
}

int
_property_create_patternlayout(struct _logger* logger, int argc, char** argv){
    if (argc != 2) return -2;
    if (!create_pattern_layout(logger,argv[1])){
        return -1;
    }
    return 0;
}

int
get_or_create_catagory(struct _catagory* c, char* name, dict_t d){
    if (!dtfind(d, name)){
        return add_catagory_child(c, name, d);
    }
    return 0;
}

int
add_catagory_child(struct _catagory* c, char* child,dict_t d){
    if (dtfind(d, child)){ return -1; }
    struct _catagory* cata = createCatagory(c, TLL_NOTSET, child);
    if (!cata) return -1;
    dtset(d, child, 0);
    return 0;
}

