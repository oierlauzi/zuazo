#pragma once

#include "Output.h"
#include "../Timing/ExternalEvent.h"

namespace Zuazo::Signal {

template <typename T>
class LazyOutput : public Output<T> {
public:
	LazyOutput(Timing::ExternalEvent& evnt, std::string&& name = "");
	LazyOutput(LazyOutput&& other) = default;
	virtual ~LazyOutput() = default;

	LazyOutput& operator=(LazyOutput&& other) = default;


	void                    setMaximumRecursion(uint rec);
	uint                    getMaximumRecursion() const;
protected:
	virtual std::shared_ptr<const T>& get() const final;

private:
	Timing::ExternalEvent&  m_event;

	mutable uint            m_maxRecursion = 1;
	mutable uint            m_recursion = 0;
};

}

#include "LazyOutput.inl"