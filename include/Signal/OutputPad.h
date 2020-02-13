#pragma once

#include <memory>
#include <set>

namespace Zuazo::Signal {

class PadBase;
template <typename T>
class InputPad;

template <typename T>
class OutputPad : public PadBase {
	friend InputPad<T>;
public:
	OutputPad(std::string&& name = "");
	OutputPad(const OutputPad& other) = delete;
	virtual ~OutputPad();

	const std::set<InputPad<T>*>&		getConsumers() const;
protected:
	virtual const T&					get() const = 0;
	
private:
	OutputPad(OutputPad&& other);
	OutputPad&							operator=(OutputPad&& other);

	std::set<InputPad<T>*>				m_consumers;
};

}

#include "OutputPad.inl"