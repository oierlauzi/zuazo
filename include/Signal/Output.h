#pragma once

#include "Layout.h"
#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Output : public Layout::OutputPad<T> {
public:
	Output(std::string&& name, Layout* owner);
	Output(const Output& other) = delete;
	Output(Output&& other) = default;
	virtual ~Output() = default;

	Output&									operator=(const Output& other) = delete;
	Output&									operator=(Output&& other) = default;

	using Layout::PadBase::setName;

	void									reset();
	void									push(T&& element);

private:
	T										m_lastElement;

	virtual const T& 						get() const override;
};

}

#include "Output.inl"