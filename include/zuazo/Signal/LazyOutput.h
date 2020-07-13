#pragma once

#include "Layout.h"
#include "Output.h"

#include <utility>
#include <functional>
#include <cstddef>

namespace Zuazo::Signal {

template <typename T>
class LazyOutput : public Output<T> {
public:
	using Callback = std::function<void()>;

	LazyOutput() = default;
	template<typename Str, typename Func>
	LazyOutput(Str&& name, Func&& callback = Callback());
	LazyOutput(const LazyOutput& other) = default;
	LazyOutput(LazyOutput&& other) = default;
	virtual ~LazyOutput() = default;

	LazyOutput& 				operator=(const LazyOutput& other) = default;
	LazyOutput& 				operator=(LazyOutput&& other) = default;

	template <typename Func>
	void						setCallback(Func&& cbk);
	const Callback&				getCallback() const;

	void						setMaximumRecursion(uint rec);
	uint						getMaximumRecursion() const;
protected:
	virtual const T&			get() const final;

private:
	Callback					m_callback;

	mutable uint				m_maxRecursion = 1;
	mutable uint				m_recursion = 0;
};

template <typename T>
class Layout::PadProxy<LazyOutput<T>> : public Layout::PadProxy<Output<T>> {
public:
	PadProxy(LazyOutput<T>& pad);
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& 					operator=(const PadProxy& other) = delete;

	void						setMaximumRecursion(uint rec);
	uint						getMaximumRecursion() const;

};


}

#include "LazyOutput.inl"