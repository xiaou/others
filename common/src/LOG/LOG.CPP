#include "LOG.H"
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/layout.h"
#include "log4cplus/loggingmacros.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "common.h"

using namespace log4cplus;
using namespace log4cplus::helpers;


///config for my log:
//
#define MAX_A_LOG_BUF_SIZE 			1024/**< 一条日志的字符串字节数最大值. */

static bool ONLY_A_LOGGER_BOOL = false;/**< 是否所有的日志只用一个logger输出(这是对文件日志而言的). */
#define MAX_LOGGER_FILENUMBER		3/**< 一个logger的最大文件数量（这是对文件日志而言的）. */
#define MAX_LOGGER_A_FILE_SIZE		(200*1024) /**< 一个日志文件的最大值（这是对文件日志而言的). */

#define _FORMAT_A_LOG_OUTPUT_SeeSrcFile_False			"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m%n"
#define _FORMAT_A_LOG_OUTPUT_SeeSrcFile_True			"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m[%F:%L]%n"
#define FORMAT_A_LOG_OUTPUT								_FORMAT_A_LOG_OUTPUT_SeeSrcFile_False



///
pthread_mutex_t * __initializeMutex();
inline void __default_log_global_init_ifNeed();

pthread_mutex_t * g_pMutex = __initializeMutex();
Logger g_logger;
Logger g_loggerFile_FATAL;
Logger g_loggerFile_ERROR;
Logger g_loggerFile_WARN;
Logger g_loggerFile_DEBUG;

#define IMP_g_pMutex_LOCK  \
	if(g_pMutex == NULL)\
		return;/*sad..*/\
	if(pthread_mutex_lock(g_pMutex) != 0)\
		return;/*sad..*/

#define IMP_g_pMutex_UNLOCK  \
	if(pthread_mutex_unlock(g_pMutex) != 0)\
		return;/*sad..*/

class AutoLocker{public:AutoLocker(){IMP_g_pMutex_LOCK} ~AutoLocker(){IMP_g_pMutex_UNLOCK}};

pthread_mutex_t * __initializeMutex()
{
	pthread_mutex_t * p = new pthread_mutex_t;
	if(pthread_mutex_init(p, 0) != 0)
	{
		delete p;
		printf("error: in InitializeMutex().File:LOG.CPP.\n");
		return NULL;//sad..
	}
	return p;
}

inline void __default_log_global_init_ifNeed()
{
	LOG_GLOBAL_INIT(false, 0, 0);/**< 请传入默认调用参数.*/
}

void LOG_GLOBAL_INIT(bool toFile, const char * fileName, LOGLEVEL level)
{
	AutoLocker();
	if(Logger::getCurrentLoggers().size())
		return;

	if(level == 0)
		level = ALL_LOG_LEVEL;

	SharedObjectPtr<Appender> append;
	std::auto_ptr<Layout> layout;
	
	if(toFile)
	{
		char buf[1024];
		
		if(fileName == NULL)
			fileName = "log/log";
		
		strcpy(buf, fileName);
		buf[1023] = 0;
		
		CDEXEDIR cdexedir;
		
		for(int i = strlen(buf) - 1; i >= 1; i--)
			if(buf[i] == '/')
			{
				buf[i] = 0;
				mkdir(buf, S_IRWXU);
				break;
			}

		//
		if(ONLY_A_LOGGER_BOOL)
		{
			sprintf(buf, "%s", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
			append->setLayout(layout);
			g_logger = Logger::getInstance(buf);
			g_logger.addAppender(append);
			g_logger.setLogLevel(level);
		}
		else
		{
			//
			sprintf(buf, "%s.fatal", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
			append->setLayout(layout);
			g_loggerFile_FATAL = Logger::getInstance(buf);
			g_loggerFile_FATAL.addAppender(append);
			g_loggerFile_FATAL.setLogLevel(level);

			//
			sprintf(buf, "%s.error", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
			append->setLayout(layout);
			g_loggerFile_ERROR = Logger::getInstance(buf);
			g_loggerFile_ERROR.addAppender(append);
			g_loggerFile_ERROR.setLogLevel(level);
			
			//
			sprintf(buf, "%s.warn", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
			append->setLayout(layout);
			g_loggerFile_WARN = Logger::getInstance(buf);
			g_loggerFile_WARN.addAppender(append);
			g_loggerFile_WARN.setLogLevel(level);
			
			//
			sprintf(buf, "%s.debug", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
			append->setLayout(layout);
			g_loggerFile_DEBUG = Logger::getInstance(buf);
			g_loggerFile_DEBUG.addAppender(append);
			g_loggerFile_DEBUG.setLogLevel(level);
		}
	}
	else
	{
		ONLY_A_LOGGER_BOOL = true;
		
		append = (new ConsoleAppender());
		append->setName("console");
		layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT));
		append->setLayout(layout);
		g_logger = Logger::getInstance("console");
		g_logger.addAppender(append);
		g_logger.setLogLevel(level);
	}
}

#define IMP_INVOKE_LOG4CPLUS_what(LEVEL) \
	if(Logger::getCurrentLoggers().size() == 0)\
		__default_log_global_init_ifNeed();\
	char buf[MAX_A_LOG_BUF_SIZE];\
	va_list args;\
	int n;\
	va_start(args, format);\
	n = vsprintf(buf, format, args);\
	va_end(args);\
	buf[MAX_A_LOG_BUF_SIZE - 1] = 0;\
	if(ONLY_A_LOGGER_BOOL)\
		LOG4CPLUS_##LEVEL(g_logger, buf);\
	else\
		LOG4CPLUS_##LEVEL(g_loggerFile_##LEVEL, buf);

void LOG_FATAL(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(FATAL);
}

void LOG_ERROR(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(ERROR);
}

void LOG_WARN(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(WARN);
}

void LOG_DEBUG(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(DEBUG);
}

