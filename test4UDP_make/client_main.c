/*
 * @file client_main.c
 * @brief client of the UDP.
 * @author xiaou
 */

#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <unistd.h>	   // for close()
#include <stdio.h>      
#include <stdlib.h>       
#include <string.h>        


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage:%s string\n", argv[0]);
        return -1;
    }
    
    int res = 0;
    int sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        printf("error: Create Socket Failed!");
        return -1;
    }
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(4321);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(sock_addr.sin_zero, 0x00, 8);
    
    try
    {        
        const char * tos = argv[1];
        int toslen = strlen(tos) + 1;
        struct sockaddr_in to = sock_addr;
        uint32_t tolen = sizeof(struct sockaddr);        
        printf("\nSending for data...\n");
        if((toslen = sendto(sock_fd, tos, toslen, 0, (struct sockaddr *)&to, tolen)) == -1)
        {
            printf("error: Sending Failed!");
            res = -1;
            throw res;
        }
        else
        {
            char s[256];
            strncpy(s, tos, toslen);
            printf("Sended data:\n[%s]\n\n", s);
        }
    }
    catch(...)
    {
        printf("error catched~");
    }
    
    close(sock_fd);
    
    return res;
}


