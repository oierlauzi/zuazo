#pragma once

#include "Layout.h"
#include "Output.h"

#include <limits>
#include <array>
#include <atomic>
#include <vector>
#include <cstddef>

namespace Zuazo::Signal {

template <typename T>
class AsyncOutput : public Output<T> {
public:
	AsyncOutput() = default;
	template<typename Str>
	AsyncOutput(Str&& name, size_t length = DEFAULT_MAX_LENGTH);
	AsyncOutput(const AsyncOutput& other) = default;
	AsyncOutput(AsyncOutput&& other) = default;
	virtual ~AsyncOutput() = default;

	AsyncOutput&				operator=(const AsyncOutput& other) = default;
	AsyncOutput&				operator=(AsyncOutput&& other) = default;

	void						setMaxDropped(int max);
	int							getMaxDropped() const;
	uint						getDropped() const;
	void						resetDropped();

	void						flushBuffer();
	size_t						getBufferSize() const;

	void						update();

	void						reset();
	template<typename Q>
	void						push(Q&& element);

	static constexpr int		DEFAULT_MAX_DROPPED = 3;
	static constexpr size_t		DEFAULT_MAX_LENGTH = 4;
private:
	std::atomic<size_t>			m_read = 0;
	std::atomic<size_t>			m_write = 0;
	std::vector<T>				m_buffer;

	int							m_maxDropped = DEFAULT_MAX_DROPPED;
	uint						m_dropped = 0;

	size_t 						getNextValue(size_t i) const;
	
};

template <typename T>
class Layout::PadProxy<AsyncOutput<T>> : public Layout::PadProxy<Output<T>> {
public:
	PadProxy(AsyncOutput<T>& pad);
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& 					operator=(const PadProxy& other) = delete;

	void						setMaxDropped(int max);
	int							getMaxDropped() const;
	uint						getDropped() const;
	void						resetDropped();

	void						flushBuffer();
	size_t						getBufferSize();
};

}

#include "AsyncOutput.inl"
