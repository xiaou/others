#include "unp.h"

static int g_bindSockfd = 0;

void
sig_chld_(int signo)
{printf(".\n");
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated!\n", pid);
	return;
}

void sig_ctrl_(int signo)
{printf("sig_strl.\n");
	close(g_bindSockfd);
	exit(0);
}

void
str_echo_(int sockfd)
{printf(".\n");
	ssize_t		n;
	char		buf[MAXLINE];

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
		Writen(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");
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
	
	g_bindSockfd = listenfd;
	
	if(listen(listenfd, 10) == -1)
		return -1;
	
	Signal(SIGCHLD, sig_chld_);
	Signal(SIGTSTP, sig_ctrl_);
	Signal(SIGINT, sig_ctrl_);

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
			str_echo_(connfd);
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

