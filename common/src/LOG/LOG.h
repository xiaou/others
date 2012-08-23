/**
 * @file LOG.h
 * @brief 日志工具.
 * @author xiaoU.
 * @date 2012 E.O.W.
 * @version 2.06
 * @par 修改记录：
 *	-2.04:修复删掉日志文件后不再输出日志的bug.支持配置文件里LOGLEVEL=LOG_LEVEL_XXX来控制输出. 
 *	-2.01:支持"~"路径.
 */

#ifndef _LOG_H_
#define _LOG_H_


#include "log4cplus/loglevel.h"
#include <string.h>
#include <limits.h>

///
typedef int LOGLEVEL;
//级别由高到低:
const LOGLEVEL  LOG_LEVEL_NOLOG = log4cplus::OFF_LOG_LEVEL;
#define 		LOG_LEVEL_OFF     LOG_LEVEL_NOLOG
const LOGLEVEL	LOG_LEVEL_FATAL = log4cplus::FATAL_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_ERROR = log4cplus::ERROR_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_WARN  = log4cplus::WARN_LOG_LEVEL;
const LOGLEVEL  LOG_LEVEL_INFO  = log4cplus::INFO_LOG_LEVEL;
const LOGLEVEL	LOG_LEVEL_DEBUG = log4cplus::DEBUG_LOG_LEVEL;
#define 		LOG_LEVEL_ALL	  LOG_LEVEL_DEBUG

/*
 * @brief 日志的全局初始化.
 * @note 如果不调用这个函数，那么默认是参数(true, 0, 0)的调用.
 * @param[in] level 日志输出级别.只有高于等于此级别的日志才被输出.(NULL means log all).
 * @param[in] toFile 是否把日志输出到文件.
 * @param[in] path 日志存储路径文件夹。支持相对路径(支持"~"这种路径)与绝对路径.(NULL则为程序所在路径下的log目录.没有则创建.)
 * @param[in] fileNamePrefix 日志文件的名称前缀.(NULL则为"log",eg:log_error.txt).
 */
void LOG_GLOBAL_INIT(LOGLEVEL level, bool toFile, const char * path, const char * fileNamePrefix);
/*
 * @brief 与上面的函数不同的是，这个可以动态的修改输出日志的级别.
 *  通过查看配置文件里的"LOGLEVEL="后面的值,如:LOGLEVEL=LOG_LEVEL_NOLOG(只支持上面define的LOG_LEVEL_XXX这些值)
 * @param[in] levelConfigFilePath 配置文件的路径(支持相对路径、绝对路径和~. 建议使用绝对路径和~).
 */
void LOG_GLOBAL_INIT(LOGLEVEL level, const char * levelConfigFilePath, bool toFile, const char * path, const char * fileNamePrefix);

/** note:一条日志输入的字节数是有限制的，超出限制将在末尾添加特殊省略号了事儿. */
void LOG_FATAL(const char * format, ...);
void LOG_ERROR(const char * format, ...);
void LOG_WARN(const char * format, ...);
void LOG_INFO(const char * format, ...);
void LOG_DEBUG(const char * format, ...);

#endif

