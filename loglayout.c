#include "loglayout.h"
#include "logadapter.h"
#include "logdef.h"
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>

#define layoutAddr(layout_type,callback)\
    (layout_type*)(&callback - (int)(((layout_type*)(NULL))->layout))

layout_callback
createPatternLayout(struct adapter * ada,ls_t args){
    struct patternLayout* pl;
    pl = (struct patternLayout*)malloc(sizeof(struct patternLayout));
    pl->pattern = lsinitcpyls(args);
    pl->layout = patternLayout;
    if (ada) ada->layout = pl->layout;
    return pl->layout;
}

void
freePatternLayout(layout_callback layout){
    if (!layout) return;
    int offset = (int)((struct patternLayout*)(NULL))->layout;
    struct patternLayout* pl = (struct patternLayout*)(&layout - offset);
    if (pl->pattern){
        lsfree(pl->pattern);
    }
    free(pl);
}

extern char num_str_100[];

ls_t
dateconverse(ls_t ls, const char* fmt , struct logmsg* msg){
    bool macroing = false;
    time_t t_now = msg->timestamp;
    struct tm now;
    if (localtime_s(&now, &t_now)){
        return ls;
    }
    for (const char* p = fmt; *p != 0; p++){
        if ('%' == *p && *(p++)){
            if (!macroing) break;
            switch (*p){
            case 'y':
                ls = lscatlen(ls, &num_str_100[2 * now.tm_year % 100], 2);
                break;
            case 'Y':
                ls = lscatfmt("%d", now.tm_year);
                break;
            case 'M':
                ls = lscatlen(ls, &num_str_100[2 * (now.tm_mon + 1)], 2);
                break;
            case 'D':
                ls = lscatlen(ls, &num_str_100[2 * now.tm_mday], 2);
                break;
            case 'h':
                ls = lscatlen(ls, &num_str_100[2 * now.tm_hour], 2);
                break;
            case 'm':
                ls = lscatlen(ls, &num_str_100[2 * now.tm_min], 2);
                break;
            case 's':
                ls = lscatlen(ls, &num_str_100[2 * now.tm_sec], 2);
                break;
            case 'l':
                break;
            default:
                break;
            }
        }else if ('{' == *p){
            macroing = true;
            continue;
        } else if ('}' == *p){
            macroing = false;
            break;
        } else{
            ls = lscat(ls, p, 1);
        }
    }
    return ls;
}

ls_t 
patternLayout(layout_callback layout, struct logmsg* msg){
    struct patternLayout* ptl = layoutAddr(struct patternLayout, layout);
    const char*p = ptl->pattern;
    ls_t message = lscreate(0, 1024);
    while (*p != 0){
        if ('%' == *p && *p++){
            switch (*p){
            case 'c':
                message = lscatls(message, msg->catagory);
                break;
            case 'd':
                message = dateconverse(message, p+1,msg);
                break;
            case 'm':
                message = lscatls(message, msg->msg);
                break;
            case 'n':
                message = lscat(message, "\\n");
                break;
            case 'p':
                message = lscatls(message, msg->prioname);
                break;
            case 'r':
                break;
            case 'R':
                message = lscatfmt(message, "%d", msg->timestamp);
                break;
            case 't':
                break;
                message = lscat(message, "\\t");
            case 'u':

                break;
            case 'x':

                break;
            }
        }
    }
    return message;
}

layout_callback
createBasicLayout(struct adapter* ada){
    struct basicLayout* bl;
    bl = (struct basicLayout*)malloc(sizeof(struct basicLayout));
    bl->layout = basicLayout;
    if (ada) ada->layout = bl->layout;
    return bl->layout;
}

void
freeBasicLayout(layout_callback layout){
    if (!layout) return;
    int offset = (int)((struct basicLayout*)(NULL))->layout;
    struct basicLayout* pl = (struct basicLayout*)(&layout - offset);
    free(pl);
}

ls_t
basicLayout(layout_callback layout,struct logmsg* msg){
    ls_t message = lsinitfmt("%d [%s]: %s \n", msg->timestamp, msg->prioname, msg->msg);
    return message;
}
