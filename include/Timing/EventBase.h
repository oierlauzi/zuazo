#pragma once

#include "Chrono.h"

#include <functional>
#include <array>

namespace Zuazo::Timing {

class EventBase {
public:
	using Callback = std::function<void()>;

	EventBase() = default;
	EventBase(const EventBase& other) = default;
	EventBase(EventBase&& other) = default;
	virtual ~EventBase() = default;

	EventBase& 			operator=(const EventBase& other) = default;
	EventBase& 			operator=(EventBase&& other) = default;

	void        		update() const;

	template <typename Cbk>
	void				setPreUpdateCallback(Cbk&& cbk);
	const Callback&		setPreUpdateCallback() const;

	template <typename Cbk>
	void				setPostUpdateCallback(Cbk&& cbk);
	const Callback&		setPostUpdateCallback() const;

protected:
	template <typename Cbk>
	void				setUpdateCallback(Cbk&& cbk);
	const Callback&		setUpdateCallback() const;

private:
	enum Moments {
		PRE_UPDATE,
		UPDATE,
		POST_UPDATE,

		COUNT
	};

	using Callbacks = std::array<Callback, COUNT>;

	Callbacks			m_callbacks;

};

}

#include "EventBase.inl"