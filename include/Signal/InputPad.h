#pragma once

#include <memory>

namespace Zuazo::Signal {

class PadBase;
template <typename T>
class OutputPad;

template <typename T>
class InputPad : public PadBase {
	friend OutputPad<T>;
public:
	void                                    setSource(OutputPad<T>* src);
	OutputPad<T>*                           getSource() const;
protected:
	InputPad(std::string&& name, Layout* owner);
	InputPad(const InputPad& other);
	InputPad(InputPad&& other);
	virtual ~InputPad();

	InputPad&                               operator=(const InputPad& other);
	InputPad&                               operator=(InputPad&& other);

	static const T							NO_SIGNAL;

	const T&         						get() const;
private:
	OutputPad<T>*                           m_source = nullptr;
};

}

#include "InputPad.inl"