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
	const std::set<InputPad<T>*>&		getConsumers() const;
protected:
	OutputPad(std::string&& name, Layout* owner);
	OutputPad(OutputPad&& other);
	OutputPad(const OutputPad& other) = delete;
	virtual ~OutputPad();

	OutputPad&							operator=(const OutputPad& other) = delete;
	OutputPad&							operator=(OutputPad&& other);

	virtual const T&					get() const = 0;
	
private:
	std::set<InputPad<T>*>				m_consumers;
};

}

#include "OutputPad.inl"