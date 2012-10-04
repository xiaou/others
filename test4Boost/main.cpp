
#include <iostream>
#include <boost/threadpool.hpp>
//#include <unistd.h>

using namespace std;
using namespace boost::threadpool;

// Some example tasks
void first_task()
{
    cout << "first task is running\n" ;
	sleep(.2);
}

void second_task()
{
    cout << "second task is running\n" ;
	sleep(.2);
}

void task_with_parameter(int value, string str)
{
    cout << "task_with_parameter(" << value << ", " << str << ")\n";
	sleep(.2);
}

int task_return_14()
{ 
	return 14; 
}

int main(int argc,char *argv[])
{
    // Create fifo thread pool container with two threads.
    pool tp(2);

    // Add some tasks to the pool.
    tp.schedule(&first_task);
    tp.schedule(&second_task);
    tp.schedule(boost::bind(task_with_parameter, 4, "number"));
	future<int> res = schedule(tp, &task_return_14);

    // Wait until all tasks are finished.
    tp.wait();

	cout<< "get res value for task_return_14():" << res.get() <<endl;

    // Now all tasks are finished!    
    return(0);
}
