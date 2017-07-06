#include "logger.h"
#include "layout.h"
#include "tinylog.h"
#include "catagory.h"
#include "logmsg.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>



#define _logger_addr(logger)\
    ((struct _logger*)(&(logger) - (int)(&((struct _logger*)(NULL))->log)))
#define _logger_priv(logger_priv_type,logger)\
    ((logger_priv_type)(_logger_addr(logger)->priv))
#define _logger_prop_free(logger,prop,fucfree)\
    if(logger&&_logger_addr(logger)->prop){\
        fucfree(_logger_addr(logger)->prop);\
    }
#define _logger_from_priv(priv)\
    ((struct _logger*)(&(priv)-(int)(&((struct _logger*)(NULL))->priv)))

long
_filelength(int fd){
    struct stat filestat;
    if(-1 == fstat(fd,&filestat)){
        return -1;
    }
    return filestat.st_size;
}
_logger_t
_create_logger(){
    struct _logger* logger =
        (struct _logger*)malloc(sizeof(struct _logger));
    if (!logger) return NULL;
    logger->logging = 0;
    logger->name = 0;
    logger->priv = 0;
    logger->privfree = 0;
    logger->owner = 0;
    logger->reopen = 0;
    logger->type = UNKOWNTYPE;
    _create_base_layout(logger);
    return logger;
}

void
_free_logger(_logger_t logger){
    if (!logger) return;
    if (logger->name){
        lsfree(logger->name);
    }
    //@todo
    //    free layout
    //
    if (logger->priv){
        if (logger->privfree){
            logger->privfree(logger->priv);
        }
    }
    free(logger);
}

int
_reopen_logger(_logger_t _logger){
    if (_logger&& _logger->reopen){
        return _logger->reopen(_logger);
    }
    return -1;
}



const ls_t 
_get_logger_name(_logger_t logger){
    return logger->name;
}

logger_owner_type
_get_logger_owner(_logger_t logger){
    return logger->owner;
}

void 
_set_logger_name(_logger_t _logger,const char* name)
{
	if (_logger->name) {
        _logger->name = lscpy(_logger->name, name);
    } else{
        _logger->name = lscreate(name, strlen(name));
    }
}

void
_set_logger_owner(_logger_t logger,logger_owner_type owner){
    logger->owner = owner;
}





logger_priv_t
_get_logger_priv(_logger_t logger){
    return logger->priv;
}




_file_priv_t
_create_file_priv(_logger_t logger){
    _file_priv_t fp = malloc(sizeof(struct _file_priv));
    fp->flag = O_APPEND | O_WRONLY | O_CREAT | O_TRUNC;
    fp->mode = O_TEXT ;
    fp->size = 0;
    fp->maxsize = DEFAULT_LOGFILE_MAXSIZE;
    fp->logfile = 0;
    if (logger){
        if (logger->priv && logger->privfree){
            logger->privfree(logger->priv);
        }
        logger->priv = fp;
        logger->privfree = _free_file_priv;
        logger->reopen = _file_logger_reopen;
        logger->logging = _file_logger_logging;
        logger->type = FILELOGGER;
    }
    return fp;
}

void
_free_file_priv(logger_priv_t _priv){
    _file_priv_t priv = (_file_priv_t)_priv;
    if (!priv) return;
    if (priv->logfile)lsfree(priv->logfile);
    free(priv);
}

void
_set_file_path(_file_priv_t priv, const char* path){
    if (priv->logfile){
        priv->logfile = lscpy(priv->logfile, path);
    } else{
        priv->logfile = lscreate(path, strlen(path));
    }
}

const char*
_get_file_path(_file_priv_t priv){
    return priv->logfile;
}

void
_set_file_maxsize(_file_priv_t priv, int size){
    priv->maxsize = size;
}

int
_get_file_maxsize(_file_priv_t priv){
    return priv->maxsize;
}

int
_file_logger_reopen(_logger_t logger){
    if (!logger) return -1;
    _file_priv_t priv = (_file_priv_t)logger->priv;
    if (!(priv->fd = open(priv->logfile, priv->flag, priv->mode))){
        return -1;
    }
    priv->size = _filelength(priv->fd);

    return 0;
}

int
_file_logger_logging(_logger_t logger,struct _log_msg* msg){
    if (!logger || !msg) return -1;
    _file_priv_t priv = (_file_priv_t)logger->priv;
    ls_t ls = 0;
    if (logger->layout){
        ls = logger->layout->layout(logger->layout, msg);
    } else{
        ls = lscreate(msg->msg, lslen(msg->msg));
    }
    if (!ls) return 0;
	priv->size += lslen(ls);
	if (priv->size >= priv->maxsize*1024*1024) {
        priv->fd = open(priv->logfile, priv->flag | O_TRUNC | O_APPEND, priv->mode);
		priv->size = 0;
	}
    write(priv->fd, ls, lslen(ls));
    lsfree(ls);
    return 0;
}




_rfile_priv_t
_create_rfile_priv(_logger_t logger){
    _rfile_priv_t priv = malloc(sizeof(struct _rfile_priv));
    priv->fp.logfile = 0;
    priv->fp.flag = O_APPEND | O_WRONLY | O_CREAT;
    priv->fp.mode = O_TEXT ;
    priv->rindex = 0;
    priv->rmax = DEFAULT_LOGFILE_RCOUNT;
    priv->fp.maxsize = DEFAULT_LOGFILE_RSIZE;
    priv->extw = (long)log10(priv->rmax) + 1;
    if (logger){
        if (logger->priv && logger->privfree){
            logger->privfree(logger->priv);
        }
        logger->priv = priv;
        logger->privfree = _free_rfile_priv;
        logger->reopen = _rfile_logger_reopen;
        logger->logging = _rfile_logger_logging;
        logger->type = ROLLINGFILELOGGER;
    }
    return priv;
}

void
_free_rfile_priv(logger_priv_free priv){
    _rfile_priv_t _priv = (_rfile_priv_t)priv;
    if (_priv&&_priv->fp.logfile) lsfree(_priv->fp.logfile);
    free(_priv);
}

_file_priv_t
_get_rfile_file_priv(_rfile_priv_t priv){
    return &priv->fp;
}

void
_set_rfile_rmax(_rfile_priv_t priv, int max){
    priv->rmax = max;
    priv->extw = (long)log10(priv->rmax) + 1;
}

int
_get_rfile_rmax(_rfile_priv_t priv){
    return priv->rmax;
}

int
_rfile_logger_reopen(_logger_t logger){
    _rfile_priv_t priv = (_rfile_priv_t)logger->priv;
    priv->rmax = priv->rmax <= 0? 1: priv->rmax;
    priv->extw = (long)log10(priv->rmax) + 1;
    ls_t back_path = lsinitcpyls(priv->fp.logfile);
    //get rolling index
    for (; priv->rindex < priv->rmax; priv->rindex++){
        ls_t temp = lsinitfmt( "%s.%.*d",back_path,priv->extw,priv->rindex+1);
        if (access(temp, 0)){
            lsfree(temp);
            break;
        }
        lsfree(temp);
    }
    lsfree(back_path);
    int fd = open(priv->fp.logfile, priv->fp.flag, priv->fp.mode);
    
    if (!fd) return -1;
    priv->fp.fd = fd;
    priv->fp.size = _filelength(fd);
    return 0;
}

int 
_rolling_over(_rfile_priv_t logger){
    if (!logger) return -1;
    logger->rindex++;
    // make sure the smaller index of backup , the nearly logging file;
    if (logger->rindex > logger->rmax){
        logger->rindex = logger->rmax;
        ls_t back_path = lsinitfmt("%s.%.*d", logger->fp.logfile, logger->extw, logger->rmax);
        int ok = remove(back_path);
        lsfree(back_path);
        if (ok) return ok;
    }
    for (int i = logger->rindex - 1; i > 0; i--){
        ls_t oldpath = lsinitfmt("%s.%.*d", logger->fp.logfile, logger->extw, i);
        ls_t newpath = lsinitfmt("%s.%.*d", logger->fp.logfile, logger->extw, i + 1);
        if (!access(newpath,0)){
            remove(newpath);
        }
        int ok = rename(oldpath, newpath);
        lsfree(oldpath);
        lsfree(newpath);
        if (ok) return ok;
    }
    //always save the nearest backup file as logfile.1
    ls_t back_path = lsinitfmt("%s.%.*d",logger->fp.logfile,logger->extw,1);
    if (!access(back_path, 0)){
        if (remove(back_path)){
            lsfree(back_path);
            return -1;
        }
    }

    if (rename(logger->fp.logfile, back_path)){
        lsfree(back_path);
        return -1;
    }
    lsfree(back_path);
    return 0;
}

int
_rfile_logger_logging(_logger_t logger , struct _log_msg* msg){
    _rfile_priv_t priv = (_rfile_priv_t)logger->priv;
    if (!priv || !msg) return -1;
    ls_t ls = 0;
    if (logger&& logger->layout){
        ls = (logger->layout->layout)(logger->layout, msg);
    } else{
        ls = lscreate(msg->msg,lslen(msg->msg));
    }
    if (!ls) return 0;
	if (priv->fp.size + lslen(ls) >= priv->fp.maxsize*1024*1024) {
        close(priv->fp.fd);
        _rolling_over(priv);
		priv->fp.size = 0;
        priv->fp.fd = open(priv->fp.logfile, priv->fp.flag, priv->fp.mode);
        if (!priv->fp.fd) {
            lsfree(ls);
            return -2;
        }
	}
    write(priv->fp.fd, ls, lslen(ls));
    priv->fp.size += strlen(ls);
    lsfree(ls);
	return 0;
}



_console_priv_t
_create_console_priv(_logger_t logger){
    _console_priv_t priv = malloc(sizeof(struct _console_priv));
    if (logger){
        if (logger->priv && logger->privfree){
            logger->privfree(logger->priv);
        }
        logger->priv = priv;
        logger->privfree = _free_console_priv;
        logger->reopen = _console_logger_reopen;
        logger->logging = _console_logger_logging;
        logger->type = CONSOLELOGGER;
    }
    return priv;
}

void
_free_console_priv(logger_priv_t priv){
    free(priv);
}

void
_set_console_stream(_console_priv_t priv, FILE* stream){
    priv->stream = stream;
}

FILE*
_get_console_stream(_console_priv_t priv){
    return priv->stream;
}

int
_console_logger_reopen(_logger_t logger){
    _console_priv_t priv = (_console_priv_t)logger->priv;
    
    return priv&& priv->stream ? 0 : -1;
}

int
_console_logger_logging(_logger_t logger, struct _log_msg* msg){
    _console_priv_t priv = (_console_priv_t)logger->priv;
    ls_t ls = 0;
    if ( logger->layout){
        ls = logger->layout->layout(logger->layout, msg);
    } else{
        ls = lscreate(msg->msg,lslen(msg->msg));
    }
    if (!ls) return 0;
    if (priv&& priv->stream){
        fprintf(priv->stream, "%s", ls);
    }
    //fprintf((FILE*)priv->stream, "%s", ls);
    lsfree(ls);
    return 0;
}




_logger_t
_create_file_logger(const char* name, struct _catagory* owner, 
                    const char* logfile){
    _logger_t logger = _create_logger();
    if (!logger) return NULL;
    _file_priv_t priv = _create_file_priv(logger);

    logger->type = FILELOGGER;
    _set_logger_name(logger, name);
    _set_file_path(priv, logfile);

    _create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        logger->owner = owner;
    }
    return logger;
}

_logger_t
_create_rfile_logger(const char* name, logger_owner_type owner,
                     const char* logfile){
    _logger_t logger = _create_logger();
    if (!logger) return NULL;
    _rfile_priv_t priv = _create_rfile_priv(logger);

    logger->type = ROLLINGFILELOGGER;
    _set_logger_name(logger, name);
    _set_file_path(&priv->fp, logfile);
    if (owner){
        _add_logger(owner, logger);
    }
    
    _create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        logger->owner = owner;
    }
    return logger;
}

_logger_t
_create_console_logger(const char* name, logger_owner_type owner,
                       FILE* stream){
    _logger_t logger = _create_logger();
    if (!logger) return NULL;
    _console_priv_t priv = _create_console_priv(logger);
    logger->type = CONSOLELOGGER;
    _set_logger_name(logger, name);
    _set_console_stream(priv,stream);
    
    _create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        logger->owner = owner;
    }
    return logger;
}
