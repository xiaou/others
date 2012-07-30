#include <unistd.h>
#include <limits.h>

#ifndef __4file_h__
#define __4file_h__


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

#endif // __4file_h__

