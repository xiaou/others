#include <iostream>
#include <sstream>
#include <boost/threadpool.hpp>
#include "callback_task.hpp"

using namespace std;
using namespace boost::threadpool;


void task_normal()
{
    cout << "task_normal()\n";
}

void task_with_parameter(int value, string str)
{
    cout << "task_with_parameter(" << value << ", " << str << ")\n";
}

bool task_loop()
{
	static int i = 0;
	cout << "task_loop:" << i <<"\n";
	return ++i != 5;
}

int task_return14()
{ 
	sleep(1);
	return 14; 
}

void callback(int ret)
{
	cout<< "callback: task_return14() return " << ret << "\n";
}

void task_test4ThreadPrivateData()
{
	cout << "task_test4ThreadPrivateData().id:";

	static map<boost::thread::id, string> s_ThreadPrivateData;

	boost::thread::id tid = boost::this_thread::get_id();
	cout << tid << "\n";

	map<boost::thread::id, string>::iterator it;
	if((it = s_ThreadPrivateData.find(tid)) == s_ThreadPrivateData.end())
	{
		it = s_ThreadPrivateData.insert(make_pair(tid, "hello")).first;
	}

	cout << tid << " has private data:" << it->second << "\n";
}

void help2SeePoolStatus(pool & tp)
{
	ostringstream os;
	os << "begin>\n";

	os << "how many threads in the pool:" << tp.size() << "\n";
	os << "how many tasks are currently executed:" << tp.active() << "\n";
	os << "how many tasks are ready and waiting for execution:" << tp.pending() << "\n";

	os << "<end.";
	cout<< "\033[1;45;33m"<< os.str() << "\033[0m" << "\n";
}

void help2AddAllTask(pool & tp)
{
	tp.schedule( callback_task<int>(&task_return14, callback) );
    tp.schedule(&task_normal);
    tp.schedule(boost::bind(task_with_parameter, 4, "number"));
	tp.schedule( looped_task_func(&task_loop, 0.5*1000) );	
	tp.schedule(&task_test4ThreadPrivateData);
}

void testCase0()
{
	cout<< "testCase0()\n" << endl;
    
	// Create fifo thread pool container with n threads.
	pool tp(0);// 0 threads in pool
	
	help2AddAllTask(tp);

	help2SeePoolStatus(tp);
	
	//Wait until all task are finished.
	tp.wait();
}

void testCase1()
{
	cout<< "testCase1()\n" << endl;
	pool tp(1);// only one thread in pool.
	help2AddAllTask(tp);
	help2SeePoolStatus(tp);
	tp.size_controller().resize(5);
	help2SeePoolStatus(tp);
	tp.wait();
	help2SeePoolStatus(tp);
}

void testCase2()
{
	cout<< "testCase2()\n" << endl;
	pool tp(10);
	help2AddAllTask(tp);
	for(int i = 0; i != 4; i++, help2SeePoolStatus(tp), sleep(.5));
	tp.wait();
}

int main(int argc,char *argv[])
{
	testCase1();

    return(0);
}


