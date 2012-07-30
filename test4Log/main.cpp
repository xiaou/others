#include "LOG.H"
#include "iostream"

int main()
{
	LOG_GLOBAL_INIT(1, 0, LOG_LEVEL_DEBUG);

	LOG_FATAL("TESt  fatal!");
	LOG_ERROR("TEst error!");
	LOG_WARN("teST warn!");
	LOG_DEBUG("test debug");
	
	std::cout<<"return 0;"<<std::endl;
	return 0;
}
