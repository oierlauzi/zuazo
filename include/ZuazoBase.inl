#include "ZuazoBase.h"

#include <type_traits>

namespace Zuazo {


template<typename Str, typename Pads>
inline ZuazoBase::ZuazoBase(Instance& instance, 
							Str&& name, 
							Pads&& pads)
	: Signal::Layout(std::forward<Str>(name), std::forward<Pads>(pads))
	, m_instance(instance)
	, m_isOpen(false)
{
}

inline ZuazoBase::~ZuazoBase(){
	assert(isOpen() == false); 
}



inline void ZuazoBase::open() {
	if(m_isOpen == false) {
		m_isOpen = true;
	} else {
		ZUAZO_BASE_LOG(*this, Severity::WARNING, "Opening an already opened element");
	}
}

inline void ZuazoBase::close() {
	if(m_isOpen == true) {
		m_isOpen = false;
	} else {
		ZUAZO_BASE_LOG(*this, Severity::WARNING, "Closing an already closed element");
	}
}

inline bool ZuazoBase::isOpen() const {
	return m_isOpen;
}



inline Instance& ZuazoBase::getInstance() const {
	return m_instance;
}



template <typename Cbk>
inline void ZuazoBase::setPreUpdateCallback(Cbk&& cbk) {
	(*m_callbacks)[PRE_UPDATE] = std::forward<Cbk>(cbk);
}

inline const ZuazoBase::UpdateCallback& ZuazoBase::setPreUpdateCallback() const {
	return (*m_callbacks)[PRE_UPDATE];
}

template <typename Cbk>
inline void ZuazoBase::setPostUpdateCallback(Cbk&& cbk) {
	(*m_callbacks)[POST_UPDATE] = std::forward<Cbk>(cbk);
}

inline const ZuazoBase::UpdateCallback& ZuazoBase::setPostUpdateCallback() const {
	return (*m_callbacks)[POST_UPDATE];
}

template <typename Cbk>
inline void ZuazoBase::setUpdateCallback(Cbk&& cbk) {
	(*m_callbacks)[UPDATE] = std::forward<Cbk>(cbk);
}

inline const ZuazoBase::UpdateCallback& ZuazoBase::setUpdateCallback() const {
	return (*m_callbacks)[UPDATE];
}



template<typename Func>
inline void ZuazoBase::visitCallbacks(const Func& func) const {
	for(const auto& cbk : *m_callbacks) {
		func(cbk);
	}
}

inline void ZuazoBase::update() const {
	visitCallbacks(std::invoke<const UpdateCallback&>);
}

inline void ZuazoBase::enableRegularUpdate(Instance::Priority prior) const {
	visitCallbacks(
		[=] (const UpdateCallback& cbk) {
			this->getInstance().addRegularCallback(cbk, prior);
		}
	);
}

inline void ZuazoBase::disableRegularUpdate() const {
	visitCallbacks(
		[=] (const UpdateCallback& cbk) {
			this->getInstance().removeRegularCallback(cbk);
		}
	);
}

inline void ZuazoBase::enablePeriodicUpdate(Instance::Priority prior, 
											Timing::Duration period) const 
{
	visitCallbacks(
		[=] (const UpdateCallback& cbk) {
			this->getInstance().addPeriodicCallback(cbk, prior, period);
		}
	);
}

inline void ZuazoBase::disablePeriodicUpdate() const {
	visitCallbacks(
		[=] (const UpdateCallback& cbk) {
			this->getInstance().removePeriodicCallback(cbk);
		}
	);
}

}