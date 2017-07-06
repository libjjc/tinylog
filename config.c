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
#include <unistd.h>
#include <stdarg.h>


#define DICT_SET_KV(d,k,v) dtset(d,k,v);
/*****************************************************************************/
struct _configure_contex {
    ls_t file;
    int line;
    struct _catagory* ctg;
    struct _logger* logger;
    struct _layout* layout;
};
typedef struct _configure_contex* _cfg_contex_t;

typedef int(*_configure_callback)(dict_t, _cfg_contex_t, ls_t, ls_t);

void _free_stack_node(_cfg_stack_node_t node);
int _print_stack_node(_callback_print print, _cfg_stack_node_t node);

_cfg_stack_t
_create_cfg_stack(int stackdepth){
    _cfg_stack_t stack = malloc(sizeof(struct _cfg_stack));
    if (!stack){
        return 0;
    }
    stack->_stack = malloc(sizeof(_cfg_stack_node_t)*stackdepth);
    if (!stack->_stack){
        free(stack);
        return 0;
    }
    stack->_top = -1;
    stack->_size = stackdepth;
    stack->_ndfree = _free_stack_node;
    stack->_ndprint = _print_stack_node;
    return stack;
}

void
_free_cfg_stack(_cfg_stack_t stack){
    int top = stack->_top;
    while (top >= 0){
        stack->_ndfree(stack->_stack[top]);
        top--;
    }
    free(stack->_stack);
    free(stack);
}

_cfg_stack_node_t
_stack_push(_cfg_stack_t stack){
    if (stack->_top + 1 >= stack->_size){
        while (stack->_size < stack->_top + 1){
            stack->_size *= 2;
        }
    }
    _cfg_stack_node_t* stk = realloc(stack->_stack, sizeof(_cfg_stack_node_t)*stack->_size);
    if (!stk){
        int top = stack->_top;
        while (top >= 0){
            stack->_ndfree(stack->_stack[top]);
            top--;
        }
        free(stack->_stack);
        return 0;
    }
    stack->_stack = stk;
    _cfg_stack_node_t node = malloc(sizeof(struct _cfg_stack_node));
    if (!node){
        return 0;
    }
    stack->_top++;
    stack->_stack[stack->_top] = node;
    return stack->_stack[stack->_top];
}

_cfg_stack_node_t
_stack_pop(_cfg_stack_t stack){
    if (stack->_top <= 0){
        return 0;
    }
    stack->_top--;
    return stack->_stack[stack->_top];
}

void
_free_stack_node(_cfg_stack_node_t node){
    if (node){
        if (node->_error){
            lsfree(node->_error);
        }
        if (node->_file){
            lsfree(node->_file);
        }
        free(node);
    }
}

int
_print_stack_node(_callback_print print, _cfg_stack_node_t node){
    return print("%-6d@%s:%d >> %s", node->_eid, node->_file, node->_line, node->_error);
}
#define _SETENCE_ERROR\
        _cfg_stack_node_t node = _stack_push(gl_stack);\
        if (node){\
            _assign_stack_node(node, contex->file, contex->line, -1,\
                               "half-baked config sentence");\
        }\
        return -1;
#define _PARAMETER_ERROR(parameter)\
        if (!contex->ctg){\
            _cfg_stack_node_t node = _stack_push(gl_stack);\
            if (node){\
                _assign_stack_node(node, contex->file, contex->line, -1,\
                                   "unknown parameter \"%s\" in root command", (parameter));\
            }\
        }
#define _COMMAND_ERROR(cmd)\
        if (!contex->ctg){\
            _cfg_stack_node_t node = _stack_push(gl_stack);\
            if (node){\
                _assign_stack_node(node, contex->file, contex->line, -1,\
                                   "An accident occured when %s excuting", (cmd));\
            }\
        }
#define _RUNTIME_ERROR(cmd)\
        if (!contex->ctg){\
            _cfg_stack_node_t node = _stack_push(gl_stack);\
            if (node){\
                _assign_stack_node(node, contex->file, contex->line, -1,\
                                   "%s", (cmd));\
            }\
        }

void
_assign_stack_node(_cfg_stack_node_t node, ls_t file, int line, int eid, const char* error, ...){
    if (node->_file){
        node->_file = lscpy(node->_file, file);
    } else{
        node->_file = lscreate(file, lslen(file));
    }
    va_list ap;
    va_start(ap, error);
    if (node->_error){
        node->_error = lscpyfmt(node->_error, error, ap);
    } else{
        node->_error = lsinitfmt(error, ap);
    }
    va_end(ap);
    node->_eid = eid;
    node->_line = line;
}

struct _cfg_stack* gl_stack = 0;
//root.newcatagory = priority;
//root.defined_catagory.newcatagory = priority;
int _root_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (!left || !right || !lslen(left) || !lslen(right)){
        _SETENCE_ERROR;
        return -1;
    }
    dt_entity_t entity = dtfind(d, left);
    if (entity){
        contex->ctg = contex->ctg ? contex->ctg : _root();
        return ((_configure_callback)(entity->val.ptv))(d, contex, left, right);
    }
    int index = lsfind(left, ".", 0);
    if (-1 == index){
        contex->ctg = _create_catagory(contex->ctg, _get_priority(right), left);
        if (!contex->ctg){
            _PARAMETER_ERROR(left);
        }
        return contex->ctg ? 0 : -1;
    }
    ls_t word = lssubls(left, 0, index);
    struct _catagory* catagory = NULL;
    if (!(catagory = _find_catagory(_root(), word))){
        catagory = _create_catagory(contex->ctg ? contex->ctg : _root(), _get_priority(right), word);
    }
    contex->ctg = catagory;
    word = lscpy(word, left + index + 1);
    if (_root_configure(d, contex, word, right)){
        _COMMAND_ERROR("root")
        lsfree(word);
        return -1;
    }

    lsfree(word);
    return 0;
}

//catagory.defined_catagory.property = value;
int
_catagory_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (!left || !right || !lslen(left) || !lslen(right)){
        _SETENCE_ERROR;
        return -1;
    }
    int index = lsfind(left, ".", 0);
    if (-1 == index){
        dt_entity_t entity = dtfind(d, left);
        if (!entity) {
            _PARAMETER_ERROR(left);
            return -1;
        }
        return ((_configure_callback)(entity->val.ptv))(d, contex, left, right);
    }
    ls_t word = lscreate(left, index);
    struct _catagory* c = _find_catagory(contex->ctg, word);
    if (c){
        contex->ctg = c;
        word = lscpy(word, left + index + 1);
        if (_catagory_configure(d, contex, word, right)){
            _COMMAND_ERROR("catagory");
            lsfree(word);
            return -1;
        }
        lsfree(word);
        return 0;
    }
    lsfree(word);
    _COMMAND_ERROR("catagory");
    return -1;
}

//logger.defined_logger.property = value;
int
_logger_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (!left || !right || !lslen(left) || !lslen(right)){
        _SETENCE_ERROR;
        return -1;
    }
    if (contex->logger){
        dt_entity_t entity = dtfind(d, left);
        if (!entity) {
            _PARAMETER_ERROR(left);
            return -1;
        }
        return ((_configure_callback)(entity->val.ptv))(d, contex, left, right);
    }
    int index = lsfind(left, ".", 0);
    ls_t word = lscreate(left, index);
    if (-1 != index){
        _logger_t logger = _find_logger(_root(), word);
        if (logger){
            contex->logger = logger;
            word = lscpy(word, left + index + 1);
            if (_logger_configure(d, contex, word, right)){
                lsfree(word);
                _COMMAND_ERROR("logger");
                return -1;
            }
        }
    }
    lsfree(word);
    return 0;
}

//layout.defined_logger.property = value;
int
_layout_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (!left || !right || !lslen(left) || !lslen(right)){
        _SETENCE_ERROR;
        return -1;
    }
    if (contex->logger){
        dt_entity_t entity = dtfind(d, left);
        if (!entity) {
            _PARAMETER_ERROR(left);
            return -1;
        }
        return ((_configure_callback)(entity->val.ptv))(d, contex, left, right);
    }
    int index = lsfind(left, ".", 0);
    ls_t word = lscreate(left, index);
    if (-1 != index){
        _logger_t logger = _find_logger(_root(), word);
        if (logger&&logger->layout){
            contex->logger = logger;
            contex->layout = logger->layout;
            word = lscpy(word, left + index + 1);
            if (_layout_configure(d, contex, word, right)){
                lsfree(word);
                _COMMAND_ERROR("layout");
                return -1;
            }
        }
    }
    lsfree(word);
    return 0;
}

/**
 *  root.priority = DEBUG;
 *  catagory.A1.priority = DEBUG;
 */
int
_ctg_priority_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (contex->ctg){
        contex->ctg->priority = _get_priority(right);
        return 0;
    }
    _SETENCE_ERROR;
    return -1;
}

/**
 * catagory.A1.logger = name,type;
 * root.logger = name,type;
 */
int
_ctg_logger_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    if (contex->ctg){
        int index = lsfind(right, ",", 0);
        if (-1 != index){
            ls_t logger = lssubls(right, 0, index);
            logger = lstrim(logger);
            ls_t type = lssubls(right, index + 1, -1);
            type = lstrim(type);
            dt_entity_t entity = dtfind(d, type);
            if (entity){
                ret = ((_configure_callback)(entity->val.ptv))(d, contex, logger, type);
            }
            lsfree(logger);
            lsfree(type);
        }
    }
    if (-1 == ret){
        _SETENCE_ERROR;
    }
    return ret;
}

/**
 * catagory.A1.logger = name,filelogger;
 * root.logger = name,filelogger;
 */
int
_file_logger_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (contex->ctg){
        _logger_t logger = _create_file_logger(left, contex->ctg, "");
        if (!logger){
            _COMMAND_ERROR("logger");
        }
        return logger ? 0 : -1;
    }
    _SETENCE_ERROR;
    return -1;
}

/**
 * catagory.A1.logger = name,rollingfilelogger;
 * root.logger = name,rollingfilelogger;
 */
int
_rfile_logger_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (contex->ctg){
        _logger_t logger = _create_rfile_logger(left, contex->ctg, "");
        if (!logger){
            _COMMAND_ERROR("logger");
        }
        return logger ? 0 : -1;
    }
    _SETENCE_ERROR;
    return -1;
}

/**
 * catagory.A1.logger = name,consolelogger;
 * root.logger = name,consolelogger;
 */
int
_console_logger_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    if (contex->ctg){
        _logger_t logger = _create_console_logger(left, contex->ctg, 0);
        if (!logger){
            _COMMAND_ERROR("logger");
        }
        return logger ? 0 : -1;
    }
    _SETENCE_ERROR;
    return -1;
}

/**
 * logger.A1.layout = basiclayout;
 * logger.A2.layout = patternlayout;
 * logger.A3.layout = patternlayout,"pattern"
 */
int
_logger_layout_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    if (!contex->logger){
        _RUNTIME_ERROR("can not found logger");
        return ret;
    }
    int index = lsfind(right, ",", 0);
    ls_t str1, str2;
    if (-1 == index){
        str1 = lscreate(right, lslen(right));
        str2 = 0;
    } else{
        str1 = lscreate(right, index);
        str2 = lscreate(right + index + 1, lslen(right) - index - 1);
    }
    dt_entity_t e = dtfind(d, str1);
    if (e){
        ret = ((_configure_callback)e->val.ptv)(d, contex, str1, str2);
    }
    lsfree(str1);
    lsfree(str2);
    return ret;
}

int
_logger_layout_create_basic(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    if (right && lslen(right)){
        _SETENCE_ERROR;
        return ret;
    }
    if (contex->logger){
        if (_create_base_layout(contex->logger)){
            ret = 0;
        }
    }
    if (-1 == ret){
        _SETENCE_ERROR;
    }
    return ret;
}

int
_logger_layout_create_pattern(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;

    if (contex->logger){
        if (right && lslen(right)){
            if (_create_pattern_layout(contex->logger, right)){
                ret = 0;
            }
        } else{
            if (_create_pattern_layout(contex->logger, "default pattern")){
                ret = 0;
            }
        }
    }
    if (-1 == ret){
        _SETENCE_ERROR;
    }
    return ret;
}

/**
 * layout.A1.type = filelogger;
 * layout.A2.type = rollingfilelogger;
 * layout.A3.type = consolelogger;
 */
int
_logger_impl_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    dt_entity_t e = dtfind(d, right);
    if (e&&contex->logger){
        void*(*callback)(_logger_t) = e->val.ptv;
        logger_priv_t priv = callback(contex->logger);
        if (0 == priv){
            _SETENCE_ERROR;
        }
        ret = priv ? 0 : -1;
    }
    return ret;
}


/**
 * logger.A1.logfile = "path";
 */
int
_logger_logfile_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    //dt_entity_t e = dtfind(d, right);
    if (*right == '"' && *(right + lslen(right) - 1) == '"'){
        ls_t file = lscreate(right + 1, lslen(right) - 2);
        if (contex->logger&&contex->logger->priv){
            _file_priv_t fpt = 0;
            switch (contex->logger->type){
            case FILELOGGER:
                fpt = contex->logger->priv;
                break;
            case ROLLINGFILELOGGER:
                fpt = &((_rfile_priv_t)contex->logger->priv)->fp;
                break;
            default:
                break;
            }
            if (fpt){
                _set_file_path(fpt, file);
                ret = 0;
            }
        }
        lsfree(file);
    }
    if (-1 == ret){
        _SETENCE_ERROR;
    }
    return ret;
}

/**
 * logger.A1.maxsize = "path";
 */
int
_logger_logfile_maxsize_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    if (contex->logger&&contex->logger->priv){
        _file_priv_t fpt = 0;
        switch (contex->logger->type){
        case FILELOGGER:
            fpt = contex->logger->priv;
            break;
        case ROLLINGFILELOGGER:
            fpt = &((_rfile_priv_t)contex->logger->priv)->fp;
            break;
        default:
            break;
        }
        if (fpt){
            int size = atoi(right);
            _set_file_maxsize(fpt, size);
            ret = 0;
        }
    }
    return ret;
}

/**
 * logger.A1.maxrolling = N;
 */
int
_logger_rlogfile_max_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    if (contex->logger&&contex->logger->priv){
        _rfile_priv_t fpt = 0;
        switch (contex->logger->type){
        case ROLLINGFILELOGGER:
            fpt = contex->logger->priv;
            break;
        default:
            break;
        }
        if (fpt){
            int size = atoi(right);
            _set_rfile_rmax(fpt, size);
            ret = 0;
        }
    }
    return ret;
}

//logger.A1.stream = stdout;
int
_logger_console_stream_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    dt_entity_t e = dtfind(d, right);
    _console_priv_t priv = 0;
    if (e&&contex->logger&&contex->logger->priv){
        if (CONSOLELOGGER == contex->logger->type){
            priv = contex->logger->priv;
            ret = 0;
        }
        priv->stream = e->val.ptv;
    }
    return ret;
}
/**
 * layout.A1.pattern = "pattern"
 */
int
_layout_pattern_configure(dict_t d, _cfg_contex_t contex, ls_t left, ls_t right){
    int ret = -1;
    right = lstrim(right);
    if (*right == '"' && *(right + lslen(right) - 1) == '"'){
        ls_t pattern = lscreate(right + 1, lslen(right) - 2);
        if (contex->layout){
            _set_layout_pattern(contex->layout->priv, pattern);
            ret = 0;
        }
        lsfree(pattern);
    }
    return ret;
}


int
_get_priority(const ls_t prior){
    return TLL_NOTSET;
}

int
_line_configure(dict_t d, ls_t line, int indexline){
    int ret = -1;
    int count = 2;
    ls_t* parts = malloc(sizeof(ls_t*) * count);
    parts[0] = lsinit(0, 16);
    parts[1] = lsinit(0, 16);
    struct _configure_contex contex;
    contex.ctg = 0;
    contex.layout = 0;
    contex.logger = 0;
    contex.line = indexline;
    parts = lssplit(line, "=", parts, &count);
    if (count == 2){
        int index = lsfind(parts[0], ".", 0);
        if (-1 != index){
            ls_t typeword = lscreate(parts[0], index);
            typeword = lstrim(typeword);
            ls_t left = lscreate(parts[0] + index + 1, lslen(parts[0]) - index - 1);
            left = lstrim(left);
            ls_t right = lsinitcpyls(parts[1]);
            right = lstrim(right);
            dt_entity_t e = dtfind(d, typeword);
            if (e){
                ret = ((_configure_callback)e->val.ptv)(d, &contex, left, right);
            }
            lsfree(right);
            lsfree(left);
            lsfree(typeword);
        }
    }
    for (int i = 0; i < count; i++){
        lsfree(parts[i]);
    }
    free(parts);
    return ret;
}


int
_tinylog_configure(const char* file, _callback_print eprint){
    if (!file) return -1;

    int fd = open(file, O_RDONLY, O_TEXT );

    if (!fd) return -1;

    struct stat fdstat;

    fstat(fd,&fdstat);

    int flen = fdstat.st_size;//filelength(fd);

    if (flen == (long)-1) return -1;

    ls_t buffer = lscreate(NULL, flen);

    if (!buffer) return -1;

    if (!read(fd, buffer, flen))return -1;

    int ret = 0;

    ls_t sentence = lscreate(NULL, 256);

    gl_stack = _create_cfg_stack(8);

    dict_t dict = dtcreate(&oksvstrimpl);

    DICT_SET_KV(dict, TL_TYPE_ROOT, _root_configure);
    DICT_SET_KV(dict, TL_TYPE_CATAGORY, _catagory_configure);
    DICT_SET_KV(dict, TL_TYPE_LOGGER, _logger_configure);
    DICT_SET_KV(dict, TL_TYPE_LAYOUT, _layout_configure);
    DICT_SET_KV(dict, TL_ACT_LOGGING, _ctg_logger_configure);
    DICT_SET_KV(dict, TL_ACT_LAYOUTING, _logger_layout_configure);
    DICT_SET_KV(dict, TL_PROPERTY_FILELOGGER, _file_logger_configure);
    DICT_SET_KV(dict, TL_PROPERTY_ROLLINGFILELOGGER, _rfile_logger_configure);
    DICT_SET_KV(dict, TL_PROPERTY_CONSOLELOGGER, _console_logger_configure);
    DICT_SET_KV(dict, TL_PROPERTY_BASICLAYOUT, _logger_layout_create_basic);
    DICT_SET_KV(dict, TL_PROPERTY_PATTERNLAYOUT, _logger_layout_create_pattern);
    DICT_SET_KV(dict, TL_PROPERTY_PRIORITY, _ctg_priority_configure);
    DICT_SET_KV(dict, TL_PROPERTY_LOGFILE, _logger_logfile_configure);
    DICT_SET_KV(dict, TL_PROPERTY_MAXSIZE, _logger_logfile_maxsize_configure);
    DICT_SET_KV(dict, TL_PROPERTY_MAXROLLING, _logger_rlogfile_max_configure);
    DICT_SET_KV(dict, TL_PROPERTY_STREAM, _logger_console_stream_configure);
    DICT_SET_KV(dict, TL_PROPERTY_SOUT, stdout);
    DICT_SET_KV(dict, TL_PROPERTY_SIN, stdin);
    DICT_SET_KV(dict, TL_PROPERTY_SERR, stderr);
    DICT_SET_KV(dict, TL_PROPERTY_PATTERN, _layout_pattern_configure);
    DICT_SET_KV(dict, TL_PROPERTY_LOGGERIMPL, _logger_impl_configure);

    int indexline = 0;
    const char* p = buffer;
    const char* f = buffer;
    while (*p){
        if (';' == *p){
            sentence = lscatlen(sentence, f, p - f);
            sentence = lstrimstr(sentence, " \r\n");
            f = p;
            f++;

            if (_line_configure(dict, sentence, indexline)){
                lsfree(sentence);
                _free_cfg_stack(gl_stack);
                return -1;
            }
            lsclear(sentence);
            indexline++;
        }

        p++;
    }

    dtfree(dict);

    lsfree(buffer);

    _free_cfg_stack(gl_stack);
    return ret;
}
