#ifndef LOG_CATAGORY_HH
#define LOG_CATAGORY_HH

#include <stdbool.h>
#include "logdef.h"
#include "logstr.h"

/**
 * 每个分类器允许最大的子分类器数量
 */
#define MAX_CATAGORY_CHILDREN 16
/**
 * 每个分类中允许的最大日志输出器数量
 */
#define MAX_CATAGORY_lOGGERS 16

struct _logger;
struct _log_msg;

/**
 * 日志分类结构体
 * priority 分类器级别
 * countChildren 当前分类器下子分类器数量
 * countLoggers 当前分类下日志输出器数量
 * name 分类器名称
 * parent 父分类器，root节点parent为NULL
 * children 子分类器
 * loggers 日志输出器
 * eoc 结束标志，也许会去掉。
 */
struct _catagory {
    int priority;
    int countLoggers;
    int countChildren;
    ls_t name;
    struct _catagory* parent;
    struct _catagory* children[MAX_CATAGORY_CHILDREN];
    struct _logger* loggers[MAX_CATAGORY_lOGGERS];
    char eoc[];
};

/**
 * 过滤函数，将不满足要求的日志消息忽略
 * _bigger_filter _smaller_filter _equal_filter 分别为大于，小于，等于分类器优先级
 */
typedef bool(*filter)(int priorityMsg, int priorityAda);

bool 
_bigger_filter(int priorityMsg, int priorityAda);

bool 
_smaller_filter(int priorityMsg, int priorityAda);

bool 
_equal_filter(int priorityMsg, int priorityAda);

/**
 * @brief
 *      创建一个分类器
 * @param parent
 *      指定父分类节点
 * @param priority
 *      需要创建的分类器的优先级
 * @param name
 *      需要创建的分类器的名称
 * @return
 *      创建的分类器
 * @note
 *      name 不能重复，如果已经创建了一个name的Catagory，再次创建则会失败
 * @see
 *      _create_null_catagory
 */
struct _catagory*
_create_catagory(struct _catagory* parent,int priority,const char* name);

/**
 * @brief
 *      创建一个空的分类器
 * @param name
 *      指定空分类器的名字
 * @return
 *      返回分类器
 * @see
 * _create_catagory
 */
struct _catagory*
_create_null_catagory(const char* name);

/**
 * @brief
 *      获取或者创建一个分类器，指定在某一分类器下面，如果存在name分类器，则返回
 *      该分类器；如果不存在，则创建一个名为name的分类器并返回。
 * @param parent
 *      指定的分类器
 * @param name
 *      指定的名称
 * @return
 *      获取或者创建的分类器
 * @note
 *      该函数有极大概率会被抛弃，该函数会导致不可避免的内存泄漏或者野指针问题。
 */
struct _catagory*
_get_create_catagory(const struct _catagory* parent,const char* name);

/**
 * @brief
 *      释放掉分类器空间
 * @param cg
 *      需要释放的catagory
 */
void
_free_catagory(struct _catagory* cg);

/**
 * @brief
 *      在指定的分类器下面添加一个分类器
 * @param parent
 *      指定的父分类器
 * @param child
 *      需要添加的子分类器
 * @return
 *      添加成功则返回0，否则返回-1
 */
int
_add_catagory(struct _catagory* parent, struct _catagory* child);

/**
 * @brief
 *      从指定的分类器中删除一个子分类器
 * @see
 *      _add_catagory
 */
int
_remove_catagory(struct _catagory* parent, struct _catagory* child);

/**
 * @brief
 *      在指定的分类器下面查找名为name的子或者孙分类器
 * @return
 *      返回查找的分类器，没有找到则返回NULL
 */
struct _catagory*
_find_catagory(struct _catagory* parent, const char* name);

/**
 * @brief
 *      添加一个日志输出器
 * @param cg
 *      需要添加分类
 * @param ada
 *      被添加的日志输出器
 * @return
 *      添加成功则返回0，否则返回-1
 * @note
 *      日志输出器的名字也不能相同
 * @deprecated
 *      后续将所有的名字都不能相同。
 */
int
_add_logger(struct _catagory* cg, struct _logger* ada);

/**
 * @brief
 *      从指定的分类器中删除指定的日志输出器
 * @return
 *      成功则返回0，否则返回-1
 */
int
_remove_logger(struct _catagory* cg, struct _logger* ada);

/**
 * @brief
 *      从指定的分类器中替换指定的日志输出器
 * @param cg
 *      指定的分类器
 * @param old
 *      被替换的日志器
 * @param logger
 *      用来替换原有日志器的新日志器
 * @return
 *      成功则返回0，否则返回-1
 */
int
_replace_logger(struct _catagory* cg, struct _logger* old,struct _logger* logger);

/**
 * @brief
 *      查找是否在指定的分类器下面存在指定的日志输出器
 * @return
 *      存在则返回true，否则false
 */
bool
_has_logger(struct _catagory* cq, struct _logger* ada);

/**
 * @brief
 *      递归查找是否含有指定的日志输出器
 * @return
 *      查找到则返回true，否则false
 * @deprecated
 *      该函数很可能会被取消，无意义的存在
 */
bool
_has_logger_recursive(struct _catagory* cq, struct _logger* logger);

/**
 * @brief
 *      在指定的分类器中查找指定名称的日志输出器
 * @param cq
 *      被查找的分类器
 * @param loggername
 *      指定的日志输出器名称
 * @return
 *      返回查找结果，没有找到则返回NULL
 */
struct _logger*
_find_logger(struct _catagory* cq, const char* loggername);

/**
 * @brief
 *      分类器日志输出日志函数
 * @param cata
 *      需要处理日志的分类器
 * @param msg
 *      需要分类器处理的日志消息
 * @return
 *      成功返回0，否则-1
 */
int
_catagory_logging(struct _catagory* cata, struct _log_msg* msg);


#endif//LOG_CATAGORY_HH
