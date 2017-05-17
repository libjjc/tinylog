#include "loghelper.h"
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
int
getTimeStamp(){
    return time(0);
}

ls_t
getTimeFormat(const char* fmt){
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
getTimeDefault(){
    return getTimeFormat("%y-%M-%d %d:%m:%s");
}

void
getYear2String(char(*y)[2], int year){
    num_to_str(y, year % 100, 2);
}

void
getYear4String(char(*y)[4], int year){
    num_to_str(y, year , 4);
}

void
getMonthString(char(*m)[2], int month){
    num_to_str(m, month , 2);
}

void
getDayString(char(*d)[2], int day){
    num_to_str(d, day, 2);
}

void
getWeekdayString(char(*w)[8], int weekday){
    num_to_str(w, weekday, 1);
}

void
getHourString(char(*h)[2], int hour){
    num_to_str(h, hour, 2);
}

void
getMinuteString(char(*m)[2], int minute){
    num_to_str(m, minute, 2);
}

void
getSecondString(char(*s)[2], int second){
    num_to_str(s, second, 2);
}

void
getMSString(char(*ms)[3], int s){
    //num_to_str(ms, s, 3);
}

ls_t
lscatnum(ls_t ls, int num){
    ls = lscatlen(ls, &num_str_100[2 * num % 100], 2);
    return ls;
}