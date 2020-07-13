#include "LazyOutput.h"

namespace Zuazo::Signal {

/*
 * LazyOutput
 */

template <typename T>
template <typename Str, typename Func>
inline LazyOutput<T>::LazyOutput(	Str&& name, 
									Func&& callback )
	: Output<T>(std::forward<Str>(name))
	, m_callback(std::forward<Func>(callback))
{
}


template <typename T>
template <typename Func>
void LazyOutput<T>::setCallback(Func&& cbk) {
	m_callback = std::forward<Func>(cbk);
}

template <typename T>
const typename LazyOutput<T>::Callback& LazyOutput<T>::getCallback() const {
	return m_callback;
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
inline const T& LazyOutput<T>::get() const{
	if(m_recursion < m_maxRecursion){
		m_recursion++;
		m_callback();
		m_recursion--;
	}
	
	return Output<T>::get();
}

/*
 * Layout::PadProxy<LazyOutput<T>>
 */

template <typename T>
inline Layout::PadProxy<LazyOutput<T>>::PadProxy(LazyOutput<T>& pad)
	: PadProxy<Output<T>>(pad)
{
}

template <typename T>
inline void Layout::PadProxy<LazyOutput<T>>::setMaximumRecursion(uint rec) {
	PadProxy<PadBase>::template get<LazyOutput<T>>().setMaximumRecursion(rec);
}

template <typename T>
inline uint Layout::PadProxy<LazyOutput<T>>::getMaximumRecursion() const {
	return PadProxy<PadBase>::template get<LazyOutput<T>>().getMaximumRecursion();
}

}