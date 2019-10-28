#pragma once

#include "EventBase.h"
#include "Chrono.h"

namespace Zuazo::Timing {

class ExternalEvent : public EventBase {
public:
	using EventBase::EventBase;

	ExternalEvent& operator=(const ExternalEvent& other) = default;
	ExternalEvent& operator=(ExternalEvent&& other) = default;

	void invoke();
};

}

#include "ExternalEvent.inl"