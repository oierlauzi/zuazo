#include <zuazo/Video.h>

#include <zuazo/Exception.h>
#include <zuazo/Macros.h>

namespace Zuazo {

VideoMode::VideoMode() {
	setFrameRateLimits(makeLimit<Any<Rate>>());
}


void VideoMode::setFrameRate(Rate rate) {
	setFrameRateLimits(makeLimit<MustBe<Rate>>(rate));
}

Rate VideoMode::getFrameRate() const {
	return getValue<Rate>(FRAME_RATE).value();
}

void VideoMode::setFrameRateLimits(TypedLimitPtr<Rate> rate) {
	set(FRAME_RATE, std::move(rate));
}

VideoMode::TypedLimitPtr<Rate> VideoMode::getFrameRateLimits() const {
	return getLimit<TypedLimitBase<Rate>>(FRAME_RATE);
}



VideoBase::VideoBase() = default;

VideoBase::VideoBase(VideoMode videoMode)
	: m_videoMode(std::move(videoMode))
{
}

VideoBase::VideoBase(const VideoBase& other) = default;

VideoBase::VideoBase(VideoBase&& other) = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(const VideoBase& other) = default;



void VideoBase::setVideoMode(const VideoMode& videoMode) {
	m_videoMode = videoMode;
}

const VideoMode& VideoBase::getVideoMode() const {
	return m_videoMode;
}


const std::vector<VideoMode>& VideoBase::getVideoModeCompatibility() const {
	return m_compatibilities;
}

void VideoBase::setVideoModeCompatibility(std::vector<VideoMode> comp) {
	m_compatibilities = std::move(comp);
}

VideoMode VideoBase::validate(const VideoMode& config) {
	VideoMode result;
	std::tie(result, std::ignore) = config.intersection(m_compatibilities.cbegin(), m_compatibilities.cend());

	if(!result) {
		throw Exception("Unsupported video mode");
	}
	
	return result;
}

}