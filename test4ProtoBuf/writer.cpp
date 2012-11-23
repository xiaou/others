#include <fstream>
#include <iostream>


using namespace std;
#define USE_DYNAMIC_COMPILE 1


#if USE_DYNAMIC_COMPILE

#include "google/protobuf/descriptor.h"
#include "google/protobuf/compiler/importer.h"
#include "google/protobuf/dynamic_message.h"

class MockErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
 public:
  MockErrorCollector() {}
  ~MockErrorCollector() {}

  std::string text_;

  // implements ErrorCollector ---------------------------------------
  void AddError(const std::string& filename, int line, int column,
                const std::string& message) {
      std::cerr << "error filename "<<filename <<" message "<<message <<std::endl;
  }
};

#else

#include "helloworld.message/helloworld.Iam.pb.h"

#endif


int main()
{
#if USE_DYNAMIC_COMPILE
//动态编译message
	MockErrorCollector errorCollector;
	google::protobuf::compiler::DiskSourceTree sourceTree; 

 	google::protobuf::compiler::Importer importer(&sourceTree, &errorCollector); 
 	sourceTree.MapPath("", "./helloworld.message/"); 

 	importer.Import("helloworld.Iam.proto"); 
 	
 	const google::protobuf::DescriptorPool * pool = importer.pool();
 	google::protobuf::DynamicMessageFactory dynamicMessageFactory;
	const google::protobuf::Descriptor *descrip = pool->FindMessageTypeByName("helloworld.Iam"); 
	assert( descrip );
	const google::protobuf::Message * prototype = dynamicMessageFactory.GetPrototype(descrip);
	
	google::protobuf::Message * pMsg = prototype->New();
	assert(pMsg);
	
	const google::protobuf::Reflection * reflect = pMsg->GetReflection();
	reflect->SetInt32(pMsg, descrip->FindFieldByName("id"), 24);
	reflect->SetString(pMsg, descrip->FindFieldByName("name"), "spr2");
	reflect->SetEnum(pMsg, descrip->FindFieldByName("sex"), descrip->FindEnumValueByName("WOMAN"));
	
	cout << pMsg->DebugString() << endl;
	
	//
	fstream output("./msg.txt", ios::out | ios::trunc | ios::binary);
	if(!pMsg->SerializeToOstream(&output))
	{
		cerr << "Failed to write msg." << endl;
		return -1;
	}
	
	delete pMsg;
#else
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
#endif
	return 0;
}


