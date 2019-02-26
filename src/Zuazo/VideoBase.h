#pragma once

#include "Utils/ScalingModes.h"
#include "Utils/Resolution.h"

namespace Zuazo{

class VideoBase{
public:
	virtual ~VideoBase()=default;

	virtual Utils::Resolution getRes() const=0;
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
