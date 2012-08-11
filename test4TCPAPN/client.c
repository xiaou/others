#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include "LOG.H"

using namespace std;

#define  MAXEPOLLSIZE  50
uint16_t g_port = 9002;
int g_epfd = 0;
struct epoll_event g_events[MAXEPOLLSIZE];
int g_nevents = 0;

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0)|O_NONBLOCK) == -1)
        return -1;
    return 0;
}

int main()
{
	int connfd = socket(AF_INET, SOCK_STREAM, 0);
	if(connfd == -1)
		return -1;
	setnonblocking(connfd);
	
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_port);
	if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1)
		return -1;

	if(connect(connfd, (struct sockaddr *)&addr, sizeof addr) == -1)
	{
		if(errno != EINPROGRESS)
		{		
			cout<<"connect failed."<<endl;
			return -1;
		}
	}

	g_epfd = epoll_create(MAXEPOLLSIZE);
	
	struct epoll_event ep_ev;

	ep_ev.events = EPOLLOUT | EPOLLET;
	ep_ev.data.fd = connfd;
	epoll_ctl(g_epfd, EPOLL_CTL_ADD, connfd, &ep_ev);
	g_nevents ++;

	int nReady;
	int optval;
	socklen_t optlen;
	while(1)
	{
		nReady = epoll_wait(g_epfd, g_events, g_nevents, 2000);
		if(nReady < 0 )
		{
			if(errno == EINTR)
				continue;
			cout<< "epoll_wait() error. errno="<<errno<<"."<<strerror(errno)<<endl;
			break;
		}
		for(int i = 0; i != nReady; i++)
		{	
			if(g_events[i].data.fd == connfd)
			{
				if(g_events[i].events == EPOLLOUT)
				{
					optlen = sizeof optval;
					if(getsockopt(connfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0)
					{
						if(optval == 0) //connect success.
						{
							//char buf[1024*32];
							char buf[1024*100];
							memset(buf, '4', sizeof buf);
							do{			
								cout<< "send len:";
								cout<< send(connfd, buf, sizeof buf, 0)<<endl;
							}while(usleep(1000 * 1000) == 0);
							continue;
						}
					}
				}
				//
				LOG_ERROR("connect faild.");
				close(connfd);
			}
		}
	}//__while(1)__

	close(g_epfd);	
	return 0;
}
