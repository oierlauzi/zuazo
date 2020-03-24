#pragma once

#pragma once

#include "Output.h"

#include <memory>
#include <mutex>
#include <vector>
#include <atomic>

namespace Zuazo::Signal {

template <typename T>
class AsyncOutput : public Output<T> {
public:
	using Output<T>::Output;

	AsyncOutput&                        operator=(AsyncOutput&& other) = default;

	void								setMaxDropped(int max);
	int									getMaxDropped() const;
	uint								getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(size_t size);
	size_t								getMaxBufferSize() const;
	size_t								getBufferSize() const;
	void								flushBuffer();

	void								update();

	void								reset();
	void								push(T&& element);

	static constexpr int				DEFAULT_MAX_DROPPED=3;
	static constexpr uint				DEFAULT_MAX_BUFFER_SIZE=3;
private:
	Timing::PeriodicEvent				m_period;
	std::mutex							m_mutex;

	int									m_maxDropped = DEFAULT_MAX_DROPPED;
	uint								m_dropped = 0;

	std::atomic<size_t>					m_read = 0;
	std::atomic<size_t>					m_write = 0;
	std::vector<T>						m_buffer = std::vector<T>(DEFAULT_MAX_BUFFER_SIZE);

	size_t								getNextValue(size_t i) const;		
};

}

#include "AsyncOutput.inl"
