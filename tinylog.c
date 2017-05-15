#include "tinylog.h"
#include "logadapter.h"
#include <string.h>

struct adapter gl_logger_root;
void 
tlinit()
{
}

void 
tlshutdown()
{
}

struct adapter * 
root()
{
	return &gl_logger_root;
}

int
tlloadcfg(const char* config) {
	return 0;
}

void 
tllog(int priority, const char * msg)
{
	if (priority > TLL_NOTSET) {
		tlnotset(msg);
	}
	else if (priority > TLL_DEBUG) {
		tldebug(msg);
	}
	else if (priority > TLL_INFO) {
		tlinfo(msg);
	}
	else if (priority > TLL_NOTICE) {
		tlnotice(msg);
	}
	else if (priority > TLL_WARN) {
		tlwarn(msg);
	}
	else if (priority > TLL_ERROR) {
		tlerror(msg);
	}
	else if (priority > TLL_ALERT) {
		tlalert(msg);
	}
	else if (priority > TLL_FATAL) {
		tlfatal(msg);
	}
	else if (priority > TLL_EMERG) {
		tlemerg(msg);
	}
	else {

	}
}

void 
tlemerg(const char * msg){

}

void 
tlfatal(const char * msg){
}

void 
tlalert(const char * msg){
}

void 
tlwarn(const char * msg){
}

void 
tlnotice(const char * msg){
}

void 
tlinfo(const char * msg){
}

void 
tldebug(const char * msg){
}

void 
tlnotset(const char * msg){
}
