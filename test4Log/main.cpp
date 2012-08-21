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


using namespace std;

char g_buf[] = "1234";


void atExit(void)
{
	cout<<"in atExit().(pid = "<< getpid()<<" )."<<endl;
usleep(1000*1000*2);
	//cout<<"\t raise(SIGQUIT)";  raise( SIGQUIT );
	int i = 3, j = 0;
    int k = i / j;	
	cout<<endl;
	cout<<"end atExit()"<<endl;
}

void signalChild(int signum)
{
	cout<<"in signalChild()."<<endl;
	pid_t pid;
	int stat;
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0 )
	{
		cout<<"\t waited pid = "<<pid<<", status = "<<stat<<endl;
		if(WIFSIGNALED(stat))
			cout<<"\t it's WIFSIGNALED."<<endl;
		if(WCOREDUMP(stat))
			cout<<"\t it's WCOREDUMP."<<endl;
	}
	cout<<"end signalChild()."<<endl;
}

class C
{
public:
	string s[4];
};

int main()
{
char buf[4];

snprintf(buf, 0, "xxoo");

return 0;
	signal(SIGCHLD, signalChild);

	pid_t id = fork();
	if(id == -1)
		return -1;
	else if(id == 0)//child
	{
		cout<<"forked pid = "<<getpid()<<endl;
		atexit(atExit);	
		return -1;
	}
	
	system("sleep 4");
	//usleep(1000*1000*10);

	return 0;
}

