/*
 * @brief 这个文件是用来定义本程序的全局配置文件的路径，以及取配置文件的变量值的.
 */


#ifndef __CGlobalConfig_h__
#define __CGlobalConfig_h__


//全局配置文件.绝对路径.
static const char * g_configFilePath = "/home/spring/work/APNs4MOA/bin/global.conf"; 

//声明.(#include "common.h")
char * GetConfKeyString(const char*, const char*, const char*, char*, size_t);
int GetConfKeyInt(const char*, const char*, const char*, char*, size_t);

//
class CGlobalConfig
{
public:
    static CGlobalConfig *GetGlobalConfig()    
    {    
        static CGlobalConfig gcf;  
        return &gcf;  
    }  
    
	char * getStrValue(const char * title, const char * key, char * buf, size_t bufsize)
	{
		return GetConfKeyString(title, key, g_configFilePath, buf, bufsize);
	}
	
	int getIntValue(const char * title, const char * key, char * buf, size_t bufsize)
	{
		return GetConfKeyInt(title, key, g_configFilePath, buf, bufsize);
	}
	
private:
	CGlobalConfig(){}
	~CGlobalConfig(){}
};

#endif // __CGlobalConfig_h__
