/*
 * 
 * note: 1. need kernel2.6.9
 *       
 */

#ifndef _CEP_H_
#define _CEP_H_

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
#include <vector>

#define	 MAXDATABUFSIZE	   1024 /**< 一个事件的数据buffer最大1kb,太大了悲剧，会分包(分包我不管).*/

enum CEPEvType
{
	CEPEvType_Connect;
	CEPEvType_Listen;
	CEPEvType_Send;
	CEPEvType_Recv;
};

struct CEP_Event
{
	int fd;  
	CEPEvType type;
    void (*call_back)(int fd, int events, void *arg);  
    int events;  
    //void *arg;  
    //int status; // 1: in epoll wait list, 0 not in  
    char buf[MAXDATABUFSIZE];   
    int len;  
    long last_active; // last active time
};

/** @brief epoll.ET模式.fd非阻塞. */
class CEP
{
public:
	CEP(){ if((m_epfd = epoll_create(1)) == -1) std::cerr<<"faild in CEP().error["<<errno<<"]:"<<strerror(errno)<<std::endl; }
	virtual ~CEP(){ close(m_epfd); }
	virtual int addEvent(CEP_Event * cep_ev);
	virtual int modEvent(CEP_Event * cep_ev);
	virtual int delEvent(CEP_Event * cep_ev);
	size_t currEventsNum(){ return m_events.size(); }
	virtual int runloop_epoll_wait();/**< retval -1 error. */

protected:
	virtual void checkTime();		
	virtual void handleEvent(struct epoll_event * ep_ev);
		
	int m_epfd;
	std::vector<CEP_Event> m_events;
};

#endif // #ifndef _CEP_H_

