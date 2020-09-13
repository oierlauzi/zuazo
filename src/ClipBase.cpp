#include <zuazo/ClipBase.h>

#include <zuazo/Utils/Functions.h>
#include <zuazo/Math/Functions.h>

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

	Impl(Duration duration, Duration timeStep, RefreshCallback refreshCbk) 
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


	void setState(State st) {
		state = st;
	}

	State getState() const {
		return state;
	}


	void setRepeat(Repeat rep) {
		repeat = rep;
	}

	Repeat getRepeat() const {
		return repeat;
	}


	void setTime(ClipBase& base, TimePoint tp) {
		const auto minTime = TimePoint();
		const auto maxTime = TimePoint(duration);

		//Ensure time point is within bounds
		switch(repeat) {
		case Repeat::REPEAT:
			//Add duration to account for negative time values
			tp = TimePoint(((tp.time_since_epoch() % duration) + duration) % duration);
			break;

		case Repeat::PING_PONG:
			//Similar to the above one, but with double of the duration. If tp falls in the "second" duration,
			//it gets inverted to simulate the ping-pong behaviour. playSpeed also gets inverted
			tp = TimePoint(((tp.time_since_epoch() % (duration * 2)) + (duration * 2)) % (duration * 2));
			assert(Math::isInRange(tp, minTime, TimePoint(duration * 2)));
			
			if(tp > maxTime) {
				playSpeed = -playSpeed;
				tp = TimePoint(2 * duration - tp.time_since_epoch());
			}

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

	TimePoint getTime() const {
		return time;
	}


	void setPlaySpeed(double speed) {
		playSpeed = speed;
	}

	double getPlaySpeed() const {
		return playSpeed;
	}


	void setProgress(ClipBase& base, double progress) {
		setTime(base, TimePoint(Duration(static_cast<Duration::rep>(getDuration().count() * progress))));
	}

	double getProgress() const {
		return static_cast<double>(getTime().time_since_epoch().count()) / static_cast<double>(getDuration().count());
	}


	void advance(ClipBase& base, Duration delta) {
		if(state == State::PLAYING){
			advanceNormalSpeed(base, Duration(static_cast<Duration::rep>(delta.count() * playSpeed)));
		} 
	}

	void rewind(ClipBase& base, Duration delta) {
		advance(base, -delta);
	}

	void advanceNormalSpeed(ClipBase& base, Duration delta) {
		setTime(base, getTime() + delta);
	}

	void rewindNormalSpeed(ClipBase& base, Duration delta) {
		advanceNormalSpeed(base, -delta);
	}

	void nextStep(ClipBase& base) {
		advanceNormalSpeed(base, timeStep);
	}

	void prevStep(ClipBase& base) {
		rewindNormalSpeed(base, timeStep);
	}

	void play() {
		setState(State::PLAYING);
	}

	void pause() {
		setState(State::PAUSED);
	}

	void stop(ClipBase& base) {
		setState(State::PAUSED);
		setTime(base, TimePoint());
	}

	bool isPlaying() const {
		return getState() == State::PLAYING;
	}


	Duration getDuration() const {
		return duration;
	}

	Duration getTimeStep() const {
		return timeStep;
	}



	void setRefreshCallback(RefreshCallback cbk) {
		refreshCallback = std::move(cbk);
	}

	const RefreshCallback& getRefreshCallback() const {
		return refreshCallback;
	}


	void setTimeStep(Duration step) {
		timeStep = step;
	}

	void setDuration(ClipBase& base, Duration dur) {
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

ClipBase::ClipBase(ClipBase&& other) = default;

ClipBase::~ClipBase() = default;


ClipBase& ClipBase::operator=(ClipBase&& other) = default;


void ClipBase::setState(State st) {
	m_impl->setState(st);
}

ClipBase::State ClipBase::getState() const {
	return m_impl->getState();
}


void ClipBase::setRepeat(Repeat repeat) {
	m_impl->setRepeat(repeat);
}

ClipBase::Repeat ClipBase::getRepeat() const {
	return m_impl->getRepeat();
}


void ClipBase::setTime(TimePoint tp) {
	m_impl->setTime(*this, tp);
}

TimePoint ClipBase::getTime() const {
	return m_impl->getTime();
}


void ClipBase::setPlaySpeed(double speed) {
	m_impl->setPlaySpeed(speed);
}

double ClipBase::getPlaySpeed() const {
	return m_impl->getPlaySpeed();
}


void ClipBase::setProgress(double progress) {
	m_impl->setProgress(*this, progress);
}

double ClipBase::getProgress() const {
	return m_impl->getProgress();
}


void ClipBase::advance(Duration delta) {
	m_impl->advance(*this, delta);
}

void ClipBase::rewind(Duration delta) {
	m_impl->rewind(*this, delta);
}

void ClipBase::advanceNormalSpeed(Duration delta) {
	m_impl->advanceNormalSpeed(*this, delta);
}

void ClipBase::rewindNormalSpeed(Duration delta) {
	m_impl->rewindNormalSpeed(*this, delta);
}

void ClipBase::nextStep() {
	m_impl->nextStep(*this);
}

void ClipBase::prevStep() {
	m_impl->prevStep(*this);
}

void ClipBase::play() {
	m_impl->play();
}

void ClipBase::pause() {
	m_impl->pause();
}

void ClipBase::stop() {
	m_impl->stop(*this);
}

bool ClipBase::isPlaying() const {
	return m_impl->isPlaying();
}


Duration ClipBase::getDuration() const {
	return m_impl->getDuration();
}

Duration ClipBase::getTimeStep() const {
	return m_impl->getTimeStep();
}



void ClipBase::setRefreshCallback(RefreshCallback cbk) {
	m_impl->setRefreshCallback(std::move(cbk));
}

const ClipBase::RefreshCallback& ClipBase::getRefreshCallback() const {
	return m_impl->getRefreshCallback();
}


void ClipBase::setTimeStep(Duration step) {
	m_impl->setTimeStep(step);
}

void ClipBase::setDuration(Duration dur) {
	m_impl->setDuration(*this, dur);
}

}