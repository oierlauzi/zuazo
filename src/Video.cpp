#include <Video.h>

namespace Zuazo {


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

VideoMode::Compatibilities VideoBase::getVideoModeCompatibility() const {
	return VideoMode::Compatibilities();
}

void VideoBase::setVideoMode(const VideoMode& videoMode) {
	m_videoMode = videoMode;
}

const VideoMode& VideoBase::getVideoMode() const {
	return m_videoMode;
}

}