#include "CEP.h"
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

using namespace std;

uint16_t g_port = 9010;

void callback(CEPEvent & cep_ev, CEP & cep, bool handledSuccess, bool * quit_epoll_wait)
{
	if(!handledSuccess)
	{
		cout<<"handled not success. CEPEvent::Type = "<<cep_ev.type<<endl;
		cep.delEvent(cep_ev);
		*quit_epoll_wait = true;
		return;
	}
	
	switch(cep_ev.type)
	{
		case CEPEvent::Type_Connect:
		{
			char * buf = cep_ev.buf();
			assert(buf);
			cep_ev.len = 1 + snprintf(buf, cep_ev.bufsize(), "this is buffer of client fd %d", cep_ev.fd);			
			cep.modEvent(cep_ev, CEPEvent::Type_Send);
		}
		break;
		case CEPEvent::Type_Send:
		{
			if(cep_ev.len > 0)
				cout<<"sended data:["<< cep_ev.buf() << "]len = "<<cep_ev.len<<endl;			
			cep.modEvent(cep_ev, CEPEvent::Type_Recv);
		}
		break;
		case CEPEvent::Type_Recv:
		{
			cout<<"Type_Recv came. cep_ev.len = "<<cep_ev.len<<endl;
			if(cep_ev.len > 0)
				cout<<"recv data:["<< cep_ev.sharedBuffer << "]len="<<cep_ev.len<<endl;
			else if(cep_ev.len < 0)
			{
				cout<<"recv data len < 0~ may be 心跳断了 ？～～"<<endl;
				cep.delEvent(cep_ev);
			}
		}
		break;
		default:
		cout<<"what's the matter?"<<endl;
	}
}

int main()
{
	bool b;
	CEP cep(0);

	const size_t nConn(1);
	int fds[nConn];

	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(g_port);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	
	for(int i = 0; i != nConn; i++)
	{
		fds[i] = socket(AF_INET, SOCK_STREAM, 0);
		if(fds[i] == -1)
			return -1;
		
		CEP::SetNonBlocking(fds[i]);
		
		//set tcp keepalive
		if(!CEP::SetKeepAlive(fds[i], 5, 2, 4))
		{	
			cout<<"CEP::SetKeepAlive() failed"<<endl;
			exit(-1);
		}
		
		if(connect(fds[i], (struct sockaddr *)&addr, sizeof addr) == -1)
		{
			if(errno == EINPROGRESS)
			{//add connect event:	
				cout<<"add Type_Connect CEPEvent."<<endl;
				b = cep.addEvent(CEPEvent(fds[i], CEPEvent::Type_Connect, callback));
				if(!b)
				{
					cout<<"add faild.may be server can't reach."<<endl;
				}
			}
			else
			{
				cout<<"connect error. error("<<errno<<"):"<<strerror(errno)<<endl;
				return -1;
			}
		}
		else
		{//connect success already. add send event:
			char buf[128];
			size_t len = 1 + snprintf(buf, 128, "this is buffer of client fd %d", fds[i]);
			
			b = cep.addEvent(CEPEvent(fds[i], CEPEvent::Type_Send, callback, buf, len));
			assert(b);
		}
	}
	
	int res = cep.runloop_epoll_wait();
	if(res == -1)
		cout<<"CEP::runloop_epoll_wait() failed!" <<endl;
	else
		cout<<"CEP::runloop_epoll_wait() return "<< res <<endl;
	
	return 0;
}
