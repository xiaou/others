#include <iostream>
#include <tr1/memory>
#include <set>
#include <vector>
#include <string.h>
#include <assert.h>
#include <stdint.h>


using namespace std;
using std::tr1::shared_ptr;


class CA{};



int main()
{	
	uint32_t maxUInt32 = 4294967295;
	
	maxUInt32 = 0xFFFFFFFF;
	
	cout << maxUInt32 << endl;
	
	return 0;
}
