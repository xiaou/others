
#include "iostream"
#include <tr1/memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <assert.h>

using namespace std;
using std::tr1::shared_ptr;
using std::auto_ptr;

class C
{
public:
	C()
	{ 

	}
	~C()
	{ 

	}
	int i;
	void func(){}
};

typedef shared_ptr<C> CSharedCPtr;

void func(C & c){}

void func2(C * c){}

int main()
{
	vector<CSharedCPtr> arr;
	//arr.push_back( new C );//err.
	arr.push_back( CSharedCPtr(new C) );

	vector<CSharedCPtr>::iterator it = arr.begin();
	func(*(arr[0]));
	func( *it->get() );
	func(**it);

	arr[0]->func();	

	(*it).get()->func();
	(*it)->func();
	(*it)->i;
}

