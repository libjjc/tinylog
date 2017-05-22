#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include "config.h"
#include "logdef.h"
#include "logstr.h"
#include "catagory.h"
#include "adapter.h"
#include "layout.h"
#include "tinylog.h"

int
configure(const char* config){
    if (!config) return -1;

    int fd = _open(config, O_RDONLY, O_TEXT | O_SEQUENTIAL);

    if (!fd) return -1;

    int flen = _filelength(fd);

    if (flen == (long)-1) return -1;

    ls_t buffer = lscreate(NULL, flen);

    if (!buffer) return -1;

    if (_read(fd, buffer, flen))return -1;

    int ret = 0;

    ls_t sentence = lscreate(NULL, 256);

    const char* p = buffer;
    const char* f = buffer;
    while (*p){
        if (';' == *p){
            sentence = lscatlen(sentence, f, p - f);
            f = p;
            if (configure_sentence(sentence)){
                lsfree(sentence);
                return -1;
            }
        }

        p++;
    }

    lsfree(buffer);

    return ret;
}

int
configure_sentence(ls_t sentence){
    if (!sentence) return -1;
    struct _log_script_tree tree;
    int size = 2;
    ls_t* lstree = (ls_t*)malloc(sizeof(ls_t*)*size);
    lstree = lssplit(sentence, "=", lstree, &size);
    if (2 != size) return -2;
    tree._left.count = tree._right.count = 4;
    tree._left.words = lssplit(lstree[0], "., \r\n", NULL, &tree._left.count);
    tree._right.words = lssplit(lstree[1], "., \r\n", NULL, &tree._right.count);
    parse_script_tree(&tree);
    return 0;
}

int
parse_script_tree( struct _log_script_tree* tree){
    if (!tree)return -1;
    if (tree->_left.count < 2)return -2;
    struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
    if (lscmp(left->words[0], TINYLOG_PREFIX)) return -2;
    if (!lscmp(left->words[1], TINYLOG_TYPE_ROOT)){
        parse_script_catagory(tree);
    }else if(!lscmp(left->words[1], TINYLOG_TYPE_CATAGORY)){
        parse_script_catagory(tree);
    } else if (!lscmp(left->words[1], TINYLOG_TYPE_ADAPTER)){
        if (left->count < 4) return -2;
        if (!lscmp(left->words[3], TINYLOG_TYPE_LAYOUT)){
            parse_script_layout(tree);
        } else{
            parse_script_adapter(tree);
        }
    } else{
        return -3;
    }

    return 0;
}

int
parse_script_root(struct _log_script_tree* tree){
    
    return 0;
}

int
parse_script_catagory(struct _log_script_tree* tree){
    struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
    if (left->count > 2)return -2;
    struct _catagory* cata = NULL;
    for (int i = 2; i < left->count; i++){
        cata = get_catagory_create(cata->name, left->words[i]);
    }
    cata = findCatagory(root(), tree->words[2]);
    if (!cata) return -1;
    cata->priority = get_priority(right->words[0]);
    return 0;
}

int
parse_script_adapter(struct _log_script_tree* tree){
    struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
    if (left->count < 4) return -2;
    struct _adapter* apt = find_adapter(root(), left->words[2]);
    if (!apt) return -1;
    return 0;
}

int
parse_script_layout(struct _log_script_tree* tree){
    return 0;
}

int
get_priority(const ls_t prior){
    return TLL_NOTSET;
}
