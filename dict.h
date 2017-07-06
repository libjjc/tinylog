#ifndef LIBC_LIBC_DICT_H
#define LIBC_LIBC_DICT_H

typedef long long i64_t;
typedef unsigned long long u64_t;

typedef float f32_t;
typedef double f64_t;
typedef long double f128_t;

/**
 * dict 节点
 * key 表示节点关键字
 * val 表示节点值
 * next 表示下一个节点
 */
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

/**
 * 一个字典dict的实现，可以理解成C++template class中的typename T
 * hash 是一个hash函数指针
 * keycmp 是dict的key的比较函数
 * keydup 是dict的key的拷贝函数
 * valdup 是dict的value的拷贝函数
 * keyfree 是dict的key的释放函数
 * valfree 是dict的value的释放函数
 */
struct _dict_impl {
    int(*hash)(void*);
    int(*keycmp)(void*, void*);
    void*(*keydup)(void*);
    void*(*valdup)(void*);
    void(*keyfree)(void*);
    void(*valfree)(void*);
};

/**
 * 表示一个dict
 * impl 表示一个dict的实现类似于C++中的template<T>
 * size 表示dict的空间大小
 * used 表示已经使用的空间大小，这里的已经使用的空间大小表示整个dict的所有节点的和
 * table 表示dict的数据
 */
struct _dict {
    struct _dict_impl* impl;
    unsigned long used;
    unsigned long size;
    struct _dict_entity** table;
};

/**
 * 用来迭代dict的迭代器。
 * d 需要进行迭代的dict
 * i 当前迭代的是队列中的第几个链表
 * e 当前迭代的实体
 * next 下一个需要迭代的实体
 */
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


/**
 * 设置dict d 中 e 的key 为 k
 */
#define dt_set_key(d,e,k)\
    do{if((d)->impl->keydup){(e)->key = (d)->impl->keydup(k);}else{(e)->key = (k);}}while(0)

/**
 * 释放dict  d  中的实体 e
 */
#define dt_free_key(d,e)\
    do{ if((d)->impl->keyfree)(d)->impl->keyfree((e)->key);}while(0)

/**
 * 设置dict d 中的实体 e 的值为 v
 */
#define dt_set_val(d,e,v)\
    do{if((d)->impl->valdup){(e)->val.ptv = (d)->impl->valdup(v);}else{(e)->val.ptv = (v);}}while(0)

/**
 *  释放dict节点e
 */
#define dt_free_val(d,e)\
    do{ if((d)->impl->valfree)(d)->impl->valfree((e)->val.ptv);}while(0)

/**
 * 设置 和获取 int64 unsigned int64 double 的值
 */
#define dt_set_val_i64(e,v)do{(e)->val.i64 = v;}while(0)
#define dt_set_val_u64(e,v)do{(e)->val.u64 = v;}while(0)
#define dt_set_val_f64(e,v)do{(e)->val.f64 = v;}while(0)

#define dt_get_val_i64(e) (e)->val.i64
#define dt_get_val_u64(e) (e)->val.u64
#define dt_get_val_f64(e) (e)->val.f64

/**
 * key值比较
 */
#define dt_key_cmp(d,k1,k2)\
    ((d)->impl->keycmp ? (d)->impl->keycmp((k1),(k2)) : (k1) == (k2))

/**
 * 计算k的hash
 */
#define dt_hash_key(d,k)\
    ((d)->impl->hash(k))
    
/**
 * @brief
 *      创建一个dict
 * @param impl
 *      需要创建的dict的实现类型，相当于C++中的template<typename T>
 * @return
 *      成功则返回一个dict，否则NULL
 */
dict_t
dtcreate(dt_impl_t impl);

/**
 * @brief
 *      创建一个dict，与dtcreate不同的是这里将初始化一个大小。
 * @param impl
 *      需要创建的dict的实现类型，相当于C++中的template<typename T>
 * @param size
 *      dict 的初始大小
 * @return
 *      成功则返回一个dict，否则NULL
 */
dict_t
dtinit(dt_impl_t impl, unsigned long size);

/**
 * @brief
 *      释放dict的空间
 * @param dict
 *      需要释放的dict空间
 */
void
dtfree(dict_t dict);

/**
 * @brief
 *      创建或者设置一个key为key，且value为val的节点
 * @param dict
 *      需要创建或者设置的dict
 * @param key
 *      需要创建或者设置的节点的key
 * @param val
 *      需要创建或者设置的节点的value
 * @return
 *      成功则返回0，否则-1
 */
int
dtset(dict_t dict, void* key, void* val);

/**
 * @brief
 *      创建或者设置一个dict钟key为key的节点，并返回该节点
 * @param dict
 *      需要创建或者设置节点的dict
 * @param key
 *      需要创建或者设置节点的key
 * @return
 *      返回创建或者需要进行设置的节点，方便直接进行写入相应类型的值。
 */
dt_entity_t
dtsetraw(dict_t dict, void* key);

/**
 * @brief
 *      实际上相当于我们常规理解的set***函数。此时如果key不存在则不会创建，并返回-1.
 * @return
 *      设置成功则返回0，否则返回-1
 */
int
dtreplace(dict_t dict, void* key, void* val);

/**
 * @brief 相当于set函数对应的setraw函数
 */
dt_entity_t
dtrepraw(dict_t dict, void* key);

/**
 * @brief
 *      删除dict中key为key的节点
 */
int
dtdelete(dict_t dict, void* key);

/**
 * @brief
 *      查找dict中为key的节点并返回该节点
 * @return
 *      如果查找到则返回该节点，否则返回NULL
 */
dt_entity_t
dtfind(dict_t dict, void* key);

/**
 * @brief
 *      重新计算整个dict的hash
 * @param dict
 *      需要重新计算的dict
 * @return
 *      计算完成后为一个全新的dict
 */
dict_t
dtrehash(dict_t dict);
/**
 * 遍历dict
 * @param foreach
 *      dict 的遍历函数
 * @return
 *      成功则返回0，否则-1
 */
int
dtforeach(dict_t dict, callback_foreach foreach);

/**
 * @brief
 *      dict的迭代函数
 * @param dict
 *      需要迭代的dict
 * @return
 *      返回dict的第一个节点
 */
dt_iterator_t
dtiterator(dict_t dict);

/**
 * @brief
 *      返回下一个节点，当遍历介绍后返回NULL
 */
dt_entity_t
dtnext(dict_t dict, dt_iterator_t entity);

/**
 * @brief
 *      释放dict的迭代器
 */
void
dtfreeitr(dt_iterator_t itr);

/**
 * @brief
 *      返回dict的size
 */
unsigned long
dtsize(dict_t dict);

/**
 * @brief
 *      返回dict中的实际占用
 */
unsigned long
dtused(dict_t dict);

/**
 * @brief
 *      返回dict中的所有节点数
 * @param dict
 * @return
 */
unsigned long
dtcount(dict_t dict);

/**
 * @brief
 *      返回所有与key值hash一致的节点数。
 */
unsigned long
dtcountkey(dict_t dict, void* key);

/**
 * okovstrimp
 *      short for "owner key owner value string implement"
 *      私有的key私有的value，字符串为key的实现方式
 * oksvstrimpl
 *      short for "owner key share value string implement"
 *      私有的key共享的value，字符串为key的实现方式
 *
 * 相当于template<typename T> 中的T
 * 可以直接dtcreate(&okovstrimpl)来创建一个新的dict
 */
extern struct _dict_impl okovstrimpl;
extern struct _dict_impl oksvstrimpl;

#endif//LIBC_LIBC_DICT_H
