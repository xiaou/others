#include <string.h>
#include <iostream>

using namespace std;


int main()
{
	string str = "";
	const char * p = str.c_str();
	if(p)
	{
		cout << p << endl;
		cout << strlen(p) << endl;
		cout << strlen(NULL) << endl;
	}

	return 0;

string s;

cout << s.length() << endl;

s.resize(4);

cout << s.length() << endl;

cout << s.capacity() << endl;

s += "xx00";
s.resize(10);
s += "yyjj";
cout << s << endl;

s = "";
s.resize(1);
cout << s.capacity() << endl;

cout << s.max_size() << endl;

return 0;

}

