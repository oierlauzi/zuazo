#include <Video.h>

namespace Zuazo {



/*
 * VideoModeCompatibility
 */



/*
 * VideoBase
 */

VideoBase::VideoBase(const VideoMode& videoMode)
	: m_videoMode(videoMode)
{
}

VideoBase::VideoBase(const VideoBase& other) = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(const VideoBase& other) = default;

VideoModeCompatibility VideoBase::getVideoModeCompatibility() const {
	return VideoModeCompatibility();
}

void VideoBase::setVideoMode(const VideoMode& videoMode) {
	m_videoMode = videoMode;
}

const VideoMode& VideoBase::getVideoMode() const {
	return m_videoMode;
}

}