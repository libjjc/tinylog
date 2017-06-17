#include "logstr.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "tinylog.h"
#include "catagory.h"
#include "logger.h"
#include "logdef.h"
#include <string.h>

#include "config.h"


int main(int argc, char** argv){

    _tlinit();
    _tinylog_configure("f:/logger.cfg");
    _tlopen();
    _tlshutdown();
    for (int i = 0; i < 1000000; i++)
    _tlfatal("this is just a test !!");
    return 0;
}