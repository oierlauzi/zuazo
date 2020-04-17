#include "EventBase.h"

namespace Zuazo::Timing {

inline void EventBase::update() const {
	//Call the callbacks in order
	for(const auto& cbk : m_callbacks) {
		if(cbk) {
			cbk();
		}
	}
}

template <typename Cbk>
inline void EventBase::setPreUpdateCallback(Cbk&& cbk) {
	m_callbacks[PRE_UPDATE] = std::forward<Cbk>(cbk);
}

inline const EventBase::Callback& EventBase::setPreUpdateCallback() const {
	return m_callbacks[PRE_UPDATE];
}


template <typename Cbk>
inline void EventBase::setPostUpdateCallback(Cbk&& cbk) {
	m_callbacks[POST_UPDATE] = std::forward<Cbk>(cbk);
}

inline const EventBase::Callback& EventBase::setPostUpdateCallback() const {
	return m_callbacks[POST_UPDATE];
}


template <typename Cbk>
inline void EventBase::setUpdateCallback(Cbk&& cbk) {
	m_callbacks[UPDATE] = std::forward<Cbk>(cbk);
}

inline const EventBase::Callback& EventBase::setUpdateCallback() const {
	return m_callbacks[UPDATE];
}

}