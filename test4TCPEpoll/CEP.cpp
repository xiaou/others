#include "CEP.h"
#include <algorithm>


/* recv "n" bytes from a descriptor(for nonblock socket) and -1 is error. 
 * 注意，依然可能没有读到想要的数量。但是会从缓冲区里读完。
 */
ssize_t	CEP::recvn(int fd, char *buf, size_t bufsize)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)buf;
	nleft = bufsize;
	while (nleft > 0) 
	{
		if ( (nread = recv(fd, ptr, nleft, 0)) < 0) 
		{
			if (errno == EINTR)/* and call recv() again */
				nread = 0;		
			else if(errno == EAGAIN || errno == EWOULDBLOCK)//recv all can recv.
				break;
			else //some error.
				return -1;
		} 
		else if (nread == 0) //EOF. recv all.
			break;				

		nleft -= nread;
		ptr   += nread;
	}
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


bool CEP::checkEvent(CEP_Event & cep_ev)
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

bool CEP::addEvent(CEP_Event & cep_ev)
{
	if(!checkEvent(cep_ev)) 
		return false;
	
	struct epoll_event ep_ev;
	ep_ev.data.ptr = &cep_ev;
	switch(cep_ev.type)
	{
		case CEPEvType_Connect:
		case CEPEvType_Send:
			ep_ev.events = EPOLLOUT | EPOLLET;
			break;
		case CEPEvType_Listen:
		case CEPEvType_Recv:
			ep_ev.events = EPOLLIN | EPOLLET;
			break;
		default:
			return false;
	}
	
	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, cep_ev.fd, &ep_ev) != 0)
		return false;
	
	cep_ev.last_active = time(NULL); 
	m_events.push_back(cep_ev);
	
	return true;
}

bool CEP::modEvent(CEP_Event & cep_ev)
{
	struct epoll_event ep_ev;
	ep_ev.data.ptr = &cep_ev;
	switch(cep_ev.type)
	{
		case CEPEvType_Connect:
		case CEPEvType_Send:
			ep_ev.events = EPOLLOUT | EPOLLET;
			break;
		case CEPEvType_Listen:
		case CEPEvType_Recv:
			ep_ev.events = EPOLLIN | EPOLLET;
			break;
		default:
			delEvent(cep_ev);
			return false;
	}
	
	cep_ev.last_active = time(NULL); 
	
	return epoll_ctl(m_epfd, EPOLL_CTL_MOD, cep_ev.fd, &ep_ev);
}

bool CEP::delEvent(size_t index)
{
	if(index >= m_events.size())
		return false;
	std::vector<CEP_Event>::iterator it = m-events.begin() + index;
	bool res = epoll_ctl(m_epfd, EPOLL_CTL_DEL, it->fd, NULL);
	m_events.erase(it);
	return res;
}

bool CEP::delEvent(CEP_Event & cep_ev)
{
	std::vector<CEP_Event>::iterator it = find(m_events.begin(), m_events.end(), cep_ev);
	if(it != m_events.end())//find.
	{
		bool res = epoll_ctl(m_epfd, EPOLL_CTL_DEL, cep_ev.fd, NULL);
		m_events.erase(it);
		return res;
	}
	return false;
}

void CEP::handleEvent(CEP_Event & cep_ev, uint32_t epoll_event_events, bool * quit_epoll_wait)
{
	switch(cep_ev.type)
	{
		case CEPEvType_Connect:
			if(epoll_event_events & EPOLLOUT)
				handleEvent4TypeConnect(cep_ev, quit_epoll_wait);
			break;
		case CEPEvType_Listen:
			if(epoll_event_events & EPOLLIN)
				handleEvent4TypeListen(cep_ev, quit_epoll_wait);
			break;
		case CEPEvType_Send:
			if(epoll_event_events & EPOLLOUT)
				handleEvent4TypeSend(cep_ev, quit_epoll_wait);
			break;
		case CEPEvType_Recv:
			if(epoll_event_events & EPOLLIN)
				handleEvent4TypeRecv(cep_ev, quit_epoll_wait);
			break;
	}
}

void CEP::handleEvent4TypeConnect(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	int optval;
	socklen_t optlen = sizeof optval;
	if(getsockopt(cep_ev.fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0 && optval == 0)
	{//connect success.
		if(cep_ev.event_done_call_back != NULL)
			event_done_call_back(cep_ev, *this, quit_epoll_wait);
	}
}

void CEP::handleEvent4TypeListen(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int clientfd;
	if((clientfd = accept(listenfd, (struct sockaddr*)&addr, &addrlen)) == -1)
	{
		if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			std::cerr<<"accept() failed.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
		}
		break;
	}	
	cep_ev.newClientFd = clientfd;
	if(cep_ev.event_done_call_back != NULL)
		event_done_call_back(cep_ev, *this, quit_epoll_wait);
}

void CEP::handleEvent4TypeSend(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	if(sendn(cep_ev.fd, cep_ev.buf, cep_ev.len) != -1 && cep_ev.event_done_call_back != NULL)
		event_done_call_back(cep_ev, *this, quit_epoll_wait);
}

void CEP::handleEvent4TypeRecv(CEP_Event & cep_ev, bool * quit_epoll_wait)
{
	if(recvn(cep_ev.fd, cep_ev.buf, MAXDATABUFSIZE) != -1 && cep_ev.event_done_call_back != NULL)
		event_done_call_back(cep_ev, *this, quit_epoll_wait);
}

void CEP::checkTime()
{
	// a simple timeout check here, every time 100, better to use a mini-heap, and add timer event  
    long now = time(NULL);  
	
	size_t n = 100 < m_events.size() ? 100 : m_events.size();
	for(size_t i = 0; i < j; i++, m_checkPos++) 
	{
		if(m_checkPos >= m_events.size()) 
			m_checkPos = 0; // recycle  
			  	
		if(m_events[m_checkPos].type != CEPEvType_Listen)// doesn't check listen fd  
		{
			long duration = now - m_events[m_checkPos].last_active;  
			if(duration > 60) // 60s timeout  
			{  
				std::cout<<"fd("<<m_events[m_checkPos].fd<<")timeout("
				<<m_events[m_checkPos].last_active<<"--"<<now<<")."<<std::endl;  
				delEvent(m_checkPos);
				close(m_events[checkPos].fd);  
			}
		}  
	}  
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
		checkTime();
		
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
			std::cerr<<"epoll_wait() faild.error["<<errno<<"]:"<<strerror(errno)<<std::endl;
			return -1;
		}
		
		CEP_Event * p_cep_ev;
		bool quit_epoll_wait(0);
		for(int i = 0; i != nReady; i++, quit_epoll_wait = false)
		{
			p_cep_ev = ep_evs_buf[i].data.ptr;
			handleEvent(*p_cep_ev, ep_evs_buf[i].events, &quit_epoll_wait);
			if(quit_epoll_wait)
				return 0;
		}	
	}
	return 0;
}
