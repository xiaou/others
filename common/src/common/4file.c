#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t getSelfEXEPath(char * buf, size_t bufsiz)
{
    ssize_t ret = readlink("/proc/self/exe", buf, bufsiz);
    if(ret != -1)
    {
        for(int i = strlen(buf); i != 0; i--)   
            if(buf[i-1] == '/')
            {
                buf[i-1] = 0;
				ret = strlen(buf)+1;
                break;
            }
    }
    return ret;
}




