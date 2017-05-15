#include "logadapter.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

struct adapter*
createFileAdapter(const char* logfile,const char* name){
    FILE* f = NULL;
    if (f = fopen(logfile, "at+")) return NULL;
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    ada->handle = ada;
    return ada;
}

int
fileAdapterHandle(struct adapter* ada , ls_t msg){
    if (!ada||!ada->handle) return -1;
    FILE* f = (FILE*)ada->handle;
    if (fputs(msg, f)) return -1;
    return 0;
}

void 
fileAdapterClose(struct adapter* ada){
    if (!ada||!ada->handle) return -1;
    FILE* f = (FILE*)ada->handle;
    if (fclose(f)) return -1;
    return 0;
}


struct adapter*
createConsileAdapter(ls_t name){
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    ada->handle = stdout;
    return ada;
}

int
consoleAdapterHandle(struct adapter* ada ,ls_t msg){
    if (!ada || ada->handle) return -1;
    if (fputs(msg, ada->handle)) return -2;
    return 0;
}

void 
consoleAdapterClose(struct adapter* ada, ls_t msg){
    if (ada && ada->handle){
        fclose((FILE*)ada->handle);
    }
}

struct adapter*
createNetAdapter(int ip, int port){
    return NULL;
}

int
netAdapterHandle(struct adapter* ada, ls_t msg){
    return -1;
}

void
netAdapterClose(){

}

struct adapter*
createNullAdapter(){
    return NULL;
}

int
nullAdapterHandle(struct adapter* ada, ls_t msg){
    return -1;
}

void
nullAdapterHandle(){

}