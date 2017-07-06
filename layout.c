#include <string.h>
#include <malloc.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <stdio.h>
#include "layout.h"
#include "logger.h"
#include "logdef.h"
#include "logmsg.h"
#include <time.h>
#define _layout_addr(lyt)\
    ((struct _layout*)(&(lyt) - (int)(&((struct _layout*)(NULL))->layout)))

#define _layout_from_impl(ipl)\
    ((struct _layout*)(&(ipl) - (int)(&((struct _layout*)(NULL))->priv)))
_layout_t
_create_layout(struct _logger* logger){
    struct _layout* layout = malloc(sizeof(struct _layout));
    if (layout){
        timestamp(&layout->ts);
    }
    return layout;
}

void
_free_layout(_layout_t layout){
    if (layout&& layout->privfree){
        layout->privfree(layout->priv);
    }
    free(layout);
}

_lyt_basic_priv_t
_create_basic_layout_impl(_layout_t layout){
    if (layout){
        layout->layout = _basic_layout;
        layout->priv = layout->privfree = 0;
    }
    return 0;
}

_lyt_pattern_priv_t
_create_pattern_layout_impl(_layout_t layout){
    _lyt_pattern_priv_t priv = malloc(sizeof(struct _pattern_layout_priv));
    if (priv){
        //@todo default pattern;
        layout->priv = priv;
        layout->privfree = _free_pattern_layout_impl;
        layout->layout = _pattern_layout;
        priv->pattern = 0;
    }
    return priv;
}

void
_free_pattern_layout_impl(layout_priv_t impl){
    _lyt_pattern_priv_t priv = (_lyt_pattern_priv_t)impl;
    if (priv && priv->pattern){
        lsfree(priv->pattern);
    }
}

const char*
_get_layout_pattern(layout_priv_t priv){
    _lyt_pattern_priv_t ppriv = (_lyt_pattern_priv_t)priv;
    return ppriv ? ppriv->pattern : 0;
}

void
_set_layout_pattern(layout_priv_t priv, const char* pattern){
    _lyt_pattern_priv_t ppriv = (_lyt_pattern_priv_t)priv;
    if (ppriv){
        if (ppriv->pattern){
            ppriv->pattern = lscpy(ppriv->pattern, pattern);
        } else{
            ppriv->pattern = lscreate(pattern,strlen(pattern));
        }
    }
}

_layout_t
_create_pattern_layout(struct _logger* logger,ls_t pattern){
    _layout_t layout = _create_layout(logger);
    if (layout){
        _lyt_pattern_priv_t priv = _create_pattern_layout_impl(layout);
        if (priv){
            _set_layout_pattern(priv, pattern);
        }
    }
    
    return layout;
}

_layout_t
_create_base_layout(struct _logger* logger){
    _layout_t layout = _create_layout(logger);
    if (layout){
        _create_basic_layout_impl(layout);
    }
    if (logger){
        logger->layout = layout;
    }
    return layout;
}


ls_t 
_strftime_wrap(ls_t ls, const struct tm* t, const char* fmt){
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
_dateconverse(ls_t ls, char ** fmt, struct _log_msg* msg){
    char* p = *fmt;
    char* s = NULL;
    char* e = NULL;
    char* l = NULL;
    struct tm t;
    if (localtime_r(&msg->ts.sec,&t)){
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
            _strftime_wrap(ls, &t, s);
            *l = temp;
            char us[8] = { 0 };
            sprintf(&us[0],"%6d",msg->ts.usec);
            ls = lscat(ls,us);
            //if (itoa(msg->ts.usec, &us[0], 10)){
            //    ls = lscat(ls, "000000");
            //} else{
            //    ls = lscat(ls, us);
            //}
            _strftime_wrap(ls, &t, l + 2);
        } else{
            char temp = *e;
            *e = 0;
            _strftime_wrap(ls, &t, s);
            *e = temp;
        }
        *fmt = e + 1;
    } else{
        _strftime_wrap(ls, &t, "%Y-%m-%d %H:%M:%S");
    }
    return ls;
}

ls_t 
_pattern_layout(_layout_t layout, struct _log_msg* msg){
    if (!layout)return 0;
    _lyt_pattern_priv_t ppriv = layout->priv;
    if (!ppriv) return 0;
    char*p = ppriv->pattern;
    ls_t message = lscreate(0, 1024);
    char* cmdarg = NULL;
    int cmdlen = 0;
    char cmd = 0;
    while (*p != 0){
        if ('%' == *p && *p++){
            switch (*p){
            case 'c':
                message = lscatls(message, msg->catagory);
                break;
            case 'd':
                p++;
                message = _dateconverse(message, &p, msg);
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
                message = lscatfmt(message, "%d", delta_ms(&layout->ts,&msg->ts));
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
_basic_layout(struct _layout* layout,struct _log_msg* msg){

    ls_t message = lsinitfmt("[%-6s] @%s \\> %s\n", msg->s_prior, msg->catagory, msg->msg);
    
    return message;
}
