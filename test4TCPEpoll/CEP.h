/*
 * 
 * note: 1. need kernel2.6.9
 *       
 */

#ifndef _CEP_H_
#define _CEP_H_


#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>


#define	 MAXDATABUFSIZE	   1024 /**< 一个事件的数据buffer最大1kb,太大了悲剧，会分包(分包我不管).*/


class CEP;

class CEP_Event;

typedef void (*CEP_Event_DoneCallback)(CEP_Event &, CEP &, bool handledSuccess, bool * quit_epoll_wait/* default is false. */);  

//
struct CEP_Event
{	
	int fd;  
	int newClientFd;// if it's Type_Listen typed CEP_Event, when event come, it'll accept a new client fd.
	
	enum Type
	{
		Type_Connect, /**< 连接成功或失败后调用回调函数。无论成功失败，此事件仅一次(EPOLLONESHOT),此后将把他移除.除非在回调里addEvent或modEvent.*/
		Type_Listen, /**< accept成功或失败后调用回调函数。newClientFd被赋值(即使回调的handledSuccess参数为true，newClientFd仍然需要判断是否为-1). */
		Type_Send, /**< 成功或失败的发送完数据后调用回调函数.len为实际发送的长度.(可能为-1，此时handledSuccess参数为false). */
		Type_Recv, /**< 成功或失败的收完缓冲区里的全部数据后调用回调函数.len为实际收到的长度.(可能为-1，此时handledSuccess参数为false). */
	} type;
    
    CEP_Event_DoneCallback callback;
    
    char * buf;   
    int len;  
    
    ///
    //
    CEP_Event()
	:newClientFd(-1), callback(NULL), buf(NULL), len(0), canRemoveFromArray(false)
    { 
		mallocBuffer(); 
	}
    
    CEP_Event(int fdA, Type typeA, CEP_Event_DoneCallback callbackA = NULL, 
			char bufA[] = NULL, int lenA = 0)
	:fd(fdA), type(typeA), callback(callbackA), buf(NULL), len(0), newClientFd(-1), canRemoveFromArray(false)
	{
		mallocBuffer(); 
		
		if((len = lenA) > MAXDATABUFSIZE)
			len = MAXDATABUFSIZE;
		memcpy(buf, bufA, len);
	}
	
	~CEP_Event()
	{ 
		if(buf)
			delete []buf;
	}
	
	///复制控制
	//
	CEP_Event(const CEP_Event & a)
	:fd(a.fd), type(a.type), callback(a.callback), buf(NULL), len(a.len), newClientFd(a.newClientFd), 
	canRemoveFromArray(a.canRemoveFromArray), last_active(a.last_active)
	{
		this->mallocBuffer(); 
		if(this->buf) 
			memcpy(this->buf, a.buf, a.len); 
	}
	CEP_Event & operator=(const CEP_Event & a)
	{   
		this->fd = a.fd;
		this->type = a.type; 
		this->callback = a.callback;
		this->buf = NULL;
		this->len = a.len; 
		this->newClientFd = a.newClientFd;
		this->canRemoveFromArray = a.canRemoveFromArray;
		this->last_active = a.last_active;
	
		this->mallocBuffer(); 
		if(this->buf) 
			memcpy(this->buf, a.buf, a.len); 
		return *this;
	}
	
private:
	void mallocBuffer()
	{
		if(buf == NULL)
		{
			if((buf = new char[MAXDATABUFSIZE]) == NULL)
			{
				std::cerr<<"malloc() faild."<<std::endl;
				fd = -1;
			}
		}
	}
	
	long last_active; // last active time 
	bool canRemoveFromArray;
	friend class CEP;
	
};


/** @brief epoll.ET模式.fd非阻塞. */
class CEP
{
public:
	CEP();
	virtual ~CEP(){ close(m_epfd); }
	virtual bool addEvent(CEP_Event & cep_ev);
	virtual bool modEvent(CEP_Event & cep_ev);
	virtual bool delEvent(CEP_Event & cep_ev);/**< 为了效率，在删除epoll事件后并不从数组里移除，仅把canRemoveFromArray设为true，在check */
	virtual bool delEvent(size_t index);/**< 删除epoll事件，并从数组里移除. */
	size_t currEventsNum(){ return m_events.size(); }
	virtual int runloop_epoll_wait();/**< retval -1 error. */

protected:
	virtual bool setEvent4epoll_event(CEP_Event & cep_ev, int epoll_ctl_op);
	virtual void checkTimeAndRemove();		
	virtual bool checkEventSocket(CEP_Event & cep_ev);/**< 检查CEP_Event::fd套接字的SO_ERROR选项，并确保O_NONBLOCK. */
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
	inline bool setNonBlocking(int sockfd);
	ssize_t	recvn(int fd, char *buf, size_t bufsize);
	ssize_t	sendn(int fd, char *buf, size_t len);
};

#endif // #ifndef _CEP_H_

