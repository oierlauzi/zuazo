#pragma once

#include "VideoBase.h"
#include "VideoStream.h"

namespace Zuazo::Video{


class VideoInputBase :
	public virtual VideoBase
{
public:
	VideoConsumer&			videoIn;

	VideoInputBase(VideoConsumer& consumer);
	VideoInputBase(VideoConsumer& consumer, const Utils::VideoMode& videoMode);
	VideoInputBase(const VideoInputBase& other)=default;
	virtual ~VideoInputBase()=default;

	virtual bool			supportsSettingScalingMode() const{ return false; }
	#define SUPPORTS_SETTING_SCALINGMODE  virtual bool supportsSettingScalingMode() const override{ return true; }

	virtual void			setScalingMode(Utils::ScalingModes scaling){};
	Utils::ScalingModes		getScalingMode() const;
protected:
	Utils::ScalingModes		m_scalingMode=Utils::ScalingModes::Stretched;
};


inline VideoInputBase::VideoInputBase(VideoConsumer& consumer) :
		videoIn(consumer)
{
}

inline VideoInputBase::VideoInputBase(VideoConsumer& consumer, const Utils::VideoMode& videoMode) :
		VideoBase(videoMode),
		videoIn(consumer)
{
}

inline Utils::ScalingModes VideoInputBase::getScalingMode() const{
	return m_scalingMode;
}

template <typename T>
class TVideoInputBase : public VideoInputBase{
public:
	TVideoInputBase();
	TVideoInputBase(const Utils::VideoMode& videoMode);
	TVideoInputBase(const TVideoInputBase& other)=default;
	virtual ~TVideoInputBase()=default;
protected:
	T m_videoConsumerPad;
};

template <typename T>
inline TVideoInputBase<T>::TVideoInputBase() :
	VideoInputBase(m_videoConsumerPad)
{
}

template <typename T>
inline TVideoInputBase<T>::TVideoInputBase(const Utils::VideoMode& videoMode) :
	VideoInputBase(m_videoConsumerPad, videoMode)
{
}

}
