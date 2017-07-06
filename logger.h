/**
 * @author
 *      j.j.chen
 * @date
 *      2017-3-5
 * @note
 *      logger为日志输出器，实现了日志的最终流向，写入控制台或者写入磁盘或其他存储设备。
 *      当前实现的日志输出方式有filelogger，rollingfilelogger，consolelogger。
 *      filelogger 每次程序启动时，将清空日志文件，因此filelogger产生的日志为临时的
 *      日志信息，仅仅保存最近一次启动程序至程序结束时的日志输出。
 *      rollingfilelogger 是分卷文件存储，经过该日志输出器的日志信息将持久化保存在文件
 *      中，当一个文件到达文件大小阀值时，会生成下一个文件，且将文件名+1.rollingfilelogger
 *      当保存的文件个数到达设定的阀值时，会删除最早的日志文件，重新生成一个最近的文件。并且
 *      rollingfilelogger产生的日志文件中，最后的编号越大表示越早的日志文件。
 *      consolelogger将日志信息输出至console
 */
#ifndef LOG_loggerHH
#define LOG_loggerHH

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "logstr.h"
#include "layout.h"
#include "logdef.h"

#define DEFAULT_LOGFILE_MAXSIZE 50
#define DEFAULT_LOGFILE_RSIZE 30
#define DEFAULT_LOGFILE_RCOUNT 20

/**
 * logfile 表示输出文件的文件名
 * fd 表示输出文件的文件句柄
 * flag 文件的打开标志
 * mode 文件的打开模式
 * size 日志文件的当前大小
 * maxsize 日志文件的最大值阀值
 */
struct _file_priv {
    ls_t logfile;
    int fd;
    int flag;
    int mode;
    int size;
    int maxsize;
};

/**
 * fp 表示继承file_priv的特性
 * rmax 表示最大回滚文件个数阀值
 * rindex 当前的日志文件索引
 * extw 表示按照当前的最大值个数来计算日志输出文件的索引宽度。比如rlog.log99，则表示2位宽度。
 * 在两位宽度下，索引位1的日志文件也需要保存为rlog.log01.
 */
struct _rfile_priv {
    struct _file_priv fp;
    int rmax;
    int rindex;
    int extw;
};

/**
 * 暂未实现，目标实现为按照日期保存的日志文件
 */
struct _dfile_priv {
    struct _file_priv fp;
    int maxdays;
    ls_t fmtday;
};

struct _console_priv {
    FILE* stream;
};

typedef struct _file_priv* _file_priv_t;
typedef struct _rfile_priv* _rfile_priv_t;
typedef struct _dfile_priv* _dfile_priv_t;
typedef struct _console_priv* _console_priv_t;

struct _catagory;
struct _logger;
typedef struct _catagory* logger_owner_type;

typedef void* logger_priv_t;
typedef void* _self_t;
typedef int(*_logger_logging)(struct _logger* self,struct _log_msg*);
typedef void(*logger_priv_free)(logger_priv_t priv);
typedef int(*logger_reopen)(struct _logger* logger);

enum _logger_type{
    UNKOWNTYPE,
    FILELOGGER,
    ROLLINGFILELOGGER,
    CONSOLELOGGER
};

/**
 * name 日志输出器的名称
 * logging 日志输出函数
 * owner 表示日志输出器所属的分类
 * layout 表示日志输出器的布局器
 * reopen 表示日志输出器的设备打开函数
 * privfree 表示日志输出器的具体实现的释放函数
 * priv 表示日志输出器的具体实现方式
 * type 表示当前的日志输出器的类型
 */
struct _logger {
    ls_t name;
    logger_owner_type owner;
    _logger_logging logging;
    _layout_t layout;
    logger_reopen reopen;
    logger_priv_free privfree;
    logger_priv_t priv;
    int type;
};
typedef struct _logger* _logger_t;

/**
 * @brief
 *      创建一个空的日志输出器
 * @return
 *      成功则返回创建的日志输出器，否则返回NULL
 */
_logger_t
_create_logger();

/**
 * 释放日志输出器的内存空间
 */
void
_free_logger(_logger_t logger);

/**
 * @brief
 *      打开/重新打开日志输出的设备（文件或者控制台等）
 * @return
 *      成功则返回0，否则-1
 */
int
_reopen_logger(_logger_t logger);

/**
 * @brief
 *      获取日志输出器的名称
 */
const ls_t
_get_logger_name(_logger_t logger);

/**
 * @brief
 *      获取日志输出器的所属分类
 */
logger_owner_type
_get_logger_owner(_logger_t logger);

void
_set_logger_name(_logger_t logger,const char* name);

void
_set_logger_owner(_logger_t logger,logger_owner_type owner);

/**
 * @brief
 *      为logger设置一个layout
 */
void
_set_logger_layout(_logger_t logger, _layout_t layout);




/**
 * @brief
 *      获取logger的实现方式指针。
 */
logger_priv_t
_get_logger_priv(_logger_t logger);




/**
 * @brief
 *      创建一个文件日志输出器的实现方式
 */
_file_priv_t
_create_file_priv(_logger_t logger);

/**
 * @brief
 *      文件日志输出器的实现方式
 * @param priv
 */
void
_free_file_priv(logger_priv_t priv);

/**
 * @brief
 *      设置文件日志输出器的日志输出文件路径
 * @param priv
 *      需要设置的日志输出路径的日志输出器
 * @param path
 *      日志文件输出路径
 */
void
_set_file_path(_file_priv_t priv,const char* path);

/**
 * @brief
 *      获取文件日志输出器的日志输出路径
 */
const char*
_get_file_path(_file_priv_t priv);

/**
 * @brief
 *      设置或者获取文件日志输出器的文件最大值
 */
void
_set_file_maxsize(_file_priv_t priv, int size);

int
_get_file_maxsize(_file_priv_t priv);

/**
 * @brief
 *      打开或者重新打开日志输出设备
 */
int
_file_logger_reopen(_logger_t logger);

/**
 * @brief
 *      向日志输出设备中写入日志信息
 * @param logger
 *      日志设备输出器
 * @param msg
 *      需要写入的日志信息
 * @return
 *      成功返回0，否则-1
 */
int
_file_logger_logging(_logger_t logger,struct _log_msg* msg);



/**
 * @brief
 *      创建一个分卷文件日志输出器的实现方式（私有数据）
 */
_rfile_priv_t
_create_rfile_priv(_logger_t logger);

void
_free_rfile_priv(logger_priv_free priv);

_file_priv_t
_get_rfile_file_priv(_rfile_priv_t priv);

/**
 * @brief
 *      设置或者获取分卷文件的最大个数
 */
void
_set_rfile_rmax(_rfile_priv_t priv,int max);

int
_get_rfile_rmax(_rfile_priv_t priv);

int
_rfile_logger_reopen(_logger_t logger);

int
_rfile_logger_logging(_logger_t logger,struct _log_msg* msg);




_console_priv_t
_create_console_priv(_logger_t logger);

void
_free_console_priv(logger_priv_t priv);

void
_set_console_stream(_console_priv_t priv, FILE* stream);

FILE*
_get_console_stream(_console_priv_t priv);

int
_console_logger_reopen(_logger_t priv);

int
_console_logger_logging(_logger_t priv,struct _log_msg* msg);




/**
 * @brief
 *      创建一个文件日志输出器
 * @param name
 *      该文件日志输出器的名称
 * @param owner
 *      该日志输出器的所属分类
 * @param logfile
 *      文件日志输出器的输出文件路径
 * @return
 *      成功则返回该日志输出器，否则，返回NULL
 */
_logger_t
_create_file_logger(const char* name, logger_owner_type owner,
                    const char* logfile);

/**
 * @brief
 *      创建一个分卷文件日志输出器
 * @param name
 *      输出器的名称
 * @param owner
 *      输出器所在的分类
 * @param logfile
 *      分卷文件日至输出器的输出文件路径
 * @return
 *      创建成功则返回输出器，否则NULL
 */
_logger_t
_create_rfile_logger( const char* name,logger_owner_type owner, 
                      const char* logfile);

/**
 * @brief
 *      创建一个console日志输出器，在log4cpp中称为StreamAppder.
 * @param name
 *      输出器的名称
 * @param owner
 *      输出器所在分类
 * @param stream
 *      这里的stream实际只支持stdout stdin stderr
 * @return
 *      成功则返回创建的logger，否则NULL
 */
_logger_t
_create_console_logger(const char* name,logger_owner_type owner,
                       FILE* stream);

#endif//LOG_loggerHH
