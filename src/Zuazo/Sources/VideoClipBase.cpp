#include "VideoClipBase.h"

using namespace Zuazo::Sources;

VideoClipBase::VideoClipBase() :
	m_nbFrames(0),
	m_frameInterval()
{
}

void VideoClipBase::setInfo(const Timing::TimeInterval& duration, u_int64_t frames, const Utils::Resolution& res){
	ClipBase::setInfo(duration);
	m_nbFrames=frames;
	m_frameInterval=Utils::Rational(duration.count(), frames * Timing::TimeInterval::TIME_UNITS_PER_SECOND);
	m_resolution=res;
}
