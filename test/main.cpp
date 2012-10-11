#include <iostream>
#include <set>
#include <vector>
#include <tr1/memory>
#include <assert.h>

using namespace std;

using std::tr1::shared_ptr;

class CA
{};


int main()
{	
	set<shared_ptr<CA> > s;
	CA * p = new CA;
	shared_ptr<CA> sh(p);
	s.insert( sh );
	s.insert(shared_ptr<CA>(new CA));
	
	cout << s.size() << endl;
	
	shared_ptr<CA> tmp = sh;
	s.erase( tmp );
	//s.insert( tmp );
	cout << s.size() << endl;
	
	
	
	
	
	return 0;
}
