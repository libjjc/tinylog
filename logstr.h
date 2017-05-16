#ifndef J_LOG_STR_HH
#define J_LOG_STR_HH

/**
 * @author j.j.c
 * @date 4/12/2017
 * @note
 *      1. logstr字符串是一个对C字符串进行简易包装的字符串处理函数集合。
 *      2. 每个logstr字符串都需要在使用结束后通过lsfree进行释放。
 *      3. 在处理函数的所有在传入第一个参数与返回类型参数均为ls_t的函数时，
 *         参数可以解读为self，返回值也是self。但是传入的self与返回的self
 *         地址可能不一样。因此，使用时，最好（务必）使用如下形式进行调用。
 *         exp：   ls_t s = lsfunc(s,param);
 */


typedef char* ls_t;
struct logstr{
    int len;
    int free;
    char str[];
};

/**
 * @brief
 *      用于初始化logstr，相当于创建一个logstr对象。
 * @param c
 *      用来填充logstr对象的内存空间。
 * @param len
 *      确定logstr的内存空间大小，即字符串所需的长度。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t s1 = lsint('a',5);
 *      s1 == "aaaaa";
 * @note
 *      每一个创建的logstr对象都需要通过lsfree来释放内存。
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t 
lsinit(char c, int len);


/**
 * @brief
 *      创建一个空的logstr字符串。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t emy = ls_mkempty();
 *      emy == "";
 * @note
 *      即便是空的字符串，也需要通过lsfree来释放。
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t
lsmkempty();


/**
 * @brief
 *      通过可变变量来初始化一个logstr对象。
 * @param fmt
 *      格式化字符串，与我们熟知的C函数printf格式一致。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t s1 = lsinitfmt("%d,%s",2017,"Welcome!");
 *      s1 == "2017,Welcome!";
 * @note
 *      每一个创建的logstr对象都需要通过lsfree来释放内存。
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t
lsinitfmt(char* fmt, ...);


/**
 * @brief
 *      通过已有的数据来构造logstr对象。
 * @param str
 *      用于构造logstr对象的数据地址。
 * @param len
 *      已有数据的长度。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t s1 = lscreate('Hello World!',12);
 *      s1 == "Hello World!";
 * @note
 *      每一个创建的logstr对象都需要通过lsfree来释放内存。
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t 
lscreate(const void* str,int len);


/**
 * @brief
 *      为已有的logstr对象创建额外的备用空间。
 * @param ls
 *      需要创建额外空间的字符串。
 * @param room
 *      额外需要的空间尺寸大小.
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t s1 = lsint('a',5);
 *      s1 == "aaaaa";
 *      lssize(s1) == 5;
 *      s1 = lsmkroom(s1,12);
 *      lssize(s1) == 17;
 *      lsavil(s1) == 12;
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t
lsmkroom(ls_t ls, int room);


/**
 * @brief
 *      释放logstr对象的内存空间。每一个logstr对象都需要在使用结束后进行释放。
 * @param ls
 *      需要进行内存空间释放的字符串。
 * @exzample
 *      ls_t s1 = lsint('a',5);
 *      lsfree(s1);
 * @note
 *      每一个创建的logstr对象都需要通过lsfree来释放内存。
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
void 
lsfree(ls_t ls);


/**
 * @brief
 *      用于返回logstr字符串的长度，相当于C函数strlen。
 * @param ls
 *      需要被返回长度的字符串。
 * @return
 *      ls字符串的长度。
 * @exzample
 *      ls_t s1 = lsint('a',5);
 *      5 == lslen(s1);
 * @note
 *      参见struct logstr定义
 * @see
 *      lsresize,lssize,lslen,lsavil,lsmemsize
 */
int 
lslen(const ls_t ls);


/**
 * @brief
 *      用于返回logstr字符串可用空间的长度.
 * @param ls
 *      需要被返回可用空间长度的字符串。
 * @return
 *      ls字符串的可用空间长度。
 * @exzample
 *      ls_t s1 = lsint('a',5);
 *       s1 = lsmkroom(s1,3);
 *       3 == lsavil(s1);
 * @note
 *      参见struct logstr定义
 * @see
 *      lsresize,lssize,lslen,lsavil,lsmemsize
 */
int
lsavil(const ls_t ls);


/**
 * @brief
 *      返回logstr字符串的首地址。
 * @exzample
 *      ls_t s1 = lscreate("Hello World!",12);
 *      s1 == lsbegin(s1);
 * @see
 *      lsend
 */
char*
lsbegin(const ls_t ls);


/**
 * @brief
 *      返回logstr字符串的尾部地址。
 * @see
 *      lsbegin
 */
char*
lsend(const ls_t ls);


/**
 * @brief
 *      返回logstr字符串的空间尺寸，包括字符串长度与可用空间。
 * @exzample
 *      lslen(s)+lsavil(s) == lssize(s)
 * @note
 * @see
 *      lslen,lsavil
 */
int 
lssize(const ls_t ls);


/**
 * @brief
 *      返回logstr字符串所占用的内存空间尺寸。包括字符串尺寸以及结构本身的内存。
 * @exzample
 *      lsmemsize(s) == lssize(s)+sizeof(struct logstr)+1
 * @see
 *      lslen,lsavil,lssize
 */
int 
lsmemsize(const ls_t ls);


/**
 * @brief
 *      重新申请空间并填充。
 * @param c
 *      用来填充logstr对象的内存空间。
 * @param size
 *      确定logstr的内存空间大小，即字符串所需的长度。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      ls_t s = lsmkempty();
 *      s = lsresize(s,0,5);
 *      5 == lssize(5);
 * @note
 *      ...
 */
ls_t
lsresize(ls_t ls,char c, int size);


/**
 * @brief
 *      清楚logstr字符串，这里仅仅是清空字符串，并不影响其内存空间的变动。
 */
void
lsclear(ls_t ls);

/**
 * @brief
 *      拷贝字符串，将str字符串中内容拷贝到ls中。
 * @note
 *      拷贝字符串可能会影响到原有logstr字符串的内存空间，当新字符串长度远远小于现有
 *      logstr字符串的空间时，logstr字符串空间会进行紧缩操作。
 * @see
 *      lscpyls
 */
ls_t 
lscpy(ls_t ls,const char* str);


/**
 * @brief
 *      拷贝字符串，将str字符串中内容拷贝到ls中。
 * @note
 *      1.拷贝字符串可能会影响到原有logstr字符串的内存空间，当新字符串长度远远小于现有
 *      logstr字符串的空间时，logstr字符串空间会进行紧缩操作。
 *      2.余lscpy不同的是lscpyls表示用来拷贝的源数据也是一个logstr
 * @see
 *      lscpyls
 */
ls_t
lscpyls(ls_t ls, const ls_t str);


/**
 * @brief
 *      用来比较两个字符串，当相同时返回0，与C 函数strcmp，memcmp相似。
 */
int 
lscmp(const ls_t left, const char* right);


/**
 * @brief
 *      用来比较两个字符串，当相同时返回0，与C 函数strcmp，memcmp相似。
 * @note
 *      与lscmp不同的是lscmpls表示用来拷贝的源数据也是一个logstr
 */
int 
lscmpls(const ls_t left, const ls_t right);


/**
 * @brief
 *      将str字符串连接到ls字符串尾部并返回。
 */
ls_t 
lscat(ls_t ls,const char* str);


/**
 * @brief
 *      将str字符串连接到ls字符串尾部并返回。
 * @note
 *      与lscat不同的是lscatls表示用来拷贝的源数据也是一个logstr
 */
ls_t
lscatls(ls_t ls, const ls_t str);

/**
 * @brief
 *      将str字符串连接到ls字符串尾部并返回。
 * @note
 *      与lscat不同的是lscatls表示用来拷贝的源数据的一定长度，而并非全部。
 */
ls_t
lscatlen(ls_t ls, const char* str, int len);


/**
 * @brief
 *      将ls字符串全部转换为小写字母，并返回
 */
ls_t
lslower(ls_t ls);


/**
 * @brief
 *      将ls字符串全部转换为大写字母，并返回
 */
ls_t
lsupper(ls_t ls);


/**
 * @brief
 *      消除ls字符串的收尾空字符，并返回
 */
ls_t
lstrim(ls_t ls);


/**
 * @brief
 *      通过可变变量来连接一个字符串。
 * @param fmt
 *      格式化字符串，与我们熟知的C函数printf格式一致。
 * @return
 *      返回logstr对象的数据（字符串）空间地址。
 * @exzample
 *      s1 == "Here we come";
 *      ls_t s1 = lscatfmt(s1 ,"%d,%s",2017,"Welcome!");
 *      s1 == "Here we come2017,Welcome!";
 * @note
 *      
 * @see
 *      lsinit,lsmkempty,lscreate,lsinitfmt,lsfree
 */
ls_t 
lscatfmt(ls_t ls, char* fmt, ...);


/**
 * @brief
 *      将ls字符串重新进行格式化。
 * @exzample
 * @note
 */
ls_t 
lsformat(ls_t ls,char* fmt, ...);


/**
 * @brief
 *      字符串的可视化处理，由snprintf.c实现。
 * @param size
 *      必须传入一个参数，且不能为NULL，当传入一个size之后，首先会由该size进行试探性的格式化，
 *      如果空间不够，则将size进行翻倍。如果足够，则返回申请的指针地址。
 * @return
 *      返回格式化之后的字符串的地址
 */
char*
_ls_va_buffer( int* size,char* fmt, char* args);

#endif//J_LOG_STR_HH