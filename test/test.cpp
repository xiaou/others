#include <iostream>

using namespace std;

class CA
{
public:
int m_i;
CA(int i):m_i(i){ }
~CA(){ cout << "m_i = " << m_i << " <-release already!" << endl; }
};

int main()
{
	void * p;
	
	{
		int i(14);
		p = &i;
	}

	cout << "wish not 14:" << *(int *)p << endl;

	{
		CA a(44);
		p = &a;
	}
	
	cout << "wish not 44:" << ((CA *)p)->m_i << endl;

	return 0;
}
