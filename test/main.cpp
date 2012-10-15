#include <iostream>
#include <tr1/memory>
#include <set>
#include <vector>

using namespace std;
using std::tr1::shared_ptr;



int main()
{	
	char sz[] = { '1', '\0', '2'};
	cout << sz << endl;
	
	string s("4");
	s.insert(1, sz, 3);
	
	cout << s.length() << endl;
	cout << s    << endl;
	
	return 0;
}
