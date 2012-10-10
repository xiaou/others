#include <iostream>
#include <set>
#include <vector>
#include <tr1/memory>
#include <assert.h>

using namespace std;

using std::tr1::shared_ptr;

int g(4);

void func(int i)
{
	cout << i << endl;
	cout << g << endl;
}

int main()
{
	func(g++);
	
	return 0;
}
