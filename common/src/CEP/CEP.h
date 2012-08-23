/**
 * @file CEP.h
 * @brief epoll ET模式封装.
 * @note  1. need kernel 2.6.9
 * @warning no warning I think.
 * @author xiaoU.
 * @date 2012 E.O.W.
 * @version 1.74
 * @par 修改记录：
 * 	-1.73:
 * 
 *  TO DO....TODO:
 * 	未来优化记录：
 * 	1.优化delEvent的逻辑.看能不能不搞循环这种耗时费力的事儿了.
 * 	2.粘包的问题.这个目前的业务逻辑实际不会发生.为了健壮性以后可以这样优化：recv的时候头两位作为数据包长度.(已局部测试，此法有效.)
 * 
 * 	-1.3:加入设置 tcp keepalive的帮助函数.但是暂时没有在类的内部使用.
 *  -1.2:构造函数增加参数timeout.
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


#define	 MAXDATABUFSIZE	   1024 /**< 一个事件的数据buffer最大1kb,太大了悲剧，会分包(分包我不管).*/


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
		/**@note 这epoll的ET模式.本地测试发现：当你收到事件后，需要mod这个事件，否则这个事件可能会不能再收到事件消息. */
		Type_Connect, /**< 连接成功或失败后调用回调函数。
		无论成功失败，此事件仅一次(epoll_ctl之EPOLLONESHOT),此后会自动delEvent这个事件.除非你在回调函数里modEvent这个事件(这也是客户代码的正常行为).*/
		Type_Listen, /**< accept成功或失败后调用回调函数。newClientFd被赋值(即使回调的handledSuccess参数为true，newClientFd仍然需要判断是否为-1). */
		Type_Send, /**< 成功或失败的发送完数据后调用回调函数.len为实际发送的长度.(可能为-1，此时handledSuccess参数为false.建议这时客户代码delEvent.因为八成对端fd已经close了或者error了). */
		Type_Recv, /**< 成功或失败的收完缓冲区里的全部数据后调用回调函数.len为实际收到的长度.(可能为-1，此时handledSuccess参数为false.建议这时客户代码delEvent.因为八成对端fd已经close了或者error了). */
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
	///辅助函数:
	//
	inline char * buf(){ return sharedBuffer == NULL ? NULL : sharedBuffer.get(); }/**< the buffer size is MAXDATABUFSIZE. */
	inline size_t bufsize(){ return sharedBuffer == NULL ? 0 : MAXDATABUFSIZE; }
	inline void coutthis(){ std::cout<<this<<std::endl; }
	inline void coutCanRemove(){ std::cout<<"canRemoveFromArray="<<canRemoveFromArray<<std::endl; }
	
	///constructor
	//
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


/** @brief epoll.ET模式.fd非阻塞(addEvent的时候会check如果不是O_NONBLOCK会设置它的). */
class CEP
{
public:
	/*
	 * maxConnTimeout is timeout. see \c m_maxConnTimeout .如果为0则不检测超时.
	 * 用了这个构造函数无论参数如何，在添加事件的时候将不自动设置套接子的tcp心跳.因为俩构造函数是两种超时策略，两策略只取一种.
	 */
	CEP(size_t maxConnTimeout = 0); 
	/* 
	 * 设置心跳.如果setKeepAlive = false，后面的参数将无意义.
	 * 后面三个参数意义请 see \c CEP::SetKeepAlive .
	 * 用了这个构造函数那么m_maxConnTimeout就等于0. CEP(0)实际等效于CEP(0,0,0,0)
	 */
	CEP(bool setKeepAlive, size_t keepalive_time, size_t keepalive_intvl, size_t keepalive_probes);
	
	virtual ~CEP(){ close(m_epfd); }
	
	virtual bool addEvent(CEPEvent cep_ev);
	virtual bool modEvent(CEPEvent & cep_ev);/**< @note mod事件如果失败会自动delEvent. */
	virtual bool modEvent(CEPEvent & cep_ev, CEPEvent::Type newType){ cep_ev.type = newType; return modEvent(cep_ev); }
	virtual bool delEvent(CEPEvent & cep_ev);/**< 删除epoll事件并close事件的fd，稍后将从数组里移除. */
	virtual bool delEvent(size_t index);/**< 删除epoll事件并close事件的fd，并马上从数组里移除. */
	virtual int runloop_epoll_wait();/**< @retval -1 error. @retval 0 the array is empty. @retval 1 user let the loop return. */
	
	size_t currEventsNum(){ return m_events.size(); }


	///user help functions.
	//
	/** set maximum file descriptor number that can be opened by this process. */
	static bool SetMaximumNumberFilesOpened(size_t num);
	/** check socket error */
	static bool SocketIsError(int sockfd);
	/** set the socked to nonblock if it's block. */
	static bool SetNonBlocking(int sockfd);
	/*
	 * @brief set the socket(for tcp) keepalive options.
	 * 会检测是不是tcp socket.
	 * @param[in] tcp_keepalive_time 探测心跳的时间周期,seconds. 
	 * 如果为0则不设置这个参数，否则将在该套接子上覆盖之前(如果设置过)的参数以及系统的tcp心跳参数:cat /proc/sys/net/ipv4/tcp_keepalive_time
	 * @param[in] tcp_keepalive_intvl 测心跳时的超时时间,seconds. 
	 * 如果为0则不设置这个参数，否则将在该套接子上覆盖之前(如果设置过)的参数以及系统的tcp心跳参数:cat /proc/sys/net/ipv4/tcp_keepalive_intvl
	 * @param[in] tcp_keepalive_probes 一次检测中发送心跳包的次数.
	 * 如果为0则不设置这个参数，否则将在该套接子上覆盖之前(如果设置过)的参数以及系统的tcp心跳参数:cat /proc/sys/net/ipv4/tcp_keepalive_probes
	 */
	static bool SetKeepAlive(int sockfd, size_t tcp_keepalive_time = 0, size_t tcp_keepalive_intvl = 0, size_t tcp_keepalive_probes = 0);
	/** recvn */
	static ssize_t Recvn(int fd, char *buf, size_t bufsize);
	/** sendn */
	static ssize_t	Sendn(int fd, char *buf, size_t len);
	//
	bool socketIsError(int sockfd){ return CEP::SocketIsError(sockfd); }
	bool setNonBlocking(int sockfd){ return CEP::SetNonBlocking(sockfd); }
	bool setKeepAlive(int sockfd, int tcp_keepalive_time = 0, int tcp_keepalive_intvl = 0, int tcp_keepalive_probes = 0)
	{ return CEP::SetKeepAlive(sockfd, tcp_keepalive_time, tcp_keepalive_intvl, tcp_keepalive_probes); }
	ssize_t	recvn(int fd, char *buf, size_t bufsize){ return CEP::Recvn(fd, buf, bufsize); }
	ssize_t	sendn(int fd, char *buf, size_t len){ return CEP::Sendn(fd, buf, len); }
	

protected:
	virtual bool setEvent4epoll_event(CEPEvent & cep_ev, int epoll_ctl_op);
	virtual void checkTimeAndRemove();
	virtual bool checkEventSocket(CEPEvent & cep_ev);/**< 检查CEPEvent::fd套接字的SO_ERROR选项.
	确保设置(了)O_NONBLOCK.  如果构造函数使用的是心跳超时策略并且不是CEPEvent::Type_Listen型的事件,就会确保套接字设置(了)心跳.*/
	virtual void handleEvent(CEPEvent & cep_ev, uint32_t epoll_event_events, bool * quit_epoll_wait);
	virtual void handleEvent4TypeConnect(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeListen(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeSend(CEPEvent & cep_ev, bool * quit_epoll_wait);
	virtual void handleEvent4TypeRecv(CEPEvent & cep_ev, bool * quit_epoll_wait);
	
	int m_epfd;
	std::vector<CSharedCEPEventPtr> m_events;
	size_t m_checkPos;
	size_t m_maxConnTimeout;/**< seconds. 如果客户连接这么久没任何数据到来，就认为超时了,会关掉连接.如果为0则不检测超时. */
	bool m_keepalive;
	size_t m_keepalive_time;
	size_t m_keepalive_intvl;
	size_t m_keepalive_probes;
};

#endif // #ifndef _CEP_H_

