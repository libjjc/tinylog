#include "logstr.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "tinylog.h"
#include "catagory.h"
#include "logadapter.h"
#include "logdef.h"
int main(int argc, char** argv){
    ls_t s1, s2, s3, s4;
    s1 = lsinit('a', 12);
    s2 = lscreate("Hello World!", 12);
    s3 = lsinitfmt("%s,%s %d", "xx", "jjc", 12312);
    s4 = lsmkempty();

    s1 = lscpyls(s1, s2);
    s4 = lscpy(s4, s3);
    int ret = lscmp(s1, s2);
    ret = lscmpls(s3, s4);
    ret = lscmp(s1, s4);
    ret = lscmpls(s3, s2);

    s1 = lsmkroom(s1, 32);
    ret = lslen(s1);
    ret = lsavil(s1);
    ret = lssize(s1);

    s1 = lscat(s1, s4);
    s1 = lscatls(s1, s3);

    s2 = lsupper(s2);
    s2 = lslower(s2);
    lsclear(s3);

    s3 = lscpy(s3, "  this is what eht fuck");
    s2 = lscpy(s2, "we are family   ");
    s4 = lscpy(s4, " ");
    s1 = lscpy(s1, "    ");

    s3 = lstrim(s3);
    s2 = lstrim(s2);
    s4 = lstrim(s4);
    s1 = lstrim(s1);


    //s1 = lscpy(s1, ",running man!");
    s2 = lscatfmt(s2, "%d,%s,%d", 100, ",running man!", 9999);

    lsfree(s1);
    lsfree(s2);
    lsfree(s3);
    lsfree(s4);

	int fd = _open("d:/2.txt", O_APPEND | O_WRONLY | O_CREAT, O_TEXT | O_NOINHERIT);
	_write(fd, "aaa\n", 4);
	_write(fd, "hello world!\n", 14);
	fd = _open("d:/2.txt", O_APPEND | O_WRONLY | O_CREAT | O_TRUNC, O_TEXT | O_NOINHERIT);
	_write(fd, "bbbb\n", 5);
	_write(fd, "end\n", 4);
	_close(fd);
    tlinit();
    struct catagory* error = createCatagory(root(), TLL_ERROR, "error test");
    struct catagory* warn = createCatagory(root(), TLL_WARN, "warn test");
    createConsoleAdapter(error, "console.error");
    createFileAdapter(warn, "d:/1.txt", "file.warn",100);
    //fprintf(stdout, "%s", "hello world!");
    //write(stdout,)
    //write((FILE*)(&stdout), "hello world!", 12);
    tllog(TLL_FATAL, "fatal error");
    tlerror("error error");
    tlwarn("warn error");
    tlshutdown();
    
    return 0;
}