#include "CEP.h"
#include "LOG.H"
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;


uint16_t g_port = 9002;


void callback4cepev(CEP_Event & cep_ev, CEP & cep, bool handledSuccess, bool * quit_epoll_wait)
{
	switch(cep_ev.type)
	{
		case CEP_Event::Type_Listen:
		{
			if(handledSuccess)
			{
				if(cep_ev.newClientFd == -1)
					return;
				
				cout<<"new client connect.fd ="<<cep_ev.fd<<endl;
		
				CEP_Event cep_ev_recv(cep_ev.newClientFd, CEP_Event::Type_Recv, callback4cepev);
				if(!cep.addEvent(cep_ev_recv))
				{
					cout<<"CEP::addEvent() wrong!"<<endl;
					exit(-1);
				}
			}
			else
			{
				cout<< "wrong when accept()" <<endl;
			}
		}
		break;
		case CEP_Event::Type_Recv:
		{
			if(cep_ev.len < 0)
			{
				cout<<"wrong in recv event"<<endl;
			}
			else if(cep_ev.len > 0)
			{
				cout<<"fuck:"<<cep_ev.len<<endl;
				char * p = cep_ev.buf + 10;
				*p = 0;
				printf("[[%s\n", cep_ev.buf);
				//sprintf(p, "xxx%s", "22");
				//cep_ev.buf[10] = 0;
				//printf("[%s]len = %d\n", cep_ev.buf, cep_ev.len);
				//cout<<"recv:["<< cep_ev.buf <<"]len = "<<cep_ev.len<<endl;
			}
		}
		break;
		default:
		cout<<"what's the metter?"<<endl;
	}
}

int main()
{
	LOG_GLOBAL_INIT(0 ,0, 0);
#if 0
	struct rlimit rt;
	/* 设置每个进程允许打开的最大文件数 */
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
	if( setrlimit(RLIMIT_NOFILE, &rt) == -1 )//need sudo.
	{
		cout<< "setrlimit faild.errno=" <<errno<<"."<<strerror(errno) <<endl;
		return -1;
	}
#endif

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
		return -1;
	
	struct sockaddr_in servaddr;
	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(g_port);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		cout<<"error: bind failed."<<endl;
		return -1;
	}

	if(listen(listenfd, 128) == -1)
	{
		cout<<"error: listen failed."<<endl;
		close(listenfd);
		return -1;
	}
	else
		cout << "listen in " << g_port << endl;
	
	//epoll~
	CEP cep;
	CEP_Event cep_ev(listenfd, CEP_Event::Type_Listen, callback4cepev);

	if(!cep.addEvent(cep_ev))
	{
		cout<< "CEP::addEvent() failed!" <<endl;
		return -1;
	}

	//epoll_wait().
	int rtn = cep.runloop_epoll_wait();
	
	if(rtn == -1)
		cout<< "CEP::runloop_epoll_wait() failed!" <<endl;

	//
	close(listenfd);
	return 0;
}



