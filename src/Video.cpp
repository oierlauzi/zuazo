#include <Video.h>

#include <Macros.h>

namespace Zuazo {

std::string_view toString(VideoModeParameters par) {
	switch(par){

	ZUAZO_ENUM2STR_CASE( VideoModeParameters, resolution )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, pixelAspectRatio )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, frameRate )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorPrimaries )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorModel )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorTransferFunction )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorSubsampling )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorRange )
	ZUAZO_ENUM2STR_CASE( VideoModeParameters, colorFormat )

	default: return "";
	}
}



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