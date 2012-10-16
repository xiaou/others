#include <iostream>
#include <tr1/memory>
#include <set>
#include <vector>

using namespace std;
using std::tr1::shared_ptr;

class CA{ public: char m_szBuf[128]; };

int main()
{	
	CA a;
	cout << sizeof a.m_szBuf << endl;
	
	return 0;

	string xx = "oo";
	const string & rr = xx;
	
	cout << rr << endl;
	//rr.clear();//error~
	
	return 0;
}
