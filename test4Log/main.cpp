#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include "LOG.h"
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/layout.h"
#include "log4cplus/loggingmacros.h"
#include "LOG.h"


using namespace std;
using namespace log4cplus;


int main()
{
	LOG_GLOBAL_INIT(0, "../conf", 1, 0, 0);

do
{
	LOG_DEBUG("2");LOG_DEBUG("2");LOG_DEBUG("2");
LOG_ERROR("0");

}while(usleep(1000*1000) == 0);
	
	cout<<"return 0;"<<endl;
	return 0;
}

