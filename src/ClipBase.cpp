#include <zuazo/ClipBase.h>

#include <zuazo/Math/Functions.h>

namespace Zuazo {

ClipBase::ClipBase(	Duration duration,
					Duration timeStep,
					RefreshCallback refreshCbk )
	: m_timePoint()
	, m_duration(duration)
	, m_timeStep(timeStep)
	, m_state(State::PAUSED)
	, m_repeat(Repeat::NONE)
	, m_playSpeed(1.0)
	, m_refreshCallback(std::move(refreshCbk))
{
}



void ClipBase::setState(State st) {
	m_state = st;
}

ClipBase::State ClipBase::getState() const {
	return m_state;
}


void ClipBase::setRepeat(Repeat repeat) {
	m_repeat = repeat;
}

ClipBase::Repeat ClipBase::getRepeat() const {
	return m_repeat;
}


void ClipBase::setTime(TimePoint tp) {
	//Ensure time point is within bounds
	switch(m_repeat) {
	case Repeat::REPEAT:
		tp = TimePoint(((tp.time_since_epoch() % m_duration) + m_duration) % m_duration);
		break;

	case Repeat::PING_PONG:
		m_playSpeed = -m_playSpeed;
		//break; //TODO

	default: //Repeat::NONE
		tp = Math::clamp(tp, TimePoint(), TimePoint(m_duration));
		break;
	}
	assert(TimePoint() <= tp && tp <= TimePoint(m_duration));

	if(tp != m_timePoint) {
		m_timePoint = tp;
		if(m_refreshCallback) m_refreshCallback(*this);
	}
}

TimePoint ClipBase::getTime() const {
	return m_timePoint;
}


void ClipBase::setPlaySpeed(double speed) {
	m_playSpeed = speed;
}

double ClipBase::getPlaySpeed() const {
	return m_playSpeed;
}


void ClipBase::advance(Duration delta) {
	if(m_state == State::PLAYING){
		advanceNormalSpeed(Duration(static_cast<Duration::rep>(delta.count() * m_playSpeed)));
	} 
}

void ClipBase::rewind(Duration delta) {
	advance(-delta);
}

void ClipBase::advanceNormalSpeed(Duration delta) {
	setTime(m_timePoint + delta);
}

void ClipBase::rewindNormalSpeed(Duration delta) {
	advanceNormalSpeed(-delta);
}

void ClipBase::nextStep() {
	advanceNormalSpeed(m_timeStep);
}

void ClipBase::prevStep() {
	rewindNormalSpeed(m_timeStep);
}

void ClipBase::play() {
	setState(State::PLAYING);
}

void ClipBase::pause() {
	setState(State::PAUSED);
}

void ClipBase::stop() {
	setState(State::PAUSED);
	setTime(TimePoint());
}

bool ClipBase::isPlaying() const {
	return getState() == State::PLAYING;
}



void ClipBase::setRefreshCallback(RefreshCallback cbk) {
	m_refreshCallback = std::move(cbk);
}

const ClipBase::RefreshCallback& ClipBase::getRefreshCallback() const {
	return m_refreshCallback;
}


void ClipBase::setTimeStep(Duration step) {
	m_timeStep = step;
}

void ClipBase::setDuration(Duration dur) {
	m_duration = dur;
}


}