/*
 * @file server_main.c
 * @brief server of the UDP.
 * @note Put 'q' to Quit.
 * @author xiaou
 */

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

static int res = 0;
void runServer(void * sock_fd);
void checkKeyboard4Quit();

int main(int argc, char *argv[])
{
    int sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        printf("error: Create Socket Failed!");
        return -1;
    }
    
    pthread_t id;
    if(pthread_create(&id, NULL, (void *(*)(void *))&runServer, &sock_fd) != 0)
    {
        printf("error: Create Server Thread Failed!");
        return -1;
    }

    checkKeyboard4Quit();

    pthread_cancel(id);
    
    close(sock_fd);

    return res;
}

void runServer(void * sock_fd_A)
{
    //can cancel by other thread.
    pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    //
    int sock_fd = *((int *)sock_fd_A);
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(4321);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(sock_addr.sin_zero, 0x00, 8);
    
    try
    {
        if(bind(sock_fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr)) != 0)
        {
            printf("error: Bind Socket Failed!");
            res = -1;
            throw res;
        }
        
        char recvs[256];
        int recvslen;
        struct sockaddr_in from;
        uint32_t fromlen;
        while(1)
        {
            printf("\nWaiting for data...\n");
            fromlen = sizeof(struct sockaddr);
            if((recvslen = recvfrom(sock_fd, recvs, 256, 0, (struct sockaddr *)&from , &fromlen)) == -1)
            {
                printf("error: Receiving Failed!");
                res = -1;
                throw res;
            }
            else
            {
                if(recvslen < 256)
                    *(recvs+recvslen) = '\0';
                else
                    *(recvs+255) = '\0';
                printf("Received data:\n[%s]\n",recvs);
            }
        }
    }
    catch(...)
    {
        printf("error catched~");
    }
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

