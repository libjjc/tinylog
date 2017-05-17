#ifndef LOG_LAYOUT_HH
#define LOG_LAYOUT_HH

#include "logstr.h"

struct adapter;
typedef ls_t(*layout_callback)(void* ,struct logmsg* );

struct patternLayout {
	ls_t pattern;
	layout_callback layout;
};

struct basicLayout {
    layout_callback layout;
};

layout_callback
createPatternLayout(struct adapter* ada, ls_t args);

void
freePatternLayout(layout_callback layout);

ls_t 
patternLayout(layout_callback layout ,struct logmsg* msg);

layout_callback
createBasicLayout(struct adapter* ada);

void
freeBasicLayout(layout_callback layout);

ls_t
basicLayout(layout_callback layout ,struct logmsg* msg);


#endif//LOG_LAYOUT_HH
