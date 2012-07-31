
#ifndef __common_h__
#define __common_h__

#include <limits.h>
#include <unistd.h>


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





#endif // __common_h__

