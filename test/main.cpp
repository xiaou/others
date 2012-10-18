#include <iostream>
#include <tr1/memory>
#include <set>
#include <map>
#include <vector>
#include <string.h>
#include <assert.h>
#include <stdint.h>


using namespace std;
using std::tr1::shared_ptr;


class CA
{
	vector<int> m_v;
	
	public:
	CA()
	{
		cout << this->m_v.size() << endl;
	}
};


int main()
{	
	CA a;
	
	return 0;
}
