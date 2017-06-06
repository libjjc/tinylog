#ifndef LOG_loggerHH
#define LOG_loggerHH

#include <time.h>
#include <stdbool.h>
#include "logstr.h"
#include "layout.h"
#include "logdef.h"

#define DEFAULT_LOGFILE_MAXSIZE 50
#define DEFAULT_LOGFILE_RSIZE 30
#define DEFAULT_LOGFILE_RCOUNT 20

struct _file_priv {
    ls_t logfile;
    int fd;
    int flag;
    int mode;
    int size;
    int maxsize;
};

struct _rfile_priv {
    struct _file_priv fp;
    int rmax;
    int rindex;
    int extw;
};

struct _dfile_priv {
    struct _file_priv fp;
    int maxdays;
    ls_t fmtday;
};

struct _console_priv {
    int stream;
};

typedef struct _file_priv* _file_priv_t;
typedef struct _rfile_priv* _rfile_priv_t;
typedef struct _dfile_priv* _dfile_priv_t;
typedef struct _console_priv* _console_priv_t;

struct _catagory;
typedef struct _catagory* logger_owner_type;

typedef void* logger_priv_t;
typedef void* _self_t;
typedef int(*logger_logging)(_self_t self,struct _log_msg*);
typedef logger_logging logger_t;
typedef int(*logger_free)(logger_t logger);
typedef int(*logger_priv_free)(logger_priv_t priv);
typedef int(*logger_reopen)(logger_t logger);

struct _logger {
    ls_t name;
    logger_owner_type owner;
    logger_t log;
    layout_t layout;
    logger_reopen reopen;
    logger_free free;
    logger_priv_free privfree;
    logger_priv_t priv;
};

logger_t
_create_logger();

void
_free_logger(logger_t logger);

int
_reopen_logger(logger_t logger);

const ls_t
_get_logger_name(logger_t logger);

logger_free
_get_logger_free(logger_t logger);

layout_t
_get_logger_layout(logger_t logger);

logger_owner_type
_get_logger_owner(logger_t logger);

void
_set_logger_name(logger_t logger,const char* name);

void
_set_logger_owner(logger_t logger,logger_owner_type owner);




void
_set_logger_priv(logger_t logger, logger_priv_t priv);

logger_priv_t
_get_logger_priv(logger_t logger);




_file_priv_t
_create_file_priv(logger_t logger);

void
_free_file_priv(_file_priv_t priv);

void
_set_file_path(_file_priv_t priv,const char* path);

const char*
_get_file_path(_file_priv_t priv);

void
_set_file_maxsize(_file_priv_t priv, int size);

int
_get_file_maxsize(_file_priv_t priv);

int
_file_logger_reopen(_file_priv_t priv);

int
_file_logger_logging(_file_priv_t priv,struct _log_msg* msg);




_rfile_priv_t
_create_rfile_priv(logger_t logger);

void
_free_rfile_priv(_rfile_priv_t priv);

_file_priv_t
_get_rfile_file_priv(_rfile_priv_t priv);

void
_set_rfile_rmax(_rfile_priv_t priv,int max);

int
_get_rfile_rmax(_rfile_priv_t priv);

int
_rfile_logger_reopen(_rfile_priv_t priv);

int
_rfile_logger_logging(_rfile_priv_t priv,struct _log_msg* msg);




_console_priv_t
_create_console_priv(logger_t logger);

void
_free_console_priv(_console_priv_t priv);

void
_set_console_stream(_console_priv_t priv, int stream);

int
_get_console_stream(_console_priv_t priv);

int
_console_logger_reopen(_console_priv_t priv);

int
_console_logger_logging(_console_priv_t priv,struct _log_msg* msg);




logger_t
_create_file_logger(const char* name, logger_owner_type owner,
                    const char* logfile);

logger_t
_create_rfile_logger( const char* name,logger_owner_type owner, 
                      const char* logfile);

logger_t
_create_console_logger(const char* name,logger_owner_type owner,
                       int stream);

#endif//LOG_loggerHH
