/*
 * @file callback_task.hpp
 * @brief add callback task for threadpool.
 */

#ifndef __callback_task_h__
#define __callback_task_h__

#include <boost/function.hpp>

namespace boost { namespace threadpool
{
	
template<class RetType>
class callback_task
{
	typedef boost::function<void (RetType)> CALLBACK;
	typedef boost::function<RetType ()> FUNCTION;

private:
	CALLBACK m_Callback;
	FUNCTION m_Function;

public:
	callback_task(FUNCTION f, CALLBACK c):m_Callback(c), m_Function(f){}

	void operator()(){ m_Callback(m_Function()); }
};


}} // namespace boost::threadpoll

#endif // __callback_task_h__

