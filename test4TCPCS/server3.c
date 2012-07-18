#include "unp.h"

static int g_bindSockfd = 0;

void sig_ctrl_(int signo)
{printf("sig_strl.\n");
	close(g_bindSockfd);
	exit(0);
}

int main()
{
	int listenfd;

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
	
	Signal(SIGTSTP, sig_ctrl_);
	Signal(SIGINT, sig_ctrl_);
	
	
	///select 
	//
	int fdsLen;
	if((fdsLen = sysconf(_SC_OPEN_MAX)) == -1)
		fdsLen = 1024; 
	printf("fdsLen = %d\n", fdsLen);

	struct pollfd fds[fdsLen];
	for(int i = 1; i < fdsLen; i++)
	{
		fds[i].fd = -1;
		fds[i].events = POLLIN;
	}
	fds[0].fd = listenfd;
	fds[0].events = POLLIN;

	int maxi = 0;//index of clients's max validate fd.	

	int sockfd;
	struct sockaddr addr;
	socklen_t childlen;
	
	int nready = 0;	
	ssize_t n; //read() retval.
	char buf[MAXLINE];
//alarm(2);	
	while(1)
	{
		nready = Poll(fds, fdsLen, -1);
		
		if(fds[0].revents & POLLIN)
		{
			childlen = sizeof addr;
			sockfd = Accept(listenfd, &addr, &childlen);
			int i;
			for(i = 1; i < fdsLen; i++)
			{
				if(fds[i].fd < 0)
				{
					fds[i].fd = sockfd;
					fds[i].events = POLLIN;
					fds[i].revents = 0;
					if(i > maxi)
						maxi = i;
					break;
				}
			}
			if(i >= fdsLen)
			{
				printf("clients so many!\n");
				close(sockfd);
			}
			if(--nready <= 0)
				continue;
		}

		for(int i = 1; i <= maxi; i++)
		{
			if(fds[i].fd < 0)
				continue;
			if(fds[i].revents & (POLLIN|POLLERR))
			{
				if((n = read(fds[i].fd, buf, MAXLINE)) < 0)
				{
					if(errno == ECONNRESET)
					{
						printf("error == ECONNRESET!\n");
						Close(fds[i].fd);
						fds[i].fd = -1;
					}
					else
						err_sys("read error!\n");
				}
				else if(n == 0)
				{
					printf("a connection closed by the client.\n");
					Close(fds[i].fd);
					fds[i].fd = -1;
				}
				else 
					Writen(fds[i].fd, buf, n);

				if(--nready <= 0)
					continue;
			}
		}

	}//_while(1)_
	
	
	//
	close(listenfd);
	return 0;
}

