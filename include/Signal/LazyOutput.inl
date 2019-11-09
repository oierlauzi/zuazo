#include "LazyOutput.h"

namespace Zuazo::Signal {

template <typename T>
inline LazyOutput<T>::LazyOutput(Timing::ExternalEvent& evnt, std::string&& name) :
	Output<T>(std::move(name)),
	m_event(evnt)
{
}

template <typename T>
inline void LazyOutput<T>::setMaximumRecursion(uint rec){
	m_maxRecursion = rec;
}

template <typename T>
inline uint LazyOutput<T>::getMaximumRecursion() const {
	return m_maxRecursion;
}

template <typename T>
inline std::shared_ptr<const T>& LazyOutput<T>::get() const{
	if(m_recursion < m_maxRecursion){
		m_recursion++;
		m_event.invoke();
		m_recursion--;
	}
	
	return Output<T>::get();
}

}