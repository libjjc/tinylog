#include <malloc.h>
#include <string.h>
#include "dict.h"

#define DICT_DEFAULT_SIZE 16
#define DICT_REHASH(d)do{if((d)->used > (d)->size * 5) dtrehash((d));}while(0);
dict_t
dtcreate(dt_impl_t impl){
    struct _dict* dict = calloc(1, sizeof(struct _dict));
    if (!dict) return 0;
    dict->impl = impl;
    dict->size = DICT_DEFAULT_SIZE;
    dict->used = 0;
    dict->table = calloc(1, dict->size*(sizeof(struct _dict_entity*)));
    if (!dict->table) return 0;
    return dict;
}

dict_t
dtinit(dt_impl_t impl, unsigned long size){
    struct _dict* dict = calloc(1, sizeof(struct _dict));
    if (!dict) return 0;
    dict->impl = impl;
    dict->size = size;
    dict->used = 0;
    dict->table = calloc(1, dict->size*(sizeof(struct _dict_entity*)));
    if (!dict->table) return 0;
    return dict;
}

void
dtfree(dict_t dict){
    dt_iterator_t itr = dtiterator(dict);
    dt_entity_t e = 0;
    while (e = dtnext(dict, itr)){
        dt_free_key(dict, e);
        dt_free_val(dict, e);
        free(e);
    }
    free(dict->table);
    free(dict);
}

int
dtset(dict_t dict, void* key, void* val){
    if (!dict) return -1;
    DICT_REHASH(dict);
    int hash = dt_hash_key(dict, key);
    int index = hash& (dict->size - 1);
    dt_entity_t e = dict->table[index];
    if (e){
        dt_entity_t pe = 0;
        while (e&&(dt_key_cmp(dict, key, e->key))){
            pe = e;
            e = e->next;
        }
        if (e){
            dt_set_val(dict, e, val);
        } else{
            if (!(e = malloc(sizeof(struct _dict_entity))))return -1;
            pe->next = e;
            e->next = 0;
        }
    } else{
        e = malloc(sizeof(struct _dict_entity));
        if (!e) return -1;
        e->next = 0;
        dict->table[index] = e;
    }
    dt_set_key(dict, e, key);
    dt_set_val(dict, e, val);
    dict->used++;
    return 0;
}

dt_entity_t
dtsetraw(dict_t dict, void* key){
    if (!dict) return 0;
    DICT_REHASH(dict);
    int hash = dt_hash_key(dict, key);
    int index = hash& (dict->size - 1);
    dt_entity_t entity = malloc(sizeof(struct _dict_entity));
    entity->next = 0;
    if (!entity) return 0;
    dt_entity_t e = dict->table[index];
    if (e){
        dt_entity_t pe = 0;
        while (e&&dt_key_cmp(dict, key, e->key)){
            pe = e;
            e = e->next;
        }
        if (e)return e;
        pe->next = entity;
    } else{
        dict->table[index] = entity;
    }
    dt_set_key(dict, entity, key);
    dict->used++;
    return entity;
}

int
dtreplace(dict_t dict, void* key, void* val){
    if (!dict) return -1;
    int hash = dt_hash_key(dict, key);
    int index = hash&(dict->size - 1);
    if (!dict->table[index]) return -1;
    dt_set_val(dict, dict->table[index], val);
    return 0;
}

dt_entity_t
dtrepraw(dict_t dict, void* key){
    if (!dict) return 0;
    int hash = dt_hash_key(dict, key);
    int index = hash&(dict->size - 1);
    if (!dict->table[index])return 0;
    return dict->table[index];
}

int
dtdelete(dict_t dict, void* key){
    if (!dict) return -1;
    int hash = dt_hash_key(dict, key);
    int index = hash&(dict->size - 1);
    if (!dict->table[index]) return -1;
    dt_free_key(dict, dict->table[index]);
    dt_free_val(dict, dict->table[index]);
    free(dict->table[index]);
    return 0;
}

dt_entity_t
dtfind(dict_t dict, void* key){
    if (!dict) return 0;
    DICT_REHASH(dict);
    int hash = dt_hash_key(dict, key);
    int index = hash& (dict->size - 1);
    dt_entity_t e = dict->table[index];
    if (e){
        while (e&&dt_key_cmp(dict, key, e->key)){
            e = e->next;
        }
        if (e)return e;
    }
    return 0;
}

dict_t
dtrehash(dict_t dict){
    if (dict->size > 2 * dict->used)return dict;
    unsigned long size = 2;
    while (size < 2 * dict->used)size *= 2;
    dt_entity_t* table = malloc( size*sizeof(dt_entity_t));
    if (!table) {
        dtfree(dict);
        return 0;
    }
    dt_iterator_t itr = dtiterator(dict);
    dt_entity_t e = 0;
    while (e = dtnext(dict,itr)){
        int hash = dt_hash_key(dict,e->key );
        int index = hash& (size - 1);
        dt_entity_t cursor = dict->table[index];
        if (!cursor) {
            dict->table[index] = e;
            e->next = 0;
        } else{
            while (cursor->next)cursor = cursor->next;
            cursor->next = e;
            e->next = 0;
        }
    }
    free(dict->table);
    dict->table = table;
    dict->size = size;
    return dict;
}

int
dtforeach(dict_t dict, callback_foreach foreach){
    dt_iterator_t itr = dtiterator(dict);
    dt_entity_t e = 0;
    int ret = 0;
    while (e = dtnext(dict, itr)){
        if (ret = foreach(dict, e)){
            return ret;
        }
    }
    return ret;
}

dt_iterator_t
dtiterator(dict_t dict){
    dt_iterator_t itr = malloc(sizeof(struct _dict_iterator));
    if (!itr)return 0;
    itr->d = dict;
    itr->i = 0;
    itr->e = 0;
    itr->next = 0;
    return itr;
}

dt_entity_t
dtnext(dict_t dict, dt_iterator_t itr){
    if (!itr->e && !itr->i){
        for (unsigned long i = 0; i < dict->size; i++){
            if (dict->table[i]){
                itr->e = dict->table[i];
                itr->i = i;
                itr->next = itr->e->next;
            }
        }
    }
    else if (itr->next){
        itr->e = itr->next;
        itr->next = itr->next->next;
    } else{
        for (unsigned long i = itr->i + 1; i < dict->size; i++){
            if (dict->table[i]){
                itr->e = dict->table[i];
                itr->i = i;
                itr->next = itr->e->next;
            }
        }
        itr->i = dict->size;
        itr->e = 0;
        itr->next = 0;
    }
    return itr->e;
}

void
dtfreeitr(dt_iterator_t itr){
    free(itr);
}

unsigned long
dtsize(dict_t dict){
    return dict->size;
}

unsigned long
dtused(dict_t dict){
    return dict->used;
}

unsigned long
dtcount(dict_t dict){
    unsigned long count = 0;
    for (unsigned long i = 0; i < dict->size; i++){
        dt_entity_t e = dict->table[i];
        while (e){
            e = e->next;
            count++;
        }
    }
    return count;
}

unsigned long
dtcountkey(dict_t dict, void* key){
    int count = 0;
    int hash = dt_hash_key(dict, key);
    int index = hash& (dict->size - 1);
    dt_entity_t e = dict->table[index];
    while (e){
        count++;
        e = e->next;
    }
    return count;
}

int
strRShash(void* str){
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    int len = strlen(str);
    while (len){
        hash = hash*a + (*((char*)str)++);
        a *= b;
        len--;
    }
    return hash;
}

int
strkeycmp(void* left, void* right){
    const char* s1 = (char*)left;
    const char* s2 = (char*)right;
    return strcmp(s1, s2);
}

void*
strcpydup(void* str){
    const char* s = str;
    int len = strlen(s) + 1;
    char* r = malloc(len);
    if (!r) return 0;
    strcpy_s(r, len, s);
    return r;
}

void
strcpyfree(void* str){
    free(str);
}

struct _dict_impl okovstrimpl = {
    strRShash,
    strkeycmp,
    strcpydup,
    strcpydup,
    strcpyfree,
    strcpyfree
};

struct _dict_impl oksvstrimpl = {
    strRShash,
    strkeycmp,
    strcpydup,
    0,
    strcpyfree,
    0
};
