#include "unp.h"

void
str_cli_(FILE *fp, int sockfd)
{
	int n;
	bool isEOF(0);
	char buf[1024];
	int filefd = fileno(fp);
	int maxfdp1 = max(filefd, sockfd) + 1;
	fd_set readfds;
	FD_ZERO(&readfds);
	while(1)
	{
		if(!isEOF)
			FD_SET(filefd, &readfds);
		FD_SET(sockfd, &readfds);
		
		if(select(maxfdp1, &readfds, NULL, NULL, NULL) == -1)
			err_quit("select return error!\n");
		
		///select returned
		//
		if(FD_ISSET(filefd, &readfds))
		{
			if( (n = Read(fileno(fp), buf, 1024)) == 0 )
			{
				isEOF = true;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(sockfd, &readfds);
			}
			else
				Writen(sockfd, buf, n);
		}
		
		if(FD_ISSET(sockfd, &readfds))
		{
			if( (n = Read(sockfd, buf, 1024)) == 0)
			{
				if(!isEOF)
					err_quit("str_cli_:server terminated prematurely.\n");
				else //read ended.
					return;
			}
			else
				Writen(fileno(stdout), buf, n); 
		}
	}
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
		return -1;
	
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERV_PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1)
		return -1;
	
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof addr) == -1)
	{
		printf("connect failed.\n");
		return -1;
	}
	
	str_cli_(stdin, sockfd);
	
	return 0;
}

