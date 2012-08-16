#include "LOG.H"
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/layout.h"
#include "log4cplus/loggingmacros.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>


using namespace log4cplus;
using namespace log4cplus::helpers;


///config for my log:
//
#define MAX_A_LOG_BUF_SIZE 			1024/**< 一条日志的字符串字节数最大值. */

static bool ONLY_A_LOGGER_BOOL = false;/**< 是否所有的日志只用一个logger输出(这是对文件日志而言的). */
#define MAX_LOGGER_FILENUMBER		10/**< 一个logger的最大文件数量（这是对文件日志而言的）. */
#define MAX_LOGGER_A_FILE_SIZE		(1024*1024*2) /**< 一个日志文件的最大值（这是对文件日志而言的). */

#define FORMAT_A_LOG_OUTPUT_SeeSrcFile_False			"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m%n"
#define FORMAT_A_LOG_OUTPUT_SeeSrcFile_True			"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m [%F:%L]%n"



///
pthread_mutex_t * __initializeMutex();
inline void __default_log_global_init_ifNeed();

pthread_mutex_t * g_pMutex = __initializeMutex();
Logger g_logger;
Logger g_loggerFile_FATAL;
Logger g_loggerFile_ERROR;
Logger g_loggerFile_WARN;
Logger g_loggerFile_INFO;
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
	LOG_GLOBAL_INIT(0, true, 0, 0);/**< 请传入默认调用参数.*/
}

void LOG_GLOBAL_INIT(LOGLEVEL level, bool toFile, const char * pathA, const char * fileNamePrefixA)
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
		char path[1024];
		if(pathA == NULL)
			sprintf(path, "log");
		else
			snprintf(path, 1024, pathA);
		CDEXEDIR cdexedir;
		DIR *dir = opendir(path);
		if(dir == NULL && mkdir(path, S_IRWXU) != 0)
		{
			sprintf(path, "log");
			mkdir(path, S_IRWXU);
		}
		else
			closedir(dir);
		chdir(path);
		getcwd(path, 1024);

		char fileNamePrefix[1024];
		if(fileNamePrefixA == NULL)
			sprintf(fileNamePrefix, "log");
		else
			sprintf(fileNamePrefix, fileNamePrefixA);

		char fileName[2048+4];
		snprintf(fileName, sizeof fileName, "%s/%s", path, fileNamePrefix);

		char buf[2048+128];
		
		//
		if(ONLY_A_LOGGER_BOOL)
		{
			snprintf(buf, sizeof buf, "%s.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
			append->setLayout(layout);
			g_logger = Logger::getInstance(buf);
			g_logger.addAppender(append);
			g_logger.setLogLevel(level);
		}
		else
		{
			//
			snprintf(buf, sizeof buf, "%s_fatal.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_True));
			append->setLayout(layout);
			g_loggerFile_FATAL = Logger::getInstance(buf);
			g_loggerFile_FATAL.addAppender(append);
			g_loggerFile_FATAL.setLogLevel(level);

			//
			snprintf(buf, sizeof buf, "%s_error.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_True));
			append->setLayout(layout);
			g_loggerFile_ERROR = Logger::getInstance(buf);
			g_loggerFile_ERROR.addAppender(append);
			g_loggerFile_ERROR.setLogLevel(level);
			
			//
			snprintf(buf, sizeof buf, "%s_warn.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
			append->setLayout(layout);
			g_loggerFile_WARN = Logger::getInstance(buf);
			g_loggerFile_WARN.addAppender(append);
			g_loggerFile_WARN.setLogLevel(level);
			
			//
			snprintf(buf, sizeof buf, "%s_info.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
			append->setLayout(layout);
			g_loggerFile_INFO = Logger::getInstance(buf);
			g_loggerFile_INFO.addAppender(append);
			g_loggerFile_INFO.setLogLevel(level);
			
			//
			snprintf(buf, sizeof buf, "%s_debug.txt", fileName);
			append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
			append->setName(buf);
			layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
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
		layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
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
	n = vsnprintf(buf, sizeof buf, format, args);\
	va_end(args);\
	snprintf(&buf[MAX_A_LOG_BUF_SIZE -1 -10], 10, "...oO0o...");\
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

void LOG_INFO(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(INFO);
}

void LOG_DEBUG(const char * format, ...)
{
	IMP_INVOKE_LOG4CPLUS_what(DEBUG);
}


