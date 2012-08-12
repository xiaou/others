#include "CEP.h"




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
		for(int i = 0; i != nReady; i++)
		{
			handleEvent(&ep_evs_buf[i]);
		}
		
	}
	return 0;
}