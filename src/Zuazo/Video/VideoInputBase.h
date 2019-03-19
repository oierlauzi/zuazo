#pragma once

#include "VideoBase.h"
#include "VideoStream.h"

namespace Zuazo::Video{


class VideoInputBase :
	public virtual VideoBase
{
public:
	using VideoBase::VideoBase;

	virtual VideoConsumer&	getVideoSource()=0;

	virtual bool			supportsSettingScalingMode() const{ return false; }
	#define SUPPORTS_SETTING_SCALINGMODE  virtual bool supportsSettingScalingMode() const override{ return true; }

	virtual void			setScalingMode(Utils::ScalingModes scaling){};
	Utils::ScalingModes		getScalingMode() const;

protected:
	Utils::ScalingModes		m_scalingMode=Utils::ScalingModes::Stretched;
};



inline Utils::ScalingModes VideoInputBase::getScalingMode() const{
	return m_scalingMode;
}






template<typename T>
class TVideoInputBase :
		public VideoInputBase
{
public:
	T						videoIn;

	using VideoInputBase::VideoInputBase;

	virtual VideoConsumer&	getVideoSource() override;
};

template<typename T>
inline VideoConsumer& TVideoInputBase<T>::getVideoSource(){
	return videoIn;
}

}
