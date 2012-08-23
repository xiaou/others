#include "LOG.h"
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
#define MAX_LOGGER_A_FILE_SIZE		(1024*1024*4) /**< 一个日志文件的最大值（这是对文件日志而言的). */

#define FORMAT_A_LOG_OUTPUT_SeeSrcFile_False		"[%D.%d{%q} %p] %m%n"	//"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m%n"
#define FORMAT_A_LOG_OUTPUT_SeeSrcFile_True			"[%d{%y/%m/%d %H:%M:%S.%q} %p] %m [%F:%L]%n"//对LOG_函数的包装是的显示源文件没有了意义——总是定位到这个LOG.cpp里了.所以这个宏基本废了.



///
pthread_mutex_t * __initializeMutex();
inline void __default_log_global_init_ifNeed();

pthread_mutex_t * g_pMutex = __initializeMutex();
bool g_inited = false;
LOGLEVEL g_level;
bool g_toFile;
char g_fileName[2052] = {0};
char g_configFile[2000];


#define IMP_g_pMutex_LOCK  \
	if(g_pMutex == NULL)\
		return;/*sad..*/\
	if(pthread_mutex_lock(g_pMutex) != 0)\
		return;/*sad..*/

#define IMP_g_pMutex_UNLOCK  \
	if(pthread_mutex_unlock(g_pMutex) != 0)\
		return;/*sad..*/

class AutoLocker{public:AutoLocker(){IMP_g_pMutex_LOCK} ~AutoLocker(){IMP_g_pMutex_UNLOCK}};

///当前工作目录切换到exe所在目录.出栈后回到原目录.
class _CDEXEDIR
{
	public:
	_CDEXEDIR()
	{
		char   exePath[PATH_MAX]; 
		getcwd(currWorkPath, PATH_MAX);
		getSelfEXEPath(exePath, PATH_MAX);
		chdir(exePath);
	}
	~_CDEXEDIR()
	{
		chdir(currWorkPath);
	}
	private:
	char currWorkPath[PATH_MAX];
	
	ssize_t getSelfEXEPath(char * buf, size_t bufsiz)
	{
   		ssize_t ret = readlink("/proc/self/exe", buf, bufsiz);
    	if(ret != -1)
    	{
        	for(int i = strlen(buf); i != 0; i--)   
        	    if(buf[i-1] == '/')
        	    {
        	        buf[i-1] = 0;
        	        break;
        	    }
    	}
    	return ret;
    }
};

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
	if(g_inited)
		return;
	g_inited = true;

	g_level = level == 0 ? ALL_LOG_LEVEL : level;
	
	if( (g_toFile = toFile) )
	{
		char path[1024];
		if(pathA == NULL)
			sprintf(path, "log");
		else
		{
			if(pathA[0] == '~')
				snprintf(path, 1024, "%s/%s", getenv("HOME"), &pathA[1]);
			else
				snprintf(path, 1024, pathA, NULL);
		}
		_CDEXEDIR cdexedir;
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
			sprintf(fileNamePrefix, fileNamePrefixA, NULL);

		snprintf(g_fileName, sizeof g_fileName, "%s/%s", path, fileNamePrefix);
	}
}

void LOG_GLOBAL_INIT(LOGLEVEL level, const char * levelConfigFilePath, bool toFile, const char * path, const char * fileNamePrefix)
{
	g_level = level;
	g_configFile[0] = 0;
	if(levelConfigFilePath != NULL)
	{
		if(levelConfigFilePath[0] == '~')
			snprintf(g_configFile, sizeof g_configFile, "%s/%s", getenv("HOME"), &levelConfigFilePath[1]);
		else
			snprintf(g_configFile, sizeof g_configFile, levelConfigFilePath, NULL);
	}
	
	LOG_GLOBAL_INIT(0, toFile, path, fileNamePrefix);
}

//从配置文件读取字符串类型数据  
char *__getConfKeyString(const char *key, const char *filename, char * buf, size_t bufSize)   
{   
    FILE *fp;   
    char szLine[1024];  
    char tmpstr[1024];  
    int rtnval;  
    int i = 0;   
    int flag = 1;   
    char *tmp;  

	char path[1024];
	if(filename[0] == '~')
	{
		snprintf(path, 1024, "%s/%s", getenv("HOME"), &filename[1]);
		filename = path;
  	}

    if((fp = fopen(filename, "r")) == NULL)   
    {   
        printf("have   no   config   level   file \n");  
        return NULL;   
    }  
    while(!feof(fp))   
    {   
        rtnval = fgetc(fp);   
        if(rtnval == EOF && sizeof(szLine) != 0)   
        {   
            break;   
        }   
        else   
        {   
            szLine[i++] = rtnval;   
        }   
        if(rtnval == '\n')   
        {   
			if(szLine[i-1] == '\r')
            	i--;

            szLine[--i] = '\0';  
            i = 0;   
            tmp = strchr(szLine, '=');   
  
            if(( tmp != NULL )&&(flag == 1))   
            {   
                if(strstr(szLine,key)!=NULL)   
                {   
                    //注释行  
                    if ('#' == szLine[0])  
                    {  
                    }  
                    else if ( '/' == szLine[0] && '/' == szLine[1] )  
                    {  
                          
                    }  
                    else  
                    {  
                        //找打key对应变量  
						strncpy(buf, tmp+1, bufSize);
						buf[bufSize-1] = '\0';
                        fclose(fp);  
                        return buf;   
                    }  
                }   
				else
				{
					memset(szLine, 0, 1024);
				}
            }  
        }  
    }  
    fclose(fp);   
    return NULL;   
}  

Logger __getLogger(LOGLEVEL level)
{
	char tmp[1024];
	char * p = NULL;
    if(strlen(g_configFile) > 0)  
		p = __getConfKeyString("LOGLEVEL", g_configFile, tmp, sizeof tmp);
	if(p)
	{
	if(strstr(p, "LOG_LEVEL_NOLOG"))
		g_level = LOG_LEVEL_NOLOG;
	else if(strstr(p, "LOG_LEVEL_OFF"))
		g_level = LOG_LEVEL_OFF;
	else if(strstr(p, "LOG_LEVEL_FATAL"))
		g_level = LOG_LEVEL_FATAL;
	else if(strstr(p, "LOG_LEVEL_ERROR"))
		g_level = LOG_LEVEL_ERROR;
	else if(strstr(p, "LOG_LEVEL_WARN"))
		g_level = LOG_LEVEL_WARN;
	else if(strstr(p, "LOG_LEVEL_INFO"))
		g_level = LOG_LEVEL_INFO;
	else if(strstr(p, "LOG_LEVEL_DEBUG"))
		g_level = LOG_LEVEL_DEBUG;
	else if(strstr(p, "LOG_LEVEL_ALL"))
		g_level = LOG_LEVEL_ALL;
	}
	
	SharedObjectPtr<Appender> append;
	std::auto_ptr<Layout> layout;

	if(!g_toFile)
	{//console
		ONLY_A_LOGGER_BOOL = true;
		
		append = (new ConsoleAppender());
		append->setName("console");
		layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
		append->setLayout(layout);
		Logger loggerConsole = Logger::getInstance("console");
		loggerConsole.removeAllAppenders();
		loggerConsole.addAppender(append);
		loggerConsole.setLogLevel(g_level);
		
		return loggerConsole;
	}
	
	char buf[sizeof g_fileName+128];
	if(ONLY_A_LOGGER_BOOL)
		snprintf(buf, sizeof buf, "%s.txt", g_fileName);
	else
	switch(level)
	{
		case LOG_LEVEL_FATAL:
			snprintf(buf, sizeof buf, "%s_fatal.txt", g_fileName);
			break;
		case LOG_LEVEL_ERROR:
			snprintf(buf, sizeof buf, "%s_error.txt", g_fileName);
			break;
		case LOG_LEVEL_WARN:
			snprintf(buf, sizeof buf, "%s_warn.txt", g_fileName);
			break;
		case LOG_LEVEL_INFO:
			snprintf(buf, sizeof buf, "%s_info.txt", g_fileName);
			break;
		case LOG_LEVEL_DEBUG:
			snprintf(buf, sizeof buf, "%s_debug.txt", g_fileName);
			break;
	}
	
	append = new RollingFileAppender(buf, MAX_LOGGER_A_FILE_SIZE, MAX_LOGGER_FILENUMBER);
	append->setName(buf);
	layout.reset(new PatternLayout(FORMAT_A_LOG_OUTPUT_SeeSrcFile_False));
	append->setLayout(layout);
	Logger logger = Logger::getInstance(buf);
	logger.removeAllAppenders();
	logger.addAppender(append);
	logger.setLogLevel(g_level);
	
	return logger;
}

#define IMP_INVOKE_LOG4CPLUS_what(LEVEL) \
	if(!g_inited)\
		__default_log_global_init_ifNeed();\
	char buf[MAX_A_LOG_BUF_SIZE];\
	va_list args;\
	int n;\
	va_start(args, format);\
	n = vsnprintf(buf, sizeof buf, format, args);\
	va_end(args);\
	snprintf(&buf[MAX_A_LOG_BUF_SIZE -1 -10], 10, "...oO0o...");\
	LOG4CPLUS_##LEVEL(__getLogger(LOG_LEVEL_##LEVEL), buf);

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


