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

///need kernel2.6.9

using namespace std;


#define  MAXEPOLLSIZE  5000
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

/* recv "n" bytes from a descriptor(for nonblock socket) and -1 is error. 
 * 注意，依然可能没有读到想要的数量。但是会从缓冲区里读完。
 */
ssize_t	recvn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ( (nread = recv(fd, ptr, nleft, 0)) < 0) 
		{
			if (errno == EINTR)/* and call recv() again */
				nread = 0;		
			else if(errno == EAGAIN || errno == EWOULDBLOCK)//recv all.
				break;
			else //some error.
				return -1;
		} 
		else if (nread == 0) //EOF. recv all.
			break;				

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

int main()
{
	LOG_GLOBAL_INIT(0 ,0, 0);
#if 0
	struct rlimit rt;
	/* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
	if( setrlimit(RLIMIT_NOFILE, &rt) == -1 )//need sudo.
	{
		cout<< "setrlimit faild.errno=" <<errno<<"."<<strerror(errno) <<endl;
		return -1;
	}
#endif
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
		return -1;

	setnonblocking(listenfd);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(g_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		cout<<"error: bind failed."<<endl;
		return -1;
	}

	if(listen(listenfd, 128) == -1)
	{
		cout<<"error: listen failed."<<endl;
		close(listenfd);
		return -1;
	}
	else
		cout << "listen in " << g_port << endl;
	
	g_epfd = epoll_create(MAXEPOLLSIZE);
	
	struct epoll_event ep_ev;

	ep_ev.events = EPOLLIN | EPOLLET;
	ep_ev.data.fd = listenfd;
	epoll_ctl(g_epfd, EPOLL_CTL_ADD, listenfd, &ep_ev);
	g_nevents ++;
	
	int nReady;
	struct sockaddr_in addr;
	socklen_t addrlen;
	int clientfd;
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
			if(g_events[i].data.fd == listenfd)
			{
				addrlen = sizeof(addr);
				if((clientfd = accept(listenfd, (struct sockaddr*)&addr, &addrlen)) == -1)
				{
					if(errno != EAGAIN && errno != EWOULDBLOCK)
					{
						LOG_WARN("accept() return error.errno=%d.%s\n"
					"current file descriptpr count is %d", errno, strerror(errno), g_nevents);
					}					
					continue;
				}
				LOG_DEBUG("有连接来自于:%s:%d.分配的 socket 为:%d", 
						inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), clientfd);
					
				setnonblocking(clientfd);
				
				ep_ev.events = EPOLLIN | EPOLLET;
				ep_ev.data.fd = clientfd;
				epoll_ctl(g_epfd, EPOLL_CTL_ADD, clientfd, &ep_ev);
				g_nevents ++;
			}
			else
			{
				if(g_events[i].events & EPOLLIN) 
				{//can read
					int fd = g_events[i].data.fd;
					static char buf[1024 * 1024 + 1];
					ssize_t len;

					len = recvn(fd, buf, 1024 * 1024);

					if(len <= 0)
					{
						if(len == -1)//error
							LOG_ERROR("error in recvn.errno=%d.%s", errno, strerror(errno));
						else if(len == 0)//EOF
						{
							epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, NULL);
							g_nevents --;
							close(fd);
						}
					}
					else 
					{
						buf[len] = '\0';
						LOG_DEBUG("recv data len = %d", len);
						//LOG_DEBUG("recv data [%s]", buf);
					}

					//epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, NULL);
					//g_nevents --;
					//close(fd);
				}
			}
		}
	}//__while(1)__

	close(g_epfd);
	close(listenfd);
	return 0;
}



