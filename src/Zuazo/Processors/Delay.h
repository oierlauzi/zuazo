#pragma once

#include "Delay.h"

#include <memory>
#include <queue>

#include "../Consumers/ConsumerBase.h"
#include "../Sources/SourceBase.h"
#include "../Timing/RegularUpdate.h"
#include "../UpdateOrder.h"
#include "../Timing/TimeInterval.h"
#include "../Timing/TimePoint.h"
#include "../Timing/Timing.h"

namespace Zuazo::Processors{

class Delay :
		public Sources::SourceBase,
		public Consumers::ConsumerBase,
		public Timing::RegularUpdate< (u_int32_t)UpdateOrder::DELAY >
{
public:
	Delay();
	Delay(const Timing::TimeInterval& delay);
	Delay(const Delay& delay)=default;
	virtual ~Delay();

	void									setDelay(const Timing::TimeInterval& delay);
	Timing::TimeInterval					getDelay();

	virtual void							update() const override;

	virtual void							open() override;
	virtual void							close() override;
private:
	struct QueueElement{
		std::shared_ptr<const Packet>		element;
		Timing::TimePoint					ts;

		QueueElement(const std::shared_ptr<const Packet>& el) :
			element(el),
			ts(Timing::timings->now())
		{
		}
	};

	Timing::TimeInterval 					m_delay;

	mutable std::queue<QueueElement>		m_queue;
};
}
