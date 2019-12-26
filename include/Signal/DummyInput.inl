#include "DummyInput.h"

namespace Zuazo::Signal {

template<typename T>
DummyInput<T>::DummyInput(std::string&& name) :
	InputPad<T>(move(name)),
	destination(*this)
{
}

template<typename T>
DummyInput<T>::DummyInput(DummyInput&& other) :
	InputPad<T>(std::move(other)),
	destination(*this)
{
	static_cast<OutputPad<T>&>(destination) = std::move(static_cast<OutputPad<T>&>(other.destination));
}




template<typename T>
inline DummyInput<T>::Destination::Destination(const DummyInput& owner) :
	m_owner(owner)
{
}

template<typename T>
inline const T& DummyInput<T>::Destination::get() const {
	return m_owner.get();
}

}