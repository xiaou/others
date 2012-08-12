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
	CEPEvType_Connect, /**< 连接成功后调用回调函数。若失败在超时后会close套接字（客户代码不用考虑会失败了）。*/
	CEPEvType_Listen, /**< accept成功后调用回调函数。废话失败了当然继续监听啊。（客户代码不爽可以继承CEP自己修改行为）。 */
	CEPEvType_Send, /**< 发送完数据后调用回调函数 */
	CEPEvType_Recv, /**< 收完缓冲区里的全部数据后调用回调函数。*/
};

struct CEP_Event
{
	int fd;  
	int newClientFd;// if it's CEPEvType_Listen typed CEP_Event, when event come, it'll accept a new client fd.
	CEPEvType type;
    void (*event_done_call_back)(CEP_Event &, CEP &, bool * quit_epoll_wait/* default is false. */);  
    char buf[MAXDATABUFSIZE];   
    int len;  
    long last_active; // last active time 
    
    CEP_Event():newClientFd(-1), event_done_call_back(NULL){}
};

/** @brief epoll.ET模式.fd非阻塞. */
class CEP
{
public:
	CEP(){ if((m_epfd = epoll_create(1)) == -1) std::cerr<<"faild in CEP().error["<<errno<<"]:"<<strerror(errno)<<std::endl; }
	virtual ~CEP(){ close(m_epfd); }
	virtual bool addEvent(CEP_Event & cep_ev);
	virtual bool modEvent(CEP_Event & cep_ev);
	virtual bool delEvent(CEP_Event & cep_ev);
	virtual bool delEvent(size_t index);
	size_t currEventsNum(){ return m_events.size(); }
	virtual int runloop_epoll_wait();/**< retval -1 error. */

protected:
	virtual void checkTime();		
	virtual bool checkEvent(CEP_Event & cep_ev);
	virtual void handleEvent(CEP_Event & cep_ev, uint32_t epoll_event_events, bool * quit_epoll_wait);
	virtual void handleEvent4TypeConnect(CEP_Event & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeListen(CEP_Event & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeSend(CEP_Event & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeRecv(CEP_Event & cep_ev, bool * quit_epoll_wait);
		
	int m_epfd;
	std::vector<CEP_Event> m_events;
	size_t m_checkPos;
	
	///help functions
	//
	inline bool setNonBlocking(int sockfd)
	{
		return (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0)|O_NONBLOCK) != -1);
	}
	ssize_t	recvn(int fd, char *buf, size_t bufsize);
	ssize_t	sendn(int fd, char *buf, size_t len);
};

#endif // #ifndef _CEP_H_

