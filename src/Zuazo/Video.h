#pragma once

#include "Utils/ScalingModes.h"
#include "Utils/Resolution.h"

namespace Zuazo{

class VideoBase{
public:
	VideoBase()=default;
	VideoBase(const Utils::Resolution& res) :
		m_resolution(res)
	{
	}
	VideoBase(const VideoBase& other)=default;
	virtual ~VideoBase()=default;

	Utils::Resolution getRes() const{
		return m_resolution;
	}
protected:
	Utils::Resolution	m_resolution;
};

class ResizeableVideoBase : public virtual VideoBase{
public:
	virtual void setRes(const Utils::Resolution& res)=0;
};

class VideoScalerBase : public virtual VideoBase{
public:
	virtual void setScalingMode(Utils::ScalingModes mode)=0;
	virtual Utils::ScalingModes getScalingMode() const=0;
};

}
