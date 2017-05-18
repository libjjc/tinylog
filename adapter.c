#include "adapter.h"
#include "layout.h"
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
    struct _adapter* apt = (struct _adapter*)malloc(sizeof(struct _adapter));\
    if(!apt){\
        free(args);\
        return NULL;\
    }

#define checkAdapter(adapter , args_type , args)\
    if(!adapter || !adapter->args) return -1;\
    args_type* args = ( args_type*)adapter->args;


#define freeAdapter(adapter,args_type)\
    if(adapter && adapter->args){\
        free((args_type*)adapter->args);\
        free(adapter);\
    }


struct _adapter*
createFileAdapter(struct _catagory* cata ,const char* logfile,const char* name,int maxsize){
	allocAdapter(apt, struct fileAdapter, args);

    args->path = lsinitcpy(logfile);
    args->maxsize = maxsize;
    args->flag = O_APPEND | O_WRONLY | O_CREAT | O_TRUNC;
    args->mode = O_TEXT | O_NOINHERIT;

    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->accept = fileAdapterAccept;
    apt->free = freeFileAdapter;
    apt->args = (adapter_arguments)args;
    if (args->fd = _open(args->path, args->flag, args->mode)){
        freeAdapter(apt, struct fileAdapter);
        return NULL;
    }
	args->size = (args->fd);
    if (cata) addAdapter(cata, apt);
    return apt;
}

int
fileAdapterAccept(struct _adapter* apt , struct _log_msg* msg){
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->fd) return -2;
    if (!msg) return -1;
    ls_t ls = (*apt->layout)(apt->layout,msg);
	fa->size += lslen(ls);
	if (fa->size >= fa->maxsize*1024*1024) {
        fa->fd = _open(fa->path, fa->flag | O_TRUNC | O_APPEND, fa->mode);
		fa->size = 0;
	}
    return _write(fa->fd, ls, lslen(ls));
}

int 
freeFileAdapter(struct _adapter* apt){
    if (!apt) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)apt->args;
    if (!fa || !fa->fd) return -1;
    int ret = _close(fa->fd);
    if (ret)return ret;
    struct fileAdapter* args = (struct fileAdapter*)apt->args;
    if (args->path) lsfree(args->path);
    freeAdapter(apt, struct fileAdapter);
    return 0;
}

struct _adapter*
createConsoleAdapter(struct _catagory* cata , ls_t name){
    allocAdapter(apt, struct consoleAdapter, args);

    args->fd = (int)stdout;

    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->accept = consoleAdapterAccept;
    apt->free = freeConsoleAdapter;
    apt->args = (adapter_arguments)args;
    if (cata) addAdapter(cata, apt);
    return apt;
}

int
consoleAdapterAccept(struct _adapter* apt ,struct _log_msg* msg){
    if (!apt || !apt->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)apt->args;
    ls_t ls = (*apt->layout)(apt->layout,msg);
    return fprintf((FILE*)ca->fd, "%s", ls);
}

int
freeConsoleAdapter(struct _adapter* apt){
    if (!apt || !apt->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)apt->args;
    if (!ca->fd)return -2;
    int ret = _close(ca->fd);
    if (ret) return ret;
    freeAdapter(apt, struct consoleAdapter);
    return 0;
}



struct _adapter * 
createRollingFileAdapter(struct _catagory * cata, ls_t name,const char* logfile , long rollingsize)
{    
	allocAdapter(apt,struct rollingFileAdapter,args)

    args->path = lsinitcpy(logfile);
    args->rollingsize = rollingsize;
    args->flag = O_APPEND | O_WRONLY | O_CREAT;
    args->mode = O_TEXT | O_NOINHERIT;
    args->index = 1;
    int fd = 0;
    ls_t path = lsinitfmt("%s_%d", args->path, args->index);
    while (!_access(path, 0)){
        args->index++;
        path = lscpyfmt("%s_%d", args->path, args->index);
        fd = _open(path, args->flag, args->mode);
        long fsz = _filelength(fd);
        if (fd&& fsz < rollingsize * 1024 * 1024){
            args->fd = fd;
            args->size = fsz;
        }
    }
    if (!args->fd){
        fd = _open(path, args->flag, args->mode);
    }
   if (!_open(args->path, args->flag, args->mode)){
        lsfree(args->path);
        freeAdapter(apt, struct rollingFileAdapter);
    }

    apt->name = lsinitcpy(name);
    apt->layout = basicLayout;
    apt->accept = rollingFileAdapterAccept;
    apt->free = freeRollingFileAdapter;
    apt->args = (adapter_arguments)args;
    if (cata) addAdapter(cata, apt);
    return apt;
}

int 
rollingFileAdapterAccept(struct _adapter * apt, struct _log_msg * msg)
{
    checkAdapter(apt, struct rollingFileAdapter, args);
    if (!args->fd) return -2;
    if (!msg) return 0;
    ls_t ls = (*apt->layout)(apt->layout,msg);
	args->size += lslen(ls);
	if (args->size >= args->rollingsize*1024*1024) {
        _close(args->fd);

        //todo
        //reopen another file
        //args->fd = _open(args->path, args->flag | O_TRUNC | O_APPEND, args->mode);
		args->size = 0;
	}
    _write(args->fd, ls, lslen(ls));
    lsfree(ls);
	return 0;
}

int 
freeRollingFileAdapter(struct _adapter * apt)
{
	return 0;
}

//struct _adapter*
//createNetAdapter(struct _catagory* catagory,const char* ip, int port){
//    return NULL;
//}
//
//int
//initNetAdapter(struct _adapter* apt, int argc, char** argv){
//    return 0;
//}
//
//int
//netAdapterHandle(struct _adapter* apt, struct _log_msg* msg){
//    return 0;
//}
//
//int
//netAdapterClose(struct _adapter* apt){
//    return 0;
//}
//
//void
//freeNetAdapter(struct _adapter* apt){
//    free(apt);
//}
//
//struct _adapter*
//createDBAdapter(struct _catagory* cata ,struct _adapter* apt){
//    return NULL;
//}
//
//int
//initDBAdapter(struct _adapter* apt, int argc, char** argv){
//    return 0;
//}
//
//int
//openDBAdapter(struct _adapter* apt){
//    return 0;
//}
//
//int
//DBAdapterHandle(struct _adapter* apt, struct _log_msg* msg){
//    return 0;
//}
//
//int
//DBAdapterClose(struct _adapter* apt){
//    return 0;
//}
//
//void
//freeDBAdapter(struct _adapter* apt){
//    free(apt);
//}