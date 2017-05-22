#include "logstr.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "tinylog.h"
#include "catagory.h"
#include "adapter.h"
#include "logdef.h"
#include <string.h>
int main(int argc, char** argv){
    int tf = _open("d:/test.txt", O_RDONLY, O_TEXT);
    
    const char* s5 = "1234567";
    const char* s6 = "567";
    const char* s7 = "789";
    char* r = strstr(s5, s6);
    r = strstr(s5, s7);
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
    ls_t ss = lsinitfmt("%.*s", 9, "hello");

    
    return 0;
}