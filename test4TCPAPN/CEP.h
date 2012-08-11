///need kernel2.6.9


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


#define	 MAXDATABUFSIZE	   1024 /**< 一个事件的数据buffer最大1kb,太大了悲剧，会分包(分包我不管).*/

struct CEP_Event
{
	

};

/** @brief epoll.ET模式.fd必须非阻塞. */
class CEP
{
	public:
	CEP(){ if((m_epfd = epoll_create(1)) == -1) std::cerr<<"faild in CEP()."<<std::endl; }
	virtual ~CEP(){ close(m_epfd); }
	virtual int addEvent(CEP_Event * cep_ev);
	virtual int modEvent(CEP_Event * cep_ev);
	virtual int delEvent(CEP_Event * cep_ev);
	virtual int waitEvents();

	protected:
	int m_epfd;
};

#endif // #ifndef _CEP_H_

