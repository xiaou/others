#include "unp.h"

void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

int main()
{
	int listenfd, connfd;
	socklen_t addrlen;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	struct sockaddr_in cliaddr, servaddr;

	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof servaddr) == -1)
	{
		printf("bind faile. errno:%d\n", errno);
		return -1;
	} 
	
	if(listen(listenfd, 10) == -1)
		return -1;
	
	Signal(SIGCHLD, sig_chld);

	while(1)
	{
		addrlen = sizeof cliaddr;
		if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &addrlen)) == -1)
		{
			if(errno == EINTR)
				continue;
			else
			{
				printf("accept error!\n");
				exit(0);
			}
		}

		if(fork() == 0)
		{
			if(close(listenfd) == -1)
			{
				printf("close error!\n");
				exit(0);
			}
			str_echo(connfd);
			exit(0);
		}
		
		if(close(connfd) == -1)
		{
			printf("close error!\n");
			exit(0);
		}
	}
	
	close(listenfd);
	return 0;
}

