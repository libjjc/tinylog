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

#define TINYLOG_TYPE_ROOT "root"
#define TINYLOG_TYPE_CATAGORY "catagory"
#define TINYLOG_TYPE_logger "adloggerer"
#define TINYLOG_TYPE_LAYOUT "layout"

#define TINYLOG_TYPE_logger_FILE "file"
#define TINYLOG_TYPE_logger_RFILE "rollingfile"
#define TINYLOG_TYPE_logger_CONSOLE "console"

#define TINYLOG_TYPE_LAYOUT_BASIC "basiclayout"
#define TINYLOG_TYPE_LAYOUT_PATTERN "patternlayout"

#define TINYLOG_PROPERTY_PRIORITY "priority"

#define TINYLOG_PROPERTY_logger "adloggerer"
#define TINYLOG_PROPERTY_FILE_logger "fileadloggerer"
#define TINYLOG_PROPERTY_RFILE_logger "rfileadloggerer"
#define TINYLOG_PROPERTY_CONSOLE_logger "consoleadloggerer"




#define TINYLOG_logger_LAYOUT "layout"
#define TINYLOG_logger_LOGFILE "logfile"
#define TINYLOG_logger_MAXSIZE "maxsize"
#define TINYLOG_logger_RCOUNT "rollingcount"
#define TINYLOG_logger_RSIZE "rollingsize"

#define TINYLOG_CSL_SOUT "stdout"
#define TINYLOG_CSL_SIN "stdin"
#define TINYLOG_CSL_SERR "stderr"

#define TINYLOG_LAYOUT_PATTERN "pattern"

int
get_priority(const ls_t prior);


int
logconfigure(const char* configfile);



#endif//TINYLOG_CONFIG_HH

