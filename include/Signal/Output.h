#pragma once

#include "OutputPad.h"
#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Output : public OutputPad<T> {
public:
	using OutputPad<T>::OutputPad;
	using PadBase::setName;

	Output&									operator=(Output&& other) = default;

	void									reset();
	void									push(T&& element);

private:
	T										m_lastElement;

	virtual const T& 						get() const override;
};

}

#include "Output.inl"