#pragma once

#include "../Timing/UpdateableBase.h"
#include "VideoBase.h"
#include "VideoStream.h"

namespace Zuazo::Video{


class VideoOutputBase :
	public virtual VideoBase
{
public:
	VideoSource& 		videoOut;

	VideoOutputBase(VideoSource& source);
	VideoOutputBase(VideoSource& source, const Utils::VideoMode& videoMode);
	VideoOutputBase(const VideoOutputBase& other)=default;
	virtual ~VideoOutputBase()=default;
};



inline VideoOutputBase::VideoOutputBase(VideoSource& source) :
		videoOut(source)
{
}

inline VideoOutputBase::VideoOutputBase(VideoSource& source, const Utils::VideoMode& videoMode) :
		VideoBase(videoMode),
		videoOut(source)
{
}

template <typename T>
class TVideoOutputBase : public VideoOutputBase{
public:
	TVideoOutputBase();
	TVideoOutputBase(const Utils::VideoMode& videoMode);
	TVideoOutputBase(const TVideoOutputBase& other)=default;
	virtual ~TVideoOutputBase()=default;
protected:
	T m_videoSourcePad;
};

template <typename T>
inline TVideoOutputBase<T>::TVideoOutputBase() :
	VideoOutputBase(m_videoSourcePad)
{
}

template <typename T>
inline TVideoOutputBase<T>::TVideoOutputBase(const Utils::VideoMode& videoMode) :
	VideoOutputBase(m_videoSourcePad, videoMode)
{
}

template<typename Q>
class TVideoOutputBase<LazyVideoSourcePad<Q>> :
	public VideoOutputBase,
	private virtual Timing::UpdateableBase
{
public:
	TVideoOutputBase();
	TVideoOutputBase(const Utils::VideoMode& videoMode);
	TVideoOutputBase(const TVideoOutputBase& other)=default;
	virtual ~TVideoOutputBase()=default;
protected:
	LazyVideoSourcePad<Q> m_videoSourcePad;
};

template <typename Q>
inline TVideoOutputBase<LazyVideoSourcePad<Q>>::TVideoOutputBase() :
	VideoOutputBase(m_videoSourcePad),
	m_videoSourcePad(*this)
{
}

template <typename Q>
inline TVideoOutputBase<LazyVideoSourcePad<Q>>::TVideoOutputBase(const Utils::VideoMode& videoMode) :
	VideoOutputBase(m_videoSourcePad, videoMode),
	m_videoSourcePad(*this)
{
}

}
