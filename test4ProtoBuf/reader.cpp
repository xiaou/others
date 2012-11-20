#include "helloworld.message/helloworld.Iam.pb.h"
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
	cout << msg.name() << endl;
	cout << helloworld::Iam::Sex_Name(msg.sex()) << endl;
	for(int i = 0; i != msg.friends_size(); i++)
		cout << msg.friends(i).name() << "\t" 
		<< helloworld::Iam::Sex_Name(msg.friends(i).sex()) << endl;
	
	if(msg.has_str())
		cout << msg.str() << endl;
	else // default value
		cout << msg.str() << endl;

	return 0;
}


