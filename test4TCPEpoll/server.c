#include "CEP.h"
#include "LOG.H"
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;


uint16_t g_port = 9011;


void callback4cepev(CEPEvent & cep_ev, CEP & cep, bool handledSuccess, bool * quit_epoll_wait)
{
	if(!handledSuccess)
		return;
		
	switch(cep_ev.type)
	{
		case CEPEvent::Type_Listen:
		{
			if(cep_ev.newClientFd == -1)
				return;
			
			cout<<"new client connect.fd ="<<cep_ev.newClientFd<<".by listen fd:"<<cep_ev.fd<<endl;

			CEPEvent cep_ev_recv(cep_ev.newClientFd, CEPEvent::Type_Recv, callback4cepev);
			if(!cep.addEvent(cep_ev_recv))
			{
				cout<<"CEP::addEvent() wrong!"<<endl;
				exit(-1);
			}	
		}
		break;
		case CEPEvent::Type_Recv:
		{
			if(cep_ev.len > 0)
			{
				cout<<"recv by fd "<< cep_ev.fd <<":["<<cep_ev.sharedBuffer<<"]len="<<cep_ev.len<<endl;
				cep.modEvent(cep_ev, CEPEvent::Type_Send);
			}
		}
		break;
		case CEPEvent::Type_Send:
		{
			if(cep_ev.len > 0)
				cout<<"sended by fd "<< cep_ev.fd << ":["<<cep_ev.sharedBuffer<<"]len="<<cep_ev.len<<endl;
			
			cep.delEvent(cep_ev);
		}
		break;
		default:
		cout<<"what's the metter?"<<endl;
	}
}

int main()
{
	LOG_GLOBAL_INIT(0 ,0, 0);

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
		cout << "fd("<<listenfd<<")"<<"listen in " << g_port << endl;
	
	//epoll~
	CEP cep;
	//cep.setMaximumNumberFilesOpened(1000*1000);
	CEPEvent cep_ev(listenfd, CEPEvent::Type_Listen, callback4cepev);

	if(!cep.addEvent(cep_ev))
	{
		cout<< "CEP::addEvent() failed!" <<endl;
		return -1;
	}

	//epoll_wait().
	int rtn = cep.runloop_epoll_wait();
	
	if(rtn == -1)
		cout<< "CEP::runloop_epoll_wait() failed!" <<endl;
	else 
		cout<< "CEP::runloop_epoll_wait() return "<<rtn<<endl;

	//
	close(listenfd);
	return 0;
}



