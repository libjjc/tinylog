#include "logadapter.h"
#include "loglayout.h"
#include "tinylog.h"
#include "catagory.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>

#define allocAdapter(apt,args_type,args)\
    args_type* args = (args_type*)malloc(sizeof(args_type));\
    if(!args) return NULL;\
    struct adapter* apt = (struct adapter*)malloc(sizeof(struct adapter));\
    if(!apt){\
        free(args);\
        return NULL;\
    }

#define checkAdapter(adapter , args_type , args)\
    if(!adapter || !adapter->args) return -1;\
    struct args_type args = (struct args_type*)adapter->args;


#define freeAdapter(adapter,args_type)\
    if(adapter && adapter->args){\
        free((struct args_type*)adapter->args);\
    }\
    if (adapter) free(adapter);


struct adapter*
createFileAdapter(struct catagory* cata ,const char* logfile,const char* name,int maxsize){
	allocAdapter(apt, struct fileAdapter, args);

    args->path = lsinitcpy(logfile);
    args->maxsize = maxsize;
  
    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->init = initFileAdapter;
    apt->open = openFileAdapter;
    apt->handler = fileAdapterHandle;
    apt->close = closeFileAdapter;
    apt->free = freeFileAdapter;
    apt->args = (adapter_arguments)args;
    if (cata) addAdapter(cata, apt);
    initFileAdapter(apt, 0, 0);
    openFileAdapter(apt);
    return apt;
}

int 
initFileAdapter(struct adapter* apt, int argc, char** argv) {
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->path){
        return -2;
    }
    fa->flag = O_APPEND | O_WRONLY | O_CREAT;
    fa->mode = O_TEXT | O_NOINHERIT;
    if (fa->fd = _open(fa->path, fa->flag, fa->mode)){
        return -2;
    }
	fa->size = (fa->fd);
	return 0;
}


int
openFileAdapter(struct adapter* apt) {
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->fd) return -2;
	return 0;
}

int
fileAdapterHandle(struct adapter* apt , struct logmsg* msg){
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->fd) return -2;
    if (!msg) return -1;
    ls_t ls = (*apt->layout)(apt->layout,msg);
	fa->size += lslen(ls);
	if (fa->size >= fa->maxsize*1024*1024) {
		fa->fd = _open(fa->path, O_APPEND | fa->flag, fa->mode);
		fa->size = 0;
	}
    return _write(fa->fd, ls, lslen(ls));
}

int 
closeFileAdapter(struct adapter* apt){
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->fd) return -1;
    return _close(fa->fd);
}

void 
freeFileAdapter(struct adapter* apt){
    if (apt && apt->args){
        free((struct fileAdapter*)apt->args);
    }
    if (apt) free(apt);
}

struct adapter*
createConsoleAdapter(struct catagory* cata , ls_t name){
    struct consoleAdapter* ca;
    ca = (struct consoleAdapter*)malloc(sizeof(struct consoleAdapter));
    if (!ca) return NULL;
    struct adapter* apt = (struct adapter*)malloc(sizeof(struct adapter));
    if (!apt){
        free(ca);
        return NULL;
    }
    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->init = initConsoleAdapter;
    apt->open = openConsoleAdapter;
    apt->handler = consoleAdapterHandle;
    apt->close = closeConsoleAdapter;
    apt->free = freeConsoleAdapter;
    apt->args = (adapter_arguments)ca;
    if (cata) addAdapter(cata, apt);
    initConsoleAdapter(apt, 0, 0);
    openConsoleAdapter(apt);
    return apt;
}

int 
initConsoleAdapter(struct adapter* apt, int argc, char** argv){
    if (!apt || !apt->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)apt->args;
    ca->fd = stdout;
    return 0;
}

int 
openConsoleAdapter(struct adapter* apt){
    return apt ? 0 : -1;
}

int
consoleAdapterHandle(struct adapter* apt ,struct logmsg* msg){
    if (!apt || !apt->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)apt->args;
    ls_t ls = (*apt->layout)(apt->layout,msg);
    //return _write(ca->fd, ls, lslen(ls));
    return fprintf(ca->fd, "%s", ls);
}

int 
closeConsoleAdapter(struct adapter* apt){
    if (!apt || !apt->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)apt->args;
    if (!ca->fd)return -2;
    return _close(ca->fd);
}

void
freeConsoleAdapter(struct adapter* apt){
    if (apt && apt->args){
        free((struct consoleAdapter*)apt->args);
    }
    if (apt) free(apt);
}



struct adapter * 
createRollingFileAdapter(struct catagory * cata, ls_t name,const char* logfile , long rollingsize)
{    
	allocAdapter(apt,struct rollingFileAdapter,args)

    args->path = lsinitcpy(logfile);
    args->rollingsize = rollingsize;

    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->init = initFileAdapter;
    apt->open = openFileAdapter;
    apt->handler = fileAdapterHandle;
    apt->close = closeFileAdapter;
    apt->free = freeFileAdapter;
    apt->args = (adapter_arguments)args;
    if (cata) addAdapter(cata, apt);
    
    return apt;
}

int 
rollingFileAdapterAccept(struct adapter * apt, struct logmsg * msg)
{
	return 0;
}

int 
freeRollingFileAdapter(struct adapter * apt)
{
	return 0;
}

//struct adapter*
//createNetAdapter(struct catagory* catagory,const char* ip, int port){
//    return NULL;
//}
//
//int
//initNetAdapter(struct adapter* apt, int argc, char** argv){
//    return 0;
//}
//
//int
//netAdapterHandle(struct adapter* apt, struct logmsg* msg){
//    return 0;
//}
//
//int
//netAdapterClose(struct adapter* apt){
//    return 0;
//}
//
//void
//freeNetAdapter(struct adapter* apt){
//    free(apt);
//}
//
//struct adapter*
//createDBAdapter(struct catagory* cata ,struct adapter* apt){
//    return NULL;
//}
//
//int
//initDBAdapter(struct adapter* apt, int argc, char** argv){
//    return 0;
//}
//
//int
//openDBAdapter(struct adapter* apt){
//    return 0;
//}
//
//int
//DBAdapterHandle(struct adapter* apt, struct logmsg* msg){
//    return 0;
//}
//
//int
//DBAdapterClose(struct adapter* apt){
//    return 0;
//}
//
//void
//freeDBAdapter(struct adapter* apt){
//    free(apt);
//}