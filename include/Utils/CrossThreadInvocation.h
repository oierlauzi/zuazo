#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

namespace Zuazo::Utils {

class CrossThreadInvocation {
public:
	CrossThreadInvocation() = default;
	CrossThreadInvocation(const CrossThreadInvocation& other) = delete;
	~CrossThreadInvocation() = default;

	class AsyncExecutionBase {
		friend CrossThreadInvocation;
	public:
		AsyncExecutionBase() = default;
		AsyncExecutionBase(const AsyncExecutionBase& other) = default;
		virtual ~AsyncExecutionBase() = default;

		AsyncExecutionBase&     operator=(const AsyncExecutionBase& other) = default;
		
	protected:
		bool                    m_invoked = false;

	private:
		virtual void            invoke() = 0;
	};

	template<typename T>
	class AsyncExecution : public AsyncExecutionBase {
		friend CrossThreadInvocation;
	public:
		AsyncExecution(const AsyncExecution& other) = default;
		AsyncExecution(AsyncExecution&& other) = default;
		virtual ~AsyncExecution() = default;

		AsyncExecution&         operator=(const AsyncExecution& other) = default;
		AsyncExecution&         operator=(AsyncExecution&& other) = default;

		T                       getValue() const;
		bool                    isReady() const;
		void                    wait() const;           
	private:
		AsyncExecution(std::future<T>&& future, CrossThreadInvocation& invoker);

		mutable std::future<T>  m_future;
		CrossThreadInvocation&  m_invoker;

		virtual void            invoke() final;
	};

	static constexpr struct WaitForCompletion {} waitForCompletion = {};

	template<typename T, typename... Args>
	std::shared_ptr<AsyncExecution<T>> execute(const std::function<T(Args...)>& func, Args... args);

	template<typename T, typename... Args>
	T                       execute(WaitForCompletion, const std::function<T(Args...)>& func, Args... args);

	void                    waitAndHandleOne();
	void                    waitAndHandleAll();

	void                    handleOneExecution();
	void                    handleAllExecutions();
private:
	std::queue<std::shared_ptr<AsyncExecutionBase>> m_invocations;

	std::mutex              m_mutex;
	std::condition_variable m_invocationAdded;
	std::condition_variable m_invocationHandled;

	void                    invoke();
};

}

#include "CrossThreadInvocation.inl"