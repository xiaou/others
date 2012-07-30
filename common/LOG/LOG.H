#ifndef _LOG_H_
#define _LOG_H_


#include "log4cplus/loglevel.h"


///
typedef int LOGLEVEL;
//级别由高到低:
const LOGLEVEL  LOG_LEVEL_NOLOG = log4cplus::OFF_LOG_LEVEL;
#define 		LOG_LEVEL_OFF     LOG_LEVEL_NOLOG
const LOGLEVEL	LOG_LEVEL_FATAL = log4cplus::FATAL_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_ERROR = log4cplus::ERROR_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_WARN  = log4cplus::WARN_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_DEBUG = log4cplus::DEBUG_LOG_LEVEL;


/*
 * @brief 日志的全局初始化.
 * @note 如果不调用这个函数，那么请关心(或修改)LOG.CPP里__default_log_global_init_ifNeed函数.
 * @param[in] toFile 是否把日志输出到文件.
 * @param[in] fileName 相对与程序文件的输出文件路径.(NULL则"log/log").
 * @param[in] level 日志输出级别.只有高于等于此级别的日志才被输出.(NULL means log all).
 */
void LOG_GLOBAL_INIT(bool toFile, const char * fileName, LOGLEVEL level);

void LOG_FATAL(const char * format, ...);
void LOG_ERROR(const char * format, ...);
void LOG_WARN(const char * format, ...);
void LOG_DEBUG(const char * format, ...);




#endif
