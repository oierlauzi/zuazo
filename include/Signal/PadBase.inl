#include "PadBase.h"

namespace Zuazo::Signal {

/*
 * PadBase
 */

template<typename Str>
inline PadBase::PadBase(Str&& name)
	: m_name(std::forward<Str>(name))
{
}

template<typename Str>
inline void PadBase::setName(Str&& name) {
	m_name = std::forward<Str>(name);
}

inline const std::string& PadBase::getName() const {
	return m_name;
}


/*
 * Layout::PadProxy<PadBase>
 */

inline Layout::PadProxy<PadBase>::PadProxy(PadBase& pad)
	: m_pad(pad)
{
}

inline const std::string& Layout::PadProxy<PadBase>::getName() const {
	return get<PadBase>().getName();
}

template <typename T>
inline T& Layout::PadProxy<PadBase>::get() {
	return static_cast<T>(m_pad);
}

template <typename T>
inline const T& Layout::PadProxy<PadBase>::get() const {
	return static_cast<T>(m_pad);
}

}