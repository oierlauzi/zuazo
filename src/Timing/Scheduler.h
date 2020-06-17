#pragma once

#include <Chrono.h>

#include <limits>
#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <memory>

namespace Zuazo::Timing {

class Scheduler {
public:
	using Priority = int32_t;
	using Callback = std::function<void()>;

	Scheduler() = default;
	Scheduler(const Scheduler& other) = delete;
	Scheduler(Scheduler&& other) = default;
	~Scheduler() = default;

	Scheduler&				operator=(const Scheduler& other) = delete;
	Scheduler&				operator=(Scheduler&& other) = default;

	void					setEpoch(TimePoint tp);
	TimePoint				getEpoch() const;

	void					gotoTime(TimePoint tp);
	TimePoint				getTime() const;
	Duration				getDeltaT() const;

	Duration               	getTimeForNextEvent() const;

	void					addRegularCallback(const std::shared_ptr<Callback>& cbk, Priority prior);
	void					removeRegularCallback(const std::shared_ptr<Callback>& cbk);

	void					addPeriodicCallback(const std::shared_ptr<Callback>& cbk, Priority prior, Duration period);
	void					removePeriodicCallback(const std::shared_ptr<Callback>& cbk);

private:
	using CallbackSet = std::vector<std::tuple<std::shared_ptr<Callback>, Priority>>;
	using PeriodMap = std::map<Duration, CallbackSet>;

	TimePoint				m_epoch;
	TimePoint 				m_currTime;
	Duration 				m_deltaT;
	CallbackSet 			m_calls;

	CallbackSet 			m_regularCallbacks;
	PeriodMap 				m_periodicCallbacks;
	
};

}