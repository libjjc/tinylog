#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
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
        parse_script_root(tree);
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
	struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
	if (left->count > 2) {
		return -2;
	}
	if (right->count >= 1) {
		root()->priority = get_priority(right->words[0]);
	}
	if (right->count >= 2) {
		for (int i = 0; i < right->count; i++) {
			adapter_accept apt = _create_null_adapter();
            _set_apt_name(apt, right->words[i]);
            addAdapter(root(), apt);
		}
	}
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
        cata = get_create_catagory(cata->name, left->words[i]);
    }
    cata = findCatagory(root(), tree->words[2]);
    if (!cata) return -1;
    if (right->count >= 1){
        cata->priority = get_priority(right->words[0]);
    }
    if (right->count >= 2){
        for (int i = 1; i < right->count; i++){
            adapter_accept apt = _create_null_adapter();
            _set_apt_name(apt, right->words[i]);
            addAdapter(root(), apt);
        }
    }
    return 0;
}

int
parse_script_adapter(struct _log_script_tree* tree){
    struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
    if (left->count <= 2) return -2;
    adapter_accept apt = NULL;
    if (left->count == 3){
        apt = find_adapter(root(), left->words[2]);
        if (!apt) return -2;
        struct _catagory* cata = _get_apt_catagory(apt);
        if (!cata) return -1;
        adapter_accept _apt_imp = NULL;
        if (right->count >= 1){
            if (!lscmp(right->words[0], TINYLOG_TYPE_APT_FILE)){
                if (right->count < 2) return -2;
                _apt_imp = _create_null_file_apt(right->words[1], _get_apt_name(apt));
                _replace_adapter(cata, apt, _apt_imp);
            } else if (!lscmp(right->words[0], TINYLOG_TYPE_APT_RFILE)){
                if (right->count < 2) return -2;
                _apt_imp = _create_null_rfile_apt(right->words[1], _get_apt_name(apt));
                _replace_adapter(cata, apt, _apt_imp);
            } else if (!lscmp(right->words[0], TINYLOG_TYPE_APT_CONSOLE)){
                if (right->count < 2) return -2;
                int stream = 0;
                if (lscmp(right->words[1], TINYLOG_CSL_SOUT)){
                    stream = (int)stdout;
                } else if (lscmp(right->words[1], TINYLOG_CSL_SIN)){
                    stream = (int)stdin;
                } else if (lscmp(right->words[1], TINYLOG_CSL_SERR)){
                    stream = (int)stderr;
                } else return -2;
                _apt_imp = _create_null_console_apt( _get_apt_name(apt),stream);
                _replace_adapter(cata, apt, _apt_imp);
            } else return -2;
            _free_adapter(apt);
            apt = _apt_imp;
        }
    }
    if (left->count >= 4){
        if (!lscmp(left->words[3], TINYLOG_TYPE_LAYOUT)){
            return parse_script_layout(tree);
        } else if (lscmp(left->words[3], TINYLOG_APT_LOGFILE)){

        } else if (lscmp(left->words[3], TINYLOG_APT_MAXSIZE)){
            int value = atoi(right->words[0]);
            if (value <= 0) return -2;
            _set_apt_maxsize(apt, value);
        } else if (lscmp(left->words[3], TINYLOG_APT_RCOUNT)){
            int value = atoi(right->words[0]);
            if (value <= 0) return -2;
            _set_apt_rcount(apt, value);
        } else if (lscmp(left->words[3], TINYLOG_APT_RSIZE)){
            int value = atoi(right->words[0]);
            if (value <= 0) return -2;
            _set_apt_maxsize(apt, value);
        } else return -2;
    }
    return 0;
}


int
parse_script_layout(struct _log_script_tree* tree){
    struct _log_script_leaf* left, *right;
    left = &tree->_left;
    right = &tree->_right;
    adapter_accept apt = find_adapter(root(), left->words[3]);
    if (!apt)return -2;
    if (left->count == 4){
        if (!lscmp(right->words[0], TINYLOG_TYPE_LAYOUT_BASIC)){
            _set_apt_layout(apt, create_base_layout(apt));
        } else if (!lscmp(right->words[0], TINYLOG_TYPE_LAYOUT_PATTERN)){
            _set_apt_layout(apt, create_pattern_layout(apt,""));
        } else return -2;
    } else if (left->count > 4){
        if (!lscmp(left->words[1], TINYLOG_LAYOUT_PATTERN)){
            set_layout_pattern(_get_apt_layout(apt), right->words[0]);
        } else return-2;
    } else return -2;
    return 0;
}

int
get_priority(const ls_t prior){
    return TLL_NOTSET;
}
