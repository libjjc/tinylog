#ifndef TINYLOG_LOGDEF_HH
#define TINYLOG_LOGDEF_HH

#include <time.h>


#if defined(__bool_true_false_are_defined)
#else
//enum _Bool{
//    false = 0,
//    true = 1,
//};
#endif

#define TLL_EMERG		0
#define TLL_FATAL		100
#define TLL_ALERT		200
#define TLL_ERROR		300
#define TLL_WARN		400
#define TLL_NOTICE		500
#define TLL_INFO		600
#define TLL_DEBUG		700
#define TLL_NOTSET		800


#define EN_EMERG		"EMERG"
#define EN_FATAL		"FATAL"
#define EN_ALERT		"ALERT"
#define EN_ERROR		"ERROR"
#define EN_WARN		    "WARN"
#define EN_NOTICE		"NOTICE"
#define EN_INFO		    "INFO"
#define EN_DEBUG		"DEBUG"
#define EN_NOTSET		"NOTSET"


#define LOG_PTN_DT_DEFAULT    "%Y-%m-%d %H:%M:%S"
#define LOG_PTN_SIMPLE
#define LOG_PTN_DEFAULT
#define LOG_PTN_RICH

typedef void* _callback_ptr;

#endif//TINYLOG_LOGDEF_HH
