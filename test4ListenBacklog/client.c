#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <unistd.h>	   // for close()
#include <pthread.h>
#include <iostream>
#include <cstdlib> //for exit()
#include <string.h>
#include <sys/wait.h> 

using namespace std;

void sendTCP_SYN()
{
	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
		return;
	
	struct sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(4333);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(sock_addr.sin_zero, 0x00, 8);

	if( connect(sock_fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in)) != 0 )
	{
	    close(sock_fd);
		cout<<"error by U:connect failed."<<endl;
		exit(0);
	}
}

void send10_TCP_SYN()
{
    void* (*func)(void *) = (void* (*)(void *))&sendTCP_SYN;
    pthread_t ptd;
	for(int i = 0; i != 10; i++)
	{
	#if 1
	    sendTCP_SYN();
	#else
		if(pthread_create(&ptd, NULL, func, NULL) != 0)
		{
			//cout<<"error by U:pthread_create failed"<<endl;
			exit(0);
		}
	#endif
	}
}


int main()
{
	pid_t pid;

    while(1)
    {
    
    cout<<"fork 100:"<<endl;

	for(int i = 0; i != 100; i++)
	{
	    pid = fork();
	    if(pid == -1)
	        return -1;
	    else if(pid == 0)
	    {
	        //cout<<"I'm child."<<endl;
	        while(1)
	        {
		        send10_TCP_SYN();	
		        usleep(10000);
	        }
	        return 0;
	    }
	    else 
	    {
	        //cout<<"I'm parent."<<endl;
	    }
	}
	
	cout<<"100 forked."<<endl;
	
	while(wait(NULL) != -1);
	
	sleep(1);
	
	}
}

