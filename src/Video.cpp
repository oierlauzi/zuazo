#include <Video.h>

#include <Exception.h>
#include <Macros.h>

namespace Zuazo {

VideoMode::VideoMode(Resolution res, Rate frameRate) 
	: Descriptor(res)
{
	setFrameRate(frameRate);
}

void VideoMode::setFrameRate(Rate rate) {
	return set(FRAME_RATE, rate);
}

Rate VideoMode::getFrameRate() const {
	return get<Rate>(FRAME_RATE);
}



VideoBase::VideoBase(const VideoMode& videoMode)
	: m_videoMode(videoMode)
{
}

VideoBase::VideoBase(const VideoBase& other) = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(const VideoBase& other) = default;



void VideoBase::setVideoMode(const VideoMode& videoMode) {
	m_videoMode = videoMode;
}

const VideoMode& VideoBase::getVideoMode() const {
	return m_videoMode;
}


const std::vector<Utils::Compatibility>& VideoBase::getVideoModeCompatibility() const {
	return m_compatibilities;
}

void VideoBase::setVideoModeCompatibility(const std::vector<Utils::Compatibility>& comp) {
	m_compatibilities = comp;
}

void VideoBase::validate(const VideoMode& config) {
	const auto validation = Utils::Compatibility::validate(m_compatibilities, config);
	if(validation.missmatchCount) {
		throw Exception("Unsupported video mode");
	}
}

}