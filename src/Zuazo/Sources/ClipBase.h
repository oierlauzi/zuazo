#pragma once

#include <bits/stdint-intn.h>
#include <sys/types.h>
#include <chrono>

#include "../Stream/SourceBase.h"
#include "../Timing/TimeInterval.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../VideoBase.h"

namespace Zuazo::Sources{

class ClipBase : virtual public Stream::SourceBase
{
public:
	ClipBase();
	ClipBase(const ClipBase& other);
	virtual ~ClipBase()=default;

	void gotoTime(const Timing::TimeInterval& tp);
	void gotoProgress(double p);
	void addTime(const Timing::TimeInterval& time);
	void subsTime(const Timing::TimeInterval& time);

	bool hasOverflow() const;
	void resetOverflow();

	Timing::TimeInterval getCurrentTime() const;
	Timing::TimeInterval getDuration() const;
	double getProgress() const;
protected:
	void setInfo(const Timing::TimeInterval& duration);
private:
	Timing::TimeInterval		m_currTime;
	Timing::TimeInterval 		m_duration;
	bool						m_hasOverflow;
};




class VideoClipBase
	: public ClipBase
	, public VideoBase
{
public:
	VideoClipBase();
	VideoClipBase(const VideoClipBase& other)=default;
	virtual ~VideoClipBase()=default;

	void nextFrame();
	void prevFrame();
	void gotoFrame(u_int64_t nb);

	Utils::Rational getInterval() const;
	Utils::Rational getRate() const;
	u_int64_t getFrameNb() const;
	Utils::Resolution getRes() const;
protected:
	void setInfo(const Timing::TimeInterval& duration, u_int64_t frames, const Utils::Resolution& res);
private:
	u_int64_t				m_nbFrames;
	Utils::Rational			m_frameInterval;
	Utils::Resolution		m_resolution;
};


/*
 * CLIP BASE'S INLINE METHODS
 */

inline void ClipBase::gotoProgress(double p){
	gotoTime(Timing::TimeInterval(
			static_cast<int64_t>(m_duration.count() * p)
	));
}

inline void ClipBase::addTime(const Timing::TimeInterval& time){
	gotoTime(m_currTime + time);
}

inline void ClipBase::subsTime(const Timing::TimeInterval& time){
	gotoTime(m_currTime - time);
}

inline bool ClipBase::hasOverflow() const{
	return m_hasOverflow;
}

inline void ClipBase::resetOverflow(){
	m_hasOverflow=false;
}

inline Timing::TimeInterval ClipBase::getCurrentTime() const{
	return m_currTime;
}

inline Timing::TimeInterval ClipBase::getDuration() const{
	return m_duration;
}

inline double ClipBase::getProgress() const{
	return (double)m_currTime.count() / m_duration.count() ;
}

/*
 * VIDEO CLIP BASE'S INLINE METHODS
 */

inline void VideoClipBase::nextFrame(){
	ClipBase::addTime(m_frameInterval);
}

inline void VideoClipBase::prevFrame(){
	ClipBase::addTime(m_frameInterval);
}

inline void VideoClipBase::gotoFrame(u_int64_t nb){
	gotoProgress((double)nb / m_nbFrames);
}

inline Utils::Rational VideoClipBase::getInterval() const {
	return m_frameInterval;
}

inline Utils::Rational VideoClipBase::getRate() const {
	return 1 / m_frameInterval;
}

inline u_int64_t VideoClipBase::getFrameNb() const {
	return m_nbFrames;
}

inline Utils::Resolution VideoClipBase::getRes() const {
	return m_resolution;
}


}
