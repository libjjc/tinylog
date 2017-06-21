#ifndef TINYLOG_CONFIG_HH
#define TINYLOG_CONFIG_HH
#include "logstr.h"

struct _cfg_stack_node {
    ls_t _error;
    int _eid;
    ls_t _file;
    int _line;
};

typedef struct _cfg_stack_node* _cfg_stack_node_t;

typedef int (*_callback_print)(const char*, ...);
typedef int (*_stack_node_print)(_callback_print,_cfg_stack_node_t);
typedef void(*_stack_node_free)(_cfg_stack_node_t);

struct _cfg_stack {
    _cfg_stack_node_t* _stack;
    _stack_node_print _ndprint;
    _stack_node_free _ndfree;
    int _size;
    int _top;
};
typedef struct _cfg_stack* _cfg_stack_t;


#define TINYLOG_PREFIX "tinylog"

#define TL_TYPE_ROOT "root"
#define TL_TYPE_CATAGORY "catagory"
#define TL_TYPE_LOGGER "logger"
#define TL_TYPE_LAYOUT "layout"

#define TL_ACT_LAYOUTING "layouting"
#define TL_ACT_LOGGING "logging"

#define TL_PROPERTY_FILELOGGER "file"
#define TL_PROPERTY_ROLLINGFILELOGGER "rollingFile"
#define TL_PROPERTY_CONSOLELOGGER "console"
#define TL_PROPERTY_BASICLAYOUT "basicLayout"
#define TL_PROPERTY_PATTERNLAYOUT "patternLayout"
#define TL_PROPERTY_PRIORITY "priority"
#define TL_PROPERTY_LOGFILE "logfile"
#define TL_PROPERTY_MAXSIZE "maxsize"
#define TL_PROPERTY_MAXROLLING "maxrolling"
#define TL_PROPERTY_STREAM "stream"
#define TL_PROPERTY_SOUT "stdout"
#define TL_PROPERTY_SIN "stdin"
#define TL_PROPERTY_SERR "stderr"
#define TL_PROPERTY_PATTERN "pattern"
#define TL_PROPERTY_LOGGERIMPL "logimpl"

int
_get_priority(const ls_t prior);

int
_tinylog_configure(const char* configfile,_callback_print eprint);



#endif//TINYLOG_CONFIG_HH

