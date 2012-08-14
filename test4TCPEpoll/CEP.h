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
#include <tr1/memory>
#if 1 //test
using namespace std;
#endif


#define	 MAXDATABUFSIZE	   1024 /**< 一个事件的数据buffer最大1kb,太大了悲剧，会分包(分包我不管).*/
#define  MAXCONNTIMEOUT	   120/*senconds*//**< 如果客户连接这么久没任何数据到来，就认为超时了,会关掉连接. */


class CEP;
class CEPEvent;
typedef void (*CEPEventDoneCallback)(CEPEvent &, CEP &, bool handledSuccess, bool * quit_epoll_wait/* default is false. */);  
typedef std::tr1::shared_ptr<CEPEvent>  CSharedCEPEventPtr;

//
struct CEPEvent
{	
public:
	int fd;  
	int newClientFd;// if it's Type_Listen typed CEPEvent, when event come, it'll accept a new client fd.
	
	enum Type
	{
		Type_Connect, /**< 连接成功或失败后调用回调函数。无论成功失败，此事件仅一次(EPOLLONESHOT),此后会自动把他移除.除非在回调里addEvent或modEvent.*/
		Type_Listen, /**< accept成功或失败后调用回调函数。newClientFd被赋值(即使回调的handledSuccess参数为true，newClientFd仍然需要判断是否为-1). */
		Type_Send, /**< 成功或失败的发送完数据后调用回调函数.len为实际发送的长度.(可能为-1，此时handledSuccess参数为false). */
		Type_Recv, /**< 成功或失败的收完缓冲区里的全部数据后调用回调函数.len为实际收到的长度.(可能为-1，此时handledSuccess参数为false). */
	} type;
    
    CEPEventDoneCallback callback;
    
    typedef std::tr1::shared_ptr<char> CSharedBuffer;
    CSharedBuffer sharedBuffer;   
    int len;  
    
private:
 	long last_active; // last active time 
	bool canRemoveFromArray;
	friend class CEP;   

public:
    CEPEvent()
	:newClientFd(-1), callback(NULL), len(0), canRemoveFromArray(false)
    { 
		mallocBuffer(); 
	}
    
    CEPEvent(int fdA, Type typeA, CEPEventDoneCallback callbackA = NULL, 
			char bufA[] = NULL, int lenA = 0)
	:fd(fdA), type(typeA), callback(callbackA), len(0), newClientFd(-1), canRemoveFromArray(false)
	{
		mallocBuffer(); 
		
		if((len = lenA) > MAXDATABUFSIZE)
			len = MAXDATABUFSIZE;
		memcpy(sharedBuffer.get(), bufA, len);
	}
	
	~CEPEvent()
	{ 
	}
	#if 1
	void coutthis(){ std::cout<<this<<std::endl; }
	#endif
private:
	static void releaseBuffer(char * buf){  if(buf) delete []buf;  }

	void mallocBuffer()
	{
		if(sharedBuffer == NULL) 
		{
			char * buf = new char[MAXDATABUFSIZE];
			if(!buf)
			{
				std::cerr<<"new char["<<MAXDATABUFSIZE<<"] faild"<<std::endl;
				fd = -1;
			}
			else
				sharedBuffer = CSharedBuffer(buf, releaseBuffer);
		}
	}	
};


/** @brief epoll.ET模式.fd非阻塞. */
class CEP
{
public:
	CEP();
	virtual ~CEP(){ close(m_epfd); }
	virtual bool addEvent(CEPEvent & cep_ev);
	virtual bool modEvent(CEPEvent & cep_ev);
	virtual bool delEvent(CEPEvent & cep_ev);/**< 为了效率，在删除epoll事件后并不从数组里移除，仅把canRemoveFromArray设为true，在check */
	virtual bool delEvent(size_t index);/**< 删除epoll事件，并从数组里移除. */
	size_t currEventsNum(){ return m_events.size(); }
	virtual int runloop_epoll_wait();/**< retval -1 error. */

protected:
	virtual bool setEvent4epoll_event(CEPEvent & cep_ev, int epoll_ctl_op);
	virtual void checkTimeAndRemove();		
	virtual bool checkEventSocket(CEPEvent & cep_ev);/**< 检查CEPEvent::fd套接字的SO_ERROR选项，并确保O_NONBLOCK. */
	virtual void handleEvent(CEPEvent & cep_ev, uint32_t epoll_event_events, bool * quit_epoll_wait);
	virtual void handleEvent4TypeConnect(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeListen(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeSend(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeRecv(CEPEvent & cep_ev, bool * quit_epoll_wait);
		
	inline bool setNonBlocking(int sockfd);
	ssize_t	recvn(int fd, char *buf, size_t bufsize);
	ssize_t	sendn(int fd, char *buf, size_t len);	
	
	int m_epfd;
	std::vector<CSharedCEPEventPtr> m_events;
	size_t m_checkPos;
};

#endif // #ifndef _CEP_H_

