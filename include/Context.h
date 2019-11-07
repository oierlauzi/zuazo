#pragma once

#include "Instance.h"

#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace Zuazo {

class Context {
public:
	Context(Instance& inst);
	Context(const Context& other) = delete;
	Context(Context&& other) = delete;
	~Context();

	Context& operator=(const Context& other) = default;
	Context& operator=(Context&& other) = default;

	operator bool();

	void use();
	void unuse();
private:
	std::unique_lock<std::mutex>			m_mainLoopLock;

	static std::atomic<size_t>				s_activeContextCount;
	static bool								s_callbacksEnabled;

	static void								incrementActiveCounter();
	static void								decrementActiveCounter();
};

}

#include "Context.inl"
