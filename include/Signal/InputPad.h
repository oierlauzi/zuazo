#pragma once

#include "PadBase.h"

#include <memory>

namespace Zuazo::Signal {

template <typename T>
class OutputPad;

template <typename T>
class InputPad : public PadBase {
	friend OutputPad<T>;
public:
	InputPad(std::string&& name = "");
	virtual ~InputPad();

	void                                    setSource(OutputPad<T>* src);
	OutputPad<T>*                           getSource() const;
protected:
	InputPad(const InputPad& other);
	InputPad(InputPad&& other);
	InputPad&                               operator=(const InputPad& other);
	InputPad&                               operator=(InputPad&& other);

	static const T							NO_SIGNAL;

	const T&         						get() const;
private:
	OutputPad<T>*                           m_source = nullptr;
};

}

#include "InputPad.inl"