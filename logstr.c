#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <vadefs.h>
#include "logstr.h"
#if defined(_MSC_VER)
#define VSNPRINTF _vsnprintf
#if _MSC_VER > 1900
#define _crt_va_start __crt_va_start_a
#define _crt_va_end __crt_va_end
#endif
#else
#ifdef TINYLOG_HAVE_SNPRINTF
#define VSNPRINTF vsnprintf
#else
/* use alternative snprintf() from http://www.ijs.si/software/snprintf/ */

#define HAVE_SNPRINTF
#define PREFER_PORTABLE_SNPRINTF
#include <stdlib.h>
#include <stdarg.h>
#include "snprintf.c"

#define VSNPRINTF portable_vsnprintf

#endif // TINYLOG_HAVE_SNPRINTF
#endif // _MSC_VER

#ifdef _LSP
#undef _LSP
#else
#define _LSP(s) ((struct _log_str*)(s-sizeof(struct _log_str)))
#endif//_LSP

ls_t 
lsinit(char c, int len){
    struct _log_str* s = malloc(sizeof(struct _log_str)+len+1);
    if (!s) return 0;
    memset(&s->str[0], c, len);
    s->str[len] = 0;
    s->len = len;
    s->free = 0;
    return &s->str[0];
}

ls_t
lsinitcpy(const char* str){
    if (!str) return NULL;
    int len = strlen(str);
    struct _log_str* s = (struct _log_str*)malloc(sizeof(struct _log_str)+len+1);
    if (!s) return 0;
    memcpy_s(&s->str[0],len,str, len);
    s->str[len] = 0;
    s->len = len;
    s->free = 0;
    return &s->str[0];
}

ls_t
lsinitcpyls(const ls_t ls){
    if (!ls) return NULL;
    struct _log_str* s = (struct _log_str*)malloc(lsmemsize(ls));
    if (!s) return 0;
    memcpy_s(&s->str[0],lssize(ls),ls, lslen(ls));
    s->str[lslen(ls)] = 0;
    s->len = lslen(ls);
    s->free = lsavil(ls);
    return &s->str[0];
}

ls_t
lscpyfmt(ls_t ls,char* fmt, ...){
    int size = 0;
    va_list args;
	_crt_va_start(args, fmt);
    char* buffer = _ls_va_buffer(&size, fmt, args);
    if (!buffer) return ls;
    return lscatlen(ls, buffer, size);
}

ls_t 
lsmkempty(){
    struct _log_str* s = malloc(sizeof(struct _log_str) + 1);
    s->len = s->free = 0;
    s->str[s->len] = 0;
    return &s->str[0];
}

ls_t lsrep(ls_t ls, const char* str , const char* _rep){
    if (!ls || !str) return ls;
    int slen = strlen(str), pos = 0,rlen = strlen(_rep);
    while (-1 != (pos = lsfind(ls, str, pos))){
        if (!(ls = lsmkroom(ls, slen))) return ls;
        if (memmove_s(&ls[pos + rlen], lssize(ls) - pos - rlen, &ls[pos], lslen(ls) - pos - slen)){
            return ls;
        }
        if (memcpy_s(&ls[pos], rlen, _rep, rlen)){
            return ls;
        }
        _LSP(ls)->len += rlen - slen;
        _LSP(ls)->free -= rlen - slen;
        *lsend(ls) = 0;
        pos += rlen;
    }
    return ls;
}

ls_t
lsrepls(ls_t ls, const ls_t str , const ls_t rep){
    if (!ls || !str) return ls;
    int slen = lslen(str), pos = 0,rlen = lslen(rep);
    while (-1 != (pos = lsfind(ls, str, pos))){
        if (!(ls = lsmkroom(ls, slen))) return ls;
        if (memmove_s(&ls[pos + rlen], lssize(ls) - pos - rlen, &ls[pos], lslen(ls) - pos - slen)){
            return ls;
        }
        if (memcpy_s(&ls[pos], rlen, rep, rlen)){
            return ls;
        }
        _LSP(ls)->len += rlen - slen;
        _LSP(ls)->free -= rlen - slen;
        *lsend(ls) = 0;
        pos += rlen;
    }
    return ls;
}

ls_t 
lsinitfmt(char* fmt, ...){
    int size = 0;
    va_list args;
	_crt_va_start(args, fmt);
    char* buffer = _ls_va_buffer(&size, fmt, args);
    if (!buffer) return NULL;
    struct _log_str* s = (struct _log_str*)malloc(sizeof(struct _log_str) + size + 1);
    if (!s) {
        free(buffer);
        return NULL;
    }
    memcpy_s(&s->str[0], size, buffer, size);
    s->free = 0;
    s->len = size;
    s->str[size] = 0;
    free(buffer);
    return &s->str[0];
}

ls_t 
lscreate(const void* str, int len){
    struct _log_str* s;
    if (str){
        s = (struct _log_str*)malloc(sizeof(struct _log_str) + len + 1);
    }else{
        s = (struct _log_str*)calloc(1, sizeof(struct _log_str) + len + 1);
    }
    if (str && len){
        memcpy_s(&s->str[0],len, str, len);
    }
    s->str[str ? len : 0] = 0;
    s->len = str ? len : 0;
    s->free = str ? 0 : len;
    return &s->str[0];
}

ls_t 
lsmkroom(ls_t ls, int room){
    if (lsavil(ls) >= room)return ls;
    struct _log_str* s;
    s = (struct _log_str*)realloc(_LSP(ls), lslen(ls)+room+ 1);
    s->free += room;
    return &s->str[0];
}

void 
lsfree(ls_t ls){
    if (!ls)return;
    struct _log_str* s = (struct _log_str*)(ls - sizeof(struct _log_str));
    free(s);
}


int 
lslen(const ls_t ls){
    return _LSP(ls)->len;
}

int 
lsavil(const ls_t ls){
    if (!ls) return 0;
    return _LSP(ls)->free;
}

char* 
lsbegin(const ls_t ls){
    return &_LSP(ls)->str[0];
}

char* 
lsend(const ls_t ls){
    return &_LSP(ls)->str[_LSP(ls)->len];
}

int 
lssize(const ls_t ls){
    if (!ls) return 0;
    return lslen(ls) + lsavil(ls);
}

int 
lsmemsize(const ls_t ls){
    return sizeof(struct _log_str) + lssize(ls) + 1;
}

ls_t 
lsresize(ls_t ls, char c, int size){
    struct _log_str* s = _LSP(ls);
    if (lssize(ls) < size || lssize(ls) / 2 > size){
        s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str) + size);
        if (!s) return NULL;
    }
    memset(&s->str[0], c, size);
    return &s->str[0];
}

void 
lsclear(ls_t ls){
    memset(lsbegin(ls), 0, lssize(ls));
    _LSP(ls)->free = lssize(ls);
    _LSP(ls)->len = 0;
}

ls_t 
lscpy(ls_t ls, const char* str){
if (!ls) return 0;
    int new_len = str ? strlen(str): 0;
    int size = lssize(ls);
    struct _log_str* s = _LSP(ls);
    if (size < new_len){
        s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str) + new_len + 1);
        size = new_len;
    }
    if (!s) return NULL;
    if (memcpy_s(&s->str[0], size, str, new_len)) return NULL;
    s->len = new_len;
    s->free = size < new_len? 0: size-new_len;
    s->str[new_len] = 0;
    return &s->str[0];
}

ls_t 
lscpyls(ls_t ls, const ls_t str){
    if (!ls) return 0;
    int new_len = str ? lslen(str) : 0;
    int size = lssize(ls);
    struct _log_str* s = _LSP(ls);
    if (size < new_len){
        s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str) + new_len + 1);
        size = new_len;
    }
    if (!s) return NULL;
    if (memcpy_s(&s->str[0], size, str, new_len)) return NULL;
    s->len = new_len;
    s->free = size < new_len? 0: size-new_len;
    s->str[new_len] = 0;
    return &s->str[0];
}

int
lsfind(ls_t ls, const char* str, int offset){
    if (!ls || !str) return -1;
    if (offset >= lslen(ls)) return -1;
    int slen = strlen(str);
    if (slen + offset > lslen(ls)) return -1;
    for (int i = offset + slen; i < lslen(ls); i++){
        while (slen-- > 0){
            if (ls[i + slen] != str[slen]) break;
        }
        if (0 == slen) return i;
    }
    return -1;
}

int 
lsfindls(ls_t ls, const ls_t str, int offset){
    if (!ls || !str) return -1;
    if (offset >= lslen(ls)) return -1;
    int slen = lslen(str);
    if (slen + offset > lslen(ls)) return -1;
    for (int i = offset + slen; i < lslen(ls); i++){
        while (slen-- >= 0){
            if (ls[i + slen] != str[slen]) break;
            if (0 == slen) return i;
        }
    }
    return -1;
}

ls_t
lssubls(const ls_t ls, int begin, int end){
    if (end < begin){
        return NULL;
    }
    int s = begin > lslen(ls) ? lslen(ls) : begin;
    int e = end > lslen(ls) ? lslen(ls) : end;
    struct _log_str* str = malloc(sizeof(struct _log_str) + end - begin + 1);
    if (!str)return NULL;
    memcpy_s(&str->str[0], e - s, &ls[s], e - s);
    str->len = e - s;
    str->free = 0;
    str->str[str->len] = 0;
    return &str->str[0];
}


ls_t *
lssplit(const ls_t ls, const char* sep, ls_t* lss, int* size){
    const char* p = ls;
    const char* s = ls;
    if (!size) return NULL;
    int szls = 0;
    while (*p){
        if (strchr(sep, *p)){
            if (p != s && p - s > 1){
                szls++;
                if (szls >= *size){
                    *size *= 2;
                    ls_t* ss = NULL;
                    if (lss){
                        ss = (ls_t*)realloc(lss, sizeof(ls_t)*(*size));
                    } else{
                        ss = (ls_t*)malloc(sizeof(ls_t)*(*size));
                    }
                    if (!ss){
                        while (*size-- > 0) lsfree(lss[*size]);
                        free(lss);
                        return NULL;
                    }
                    lss = ss;
                }
                if (!(lss[szls] = lscreate(s, p - s))){
                    while (*size-- > 0) lsfree(lss[*size]);
                    free(lss);
                    return NULL;
                }
            }
            s = p;
        }
        p++;
    }
    *size = szls;
    return lss;
}


int 
lscmp(const ls_t left, const char* right){
    if (!left) return right ? -1 : 0;
    if (!right) return left ? 1 : 0;
    int ret,ml,ll = lslen(left), lr = strlen(right);
    ml = ll > lr ? lr : ll;
    ret = memcmp(left, right, ml);
    if (!ret) return ll - lr;
    return ret;
}

int 
lscmpls(const ls_t left, const ls_t right){
    if (!left) return right ? -1 : 0;
    if (!right) return left ? 1 : 0;
    int ret, ml, ll = lslen(left), lr = lslen(right);
    ml = ll > lr ? lr : ll;
    ret = memcmp(left, right, ml);
    if (!ret) return ll - lr;
    return ret;
}

ls_t 
lscat(ls_t ls, const char* str){
    if (!ls) return NULL;
    if (!str) return lsbegin(ls);
    int slen = strlen(str);
    if (lsavil(ls) >= slen){
        if (memcpy_s(lsend(ls), lsavil(ls), str, slen)) return NULL;
        _LSP(ls)->len += slen;
        _LSP(ls)->free -= slen;
        *lsend(ls) = 0;
        return &_LSP(ls)->str[0];
    }
    struct _log_str* s;
    s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str)+lssize(ls) + slen);
    if (!s) return NULL;
    if (memcpy_s(&s->str[s->len], slen, str, slen)) return NULL;
    s->len += slen, s->free = 0;
    s->str[s->len] = 0;
    return &s->str[0];
}

ls_t 
lscatls(ls_t ls, const ls_t str){
    if (!ls) return NULL;
    if (!str) return lsbegin(ls);
    int slen = lslen(str);
    if (lsavil(ls) >= slen){
        if (memcpy_s(lsend(ls), lsavil(ls), str, slen)) return NULL;
        _LSP(ls)->len += slen;
        _LSP(ls)->free -= slen;
        *lsend(ls) = 0;
        return lsbegin(ls);
    }
    struct _log_str* s;
    s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str)+lssize(ls) + slen+1);
    if (!s) return NULL;
    if (memcpy_s(&s->str[s->len], slen, str, slen)) return NULL;
    s->len += slen, s->free = 0;
    s->str[s->len] = 0;
    return s->str;
}

ls_t
lscatlen(ls_t ls, const char* str, int len){
    if (!ls) return NULL;
    if (!str) return lsbegin(ls);
    int slen = len;
    if (lsavil(ls) >= slen){
        if (memcpy_s(lsend(ls), lsavil(ls), str, slen)) return NULL;
        _LSP(ls)->len += slen;
        _LSP(ls)->free -= slen;
        *lsend(ls) = 0;
        return lsbegin(ls);
    }
    struct _log_str* s;
    s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str)+lssize(ls) + slen+1);
    if (!s) return NULL;
    if (memcpy_s(&s->str[s->len], slen, str, slen)) return NULL;
    s->len += slen, s->free = 0;
    s->str[s->len] = 0;
    return s->str;
}

ls_t 
lslower(ls_t ls){
    char* p = lsbegin(ls), *q = lsend(ls);
    for (; p != q; p++)
        *p = tolower(*p);
    return ls;
}

ls_t 
lsupper(ls_t ls){
    char* p = lsbegin(ls), *q = lsend(ls);
    for (; p != q; p++)
        *p = toupper(*p);
    return ls;
}

ls_t 
lstrim(ls_t ls){
    char* p = lsbegin(ls);
    char* q = lsend(ls)-1;
    for (; p <= q ;){
        if (*p != ' ' && *q != ' ') break;
        if (*p == ' ') p++;
        if (*q == ' '&&p != q) q--;
    }
    int len = p > q ? 0 : q - p + 1;
    memmove_s(lsbegin(ls), lssize(ls), p, len);
    _LSP(ls)->free += lssize(ls) - len;
    _LSP(ls)->len = len;
    *lsend(ls) = 0;
    return lsbegin(ls);
}

ls_t 
lscatfmt(ls_t ls, char* fmt, ...){
    int size = 0;
    va_list args;
    _crt_va_start(args, fmt);
    char* buffer = _ls_va_buffer(&size, fmt, args);
    if (!buffer) return NULL;
    struct _log_str* s = _LSP(ls);
    if (lsavil(ls) < size){
        if (!(s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str) + lssize(ls) + size + 1))){
            free(buffer);
            return 0;
        }
    }
    if (memcpy_s(&s->str[s->len],size , buffer, size)) {
        free(buffer);
        return 0;
    }
    free(buffer);
    s->len += size;
    s->str[s->len] = 0;
    return &s->str[0];
}
       

ls_t 
lsformat(ls_t ls,char* fmt, ...){
    if (!ls)return NULL;
    int size = 0;
    va_list args;
    _crt_va_start(args, fmt);
    char* buffer = _ls_va_buffer(&size, fmt, args);
    if (!buffer)return NULL;
    struct _log_str* s = _LSP(ls);
    if (lslen(ls) + lsavil(ls) < size){
        struct _log_str* s;
        if (NULL == (s = (struct _log_str*)realloc(_LSP(ls), sizeof(struct _log_str)+size))){
            free(buffer);
            return NULL;
        }
    }
    if (memcpy_s(&s->str[0], size, buffer, size)){
        free(buffer);
        return NULL;
    }
    s->free = lslen(ls) + lsavil(ls) - size;
    s->len = size;
    return &s->str[0];
}

char* 
_ls_va_buffer(int* size, char* fmt, va_list args){
    if (!fmt||!size) return NULL;
    *size = *size ? *size : 512;
    char* buf = (char*)malloc(sizeof(char)*(*size));

    while (1) {
        va_list args_copy;

#if defined(_MSC_VER) || defined(__BORLANDC__)
        args_copy = args;
#else
        va_copy(args_copy, args);
#endif

        int n = VSNPRINTF(buf, *size, fmt, args_copy);

        _crt_va_end(args_copy);

        if ((n > -1) && (n < *size)) {
            *size = n;
            break;
        }

        *size = (n > -1) ? n + 1 : *size * 2;

        free(buf);
        buf = (ls_t)malloc(sizeof(char)*(*size));
    }
    return buf;
}


