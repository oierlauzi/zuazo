#include "ZuazoBase.h"

namespace Zuazo {


template<typename Str>
inline ZuazoBase::ZuazoBase(const Instance& instance, Str&& name)
	: Signal::Layout(std::forward<Str>(name))
	, m_instance(instance)
{
}

inline const Instance& ZuazoBase::getInstance() const {
	return m_instance;
}

inline void ZuazoBase::open() {
	if(m_isOpen == false) {
		m_isOpen = true;
	} else {
		ZUAZO_LOG(getInstance(), Verbosity::WARNING, "Opening an already opened element: " + getName());
	}
}

inline void ZuazoBase::close() {
	if(m_isOpen == true) {
		m_isOpen = false;
	} else {
		ZUAZO_LOG(getInstance(), Verbosity::WARNING, "Closing an already closed element" + getName());
	}
}

inline bool ZuazoBase::isOpen() const {
	return m_isOpen;
}

	
};

}