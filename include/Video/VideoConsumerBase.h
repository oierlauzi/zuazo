#pragma once

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

	virtual bool			supportsSettingScalingMode() const{ return false; }
	#define SUPPORTS_SETTING_SCALINGMODE  virtual bool supportsSettingScalingMode() const override{ return true; }

	virtual void			setScalingMode(Utils::ScalingModes scaling){};
	Utils::ScalingModes		getScalingMode() const;
protected:
	Utils::ScalingModes		m_scalingMode=Utils::ScalingModes::Stretched;
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

inline Utils::ScalingModes VideoConsumerBase::getScalingMode() const{
	return m_scalingMode;
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
