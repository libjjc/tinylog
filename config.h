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

#define TINYLOG_TYPE_adapterFILEADAPTER "file"
#define TINYLOG_TYPE_adapterRFILEADAPTER "rollingfile"
#define TINYLOG_TYPE_adapterCONSOLE "console"

#define TINYLOG_TYPE_LAYOUT_BASIC "basiclayout"
#define TINYLOG_TYPE_LAYOUT_PATTERN "patternlayout"

#define TINYLOG_adapterLOGFILE "logfile"
#define TINYLOG_adapterMAXSIZE "maxsize"
#define TINYLOG_adapterRCOUNT "rollingcount"
#define TINYLOG_adapterRSIZE "rollingsize"

#define TINYLOG_LAYOUT_PATTERN "pattern"

int
configure(const char* config);

int
configure_sentence(ls_t sentence);

int
parse_script_tree(struct _log_script_tree* tree);

int
parse_script_root(struct _log_script_tree* tree);

int
parse_script_catagory(struct _log_script_tree* tree);

int
parse_script_adapter(struct _log_script_tree* tree);

int
parse_script_layout(struct _log_script_tree* tree);

int
get_priority(const ls_t prior);

#endif//TINYLOG_CONFIG_HH

