#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include "config.h"
#include "logdef.h"
#include "logstr.h"

int
configure(const char* config){
    if (!config) return -1;

    int fd = _open(config, O_RDONLY, O_TEXT | O_SEQUENTIAL);

    if (!fd) return -1;

    int flen = _filelength(fd);

    if (flen == (long)-1) return -1;

    ls_t buffer = lscreate(NULL, flen);

    if (!buffer) return -1;

    if (_read(fd, buffer, flen))return -1;

    int ret = 0;

    ls_t line = lscreate(NULL, 256);

    const char* p = buffer;
    const char* f = buffer;

    while (*p){
        if ('\n' == *p || '\r' == *p){
            line = lscatlen(line, f, p - f);
            f = p;
            if (lsconfigure(line)){
                lsfree(line);
                return -1;
            }
        }
        p++;
    }
    lsfree(buffer);

    return ret;
}

int 
lineconfigure(ls_t ls){
    int size = 8;
    ls_t* lss = (ls_t*)malloc(sizeof(ls_t*)* size);
    lss = lssplit(ls, ",. ", lss, &size);
    if (!lss) return -1;
	for (int i = 0; i < size; i++) {
		
	}
    while (--size >= 0)lsfree(lss[size]);
    free(lss);
    return 0;
}

int 
_config_root(int argc, char ** argv)
{
	return 0;
}

int 
_config_catagory(const char * name, int argc, char ** argv)
{
	return 0;
}

int 
_config_adapter(const char * name, int argc, char ** argv)
{
	return 0;
}

