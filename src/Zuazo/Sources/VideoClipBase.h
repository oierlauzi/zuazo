#pragma once

#include <sys/types.h>
#include <chrono>

#include "../Timing/TimeInterval.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../VideoBase.h"
#include "ClipBase.h"

namespace Zuazo::Sources{

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
 * INLINE METHODS
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
