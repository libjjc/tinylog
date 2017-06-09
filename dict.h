#ifndef LIBC_LIBC_DICT_H
#define LIBC_LIBC_DICT_H

typedef long long i64_t;
typedef unsigned long long u64_t;

typedef float f32_t;
typedef double f64_t;
typedef long double f128_t;

struct _dict_entity {
    void* key;
    union _dict_val {
        void* ptv;
        i64_t i64;
        u64_t u64;
        f64_t f64;
    };
    union _dict_val val;
    struct _dict_entity* next;
};

struct _dict_impl {
    int(*hash)(void*);
    int(*keycmp)(void*, void*);
    void*(*keydup)(void*);
    void*(*valdup)(void*);
    void(*keyfree)(void*);
    void(*valfree)(void*);
};

struct _dict {
    struct _dict_impl* impl;
    unsigned long used;
    unsigned long size;
    struct _dict_entity** table;
};

struct _dict_iterator {
    struct _dict * d;
    unsigned long i;
    struct _dict_entity* e;
    struct _dict_entity* next;
};

typedef struct _dict_entity* dt_entity_t;
typedef struct _dict_impl* dt_impl_t;
typedef struct _dict* dict_t;
typedef struct _dict_iterator* dt_iterator_t;

typedef int(*callback_foreach)(dict_t dict, dt_entity_t);


#define dt_set_key(d,e,k)\
    do{if((d)->impl->keydup){(e)->key = (d)->impl->keydup(k);}else{(e)->key = (k);}}while(0)

#define dt_free_key(d,e)\
    do{ if((d)->impl->keyfree)(d)->impl->keyfree((e)->key);}while(0)

#define dt_set_val(d,e,v)\
    do{if((d)->impl->valdup){(e)->val.ptv = (d)->impl->valdup(v);}else{(e)->val.ptv = (v);}}while(0)

#define dt_free_val(d,e)\
    do{ if((d)->impl->valfree)(d)->impl->valfree((e)->val.ptv);}while(0)

#define dt_set_val_i64(e,v)do{(e)->val.i64 = v;}while(0)
#define dt_set_val_u64(e,v)do{(e)->val.u64 = v;}while(0)
#define dt_set_val_f64(e,v)do{(e)->val.f64 = v;}while(0)

#define dt_get_val_i64(e) (e)->val.i64
#define dt_get_val_u64(e) (e)->val.u64
#define dt_get_val_f64(e) (e)->val.f64

#define dt_key_cmp(d,k1,k2)\
    ((d)->impl->keycmp ? (d)->impl->keycmp((k1),(k2)) : (k1) == (k2))

#define dt_hash_key(d,k)\
    ((d)->impl->hash(k))
    

dict_t
dtcreate(dt_impl_t impl);

dict_t
dtinit(dt_impl_t impl, unsigned long size);

void
dtfree(dict_t dict);

int
dtset(dict_t dict, void* key, void* val);

dt_entity_t
dtsetraw(dict_t dict, void* key);

int
dtreplace(dict_t dict, void* key, void* val);

dt_entity_t
dtrepraw(dict_t dict, void* key);

int
dtdelete(dict_t dict, void* key);

dt_entity_t
dtfind(dict_t dict, void* key);

dict_t
dtrehash(dict_t dict);

int
dtforeach(dict_t dict, callback_foreach foreach);

dt_iterator_t
dtiterator(dict_t dict);

dt_entity_t
dtnext(dict_t dict, dt_iterator_t entity);

void
dtfreeitr(dt_iterator_t itr);

unsigned long
dtsize(dict_t dict);

unsigned long
dtused(dict_t dict);

unsigned long
dtcount(dict_t dict);

unsigned long
dtcountkey(dict_t dict, void* key);

extern struct _dict_impl okovstrimpl;
extern struct _dict_impl oksvstrimpl;

#endif//LIBC_LIBC_DICT_H
