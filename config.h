#ifndef TINYLOG_CONFIG_HH
#define TINYLOG_CONFIG_HH
#include "logstr.h"



struct _log_script_tree {
    struct _log_script_leaf {
        ls_t* words;
        int count;
    };
    struct _log_script_leaf _left;
    struct _log_script_leaf _right;
};

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

