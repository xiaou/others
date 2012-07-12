#include "unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, 1);
		sleep(1);
		Writen(sockfd, sendline+1, strlen(sendline)-1);

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
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
	
	str_cli(stdin, sockfd);

	return 0;
}

