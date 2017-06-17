#ifndef TINYLOG_CONFIG_HH
#define TINYLOG_CONFIG_HH
#include "logstr.h"

struct _cfgerr_stack_node {
    ls_t _error;
    int _id;
    ls_t _file;
    int line;
};

typedef struct _cfgerr_stack_node* _cfgerr_stack_node_t;

struct _cfgerr_stack {
    _cfgerr_stack_node_t _stack;
    int size;
    int depth;
    int index;
};
typedef struct _cfgerr_stack* _cfgerr_stack_t;

_cfgerr_stack_t
_create_cfgerr_stack(int stackdepth);

void
_free_cfgerr_stack();


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
_tinylog_configure(const char* configfile);



#endif//TINYLOG_CONFIG_HH

