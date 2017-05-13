#ifndef LOG_ADAPTER_HH
#define LOG_ADAPTER_HH

#include "logstr.h"

typedef void* adapter_handle;
typedef void(*adapter_handler)(ls_t msg);

#define MAX_HANDLER_CHILDREN 16
#define MAX_ADA_NAME_LENGTH 32
struct adapter {
    adapter_handle handle;
    struct adapter* parent;
    int count;
    ls_t name;
    struct adapter* children[MAX_HANDLER_CHILDREN];
};


int
addAdapter(struct adapter* ada,struct adapter* handle);

struct adapter*
getAdapter(struct adapter* ada,ls_t name);

void
removeAdapter(struct adapter* ada,ls_t name);

void
clearAdapters(struct adapter* ada);

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

int
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
