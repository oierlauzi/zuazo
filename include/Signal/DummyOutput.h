#pragma once

#include "PadBase.h"
#include "OutputPad.h"
#include "InputPad.h"

namespace Zuazo::Signal {

template <typename T>
class DummyOutput : public OutputPad<T> {
public:
	class Source;

	using OutputPad<T>::OutputPad;
	using PadBase::setName;

	Source source;
protected:
	virtual const std::shared_ptr<const T>& get() const final;
};

template <typename T>
class DummyOutput<T>::Source : public InputPad<T> {
	friend DummyOutput;
private:
	Source() = default;
	Source(const Source& other) = default;
	Source(Source&& other) = default;
	virtual ~Source() = default;

	Source& operator=(const Source& other) = default;
	Source& operator=(Source&& other) = default;
};

}

#include "DummyOutput.inl"