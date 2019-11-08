#pragma once

#include <atomic>
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
		AsyncExecutionBase(const AsyncExecutionBase& other) = delete;
		virtual ~AsyncExecutionBase() = default;

		AsyncExecutionBase&     operator=(const AsyncExecutionBase& other) = delete;
		
	protected:
		mutable std::mutex		m_wait;
		std::unique_lock<std::mutex> m_lock = std::unique_lock<std::mutex>(m_wait);

	private:
		virtual void            invoke() = 0;
	};

	template<typename T>
	class AsyncExecution : public AsyncExecutionBase {
		friend CrossThreadInvocation;
	public:
		AsyncExecution(const AsyncExecution& other) = delete;
		virtual ~AsyncExecution() = default;

		AsyncExecution&         operator=(const AsyncExecution& other) = delete;

		T                       getValue() const;      
	private:
		AsyncExecution(std::future<T>&& future, CrossThreadInvocation& invoker);

		mutable std::future<T>  m_future;
		CrossThreadInvocation&  m_invoker;

		virtual void            invoke() final;
	};

	static constexpr struct WaitForCompletion {} waitForCompletion = {};

	template<typename T, typename... Args>
	std::shared_ptr<AsyncExecution<T>> execute(const std::function<T(Args...)>& func, Args... args);

	void                    handleOneExecution();
	void                    handleAllExecutions();
private:
	std::queue<std::shared_ptr<AsyncExecutionBase>> m_invocations;

	void                    invoke();
};

}

#include "CrossThreadInvocation.inl"