#pragma once

#include "InputPad.h"

#include <memory>
#include <limits>

namespace Zuazo::Signal {


template <typename T>
class InputPad;

template <typename T>
class Input : public InputPad<T> {
public:
	using InputPad<T>::InputPad;
	using PadBase::setName;

	Input&                              operator=(const Input& other) = default;
	Input&                              operator=(Input&& other) = default;

	void                                setHold(bool hold);
	bool                                getHold() const;

	void                                reset();
	const T&							get() const;
	bool                                hasChanged() const;

private:   
	mutable T							m_lastElement;
	bool                                m_hold = false;
};

}

#include "Input.inl"