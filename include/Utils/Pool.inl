#include "Pool.h"

namespace Zuazo::Utils {

template <typename T>
template<typename Func>
inline Pool<T>::Pool(Func&& cbk)
	: m_allocCallback(std::forward<Func>(cbk))
{
}


template <typename T>
inline typename Pool<T>::Ref Pool<T>::acquire() const {
	for(const auto& element : m_elements) {
		if(element.use_count() == 1) {
			return element;
		}
	}

	m_elements.push_back(m_allocCallback());
	return m_elements.back();
}

template <typename T>
inline void Pool<T>::clear() {
	m_elements.clear();
}

template <typename T>
inline void Pool<T>::clearUnused() {
	auto ite = m_elements.begin();

	while(ite != m_elements.end()) {
		if(ite->use_count() == 1) {
			ite = m_elements.erase(ite);
		} else {
			++ite;
		}
	}
}

}