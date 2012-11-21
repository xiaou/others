#include "helloworld.message/helloworld.Iam.pb.h"
#include <fstream>
#include <iostream>
#include "google/protobuf/descriptor.h"

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
	
	#if 1
	const google::protobuf::Descriptor * descrip = msg.GetDescriptor();
	cout << "\n" << descrip->name() << endl;
	cout << descrip->full_name() << endl;
	cout << descrip->index() << endl;
	//cout << descrip->DebugString() << endl;
	
	const google::protobuf::FieldDescriptor * fieldDescrip = descrip->FindFieldByName("friends");
	assert(fieldDescrip);
	cout << "\n" << fieldDescrip->full_name() << endl;	
	cout << fieldDescrip->DebugString() << endl;	
	
	assert( fieldDescrip->file() == descrip->file() );
	
	const google::protobuf::FileDescriptor * fileDescrip = descrip->file();
	cout << "\n" << fileDescrip->name() << endl;
	cout << fileDescrip->message_type_count() << endl;
	assert( fileDescrip->message_type(0) == descrip );
	cout << fileDescrip->message_type(1)->full_name() << endl;
	
	const google::protobuf::DescriptorPool * pool = fileDescrip->pool();
	
	
	return 0;
	#endif
	
	cout << "debug string:\n[" << msg.DebugString() << "]\n" << endl;

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


