#ifndef LOG_ADAPTER_HH
#define LOG_ADAPTER_HH

#include "logstr.h"
#include "loglayout.h"
#include <time.h>

struct logmsg {
    int priority;
    time_t timestamp;
    ls_t prioname;
    ls_t catagory;
    ls_t msg;
};

typedef void* adapter_arguments;
typedef int(*adapter_init)(struct adapter*,int, char**);
typedef int(*adapter_open)(struct adapter*);
typedef int(*adapter_handler)(struct adapter*,struct logmsg*);
typedef int(*adapter_close)(struct adapter*);
typedef void(*adapter_free)(struct adapter*);


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
    int mode;
    long index;
    long size;
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

struct adapter {
    adapter_arguments args;
	layout_callback layout;
    adapter_init init;
    adapter_open open;
    adapter_handler handler;
    adapter_close close;
    adapter_free free;
    ls_t name;
};

struct adapter*
createFileAdapter(struct catagory* cata ,const char* logfile,const char* name,int maxsize);

int 
initFileAdapter(struct adapter* ada, int argc, char** argv);

int
openFileAdapter(struct adapter* ada);

int 
fileAdapterHandle(struct adapter* ada , struct logmsg* msg);

int
closeFileAdapter(struct adapter* ada);

void
freeFileAdapter(struct adapter* ada);

struct adapter*
createConsoleAdapter(struct catagory* cata,ls_t name);

int
initConsoleAdapter(struct adapter* ada, int argc, char** argv);

int 
openConsoleAdapter(struct adapter* ada);

int
consoleAdapterHandle(struct adapter* ada,struct logmsg* msg);

int
closeConsoleAdapter(struct adapter* ada);

void
freeConsoleAdapter(struct adapter* ada);

struct adapter*
createRollingFileAdapter(struct catagory* cata, ls_t name,const char* logfile,long rollingsize);

int
rollingFileAdapterAccept(struct adapter* apt, struct logmsg* msg);

int
freeRollingFileAdapter(struct adapter* apt);
    
//struct adapter*
//createNetAdapter(struct catagory* cata,const char* ip , int port);
//
//int
//initNetAdapter(struct adapter* ada, int argc, char** argv);
//
//int
//netAdapterHandle(struct adapter* ada,struct logmsg* msg);
//
//int
//netAdapterClose(struct adapter* ada);
//
//void
//freeNetAdapter(struct adapter* ada);
//
//struct adapter*
//createDBAdapter(struct catagory* cata,struct adapter* ada);
//
//int
//initDBAdapter(struct adapter* ada, int argc, char** argv);
//
//int
//openDBAdapter(struct adapter* ada);
//
//int
//DBAdapterHandle(struct adapter* ada,struct logmsg* msg);
//
//int
//DBAdapterClose(struct adapter* ada);
//
//void
//freeDBAdapter(struct adapter* ada);



#endif//LOG_ADAPTER_HH
