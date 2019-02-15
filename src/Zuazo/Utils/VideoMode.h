#pragma once

#include "Rational.h"
#include "Resolution.h"
#include "PixelTypes.h"

namespace Zuazo::Utils{

struct VideoMode{
	Resolution		resolution;
	PixelTypes		pixelType;
	Rational		framerate;

	constexpr VideoMode() :
		pixelType(PixelTypes::RGBA)
	{
	}

	constexpr VideoMode(const Resolution& res, PixelTypes pixType, const Rational& rate) :
		resolution(res),
		pixelType(pixType),
		framerate(rate)
	{
	}

	VideoMode(const VideoMode& other)=default;
	~VideoMode()=default;
};

}
