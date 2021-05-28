#include <zuazo/ClipBase.h>

#include <zuazo/StringConversions.h>
#include <zuazo/Utils/Functions.h>
#include <zuazo/Math/Comparisons.h>
#include <zuazo/Math/Periodic.h>

#include <cassert>

namespace Zuazo {

/*
 * ClipBase::Impl
 */

struct ClipBase::Impl {
	TimePoint						time;
	Duration						duration;
	Duration						timeStep;

	State							state;
	Repeat							repeat;
	double							playSpeed;

	RefreshCallback					refreshCallback;

	Impl(Duration duration, Duration timeStep, RefreshCallback refreshCbk) noexcept
		: time()
		, duration(duration)
		, timeStep(timeStep)
		, state(State::PAUSED)
		, repeat(Repeat::NONE)
		, playSpeed(1.0)
		, refreshCallback(std::move(refreshCbk))
	{
	}

	~Impl() = default;


	void setState(State st) noexcept {
		state = st;
	}

	State getState() const noexcept {
		return state;
	}


	void setRepeat(Repeat rep) noexcept {
		repeat = rep;
	}

	Repeat getRepeat() const noexcept {
		return repeat;
	}


	void setTime(ClipBase& base, TimePoint tp) noexcept {
		const auto minTime = TimePoint();
		const auto maxTime = TimePoint(duration);

		//Ensure time point is within bounds
		switch(repeat) {
		case Repeat::REPEAT:
			//Add duration to account for negative time values
			tp = TimePoint(Math::sawtooth(duration, tp.time_since_epoch()));
			break;

		case Repeat::PING_PONG:
			//Similar to the above one, but with double of the duration. If tp falls in the "second" duration,
			//it gets inverted to simulate the ping-pong behaviour. playSpeed also gets inverted
			if(tp < minTime) playSpeed = Math::abs(playSpeed);
			else if(tp > maxTime) playSpeed = -Math::abs(playSpeed);
			tp = TimePoint(Math::triangle(duration * 2, tp.time_since_epoch()) / 2);
			break;

		default: //Repeat::NONE
			tp = Math::clamp(tp, minTime, maxTime);
			break;
		}
		assert(Math::isInRange(tp, minTime, maxTime));

		if(tp != time) {
			time = tp;
			Utils::invokeIf(refreshCallback, base);
		}
	}

	TimePoint getTime() const noexcept {
		return time;
	}


	void setPlaySpeed(double speed) noexcept {
		playSpeed = speed;
	}

	double getPlaySpeed() const noexcept {
		return playSpeed;
	}


	void setProgress(ClipBase& base, double progress) noexcept {
		setTime(base, TimePoint(Duration(static_cast<Duration::rep>(getDuration().count() * progress))));
	}

	double getProgress() const noexcept {
		return static_cast<double>(getTime().time_since_epoch().count()) / static_cast<double>(getDuration().count());
	}


	void advance(ClipBase& base, Duration delta) noexcept {
		if(state == State::PLAYING){
			advanceNormalSpeed(base, Duration(static_cast<Duration::rep>(delta.count() * playSpeed)));
		} 
	}

	void rewind(ClipBase& base, Duration delta) noexcept {
		advance(base, -delta);
	}

	void advanceNormalSpeed(ClipBase& base, Duration delta) noexcept {
		setTime(base, getTime() + delta);
	}

	void rewindNormalSpeed(ClipBase& base, Duration delta) noexcept {
		advanceNormalSpeed(base, -delta);
	}

	void nextStep(ClipBase& base) noexcept {
		advanceNormalSpeed(base, timeStep);
	}

	void prevStep(ClipBase& base) noexcept {
		rewindNormalSpeed(base, timeStep);
	}

	void play() noexcept {
		setState(State::PLAYING);
	}

	void pause() noexcept {
		setState(State::PAUSED);
	}

	void stop(ClipBase& base) noexcept {
		setState(State::PAUSED);
		setTime(base, TimePoint());
	}

	bool isPlaying() const noexcept {
		return getState() == State::PLAYING;
	}


	Duration getDuration() const noexcept {
		return duration;
	}

	Duration getTimeStep() const noexcept {
		return timeStep;
	}



	void setRefreshCallback(RefreshCallback cbk) noexcept {
		refreshCallback = std::move(cbk);
	}

	const RefreshCallback& getRefreshCallback() const noexcept {
		return refreshCallback;
	}


	void setTimeStep(Duration step) noexcept {
		timeStep = step;
	}

	void setDuration(ClipBase& base, Duration dur) noexcept {
		assert(dur >= Duration(0));
		duration = dur;
		setTime(base, getTime()); //So that in case it is shorter, time gets updated
	}

};


/*
 * ClipBase
 */

ClipBase::ClipBase(	Duration duration,
					Duration timeStep,
					RefreshCallback refreshCbk )
	: m_impl({}, duration, timeStep, std::move(refreshCbk))
{
}

ClipBase::ClipBase(ClipBase&& other) noexcept = default;

ClipBase::~ClipBase() = default;


ClipBase& ClipBase::operator=(ClipBase&& other) noexcept = default;


void ClipBase::setState(State st) noexcept {
	m_impl->setState(st);
}

ClipBase::State ClipBase::getState() const noexcept {
	return m_impl->getState();
}


void ClipBase::setRepeat(Repeat repeat) noexcept {
	m_impl->setRepeat(repeat);
}

ClipBase::Repeat ClipBase::getRepeat() const noexcept {
	return m_impl->getRepeat();
}


void ClipBase::setTime(TimePoint tp) noexcept {
	m_impl->setTime(*this, tp);
}

TimePoint ClipBase::getTime() const noexcept {
	return m_impl->getTime();
}


void ClipBase::setPlaySpeed(double speed) noexcept {
	m_impl->setPlaySpeed(speed);
}

double ClipBase::getPlaySpeed() const noexcept {
	return m_impl->getPlaySpeed();
}


void ClipBase::setProgress(double progress) noexcept {
	m_impl->setProgress(*this, progress);
}

double ClipBase::getProgress() const noexcept {
	return m_impl->getProgress();
}


void ClipBase::advance(Duration delta) noexcept {
	m_impl->advance(*this, delta);
}

void ClipBase::rewind(Duration delta) noexcept {
	m_impl->rewind(*this, delta);
}

void ClipBase::advanceNormalSpeed(Duration delta) noexcept {
	m_impl->advanceNormalSpeed(*this, delta);
}

void ClipBase::rewindNormalSpeed(Duration delta) noexcept {
	m_impl->rewindNormalSpeed(*this, delta);
}

void ClipBase::nextStep() noexcept {
	m_impl->nextStep(*this);
}

void ClipBase::prevStep() noexcept {
	m_impl->prevStep(*this);
}

void ClipBase::play() noexcept {
	m_impl->play();
}

void ClipBase::pause() noexcept {
	m_impl->pause();
}

void ClipBase::stop() noexcept {
	m_impl->stop(*this);
}

bool ClipBase::isPlaying() const noexcept {
	return m_impl->isPlaying();
}


Duration ClipBase::getDuration() const noexcept {
	return m_impl->getDuration();
}

Duration ClipBase::getTimeStep() const noexcept {
	return m_impl->getTimeStep();
}



void ClipBase::setRefreshCallback(RefreshCallback cbk) noexcept {
	m_impl->setRefreshCallback(std::move(cbk));
}

const ClipBase::RefreshCallback& ClipBase::getRefreshCallback() const noexcept {
	return m_impl->getRefreshCallback();
}


void ClipBase::setTimeStep(Duration step) noexcept {
	m_impl->setTimeStep(step);
}

void ClipBase::setDuration(Duration dur) noexcept {
	m_impl->setDuration(*this, dur);
}





std::string_view toString(ClipBase::State state) noexcept {
	switch(state){

	ZUAZO_ENUM2STR_CASE( ClipBase::State, PAUSED )
	ZUAZO_ENUM2STR_CASE( ClipBase::State, PLAYING )

	default: return "";
	}
}

bool fromString(std::string_view str, ClipBase::State& state) {
	return enumFromString(str, state);
}

std::ostream& operator<<(std::ostream& os, ClipBase::State state) {
	return os << toString(state);
}



std::string_view toString(ClipBase::Repeat repeat) noexcept {
	switch(repeat){

	ZUAZO_ENUM2STR_CASE( ClipBase::Repeat, REPEAT )
	ZUAZO_ENUM2STR_CASE( ClipBase::Repeat, PING_PONG )

	default: return "";
	}
}

bool fromString(std::string_view str, ClipBase::Repeat& repeat) {
	return enumFromString(str, repeat);
}

std::ostream& operator<<(std::ostream& os, ClipBase::Repeat repeat) {
	return os << toString(repeat);
}


}