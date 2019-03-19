#pragma once

#include "VideoBase.h"
#include "VideoStream.h"

namespace Zuazo::Video{


class VideoOutputBase :
	public virtual VideoBase
{
public:
	using VideoBase::VideoBase;

	virtual VideoSource&	getVideoSource()=0;
};

template<typename T>
class TVideoOutputBase :
		public VideoOutputBase
{
public:
	T						videoOut;

	using VideoOutputBase::VideoOutputBase;

	virtual VideoSource&	getVideoSource() override;
};

template<typename T>
inline VideoSource& TVideoOutputBase<T>::getVideoSource(){
	return videoOut;
}

}
