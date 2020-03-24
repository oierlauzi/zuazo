#pragma once

#include "Chrono.h"

#include <functional>

namespace Zuazo::Timing {

class EventBase {
public:
	using UpdateCbk = std::function<void()>;

	EventBase() = default;
	EventBase(const EventBase& other) = default;
	EventBase(EventBase&& other) = default;
	virtual ~EventBase() = default;

	EventBase& 			operator=(const EventBase& other) = default;
	EventBase& 			operator=(EventBase&& other) = default;

	virtual void        enable();
	virtual void        disable();
	bool                isEnabled() const;

	void				setCallback(UpdateCbk&& cbk);
	const UpdateCbk&	getCallback() const;
protected:
	void        		update();
	
private:
	bool                m_isEnabled = false;
	UpdateCbk			m_callback;
};

}