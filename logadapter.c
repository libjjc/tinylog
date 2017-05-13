#include "logadapter.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
int
addAdapter(struct adapter* ada, struct adapter* handler) {
    if (!ada) return -1;
    if (ada->count == MAX_HANDLER_CHILDREN) return -2;
    if (getAdapter(ada,ada->name))return -3;
    handler->parent = ada;
    ada->children[ada->count] = handler;
    ada->count++;
    return 0;
}

struct adapter*
getAdapter(struct adapter* ada, ls_t name) {
    if (!ada) return 0;
    if (!lscmpls(ada->name, name)) return ada;
    struct adapter* ptr = NULL;
    for (int i = 0; i < ada->count; i++){
        if (!(ptr = getAdapter(ada->children[i], name))){
            return ptr;
        }
    }
    return NULL;
}

void
removeAdapter(struct adapter* ada, ls_t name){
    if (!ada) return ;
    struct adapter* ptr = getAdapter(ada, name);
    if (!ptr || !ptr->parent) return;

    struct adapter* s = ptr->parent->children[0];
    struct adapter* e = ptr->parent->children[MAX_HANDLER_CHILDREN];
    struct adapter* p = ptr;
    int size = sizeof(struct adapter*);
    memmove_s(p, (e - p)*size, p + size, size*(e - p - 1));
    ptr->parent->count--;
}

void 
clearAdapters(struct adapter* ada){
    if (!ada) return;
    ada->count = 0;
}

struct adapter*
createFileAdapter(const char* logfile,const char* name){
    FILE* f = NULL;
    if (f = fopen(logfile, "at+")) return NULL;
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    ada->handle = ada;
    ada->parent = NULL;
    ada->count = 0;
    lscpy(ada->name, name);
    return ada;
}

int 
initFileAdapter(struct adapter* ada, int argc, char** argv) {
	return 0;
}

int
openFileAdapter(struct adapter* ada) {
	return 0;
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
    if (!ada||!ada->handle) return ;
    FILE* f = (FILE*)ada->handle;
    if (fclose(f)) return ;
    return ;
}


struct adapter*
createConsoleAdapter(ls_t name){
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    ada->handle = stdout;
    lscpy(ada->name, name);
    ada->count = 0;
    ada->parent = NULL;
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
createNetAdapter(const char* ip, int port){
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
createDBAdapter(struct adapter* ada, ls_t msg) {
	return NULL;
}

int
DBAdapterHandle(const char* connect_string) {
	return 0;
}

void 
DBAdapterClose() {

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
nullAdapterClose(){

}