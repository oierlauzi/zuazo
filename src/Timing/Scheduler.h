#pragma once

#include <zuazo/Chrono.h>

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
	Scheduler(Scheduler&& other) noexcept  = default;
	~Scheduler() = default;

	Scheduler&				operator=(const Scheduler& other) = delete;
	Scheduler&				operator=(Scheduler&& other) noexcept  = default;

	void					setEpoch(TimePoint tp) noexcept ;
	TimePoint				getEpoch() const noexcept ;

	void					gotoTime(TimePoint tp);
	TimePoint				getTime() const noexcept ;
	Duration				getDeltaT() const noexcept ;

	Duration               	getTimeForNextEvent() const noexcept;

	void					addRegularCallback(std::shared_ptr<const Callback> cbk, Priority prior);
	void					removeRegularCallback(const std::shared_ptr<const Callback>& cbk);

	void					addPeriodicCallback(std::shared_ptr<const Callback> cbk, Priority prior, Duration period);
	void					removePeriodicCallback(const std::shared_ptr<const Callback>& cbk);

private:
	using CallbackSet = std::vector<std::tuple<std::shared_ptr<const Callback>, Priority>>;
	using PeriodMap = std::map<Duration, CallbackSet>;

	TimePoint				m_epoch;
	TimePoint 				m_currTime;
	Duration 				m_deltaT;
	CallbackSet 			m_calls;

	CallbackSet 			m_regularCallbacks;
	PeriodMap 				m_periodicCallbacks;
	
};

}