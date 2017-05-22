#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include "layout.h"
#include "adapter.h"
#include "logdef.h"
#include "logmsg.h"

#define layoutAddr(layout_type,callback)\
    ((layout_type*)(&callback - (int)(((layout_type*)(NULL))->layout)))

layout_callback
create_pattern_layout(adapter_accept apt,ls_t pattern){
    struct _layout* pl =
        (struct _layout*)malloc(sizeof(struct _layout));
    if (pattern){
        pl->pattern = lsinitcpyls(pattern);
    } else{
        pl->pattern = lscreate("", 0);
    }
    pl->layout = patternLayout;
    pl->free = layout_general_free;
    timestamp(&pl->ts);
    if (apt){
        _set_apt_layout(apt, pl->layout);
    }
    return pl->layout;
}

layout_callback
create_base_layout(adapter_accept apt){
    struct _layout* pl =
        (struct _layout*)malloc(sizeof(struct _layout));

    pl->layout = basicLayout;
    pl->free = layout_general_free;
    timestamp(&pl->ts);
    if (apt){
        _set_apt_layout(apt, pl->layout);
    }
    return pl->layout;
}

void
free_layout(layout_callback layout){
    if (!layout) return;
    layout_free _free = layoutAddr(struct _layout, layout)->free;
    _free(layout);
}

void
layout_general_free(layout_callback layout){
    if (!layout) return;
    struct _layout *_layout = layoutAddr(struct _layout, layout);
    if (_layout->pattern){
        lsfree(_layout->pattern);
    }
    free(_layout);
}

int
set_layout_pattern(layout_callback layout, const char* pattern){
    if (!layout) return -1;
    struct _layout* _layout = layoutAddr(struct _layout, layout);
    _layout->layout = patternLayout;
    if (_layout->pattern){
        _layout->pattern = lscpy(_layout->pattern, pattern);
    } else{
        _layout->pattern = lscreate(pattern, strlen(pattern));
    }
    return 0;
}

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
    struct _layout* ptl = layoutAddr(struct _layout, layout);
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
                message = lscatfmt(message, "%d", delta_ms_epoch(&msg->ts));
                break;
            case 'R':
                message = lscatfmt(message, "%d", delta_s_epoch(&msg->ts));
                break;
            case 't':
                message = lscat(message, "\\t");
                break;
            case 'u':
                message = lscatfmt(message, "%d", delta_ms(&ptl->ts,&msg->ts));
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


ls_t
basicLayout(layout_callback layout,struct _log_msg* msg){
    ls_t message = lsinitfmt("%ld [%s]: %s \n", msg->ts, msg->s_prior, msg->msg);
    return message;
}
