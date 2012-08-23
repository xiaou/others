
#ifndef __common_h__
#define __common_h__

#include <limits.h>
#include <unistd.h>

/** return exe path(不以'/'结尾). @retval -1 error. */
ssize_t getSelfEXEPath(char * buf, size_t bufsiz);

class CDEXEDIR
{
	public:
	CDEXEDIR()
	{
		char   exePath[PATH_MAX]; 
		getcwd(currWorkPath, PATH_MAX);
		getSelfEXEPath(exePath, PATH_MAX);
		chdir(exePath);
	}
	~CDEXEDIR()
	{
		chdir(currWorkPath);
	}
	private:
	char currWorkPath[PATH_MAX];
};

//从配置文件读取字符串类型数据  
char *GetConfKeyString(const char *title, const char *key, const char *filename, char * buf, size_t bufSize);

//从配置文件读取整类型数据  
int GetConfKeyInt(const char *title, const char *key, const char *filename, char * buf, size_t bufSize);



#endif // __common_h__

