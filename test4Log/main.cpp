#include "LOG.H"
#include "iostream"

int main()
{
	//LOG_GLOBAL_INIT(1, 0, LOG_LEVEL_DEBUG);
	LOG_FATAL("xx");

	for(int i = 0; i != 1; i++);
	
	std::cerr<<"test cerr"<<std::endl;

	std::cout<<"return 0;"<<std::endl;
	return 0;
}
