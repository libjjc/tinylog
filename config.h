#ifndef TINYLOG_CONFIG_HH
#define TINYLOG_CONFIG_HH
#include "logstr.h"

struct _log_config {
	union param {
		struct _cfg_root {
			ls_t name;
			int priority;
		}_root;
		struct cfg_catagory {
			ls_t name;
			int priority;
			ls_t parent;
			ls_t adapter;
		}_catagory;
		union cfg_adapter {
			struct _file_adaptercontent {
				ls_t logfile;
				int maxsize;
			}_fac;
			struct _rolling_file_adaptercontent {
				ls_t logfile;
				int rsize;
				int rmax;
			}_rfac;
			struct _console_adaptercontent {
				int keep;
			}_cac;
		}_adapter;
        int _apt_type;
	};
	union param args ;
	int type;
};

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
#define TINYLOG_TYPE_ADAPTER "adapter"
#define TINYLOG_TYPE_LAYOUT "layout"

#define TINYLOG_TYPE_APT_FILE "file"
#define TINYLOG_TYPE_APT_RFILE "rollingfile"
#define TINYLOG_TYPE_APT_CONSOLE "console"

#define TINYLOG_TYPE_LAYOUT_BASIC "basiclayout"
#define TINYLOG_TYPE_LAYOUT_PATTERN "patternlayout"

#define TINYLOG_PROPERTY_PRIORITY "priority"

#define TINYLOG_PROPERTY_ADAPTER "adapter"
#define TINYLOG_PROPERTY_FILE_ADAPTER "fileadapter"
#define TINYLOG_PROPERTY_RFILE_ADAPTER "rfileadapter"
#define TINYLOG_PROPERTY_CONSOLE_ADAPTER "consoleadapter"




#define TINYLOG_APT_LAYOUT "layout"
#define TINYLOG_APT_LOGFILE "logfile"
#define TINYLOG_APT_MAXSIZE "maxsize"
#define TINYLOG_APT_RCOUNT "rollingcount"
#define TINYLOG_APT_RSIZE "rollingsize"

#define TINYLOG_CSL_SOUT "stdout"
#define TINYLOG_CSL_SIN "stdin"
#define TINYLOG_CSL_SERR "stderr"

#define TINYLOG_LAYOUT_PATTERN "pattern"

int
get_priority(const ls_t prior);


int
logconfigure(const char* configfile);



#endif//TINYLOG_CONFIG_HH

