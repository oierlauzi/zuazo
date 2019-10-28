#pragma once

#include "PadBase.h"
#include "OutputPad.h"
#include "InputPad.h"

namespace Zuazo::Signal {

template <typename T>
class DummyInput : public InputPad<T> {
public:
	class Destination; friend Destination;

	DummyInput(std::string&& name = "");
	DummyInput(DummyInput&& other);
	virtual ~DummyInput() = default;

	using PadBase::setName;

	Destination destination;
};

template <typename T>
class DummyInput<T>::Destination : public OutputPad<T> {
	friend DummyInput;
private:
	Destination(const DummyInput& owner);
	Destination(const Destination& other) = delete;
	virtual ~Destination() = default;

	Destination& operator=(Destination&& other) = default;

	virtual const std::shared_ptr<const T>& get() const final;

	const DummyInput& m_owner;
};

}

#include "DummyInput.inl"