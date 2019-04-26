#pragma once

#include "VideoBase.h"
#include "VideoStream.h"
#include "../Timing.h"

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





template<>
class TVideoSourceBase<LazyVideoSourcePad> :
	public VideoSourceBase,
	protected UpdateableBase
{
public:
	TVideoSourceBase();
	TVideoSourceBase(const Utils::VideoMode& videoMode);
	TVideoSourceBase(const TVideoSourceBase& other)=default;
	virtual ~TVideoSourceBase()=default;

protected:
	LazyVideoSourcePad m_videoSourcePad;

	virtual void enable() override;
	virtual void disable() override;
};

inline TVideoSourceBase<LazyVideoSourcePad>::TVideoSourceBase() :
	VideoSourceBase(m_videoSourcePad),
	m_videoSourcePad(*this)
{
}

inline TVideoSourceBase<LazyVideoSourcePad>::TVideoSourceBase(const Utils::VideoMode& videoMode) :
	VideoSourceBase(m_videoSourcePad, videoMode),
	m_videoSourcePad(*this)
{
}

inline void TVideoSourceBase<LazyVideoSourcePad>::enable(){
	m_videoSourcePad.enable();
}

inline void TVideoSourceBase<LazyVideoSourcePad>::disable(){
	m_videoSourcePad.disable();
}

}
