#ifndef LOG_adapterHH
#define LOG_adapterHH

#include <time.h>
#include <stdbool.h>
#include "logstr.h"
#include "layout.h"
#include "logdef.h"

typedef void* adapter_arguments;
typedef int(*adapter_accept)(_callback_ptr,struct _log_msg*);
typedef int(*adapter_free)(_callback_ptr);
typedef adapter_accept(*set_int_callback)(int*, int);
typedef adapter_accept(*set_flt_callback)(float*, float);
typedef adapter_accept(*set_dbl_callback)(double*, double);
typedef adapter_accept(*set_bool_callback)(bool*, bool);
typedef adapter_accept(*set_lst_callback)(ls_t, const ls_t);
typedef adapter_accept(*set_ptr_callback)(void**, void*);


struct _adapter {
    ls_t name;
    adapter_accept accept;
    adapter_free free;
    layout_callback layout;
    union args {
        struct _file_args {
            ls_t logfile;
            int fd;
            int flag;
            int mode;
            long fsize;
            long maxsize;
            union file_ext{
                struct _rol_file_args {
                    int rcount;
                    int index;
                    int extw;
                };
                struct _daily_file_args {
                    int maxday;
                    ls_t dayfmt;
                };
                struct _rol_file_args _rfa;
                struct _daily_file_args _dfa;
            };
            union file_ext _file_ext;
        };
        struct _console_args {
            int _stream;
        };
        struct _file_args _file;
        struct _console_args _csl;
    };
    union args _args;

};

adapter_accept
_create_null_adapter();

const ls_t
_get_apt_name(adapter_accept apt);

adapter_free
_get_apt_free(adapter_accept apt);

layout_callback
_get_apt_layout(adapter_accept apt);

int
_set_apt_name(adapter_accept apt,const char* name);

int
_set_apt_file(adapter_accept apt, const char* logfile);

int
_set_apt_fsize(adapter_accept apt, long maxsize);

int
_set_apt_rcount(adapter_accept apt, long rcount);

int
_set_apt_dcount(adapter_accept apt, int days);

int
_set_apt_dayfmt(adapter_accept apt, const char* fmt);

int
_set_apt_stream(adapter_accept apt, int stream);

void
_set_apt_layout(adapter_accept apt, layout_callback layout);

adapter_accept
_create_file_apt(struct _catagory* cata, const char* name, const char* logfile, int maxsize);

int
_free_file_apt(adapter_accept apt);

int
_file_apt_accept(adapter_accept apt, struct _log_msg* msg);

adapter_accept
_create_rfile_apt(struct _catagory* cata, const char* name, const char* logfile, long rsize, long rcount);

int
_free_rfile_apt(adapter_accept apt);

int
_rfile_apt_accept( adapter_accept apt,struct _log_msg* msg);

adapter_accept
_create_console_apt(struct _catagory* cata, const char* name);

int
_console_apt_accept(adapter_accept apt,struct _log_msg* msg);

int
_free_console_apt(adapter_accept apt);


#endif//LOG_adapterHH
