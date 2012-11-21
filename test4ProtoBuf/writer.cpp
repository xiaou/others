#include "helloworld.message/helloworld.Iam.pb.h"
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	helloworld::Iam msg;
	msg.set_id(44);
	msg.set_name("xiaou");
	msg.set_sex(helloworld::Iam::MAN);
	msg.set_str("helloworldxxx");
	helloworld::Friend * f = msg.add_friends();
	f->set_name("lucy");
	f->set_sex(helloworld::Iam::WOMAN);
	f = msg.add_friends();
	f->set_name("lily");
	f->set_sex(helloworld::Iam::MAN);
	
	string strBinary;
	msg.SerializeToString(&strBinary);
	cout << "will write [\n" << strBinary << "\n] to file." << endl; 

	fstream output("./msg.txt", ios::out | ios::trunc | ios::binary);
	if(!msg.SerializeToOstream(&output))
	{
		cerr << "Failed to write msg." << endl;
		return -1;
	}

	return 0;
}


