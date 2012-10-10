#include <string.h>
#include <iostream>

using namespace std;

void func(const string & str)
{

}

int main()
{

//func();
//return 0;

string s;

cout << s.length() << endl;

s.resize(4);

cout << s.length() << endl;

cout << s.capacity() << endl;

s += "xx00";
s.resize(10);
s += "yyjj";
cout << s << endl;

//s = "";
//s.resize(0);
s.clear();
cout << s.empty() << endl;
cout << s.length() << endl;
cout << s.capacity() << endl;

cout << s.max_size() << endl;

return 0;

}

