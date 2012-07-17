#include "unp.h"

static int g_bindSockfd = 0;

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
	fd_set allfds;
	fd_set rfds;
	FD_ZERO(&allfds);
	FD_SET(listenfd, &allfds);
	int maxfd = listenfd;
	int nready = 0;
	
	const int clientsLen = FD_SETSIZE - 1;
	int clients[clientsLen];
	int maxi = -1;//index of clients's max validate fd.
	for(int i = 0; i != clientsLen; i++)
		clients[i] = -1;
	
	int sockfd;
	struct sockaddr addr;
	socklen_t childlen;
	
	ssize_t n; //read() retval.
	char buf[MAXLINE];
	
	while(1)
	{
		rfds = allfds;
		nready = Select(maxfd + 1, &rfds, NULL, NULL, NULL);

		if(FD_ISSET(listenfd, &rfds))
		{
			childlen = sizeof addr;
			sockfd = Accept(listenfd, &addr, &childlen);
			int i = 0;
			for(; i != clientsLen; i++)
			{
				if(clients[i] < 0)
				{
					clients[i] = sockfd;
					if(i > maxi)
						maxi = i;
					FD_SET(clients[i], &allfds);
					if(clients[i] > maxfd)
						maxfd = clients[i];
					break;
				}
			}
			
			if(i == clientsLen)
			{
				printf("clients so many!\n");
				close(sockfd);
			}
			
			if(--nready <= 0)
				continue;
		}
		
		for(int i = 0; i <= maxi; i++)
		{
			if(clients[i] < 0) 
				continue;
			if(FD_ISSET(clients[i], &rfds))
			{
				if((n = Read(clients[i], buf, MAXLINE)) == 0)
				{//connect closed by client
					Close(clients[i]);
					FD_CLR(clients[i], &allfds);
					clients[i] = -1;
					
				}
				else
					Writen(clients[i], buf, n);
					
				if(--nready <= 0)
					break;
			}
		}
		
	}//_while(1)_
	
	
	close(listenfd);
	return 0;
}

