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
    wait();
    return m_future.get();
    
}

template<typename T>
inline bool CrossThreadInvocation::AsyncExecution<T>::isReady() const{
    return m_invoked;
}

template<typename T>
inline void CrossThreadInvocation::AsyncExecution<T>::wait() const{
    std::unique_lock<std::mutex> lock(m_invoker.m_mutex);

    //wait until is ready
    while( !isReady() ){
        m_invoker.m_invocationHandled.wait(lock);
    }
}       

template<typename T>
inline void CrossThreadInvocation::AsyncExecution<T>::invoke() {
    m_future.wait();
}






template<typename T, typename... Args>
inline std::shared_ptr<CrossThreadInvocation::AsyncExecution<T>> CrossThreadInvocation::execute(const std::function<T(Args...)>& func, Args... args){
    std::lock_guard<std::mutex> lock(m_mutex);
    
    //Create a future object
    std::shared_ptr<AsyncExecution<T>> invocation(new AsyncExecution<T>(
        std::future<T>(std::async(std::launch::deferred, func, args...)),
        *this
    ));

    //Set the execution
    m_invocations.emplace( 
        std::static_pointer_cast<AsyncExecutionBase>(invocation)
    );

    //Signal the other thread
    m_invocationAdded.notify_one();

    return invocation;
}

template<typename T, typename... Args>
inline T CrossThreadInvocation::execute(WaitForCompletion, const std::function<T(Args...)>& func, Args... args){
    auto invocation = execute(func, args...);

    //Return result
    return invocation->getValue();
}

inline void CrossThreadInvocation::waitAndHandleOne() {
    std::unique_lock<std::mutex> lock(m_mutex);

    while( !m_invocations.size() ){
        m_invocationAdded.wait(lock);
    }

    invoke();

    m_invocationHandled.notify_all();
}

inline void CrossThreadInvocation::waitAndHandleAll() {
    std::unique_lock<std::mutex> lock(m_mutex);

    while( !m_invocations.size() ){
        m_invocationAdded.wait(lock);
    }

    while(m_invocations.size()){
        invoke();
    }

    m_invocationHandled.notify_all();
}

inline void CrossThreadInvocation::handleOneExecution() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if(m_invocations.size()){
        invoke();
        m_invocationHandled.notify_all();
    }
}

inline void CrossThreadInvocation::handleAllExecutions() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if(m_invocations.size()){
        m_invocationHandled.notify_all();
    }

    while(m_invocations.size()){
        invoke();
    }
}

inline void CrossThreadInvocation::invoke() {
    m_invocations.front()->invoke();
    m_invocations.front()->m_invoked = true;
    m_invocations.pop();
}

}