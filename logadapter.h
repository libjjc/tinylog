#ifndef LOG_ADAPTER_HH
#define LOG_ADAPTER_HH

#include "logstr.h"

typedef void* adapter_handle;
typedef int(*adapter_init)(int, char**);
typedef int(*adapter_open)();
typedef void(*adapter_handler)();
typedef void(*adapter_close)();

struct adapter {
    adapter_handle handle;
    adapter_init init;
    adapter_open open;
    adapter_handler handler;
    adapter_close close;
};


struct adapter*
createFileAdapter(const char* logfile,const char* name);

int 
fileAdapterHandle(struct adapter* ada , ls_t msg);

void
fileAdapterClose(struct adapter* ada);

struct adapter*
createConsoleAdapter(ls_t name);

int
consoleAdapterHandle(struct adapter* ada,ls_t msg);
    
struct adapter*
createNetAdapter(int ip,int port);

int
netAdapterHandle(struct adapter* ada,ls_t msg);

void
netAdapterClose();

struct adapter*
createNullAdapter();

int
nullAdapterHandle(struct adapter* ada,ls_t msg);

void
nullAdapterClose();

#endif//LOG_ADAPTER_HH
