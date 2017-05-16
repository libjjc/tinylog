#ifndef LOG_ADAPTER_HH
#define LOG_ADAPTER_HH

#include "logstr.h"

struct logmsg;
typedef ls_t(*layout_callback)(ls_t args,struct logmsg* msg);

typedef void* adapter_arguments;
typedef void* adapter_handle;
typedef int(*adapter_init)(int, char**);
typedef int(*adapter_open)();
typedef void(*adapter_handler)();
typedef void(*adapter_close)();


struct layout {
	layout_callback format;
	ls_t args;
};

struct fileAdapterArgs {
    ls_t path;
    int fd;
    int mode;
};

struct rollingFileAdapterArgs {
    ls_t path;
    int fd;
    int mode;
    int szbfile;
    int currentIndex;
};

struct remoteAdapterArgs {
    ls_t ip;
    int port;
    int socket;
    int mode;
};


struct adapter {
    adapter_arguments args;
    adapter_handle handle;
	struct layout* layout;
    adapter_init init;
    adapter_open open;
    adapter_handler handler;
    adapter_close close;
};
struct layout*
createEmptyLayout();

struct layout*
createLayout(struct adapter* ada, layout_callback format, ls_t args);

void
freeLayout(struct layout* layout);

void
setLayoutArgs(struct layout* layout, layout_callback format, ls_t args);

ls_t 
patternLayout(ls_t args, struct logmsg* msg);

ls_t
defaultLayout(ls_t args, struct logmsg* msg);



struct adapter*
createFileAdapter(const char* logfile,const char* name);

int 
initFileAdapter(struct adapter* ada, int argc, char** argv);

int
openFileAdapter(struct adapter* ada);

int 
fileAdapterHandle(struct adapter* ada , ls_t msg);

void
fileAdapterClose(struct adapter* ada);

struct adapter*
createConsoleAdapter(ls_t name);

int
consoleAdapterHandle(struct adapter* ada,ls_t msg);
    
struct adapter*
createNetAdapter(const char* ip , int port);

int
netAdapterHandle(struct adapter* ada,ls_t msg);

void
netAdapterClose();

struct adapter*
createDBAdapter(struct adapter* ada, ls_t msg);

int
DBAdapterHandle(const char* connect_string);

void
DBAdapterClose();

struct adapter*
createNullAdapter();

int
nullAdapterHandle(struct adapter* ada,ls_t msg);

void
nullAdapterClose();

#endif//LOG_ADAPTER_HH
