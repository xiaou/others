#include "helloworld.Iam.pb.h"
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	helloworld::Iam msg;

	fstream input("./msg.txt", ios::in | ios::binary);
	if(!msg.ParseFromIstream(&input))
	{
		cerr << "Failed to parse msg." << endl;
		return -1;
	}

	cout << msg.id() << endl;
	cout << msg.username() << endl;
	if(msg.has_str())
		cout << msg.str() << endl;
	
	return 0;
}


