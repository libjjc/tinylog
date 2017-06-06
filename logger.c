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

logger_t
_create_logger(){
    struct _logger* logger =
        (struct _logger*)malloc(sizeof(struct _logger*));
    if (!logger) return NULL;
    logger->log = 0;
    logger->free = _free_logger;
    return logger->log;
}

void
_free_logger(logger_t logger){
    struct _logger* _logger = _logger_addr(logger);
    if (!_logger) return;
    if (_logger->name){
        lsfree(_logger->name);
    }
    //@todo
    //    free layout
    //
    if (_logger->priv){
        if (_logger->privfree){
            _logger->privfree(_logger->priv);
        } else{
            free(_logger->priv);
        }
    }
    if (_logger->free){
        _logger->free(logger);
    }
    
}

int
_reopen_logger(logger_t logger){
    struct _logger* _logger = _logger_addr(logger);
    if (_logger&& _logger->reopen){
        return _logger->reopen(logger);
    }
    return -1;
}

const ls_t 
_get_logger_name(logger_t logger){
	return _logger_addr(logger)->name;
}

logger_free
_get_logger_free(logger_t logger){
    return _logger_addr(logger)->free;
}

layout_t
_get_logger_layout(logger_t logger)
{
    return _logger_addr(logger)->layout;
}

logger_owner_type
_get_logger_owner(logger_t logger){
    return _logger_addr(logger)->owner;
}

void 
_set_logger_name(logger_t logger,const char* name)
{
	struct _logger* _logger = _logger_addr(logger);
	if (_logger->name) {
        _logger->name = lscpy(_logger->name, name);
    } else{
        _logger->name = lscreate(name, strlen(name));
    }
	return 0;
}

void
_set_logger_owner(logger_t logger,logger_owner_type owner){
    _logger_addr(logger)->owner = owner;
}



void
_set_logger_priv(logger_t logger, logger_priv_t priv){
    _logger_addr(logger)->priv = priv;
}

logger_priv_t
_get_logger_priv(logger_t logger){
    return _logger_addr(logger)->priv;
}




_file_priv_t
_create_file_priv(logger_t lgr){
    _file_priv_t fp = malloc(sizeof(struct _file_priv));
    fp->flag = O_APPEND | O_WRONLY | O_CREAT | O_TRUNC;
    fp->mode = O_TEXT | O_NOINHERIT;
    fp->size = 0;
    fp->maxsize = DEFAULT_LOGFILE_MAXSIZE;
    struct _logger* logger = _logger_addr(lgr);
    if (logger&& lgr){
        logger->priv = fp;
        logger->privfree = _free_file_priv;
        logger->reopen = _file_logger_reopen;
        logger->log = _file_logger_logging;
    }
    return fp;
}

void
_free_file_priv(_file_priv_t priv){
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
_file_logger_reopen(_file_priv_t priv){
    //struct _logger* logger = _logger_from_priv(priv); 
    //if (!logger) return -1;

    if (!(priv->fd = _open(priv->logfile, priv->flag, priv->mode))){
        return -1;
    }
    priv->size = _filelength(priv->fd);

    return 0;
}

int
_file_logger_logging(_file_priv_t priv,struct _log_msg* msg){
    struct _logger* logger = _logger_from_priv(priv);
    if (!logger || !msg) return -1;
    ls_t ls = 0;
    if (logger->layout){
        ls = logger->layout(logger->layout, msg);
    } else{
        ls = lscreate(msg->msg, lslen(msg->msg));
    }
    if (!ls) return 0;
	priv->size += lslen(ls);
	if (priv->size >= priv->maxsize*1024*1024) {
        priv->fd = _open(priv->logfile, priv->flag | O_TRUNC | O_APPEND, priv->mode);
		priv->size = 0;
	}
    _write(priv->fd, ls, lslen(ls));
    lsfree(ls);
    return 0;
}




_rfile_priv_t
_create_rfile_priv(logger_t lgr){
    _rfile_priv_t priv = malloc(sizeof(struct _rfile_priv));
    priv->fp.flag = O_APPEND | O_WRONLY | O_CREAT;
    priv->fp.mode = O_TEXT | O_NOINHERIT;
    priv->rindex = 0;
    priv->rmax = DEFAULT_LOGFILE_RCOUNT;
    priv->fp.maxsize = DEFAULT_LOGFILE_RSIZE;
    priv->extw = (long)log10(priv->rmax) + 1;
    struct _logger* logger = _logger_addr(lgr);
    if (logger&& lgr){
        logger->priv = priv;
        logger->privfree = _free_rfile_priv;
        logger->reopen = _rfile_logger_reopen;
        logger->log = _rfile_logger_logging;
    }
    return priv;
}

void
_free_rfile_priv(_rfile_priv_t priv){
    if (priv && priv->fp.logfile) lsfree(priv->fp.logfile);
    free(priv);
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
_rfile_logger_reopen(_rfile_priv_t priv){
    priv->rmax = priv->rmax <= 0? 1: priv->rmax;
    priv->extw = (long)log10(priv->rmax) + 1;
    ls_t back_path = lsinitcpyls(priv->fp.logfile);
    for (; priv->rindex < priv->rmax; priv->rindex++){
        back_path = lscatfmt(back_path, ".%.*s",priv->extw,priv->rindex+1);
        if (!_access(back_path, 0)){
            break;
        }
    }
    lsfree(back_path);
    int fd = _open(priv->fp.logfile, priv->fp.flag, priv->fp.mode);
    
    if (!fd) return -1;

    return 0;
}

int 
_rolling_over(_rfile_priv_t logger){
    if (!logger) return -1;
    ls_t back_path = lsinitfmt(logger->fp.logfile,".%.*d",logger->extw,1);
    if (logger->rindex + 1 >= logger->rmax){
        remove(back_path);
        for (int i = 1; i < logger->rmax; i++){
            ls_t path_old = lsinitfmt("%s.%.*d", logger->fp.logfile, i + 1);
            if (!_access(path_old, 0)){
                ls_t path_new = lsinitfmt("%s.%.*d", logger->fp.logfile, i);
                rename(path_old, path_new);
                lsfree(path_old);
                lsfree(path_new);
            } else{
                break;
            }
        }
        rename(logger->fp.logfile, back_path);
        lsfree(back_path);
        return 0;
    }
    rename(logger->fp.logfile, back_path);
    logger->rindex++;
    lsfree(back_path);
    return 0;
}

int
_rfile_logger_logging(_rfile_priv_t priv , struct _log_msg* msg){
    if (!priv || !msg) return -1;
    struct _logger* _logger = _logger_from_priv(priv);
    ls_t ls = 0;
    if (_logger&& _logger->layout){
        ls = (_logger->layout)(_logger->layout, msg);
    } else{
        ls = lscreate(msg->msg,lslen(msg->msg));
    }
    if (!ls) return 0;
	if (priv->fp.size + lslen(ls) >= priv->fp.maxsize*1024*1024) {
        _close(priv->fp.fd);
        _rolling_over(_logger);
		priv->fp.size = 0;
        priv->fp.fd = _open(priv->fp.logfile, priv->fp.flag, priv->fp.mode);
        if (!priv->fp.fd) {
            lsfree(ls);
            return -2;
        }
	}
    _write(priv->fp.fd, ls, lslen(ls));
    priv->fp.size += strlen(ls);
    lsfree(ls);
	return 0;
}



_console_priv_t
_create_console_priv(logger_t lgr){
    _console_priv_t priv = malloc(sizeof(struct _console_priv));
    struct _logger* logger = _logger_addr(lgr);
    if (logger&& lgr){
        logger->priv = priv;
        logger->privfree = _free_console_priv;
        logger->reopen = _console_logger_reopen;
        logger->log = _console_logger_logging;
    }
    return priv;
}

void
_free_console_priv(_console_priv_t priv){
    free(priv);
}

void
_set_console_stream(_console_priv_t priv, int stream){
    priv->stream = stream;
}

int
_get_console_stream(_console_priv_t priv){
    return priv->stream;
}

int
_console_logger_reopen(_console_priv_t priv){
    return priv&& priv->stream ? 0 : -1;
}

int
_console_logger_logging(_console_priv_t priv, struct _log_msg* msg){
    struct _logger* _logger = _logger_from_priv(priv);
    ls_t ls = 0;
    if (_logger&& _logger->layout){
        ls = (_logger->layout)(_logger->layout, msg);
    } else{
        ls = lscreate(msg->msg,lslen(msg->msg));
    }
    if (!ls) return 0;
    fprintf((FILE*)priv->stream, "%s", ls);
    lsfree(ls);
    return 0;
}




logger_t
_create_file_logger(const char* name, struct _catagory* owner, 
                    const char* logfile){
    logger_t logger = _create_logger();
    struct _logger* _logger = _logger_addr(logger);
    if (!logger) return NULL;
    _file_priv_t priv = _create_file_priv(logger);

    _set_logger_name(logger, name);
    _set_file_path(priv, logfile);
    if (owner){
        _add_logger(owner, logger);
    }
    
    create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        _logger->owner = owner;
    }
    return logger;
}

logger_t
_create_rfile_logger(const char* name, logger_owner_type owner,
                     const char* logfile){
    logger_t logger = _create_logger();
    struct _logger* _logger = _logger_addr(logger);
    if (!logger) return NULL;
    _rfile_priv_t priv = _create_rfile_priv(logger);

    _set_logger_name(logger, name);
    _set_file_path(priv->fp.logfile, logfile);
    if (owner){
        _add_logger(owner, logger);
    }
    
    create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        _logger->owner = owner;
    }
    return logger;
}

logger_t
_create_console_logger(const char* name, logger_owner_type owner,
                       int stream){
    logger_t logger = _create_logger();
    struct _logger* _logger = _logger_addr(logger);
    if (!logger) return NULL;
    _rfile_priv_t priv = _create_rfile_priv(logger);

    _set_logger_name(logger, name);
    _set_console_stream(priv,stream);
    
    create_base_layout(logger);

    if (owner) {
        _add_logger(owner, logger);
        _logger->owner = owner;
    }
    return logger;
}
