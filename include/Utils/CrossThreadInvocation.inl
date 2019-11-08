#include "CrossThreadInvocation.h"

#include <utility>

namespace Zuazo::Utils {

template<typename T>
inline CrossThreadInvocation::AsyncExecution<T>::AsyncExecution(std::future<T>&& future, CrossThreadInvocation& invoker) :
	m_future(std::forward<std::future<T>>(future)),
	m_invoker(invoker)
{
}

template<typename T>
inline T CrossThreadInvocation::AsyncExecution<T>::getValue() const{
	std::lock_guard<std::mutex> lock(m_wait); //Wait on the mutex until its freed
	return m_future.get();
	
}    

template<typename T>
inline void CrossThreadInvocation::AsyncExecution<T>::invoke() {
	m_future.wait(); //Execute the future
	m_lock.unlock();
}






template<typename T, typename... Args>
inline std::shared_ptr<CrossThreadInvocation::AsyncExecution<T>> CrossThreadInvocation::execute(const std::function<T(Args...)>& func, Args... args){
	//Create a future object
	std::shared_ptr<AsyncExecution<T>> invocation(new AsyncExecution<T>(
		std::future<T>(std::async(std::launch::deferred, func, std::forward<Args>(args)...)),
		*this
	));

	//Set the execution
	m_invocations.emplace( 
		std::static_pointer_cast<AsyncExecutionBase>(invocation)
	);

	return invocation;
}

inline void CrossThreadInvocation::handleOneExecution() {
	if(m_invocations.size()){
		invoke();
	}
}

inline void CrossThreadInvocation::handleAllExecutions() {
	while(m_invocations.size()){
		invoke();
	}
}

inline void CrossThreadInvocation::invoke() {
	m_invocations.front()->invoke();
	m_invocations.pop();
}

}