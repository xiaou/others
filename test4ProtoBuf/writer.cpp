#include "helloworld.Iam.pb.h"
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	helloworld::Iam msg;
	msg.set_id(44);
	msg.set_username("xiaou");
	msg.set_str("xxxx");
	
	fstream output("./msg.txt", ios::out | ios::trunc | ios::binary);

	if(!msg.SerializeToOstream(&output))
	{
		cerr << "Failed to write msg." << endl;
		return -1;
	}

	return 0;
}


