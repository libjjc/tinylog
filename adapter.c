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
#include <math.h>

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
        if(adapter->name)lsfree(adapter->name);\
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
    _write(fa->fd, ls, lslen(ls));
    lsfree(ls);
    return 0;
}

int 
freeFileAdapter(struct _adapter* apt){
    checkAdapter(apt, struct fileAdapter, args);
    if (!args->fd) return -1;
    int ret = _close(args->fd);
    if (ret)return ret;
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
    fprintf((FILE*)ca->fd, "%s", ls);
    lsfree(ls);
    return 0;
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
createRollingFileAdapter(struct _catagory * cata, ls_t name,const char* logfile , long rollingsize,long rollingcount)
{    
	allocAdapter(apt,struct rollingFileAdapter,args)

    args->path = lsinitcpy(logfile);
    args->rollingsize = rollingsize;
    args->flag = O_APPEND | O_WRONLY | O_CREAT;
    args->mode = O_TEXT | O_NOINHERIT;
    args->index = 0;
    args->rollingcount = rollingcount <= 0? 1: rollingcount;
    args->extw = (long)log10(args->rollingcount) + 1;
    ls_t back_path = lsinitcpyls(args->path);
    for (; args->index < args->rollingcount; args->index++){
        back_path = lscatfmt(back_path, ".%.*s",args->extw,args->index+1);
        if (!_access(back_path, 0)){
            break;
        }
    }
    lsfree(back_path);
    int fd = _open(args->path, args->flag, args->mode);
    if (!fd){
        lsfree(args->path);
        freeAdapter(apt, struct rollingFileAdapter);
        return NULL;
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
rollingover(struct rollingFileAdapter* args){
    if (!args) return -1;
    ls_t back_path = lsinitfmt(args->path,".%.*d",args->extw,1);
    if (args->index + 1 >= args->rollingcount){
        remove(back_path);
        for (int i = 1; i < args->rollingcount; i++){
            ls_t path_old = lsinitfmt("%s.%.*d", args->path, i + 1);
            if (!_access(path_old, 0)){
                ls_t path_new = lsinitfmt("%s.%.*d", args->path, i);
                rename(path_old, path_new);
                lsfree(path_old);
                lsfree(path_new);
            } else{
                break;
            }
        }
        rename(args->path, back_path);
        lsfree(back_path);
        return 0;
    }
    rename(args->path, back_path);
    args->index++;
    lsfree(back_path);
    return 0;
}

int 
rollingFileAdapterAccept(struct _adapter * apt, struct _log_msg * msg)
{
    checkAdapter(apt, struct rollingFileAdapter, args);
    if (!args->fd) return -2;
    if (!msg) return 0;
    ls_t ls = (*apt->layout)(apt->layout,msg);
	if (args->size + lslen(ls) >= args->rollingsize*1024*1024) {
        _close(args->fd);
        rollingover(args);
		args->size = 0;
        args->fd = _open(args->path, args->flag, args->mode);
        if (!args->fd) {
            lsfree(ls);
            return -2;
        }
	}
    _write(args->fd, ls, lslen(ls));
    args->size += strlen(ls);
    lsfree(ls);
	return 0;
}

int 
freeRollingFileAdapter(struct _adapter * apt)
{
    checkAdapter(apt, struct rollingFileAdapter,args);
    if (args->fd) _close(args->fd);
    if (args->path) lsfree(args->path);
    freeAdapter(apt,struct rollingFileAdapter);
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