#include "logadapter.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>


struct layout * 
createEmptyLayout()
{
	return NULL;
}

struct layout * 
createLayout(struct adapter * ada, layout_callback format, ls_t args)
{
	return NULL;
}

void 
setLayoutArgs(struct layout * layout, layout_callback format, ls_t args)
{
}

ls_t 
patternLayout(ls_t args, struct logmsg * msg)
{
	return "";
}

ls_t 
defaultLayout(ls_t args, struct logmsg * msg)
{
	return "";
}

struct adapter*
createFileAdapter(const char* logfile,const char* name){
    FILE* f = NULL;
    if (f = fopen(logfile, "at+")) return NULL;
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    ada->handle = ada;
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