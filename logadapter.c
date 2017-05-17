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

#define allocAdapter(adapter,args_type,args)\
    struct args_type* args = (struct args_type*)malloc(sizeof(struct args_type));\
    if(!args) return NULL;\
    struct adapter* adapter = (struct adapter*)malloc(sizeof(struct adapter));\
    if(!adapter){\
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

<<<<<<< HEAD
=======
void 
freeLayout(struct layout * layout){
}

void 
setLayoutArgs(struct layout * layout, layout_callback format, ls_t args){
}

ls_t 
patternLayout(ls_t args, struct logmsg * msg){
	return "";
}

ls_t 
defaultLayout(ls_t args, struct logmsg * msg){
	return "";
}
>>>>>>> 39bea13f7fcc43e88fe1cc2090f804761967b097

struct adapter*
createFileAdapter(struct catagory* cata ,const char* logfile,const char* name,int maxsize){
    struct fileAdapter* fa = (struct fileAdapter*)malloc(sizeof(struct fileAdapter));
    if (!fa) return NULL;
    fa->path = lsinitcpy(logfile);
    fa->maxsize = maxsize;
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    if (!ada){
        free(fa);
        return NULL;
    }
    ada->name = lsinitcpy(name);
    ada->layout = basicLayout;
    ada->init = initFileAdapter;
    ada->open = openFileAdapter;
    ada->handler = fileAdapterHandle;
    ada->close = closeFileAdapter;
    ada->free = freeFileAdapter;
    ada->args = (adapter_arguments)fa;
    if (cata) addAdapter(cata, ada);
    initFileAdapter(ada, 0, 0);
    openFileAdapter(ada);
    return ada;
}

int 
initFileAdapter(struct adapter* ada, int argc, char** argv) {
    if (!ada) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)ada->args;
    if (!fa || !fa->path){
        return -2;
    }
    fa->flag = O_APPEND | O_WRONLY | O_CREAT;
    fa->mode = O_TEXT | O_NOINHERIT;
    if (fa->fd = _open(fa->path, fa->flag, fa->mode)){
        return -2;
    }
	return 0;
}


int
openFileAdapter(struct adapter* ada) {
    if (!ada) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)ada->args;
    if (!fa || !fa->fd) return -2;
	return 0;
}

int
fileAdapterHandle(struct adapter* ada , struct logmsg* msg){
    if (!ada) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)ada->args;
    if (!fa || !fa->fd) return -2;
    if (!msg) return -1;
    ls_t s = (*ada->layout)(ada->layout,msg);
    return _write(fa->fd, s, lslen(s));
}

int 
closeFileAdapter(struct adapter* ada){
    if (!ada) return -1;
    struct fileAdapter* fa = (struct fileAdapter*)ada->args;
    if (!fa || !fa->fd) return -1;
    return _close(fa->fd);
}

void 
freeFileAdapter(struct adapter* ada){
    if (ada && ada->args){
        free((struct fileAdapter*)ada->args);
    }
    if (ada) free(ada);
}


struct adapter*
createConsoleAdapter(struct catagory* cata , ls_t name){
    struct consoleAdapter* ca;
    ca = (struct consoleAdapter*)malloc(sizeof(struct consoleAdapter));
    if (!ca) return NULL;
    struct adapter* ada = (struct adapter*)malloc(sizeof(struct adapter));
    if (!ada){
        free(ca);
        return NULL;
    }
    ada->name = lsinitcpy(name);
    ada->layout = basicLayout;
    ada->init = initConsoleAdapter;
    ada->open = openConsoleAdapter;
    ada->handler = consoleAdapterHandle;
    ada->close = closeConsoleAdapter;
    ada->free = freeConsoleAdapter;
    ada->args = (adapter_arguments)ca;
    if (cata) addAdapter(cata, ada);
    initConsoleAdapter(ada, 0, 0);
    openConsoleAdapter(ada);
    return ada;
}

int 
initConsoleAdapter(struct adapter* ada, int argc, char** argv){
    if (!ada || !ada->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)ada->args;
    ca->fd = stdout;
    return 0;
}

int 
openConsoleAdapter(struct adapter* ada){
    return ada ? 0 : -1;
}

int
consoleAdapterHandle(struct adapter* ada ,struct logmsg* msg){
    if (!ada || !ada->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)ada->args;
    ls_t ls = (*ada->layout)(ada->layout,msg);
    //return _write(ca->fd, ls, lslen(ls));
    return fprintf(ca->fd, "%s", ls);
}

int 
closeConsoleAdapter(struct adapter* ada){
    if (!ada || !ada->args) return -1;
    struct consoleAdapter* ca = (struct consoleAdapter*)ada->args;
    if (!ca->fd)return -2;
    return _close(ca->fd);
}

void
freeConsoleAdapter(struct adapter* ada){
    if (ada && ada->args){
        free((struct consoleAdapter*)ada->args);
    }
    if (ada) free(ada);
}

//struct adapter*
//createNetAdapter(struct catagory* catagory,const char* ip, int port){
//    return NULL;
//}
//
//int
//initNetAdapter(struct adapter* ada, int argc, char** argv){
//    return 0;
//}
//
//int
//netAdapterHandle(struct adapter* ada, struct logmsg* msg){
//    return 0;
//}
//
//int
//netAdapterClose(struct adapter* ada){
//    return 0;
//}
//
//void
//freeNetAdapter(struct adapter* ada){
//    free(ada);
//}
//
//struct adapter*
//createDBAdapter(struct catagory* cata ,struct adapter* ada){
//    return NULL;
//}
//
//int
//initDBAdapter(struct adapter* ada, int argc, char** argv){
//    return 0;
//}
//
//int
//openDBAdapter(struct adapter* ada){
//    return 0;
//}
//
//int
//DBAdapterHandle(struct adapter* ada, struct logmsg* msg){
//    return 0;
//}
//
//int
//DBAdapterClose(struct adapter* ada){
//    return 0;
//}
//
//void
//freeDBAdapter(struct adapter* ada){
//    free(ada);
//}