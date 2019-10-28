#pragma once

#pragma once

#include "Output.h"
#include "../Timing/Scheduler.h"

#include <memory>
#include <mutex>
#include <queue>

namespace Zuazo::Signal {

template <typename T>
class AsyncOutput :
		public Output<T>,
		public Timing::Scheduler::Event
{
public:
	AsyncOutput(std::string&& name = "");
	AsyncOutput(const AsyncOutput& other) = delete;
	AsyncOutput(AsyncOutput&& other) = default;
	virtual ~AsyncOutput() = default;

	AsyncOutput&                        operator=(AsyncOutput&& other) = default;

	void								setMaxDropped(int32_t max);
	int32_t								getMaxDropped() const;
	uint32_t							getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(int32_t size);
	int32_t								getMaxBufferSize() const;
	uint32_t							getBufferSize() const;
	void								flushBuffer();

	void								reset();
	void								push(std::shared_ptr<const T>&& element);

	virtual void 						update() override;
private:
	static constexpr int32_t			DEFAULT_MAX_DROPPED=3;
	static constexpr int32_t			DEFAULT_MAX_BUFFER_SIZE=3;

	int32_t								m_maxDropped = DEFAULT_MAX_DROPPED;
	int32_t								m_maxBufferSize = DEFAULT_MAX_BUFFER_SIZE;

	uint32_t							m_dropped = 0;
	std::queue<std::shared_ptr<const T>> m_buffer;

	std::mutex							m_mutex;
};

}

#include "AsyncOutput.inl"
