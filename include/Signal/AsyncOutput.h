#pragma once

#pragma once

#include "Output.h"
#include "../Timing/Scheduler.h"

#include <memory>
#include <mutex>
#include <vector>
#include <atomic>

namespace Zuazo::Signal {

template <typename T>
class AsyncOutput :
		public Output<T>,
		public Timing::Scheduler::Event
{
public:
	using Output<T>::Output;

	AsyncOutput&                        operator=(AsyncOutput&& other) = default;

	void								setMaxDropped(int32_t max);
	int32_t								getMaxDropped() const;
	uint32_t							getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(size_t size);
	size_t								getMaxBufferSize() const;
	size_t								getBufferSize() const;
	void								flushBuffer();

	void								reset();
	void								push(std::shared_ptr<const T>&& element);

	virtual void 						update() override;

	static constexpr int32_t			DEFAULT_MAX_DROPPED=3;
	static constexpr size_t				DEFAULT_MAX_BUFFER_SIZE=3;
private:
	std::mutex							m_mutex;

	int32_t								m_maxDropped = DEFAULT_MAX_DROPPED;
	uint32_t							m_dropped = 0;

	std::atomic<size_t>					m_read = 0;
	std::atomic<size_t>					m_write = 0;
	std::vector<std::shared_ptr<const T>> m_buffer = std::vector<std::shared_ptr<const T>>(DEFAULT_MAX_BUFFER_SIZE);

	size_t								getNextValue(size_t i) const;		
};

}

#include "AsyncOutput.inl"
