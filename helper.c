#include "helper.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

const char* num_str_100 = 
"00010203040506070809"
"10111213141516171819"
"20212223242526272829"
"30313233343536373839"
"40414243444546474849"
"50515253545556575859"
"60616263646566676869"
"70717273747576777879"
"80818283848586878889"
"90919293949596979899";

static const char* num_61[61] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60"
};

#define num_to_str(str,num,digit)\
    memcpy_s(str,(digit),&num_str_100[2*(num)],(digit));

ls_t
strtimenow(const char* fmt){
    time_t t_now;
    time(&t_now);
    struct tm now;
    if (localtime_s(&now, &t_now)){
        return NULL;
    }
    int len = strlen(fmt);
    ls_t time = lsinit(0, len*2);
    char flag = 0;
    bool esc = 0;
    for (const char* p = fmt; *p != 0; p++){
        if ('%' == *p ){
            if (esc) time = lscat(time, "%");
            else esc = true;
            continue;
        }
        if (!esc){
            time = lscatlen(time, p, 1);
            continue;
        }
        if ('y' == *p){
            time = lscatlen(time, &num_str_100[2*now.tm_year % 100], 2);
        } else if ('Y' == *p){
            char sy[5] = { 0 };
            _itoa_s(now.tm_year + 1900, &sy[0], 4, 10);
            time = lscat(time, &sy[0]);
        } else if ('M' == *p ){
            time = lscatlen(time, &num_str_100[2*(now.tm_mon+1)],2);
        } else if ('d' == *p ){
            time = lscatlen(time, &num_str_100[2*now.tm_mday],2);
        } else if ('h' == *p){
            time = lscatlen(time, &num_str_100[2 * now.tm_hour], 2);
        } else if ('m' == *p){
            time = lscatlen(time, &num_str_100[2 * now.tm_min], 2);
        } else if ('s' == *p){
            time = lscatlen(time, &num_str_100[2 * now.tm_sec], 2);
        } else{
            time = lscat(time, "%");
            time = lscatlen(time, p, 1);
        }
        esc = false;
    }
    return time;
}

ls_t
strnow(){
    return strtimenow("%y-%M-%d %d:%m:%s");
}
