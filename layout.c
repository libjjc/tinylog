#include "layout.h"
#include "adapter.h"
#include "logdef.h"
#include "logmsg.h"
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

#define layoutAddr(layout_type,callback)\
    (layout_type*)(&callback - (int)(((layout_type*)(NULL))->layout))

layout_callback
createPatternLayout(struct _adapter * ada,ls_t args){
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
strftime_wrap(ls_t ls, const struct tm* t, const char* fmt){
    int szbuf = 4 * strlen(fmt);
    char* buf = (char*)malloc(sizeof(char)*szbuf);
    if (!buf) return ls;
    int szneed = strftime(buf, szbuf, fmt, t);
    if (szneed > szbuf){
        szbuf = szneed;
        buf = realloc(buf, szbuf);
        if (!buf) return ls;
        strftime(buf, szbuf, fmt, t);
    }
    ls = lscat(ls,buf);
    free(buf);
    return ls;
}

ls_t
dateconverse(ls_t ls, char ** fmt, struct _log_msg* msg){
    char* p = *fmt;
    char* s = NULL;
    char* e = NULL;
    char* l = NULL;
    struct tm t;
    if (localtime_s(&t, &msg->ts.sec)){
        return ls;
    }
    while (*p){
        if ('%' == *p && *(p++)){
            if (s&& 'l' == *p){
                l = p-1;
            } else{
                ls = lscatlen(ls, p, 1);
            }
        }else if ('{' == *p ){
            s = p+1;
        } else if ('}' == *p && s && p > s){
            e = p;
        } else ls = lscatlen(ls, p, 1);
        p++;
    }
    if (s && e){
        if (l){
            char temp = *l;
            *l = 0;
            strftime_wrap(ls, &t, s);
            *l = temp;
            char us[8] = { 0 };
            if (_itoa_s(msg->ts.usec, us, 8, 10)){
                ls = lscat(ls, "000000");
            } else{
                ls = lscat(ls, us);
            }
            strftime_wrap(ls, &t, l + 2);
        } else{
            char temp = *e;
            *e = 0;
            strftime_wrap(ls, &t, s);
            *e = temp;
        }
        *fmt = e + 1;
    } else{
        strftime_wrap(ls, &t, "%Y-%m-%d %H:%M:%S");
    }
    return ls;
}

ls_t 
patternLayout(layout_callback layout, struct _log_msg* msg){
    struct patternLayout* ptl = layoutAddr(struct patternLayout, layout);
    char*p = ptl->pattern;
    ls_t message = lscreate(0, 1024);
    char* cmdarg = NULL;
    int cmdlen = 0;
    char cmd = 0;
    while (*p != 0){
        if ('%' == *p && *p++){
            switch (*p){
            case 'c':
                message = lscatls(message, msg->cagy);
                break;
            case 'd':
                p++;
                message = dateconverse(message, &p, msg);
                break;
            case 'm':
                message = lscatls(message, msg->msg);
                break;
            case 'n':
                message = lscat(message, "\\n");
                break;
            case 'p':
                message = lscatls(message, msg->s_prior);
                break;
            case 'r':
                break;
            case 'R':
                message = lscatfmt(message, "%d", msg->ts.sec);
                break;
            case 't':
                break;
                message = lscat(message, "\\t");
            case 'u':

                break;
            case 'x':

                break;
            default:
                message = lscatlen(message, p, 1);
            }
        } else{
            message = lscatlen(message, p, 1);
        }
    }
    return message;
}

layout_callback
createBasicLayout(struct _adapter* ada){
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
basicLayout(layout_callback layout,struct _log_msg* msg){
    ls_t message = lsinitfmt("%ld [%s]: %s \n", msg->ts, msg->s_prior, msg->msg);
    return message;
}
