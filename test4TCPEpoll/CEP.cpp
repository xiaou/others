#include "CEP.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <algorithm>


CEP::CEP()
{ 
	if((m_epfd = epoll_create(1)) == -1) 
		std::cerr<<"faild in CEP().error["<<errno<<"]:"<<strerror(errno)<<std::endl; 
}

bool CEP::checkEventSocket(CEP_Event & cep_ev)
{
	int optval;
	socklen_t optlen = sizeof optval;
	if(getsockopt(cep_ev.fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) != 0)
	{
		std::cerr<<"getsockopt() faild.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
		return false;
	}
	if(optval != 0)
	{
		std::cerr<<"wrong socket."<<std::endl;
		return false;
	}
	if(fcntl(cep_ev.fd, F_GETFL, 0) & O_NONBLOCK)
		return true;
	else
		return setNonBlocking(cep_ev.fd);
}

bool CEP::setEvent4epoll_event(CEP_Event & cep_ev, int epoll_ctl_op)
{
	struct epoll_event ep_ev;
	ep_ev.data.ptr = &cep_ev;
	switch(cep_ev.type)
	{
		case CEP_Event::Type_Connect:
			ep_ev.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
			break;
		case CEP_Event::Type_Send:
			ep_ev.events = EPOLLOUT | EPOLLET;
			break;
		case CEP_Event::Type_Listen:
		case CEP_Event::Type_Recv:
			ep_ev.events = EPOLLIN | EPOLLET;
			break;
		default:
			return false;
	}

	return (epoll_ctl(m_epfd, epoll_ctl_op, cep_ev.fd, &ep_ev) == 0);		
}

bool CEP::addEvent(CEP_Event & cep_ev)
{
	if(!checkEventSocket(cep_ev)) 
		return false;
		
	if(setEvent4epoll_event(cep_ev, EPOLL_CTL_ADD) == false)
		return false;
		
	cep_ev.last_active = time(NULL); 
	cep_ev.canRemoveFromArray = false;
	
	m_events.push_back(cep_ev);
	
	return true;
}

bool CEP::modEvent(CEP_Event & cep_ev)
{
	if(!checkEventSocket(cep_ev)) 
		return false;
		
	if(setEvent4epoll_event(cep_ev, EPOLL_CTL_MOD) == false)
	{
		delEvent(cep_ev);
		return false;
	}
	
	cep_ev.last_active = time(NULL); 
	cep_ev.canRemoveFromArray = false;
	
	return true;
}

bool CEP::delEvent(size_t index)
{
	if(index >= m_events.size())
		return false;
	std::vector<CEP_Event>::iterator it = m_events.begin() + index;
	bool res = epoll_ctl(m_epfd, EPOLL_CTL_DEL, it->fd, NULL);
	m_events.erase(it);
	return res;
}

bool CEP::delEvent(CEP_Event & cep_ev) 
{
#if 0
	std::vector<CEP_Event>::iterator it = find(m_events.begin(), m_events.end(), cep_ev);
	if(it != m_events.end())//find.
	{
		bool res = epoll_ctl(m_epfd, EPOLL_CTL_DEL, cep_ev.fd, NULL);
		m_events.erase(it);
		return res;
	}
	return false;
#endif
	bool res = epoll_ctl(m_epfd, EPOLL_CTL_DEL, cep_ev.fd, NULL);
	cep_ev.canRemoveFromArray = true;
	return res;
}

int CEP::runloop_epoll_wait()
{
	if(m_epfd == -1)
		return -1;
	
	struct epoll_event * ep_evs_buf;
	size_t n_ep_evs_buf = 128;
	if((ep_evs_buf = (struct epoll_event *)malloc(n_ep_evs_buf * sizeof(struct epoll_event))) == NULL)
	{
		std::cerr<<"malloc faild.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
		return -1;
	}
	
	m_checkPos = 0;
	
	while(1)
	{
		checkTimeAndRemove();
		
		size_t currEventsNum = m_events.size();
		if(currEventsNum == 0)
			return 0;
		if(currEventsNum > n_ep_evs_buf)
		{
			n_ep_evs_buf = currEventsNum + 128;
			free(ep_evs_buf);
			if((ep_evs_buf = (struct epoll_event *)malloc(n_ep_evs_buf * sizeof(struct epoll_event))) == NULL)
			{
				std::cerr<<"malloc() faild.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
				return -1;
			}
		}
	
		int nReady = epoll_wait(m_epfd, ep_evs_buf, currEventsNum, 2000);
		if(nReady < 0)
		{
			if(errno == EINTR)
				continue;
			std::cerr<<"epoll_wait() faild.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
			return -1;
		}
		
		CEP_Event * p_cep_ev;
		bool quit_epoll_wait(0);
		for(int i = 0; i != nReady; i++, quit_epoll_wait = false)
		{
			p_cep_ev = (CEP_Event *)ep_evs_buf[i].data.ptr;
			handleEvent(*p_cep_ev, ep_evs_buf[i].events, &quit_epoll_wait);
			if(quit_epoll_wait)
				return 0;
		}	
	}
	return 0;
}

void CEP::checkTimeAndRemove()
{
	// a simple timeout check here, every time 100, better to use a mini-heap, and add timer event  
    long now = time(NULL);  
	
	size_t n = 100 < m_events.size() ? 100 : m_events.size();
	for(size_t i = 0; i < n; i++, m_checkPos++) 
	{
		if(m_checkPos >= m_events.size()) 
			m_checkPos = 0; // recycle  
			
		if(m_events[m_checkPos].canRemoveFromArray)
		{
			std::vector<CEP_Event>::iterator it = m_events.begin() + m_checkPos;
			m_events.erase(it);
			//数组大小改变了.
			m_checkPos -= 1;
			continue;
		}
			  	
		if(m_events[m_checkPos].type != CEP_Event::Type_Listen)// doesn't check listen fd  
		{
			long duration = now - m_events[m_checkPos].last_active;  
			if(duration > 60) // 60s timeout  
			{  
				std::cout<<"fd("<<m_events[m_checkPos].fd<<")timeout("
				<<m_events[m_checkPos].last_active<<"--"<<now<<")."<<std::endl;  
				delEvent(m_checkPos);
				close(m_events[m_checkPos].fd);  
				//数组大小改变了.
				m_checkPos -= 1;
			}
		}  
	}  
}

void CEP::handleEvent(CEP_Event & cep_ev, uint32_t epoll_event_events, bool * quit_epoll_wait)
{
	switch(cep_ev.type)
	{
		case CEP_Event::Type_Connect:
			if(epoll_event_events & EPOLLOUT)
				handleEvent4TypeConnect(cep_ev, quit_epoll_wait);
			break;
		case CEP_Event::Type_Listen:
			if(epoll_event_events & EPOLLIN)
				handleEvent4TypeListen(cep_ev, quit_epoll_wait);
			break;
		case CEP_Event::Type_Send:
			if(epoll_event_events & EPOLLOUT)
				handleEvent4TypeSend(cep_ev, quit_epoll_wait);
			break;
		case CEP_Event::Type_Recv:
			if(epoll_event_events & EPOLLIN)
				handleEvent4TypeRecv(cep_ev, quit_epoll_wait);
			break;
	}
}

void CEP::handleEvent4TypeConnect(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	int optval;
	socklen_t optlen = sizeof optval;
	bool connectSuccess = (getsockopt(cep_ev.fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0 && optval == 0);

	cep_ev.canRemoveFromArray = true;
	
	if(cep_ev.callback != NULL)
			cep_ev.callback(cep_ev, *this, connectSuccess, quit_epoll_wait);
}

void CEP::handleEvent4TypeListen(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int clientfd;
	bool hasError(0);
	if((clientfd = accept(cep_ev.fd, (struct sockaddr*)&addr, &addrlen)) == -1)
	{
		if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			std::cerr<<"accept() failed.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
			hasError = true;
		}
	}	
	cep_ev.newClientFd = clientfd;
	if(cep_ev.callback != NULL)
		cep_ev.callback(cep_ev, *this, !hasError, quit_epoll_wait);
}

void CEP::handleEvent4TypeSend(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	ssize_t len = sendn(cep_ev.fd, cep_ev.buf, cep_ev.len);
	cep_ev.len = len;
	if(cep_ev.callback != NULL)
		cep_ev.callback(cep_ev, *this, len != -1, quit_epoll_wait);
}

void CEP::handleEvent4TypeRecv(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	ssize_t len = recvn(cep_ev.fd, cep_ev.buf, MAXDATABUFSIZE);
	cep_ev.len = len;
	if(cep_ev.callback != NULL)
		cep_ev.callback(cep_ev, *this, len != -1, quit_epoll_wait);
}

/* 
 * recv "n" bytes from a descriptor(for nonblock socket) and -1 is error. 
 * 注意，依然可能没有读到想要的数量。但是会从缓冲区里读完。
 */
ssize_t	CEP::recvn(int fd, char *buf, size_t bufsize)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)buf;
	nleft = bufsize;
//static int i = 0;
//std::cout<<i++<<"\n in while: bufsize = "<<bufsize<<std::endl;
	while (nleft > 0) 
	{
	//std::cout<<"in while: nleft = "<<nleft<<std::endl;
		if ( (nread = recv(fd, ptr, nleft, 0)) < 0) 
		{//std::cout<<"xxxxxxxxxxxxxxxxxxxxxx"<<std::endl;
			if (errno == EINTR)/* and call recv() again */
				nread = 0;		
			else if(errno == EAGAIN || errno == EWOULDBLOCK)//recv all can recv.
				break;
			else //some error.
				return -1;
		} 
		else if (nread == 0) //EOF. recv all.
		{
			//std::cout<<"eof"<<std::endl;
			break;				
		}
		nleft -= nread;
		ptr   += nread;
	}
//std::cout<<i-1<<"> bufsize - nleft ="<<bufsize - nleft<<std::endl;
	return(bufsize - nleft);
}

ssize_t	CEP::sendn(int fd, char *buf, size_t len)
{
	size_t	nleft;
	ssize_t	nsend;
	char	*ptr;

	ptr = (char *)buf;
	nleft = len;
	while (nleft > 0) 
	{
		if ( (nsend = send(fd, ptr, nleft, 0)) < 0) 
		{
			if (errno == EINTR)/* and call send() again */
				nsend = 0;		
			else if(errno == EAGAIN || errno == EWOULDBLOCK)//send all can send.
				break;
			else //some error.
				return -1;
		} 	
		
		nleft -= nsend;
		ptr   += nsend;
	}
	return(len - nleft);
}

inline bool CEP::setNonBlocking(int sockfd)
{
	return (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0)|O_NONBLOCK) != -1);
}

