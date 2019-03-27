#pragma once

#include "VideoBase.h"
#include "VideoStream.h"
#include "../Timing/UpdateableBase.h"

#include <memory>

namespace Zuazo::Video{


class VideoSourceBase :
	public virtual VideoBase
{
public:
	VideoSource& 		videoOut;

	VideoSourceBase(VideoSource& source);
	VideoSourceBase(VideoSource& source, const Utils::VideoMode& videoMode);
	VideoSourceBase(const VideoSourceBase& other)=default;
	virtual ~VideoSourceBase()=default;
};

inline VideoSourceBase::VideoSourceBase(VideoSource& source) :
		videoOut(source)
{
}

inline VideoSourceBase::VideoSourceBase(VideoSource& source, const Utils::VideoMode& videoMode) :
		VideoBase(videoMode),
		videoOut(source)
{
}




template <typename T>
class TVideoSourceBase : public VideoSourceBase{
public:
	TVideoSourceBase();
	TVideoSourceBase(const Utils::VideoMode& videoMode);
	TVideoSourceBase(const TVideoSourceBase& other)=default;
	virtual ~TVideoSourceBase()=default;
protected:
	T m_videoSourcePad;
};

template <typename T>
inline TVideoSourceBase<T>::TVideoSourceBase() :
	VideoSourceBase(m_videoSourcePad)
{
}

template <typename T>
inline TVideoSourceBase<T>::TVideoSourceBase(const Utils::VideoMode& videoMode) :
	VideoSourceBase(m_videoSourcePad, videoMode)
{
}





template<typename Q>
class TVideoSourceBase<LazyVideoSourcePad<Q>> :
	public VideoSourceBase,
	private virtual Timing::UpdateableBase
{
public:
	TVideoSourceBase();
	TVideoSourceBase(const Utils::VideoMode& videoMode);
	TVideoSourceBase(const TVideoSourceBase& other)=default;
	virtual ~TVideoSourceBase()=default;
protected:
	LazyVideoSourcePad<Q> m_videoSourcePad;
};

template <typename Q>
inline TVideoSourceBase<LazyVideoSourcePad<Q>>::TVideoSourceBase() :
	VideoSourceBase(m_videoSourcePad),
	m_videoSourcePad(*this)
{
}

template <typename Q>
inline TVideoSourceBase<LazyVideoSourcePad<Q>>::TVideoSourceBase(const Utils::VideoMode& videoMode) :
	VideoSourceBase(m_videoSourcePad, videoMode),
	m_videoSourcePad(*this)
{
}

}
