#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <unistd.h>	   // for close()
#include <stdio.h>      
#include <stdlib.h>      
#include <string.h>       
#include <pthread.h>
//for keyboard check:
#include <sys/select.h>
#include <termios.h>
#include <ctype.h>


#ifndef   MAXPATH 
const   int   MAXPATH=1024; 
#endif   //MAXPATH 


ssize_t getSelfEXEPath(char * buf, size_t bufsiz)
{
    ssize_t ret = readlink("/proc/self/exe", buf, bufsiz);
    if(ret != -1)
    {
        for(int i = strlen(buf); i != 0; i--)   
            if(buf[i-1] == '/')
            {
                buf[i-1] = 0;
                break;
            }
    }
    return ret;
}

int getBacklogValue()
{
	int ret = 5;

    char   buf[MAXPATH]; 
    getSelfEXEPath(buf, MAXPATH);
    strcat(buf, "/server_config.txt");

	FILE * pFile = fopen(buf, "rb");
	if(pFile == NULL)
	{
		printf("\nwarning by U:open %s failed.\n", buf);
	}
	else
	{
        size_t n = MAXPATH;
        char * p = (char *)buf;
		getline(&p, &n, pFile);
		sscanf(buf, "LISTENQ=%d", &ret);
		fclose(pFile);
	}

	return ret;
}

void checkKeyboard4Quit()
{
    int STDIN = 0;
    struct timeval tv = {0,0};
    struct termios term , termbak;
    char   ch;
    fd_set    fd;
    
    FD_ZERO(&fd);
    FD_SET( STDIN ,&fd);

    tcgetattr(STDIN, &term);
    termbak = term;
    term.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN, TCSANOW, &term);    

    while(1)
    {
        FD_ZERO(&fd);
        FD_SET( STDIN ,&fd);
        if(   1 == select( STDIN+1,&fd,NULL,NULL,&tv) 
             && 1 == read( STDIN , &ch , 1 ) 
             && ('q' == tolower(ch) || 'Q' == tolower(ch)) 
          )
        {
            break;
        }
        fflush(stdout);
        usleep(100000);
    }

    tcsetattr(STDIN,TCSANOW,&termbak);
}

int main()
{
	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0)
		return -1;

	struct sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(4333);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(sock_addr.sin_zero, 0x00, 8);

	if(bind(sock_fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in)) != 0)
	{
		printf("error by U:bind failed.");
		close(sock_fd);
		return -1;
	}

	int backlog = getBacklogValue();
    printf("listen(backlog=%d)\n", backlog);    
	
    if(listen(sock_fd, backlog) == -1)
	{
		printf("\nerror by U: listen failed.\n");
		close(sock_fd);
		return -1;
	}
	
	printf("Input 'q' to quit.\n");
    checkKeyboard4Quit();

    close(sock_fd);
	return 0;
}

