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

struct A {
    int a;
    char b;
};
struct B {
    struct A c;
    float d;
};
int main(int argc, char** argv){
    _tinylog_configure("f:/logger.cfg");
    
    return 0;
}