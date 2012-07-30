#include "LOG.H"
#include "iostream"

int main()
{
	LOG_GLOBAL_INIT(0, 0, LL_FATAL_LOG_LEVEL);

	LOG_FATAL("TESt  fatal!");
	LOG_ERROR("TEst error!");
	LOG_WARN("teST warn!");
	LOG_DEBUG("test debug");
	
	std::cout<<"return 0;"<<std::endl;
	return 0;
}