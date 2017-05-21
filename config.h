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
			struct _file_adapter_content {
				ls_t logfile;
				int maxsize;
			}_fac;
			struct _rolling_file_adapter_content {
				ls_t logfile;
				int rsize;
				int rmax;
			};
			struct _console_adapter_content {
				int keep;
			};
		};
	};
	union param args ;
	int type;
};
int
configure(const char* config);

int
lineconfigure(ls_t line);

int
_config_root(int argc, char** argv);

int
_config_catagory(const char* name, int argc, char** argv);

int
_config_adapter(const char* name, int argc, char** argv);
#endif//TINYLOG_CONFIG_HH

