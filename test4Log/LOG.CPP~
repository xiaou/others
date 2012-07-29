#include "LOG.H"
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/layout.h"
#include "log4cplus/loggingmacros.h"

#include <stdio.h>


using namespace log4cplus;
using namespace log4cplus::helpers;


static Logger g_logger;


inline void __default_log_global_init_ifNeed()
{
	if(Logger::getCurrentLoggers().size() == 0)
		LOG_GLOBAL_INIT(false, 0, 0);/**< 请传入默认调用参数.*/
}

void LOG_GLOBAL_INIT(bool toFile, char * fileName, LOGLEVEL level)
{
	if(Logger::getCurrentLoggers().size())
		return;

	if(level == 0)
		level = ALL_LOG_LEVEL;

	if(toFile)
	{

	}
	else
	{
		SharedObjectPtr<Appender> append(new ConsoleAppender());
		append->setName("console");

		std::auto_ptr<Layout> layout(new PatternLayout("%d{%m/%d/%y %H:%M:%S} - %m [%l]%n"));
		append->setLayout(layout);

		Logger logger = Logger::getInstance("console");
		logger.addAppender(append);
		logger.setLogLevel(level);

		g_logger = logger;
	}
}

#define IMP_INVOKE_LOG4CPLUS_what(LEVEL) \
	char buf[1024];\
	va_list args;\
	int n;\
	va_start(args, format);\
	n = vsprintf(buf, format, args);\
	va_end(args);\
	buf[1023] = 0;\
	LOG4CPLUS_##LEVEL(g_logger, buf);

void LOG_FATAL(const char * format, ...)
{
	__default_log_global_init_ifNeed();
	IMP_INVOKE_LOG4CPLUS_what(FATAL);
}

void LOG_ERROR(const char * format, ...)
{
	__default_log_global_init_ifNeed();
	IMP_INVOKE_LOG4CPLUS_what(ERROR);
}

void LOG_WARN(const char * format, ...)
{
	__default_log_global_init_ifNeed();
	IMP_INVOKE_LOG4CPLUS_what(WARN);
}

void LOG_DEBUG(const char * format, ...)
{
	__default_log_global_init_ifNeed();
	IMP_INVOKE_LOG4CPLUS_what(DEBUG);
}

