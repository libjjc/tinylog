#ifndef LOG_ADAPTER_HH
#define LOG_ADAPTER_HH

#include <time.h>

#include "logstr.h"
#include "layout.h"


typedef void* adapter_arguments;
typedef int(*adapter_accept)(struct _adapter*,struct _log_msg*);
typedef int(*adapter_free)(struct _adapter*);


struct fileAdapter {
    ls_t path;
    int fd;
    int flag;
    int mode;
	long size;
    long maxsize;
};

struct rollingFileAdapter{
    ls_t path;
    int fd;
    int flag;
    int mode;
    long index;
    long size;
    long extw;
    long rollingcount;
	long rollingsize;
};

struct consoleAdapter {
    int fd;
};

struct remoteAdapter{
    ls_t ip;
    int port;
    int socket;
    int mode;
};

struct winntEventAdapter{
    int nul;
};

struct syslogAdapter {
    int nul;
};

struct _adapter {
    adapter_arguments args;
	layout_callback layout;
    adapter_accept accept;
    adapter_free free;
    ls_t name;
};

struct _adapter*
createFileAdapter(struct _catagory* cata ,const char* logfile,const char* name,int maxsize);

int 
fileAdapterAccept(struct _adapter* ada , struct _log_msg* msg);

int
freeFileAdapter(struct _adapter* ada);

struct _adapter*
createConsoleAdapter(struct _catagory* cata,ls_t name);

int
consoleAdapterAccept(struct _adapter* ada,struct _log_msg* msg);

int
freeConsoleAdapter(struct _adapter* ada);

struct _adapter*
createRollingFileAdapter(struct _catagory* cata, ls_t name,const char* logfile,long rollingsize,long rollingMax);

int
rollingFileAdapterAccept(struct _adapter* apt, struct _log_msg* msg);

int
freeRollingFileAdapter(struct _adapter* apt);
    
//struct _adapter*
//createNetAdapter(struct _catagory* cata,const char* ip , int port);
//
//int
//initNetAdapter(struct _adapter* ada, int argc, char** argv);
//
//int
//netAdapterHandle(struct _adapter* ada,struct _log_msg* msg);
//
//int
//netAdapterClose(struct _adapter* ada);
//
//void
//freeNetAdapter(struct _adapter* ada);
//
//struct _adapter*
//createDBAdapter(struct _catagory* cata,struct _adapter* ada);
//
//int
//initDBAdapter(struct _adapter* ada, int argc, char** argv);
//
//int
//openDBAdapter(struct _adapter* ada);
//
//int
//DBAdapterHandle(struct _adapter* ada,struct _log_msg* msg);
//
//int
//DBAdapterClose(struct _adapter* ada);
//
//void
//freeDBAdapter(struct _adapter* ada);



#endif//LOG_ADAPTER_HH
