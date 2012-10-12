#include <iostream>
#include <set>

using namespace std;

int main()
{
	set<int> iset;
	set<int>::iterator it = iset.insert(4).first;
	(*it)++;

	return 0;
}
