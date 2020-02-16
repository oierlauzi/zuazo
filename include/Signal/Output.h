#pragma once

#include "OutputPad.h"
#include <memory>

namespace Zuazo::Signal {

template <typename T>
class Output : public OutputPad<T> {
public:
	Output(std::string&& name, Layout* owner = nullptr);
	Output(const Output& other) = delete;
	Output(Output&& other) = default;
	virtual ~Output() = default;

	Output&									operator=(const Output& other) = delete;
	Output&									operator=(Output&& other) = default;

	using PadBase::setName;

	void									reset();
	void									push(T&& element);

private:
	T										m_lastElement;

	virtual const T& 						get() const override;
};

}

#include "Output.inl"