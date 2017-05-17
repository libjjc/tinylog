#ifndef LOG_HELPER_HH
#define LOG_HELPER_HH

#include <time.h>
#include <stdbool.h>
#include "logstr.h"

int
getTimeStamp();

ls_t
getTimeFormat(const char* fmt);

ls_t
getTimeDefault();

void
getYear2String(char(*y)[2], int year);

void
getYear4String(char(*y)[4], int year);

void
getMonthString(char(*m)[2], int month);

void
getDayString(char(*d)[2], int day);

void
getWeekdayString(char(*w)[8], int weekday);

void
getHourString(char(*h)[2], int hour);

void
getMinuteString(char(*m)[2], int minute);

void
getSecondString(char(*s)[2], int second);

void
getMSString(char(*ms)[3], int s);

ls_t
lscatnum(ls_t ls, int num);



#endif//LOG_HELPER_HH
