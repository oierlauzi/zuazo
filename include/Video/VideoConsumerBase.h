#pragma once

#include "../Utils/ScalingMode.h"
#include "../Utils/ScalingFilter.h"

#include "VideoBase.h"
#include "VideoStream.h"

namespace Zuazo::Video{


class VideoConsumerBase :
	public virtual VideoBase
{
public:
	VideoConsumer&			videoIn;

	VideoConsumerBase(VideoConsumer& consumer);
	VideoConsumerBase(VideoConsumer& consumer, const Utils::VideoMode& videoMode);
	VideoConsumerBase(const VideoConsumerBase& other)=default;
	virtual ~VideoConsumerBase()=default;

	virtual bool			supportsGettingScalingMode() const{ return false; }
	virtual bool			supportsSettingScalingMode() const{ return false; }
	#define SUPPORTS_GETTING_SCALINGMODE virtual bool supportsGettingScalingMode() const override{ return true; }
	#define SUPPORTS_SETTING_SCALINGMODE virtual bool supportsSettingScalingMode() const override{ return true; }
	Utils::ScalingMode		getScalingMode() const;
	virtual void			setScalingMode(Utils::ScalingMode scaling){};

	virtual bool			supportsGettingScalingFilter() const{ return false; }
	virtual bool			supportsSettingScalingFilter() const{ return false; }
	#define SUPPORTS_GETTING_SCALINGFILTER virtual bool supportsGettingScalingFilter() const override{ return true; }
	#define SUPPORTS_SETTING_SCALINGFILTER virtual bool supportsSettingScalingFilter() const override{ return true; }
	Utils::ScalingFilter	getScalingFilter() const;
	virtual void			setScalingFilter(Utils::ScalingFilter scaling){};
protected:
	Utils::ScalingMode		m_scalingMode=Utils::ScalingMode::Stretched;
	Utils::ScalingFilter	m_scalingFilter=Utils::ScalingFilter::Nearest;

};


inline VideoConsumerBase::VideoConsumerBase(VideoConsumer& consumer) :
		videoIn(consumer)
{
}

inline VideoConsumerBase::VideoConsumerBase(VideoConsumer& consumer, const Utils::VideoMode& videoMode) :
		VideoBase(videoMode),
		videoIn(consumer)
{
}

inline Utils::ScalingMode VideoConsumerBase::getScalingMode() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_scalingMode;
}

inline Utils::ScalingFilter VideoConsumerBase::getScalingFilter() const{
	std::lock_guard<std::mutex> lock(m_videoModeMutex);
	return m_scalingFilter;
}



template <typename T>
class TVideoConsumerBase : public VideoConsumerBase{
public:
	TVideoConsumerBase();
	TVideoConsumerBase(const Utils::VideoMode& videoMode);
	TVideoConsumerBase(const TVideoConsumerBase& other)=default;
	virtual ~TVideoConsumerBase()=default;
protected:
	T m_videoConsumerPad;
};

template <typename T>
inline TVideoConsumerBase<T>::TVideoConsumerBase() :
	VideoConsumerBase(m_videoConsumerPad)
{
}

template <typename T>
inline TVideoConsumerBase<T>::TVideoConsumerBase(const Utils::VideoMode& videoMode) :
	VideoConsumerBase(m_videoConsumerPad, videoMode)
{
}

}
