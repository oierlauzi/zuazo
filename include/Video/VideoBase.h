#pragma once

#include <mutex>
#include <set>

#include "../Utils/Codec.h"
#include "../Utils/PixelFormat.h"
#include "../Utils/Rational.h"
#include "../Utils/Resolution.h"
#include "../Utils/VideoMode.h"

namespace Zuazo::Video{

class VideoBase{
public:
	VideoBase()=default;
	VideoBase(const VideoBase& other)=default;
	VideoBase(const Utils::VideoMode& vidMode);
	virtual ~VideoBase()=default;

	virtual bool							supportsGettingPixelFormat() const { return false; }
	virtual bool							supportsSettingPixelFormat() const { return false; }
	virtual bool							supportsAnyPixelFormat() const { return false; }
	virtual bool							supportsListingPixelFormats() const { return false; }
	#define SUPPORTS_GETTING_PIXELFORMAT 	virtual bool supportsGettingPixelFormat() const override { return true; }
	#define SUPPORTS_SETTING_PIXELFORMAT 	virtual bool supportsSettingPixelFormat() const override { return true; }
	#define SUPPORTS_ANY_PIXELFORMAT 		virtual bool supportsAnyPixelFormat() const override { return true; }
	#define SUPPORTS_LISTING_PIXELFORMATS	virtual bool supportsListingPixelFormats() const { return true; }
	virtual std::set<Utils::PixelFormat>	getSupportedPixelFormats() const{ return std::set<Utils::PixelFormat>(); }
	const Utils::PixelFormat& 				getPixelFormat() const;
	virtual void							setPixelFormat(const Utils::PixelFormat& pixFmt){}

	virtual bool							supportsGettingResolution() const { return false; }
	virtual bool							supportsSettingResolution() const { return false; }
	virtual bool							supportsAnyResolution() const { return false; }
	virtual bool							supportsListingResolutions() const { return false; }
	#define SUPPORTS_GETTING_RESOLUTION 	virtual bool supportsGettingResolution() const override { return true; }
	#define SUPPORTS_SETTING_RESOLUTION 	virtual bool supportsSettingResolution() const override { return true; }
	#define SUPPORTS_ANY_RESOLUTION			virtual bool supportsAnyResolution() const override { return true; }
	#define SUPPORTS_LISTING_RESOLUTIONS	virtual bool supportsListingResolutions() const { return true; }
	virtual std::set<Utils::Resolution>		getSupportedResolutions() const{ return std::set<Utils::Resolution>(); }
	const Utils::Resolution& 				getResolution() const;
	virtual void							setResolution(const Utils::Resolution& res){}

	virtual bool							supportsGettingCodec() const { return false; }
	virtual bool							supportsSettingCodec() const { return false; }
	virtual bool							supportsAnyCodec() const { return false; }
	virtual bool							supportsListingCodecs() const { return false; }
	#define SUPPORTS_GETTING_CODEC 			virtual bool supportsGettingCodec() const override { return true; }
	#define SUPPORTS_SETTING_CODEC 			virtual bool supportsSettingCodec() const override { return true; }
	#define SUPPORTS_ANY_CODEC 				virtual bool supportsAnyCodec() const override { return true; }
	#define SUPPORTS_LISTING_CODECS			virtual bool supportsListingCodecs() const { return true; }
	virtual std::set<Utils::Codec>			getSupportedCodecs() const{ return std::set<Utils::Codec>(); }
	const Utils::Codec&						getCodec() const;
	virtual void							setCodec(const Utils::Codec& codec){}

	virtual bool							supportsGettingFramerate() const { return false; }
	virtual bool							supportsSettingFramerate() const { return false; }
	virtual bool							supportsAnyFramerate() const { return false; }
	virtual bool							supportsListingFramerates() const { return false; }
	#define SUPPORTS_GETTING_FRAMERATE		virtual bool supportsGettingFramerate() const override { return true; }
	#define SUPPORTS_SETTING_FRAMERATE		virtual bool supportsSettingFramerate() const override { return true; }
	#define SUPPORTS_ANY_FRAMERATE 			virtual bool supportsAnyFramerate() const override { return true; }
	#define SUPPORTS_LISTING_FRAMERATES		virtual bool supportsListingFramerates() const { return true; }
	virtual std::set<Utils::Rational>		getSupportedFramerates() const{ return std::set<Utils::Rational>(); }
	const Utils::Rational&					getFramerate() const;
	virtual void							setFramerate(const Utils::Rational& rat){}

	virtual bool							supportsGettingProgressive() const { return false; }
	virtual bool							supportsSettingProgressive() const { return false; }
	#define SUPPORTS_GETTING_PROGRESSIVE	virtual bool supportsGettingProgressive() const override { return true; }
	#define SUPPORTS_SETTING_PROGRESSIVE	virtual bool supportsSettingProgressive() const override { return true; }
	bool									isProgressive() const;
	virtual void							setProgressive(bool progressive){}

	virtual bool							supportsGettingVideoMode() const { return false; }
	virtual bool							supportsSettingVideoMode() const { return false; }
	virtual bool							supportsListingVideoModes() const { return false; }
	virtual std::set<Utils::VideoMode> 		getSupportedVideoModes() const{ return std::set<Utils::VideoMode>(); }
	#define SUPPORTS_GETTING_VIDEOMODE 		virtual bool supportsGettingVideoMode() const override { return true; }
	#define SUPPORTS_SETTING_VIDEOMODE 		virtual bool supportsSettingVideoMode() const override { return true; }
	#define SUPPORTS_LISTING_VIDEOMODES 	virtual bool supportsListingVideoModes() const override { return true; }
	const Utils::VideoMode& 				getVideoMode() const;
	virtual void							setVideoMode(const Utils::VideoMode& videoMode){}
protected:
	mutable std::mutex						m_videoModeMutex;
	Utils::VideoMode						m_videoMode;
};

inline VideoBase::VideoBase(const Utils::VideoMode& vidMode) :
	m_videoMode(vidMode)
{
}

inline const Utils::PixelFormat& VideoBase::getPixelFormat() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode.pixFmt;
}

inline const Utils::Resolution& VideoBase::getResolution() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode.res;
}

inline const Utils::Codec& VideoBase::getCodec() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode.codec;
}

inline const Utils::Rational& VideoBase::getFramerate() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode.frameRate;
}

inline bool VideoBase::isProgressive() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode.progressive;
}

inline const Utils::VideoMode& VideoBase::getVideoMode() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_videoMode;
}

}
