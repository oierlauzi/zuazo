#pragma once

#include "ColorPrimaries.h"
#include "ColorModel.h"
#include "ColorTransferFunction.h"
#include "ColorRange.h"
#include "ColorFormat.h"
#include "Resolution.h"
#include "Math/Rational.h"
#include "Timing/Chrono.h"

namespace Zuazo {

struct VideoMode {
	Resolution				resolution = Resolution(0, 0);
	AspectRatio 			pixelAspectRatio = AspectRatio(1, 1);
	Timing::Rate 			frameRate = Timing::Rate(0, 1);
	ColorPrimaries			colorPrimaries = ColorPrimaries::NONE;
	ColorModel				colorModel = ColorModel::NONE;
	ColorTransferFunction	colorTransferFunction = ColorTransferFunction::NONE;
	ColorRange				colorRange = ColorRange::NONE;
	ColorFormat				colorFormat = ColorFormat::NONE;
};

}