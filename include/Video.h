#pragma once

#include "ColorPrimaries.h"
#include "ColorModel.h"
#include "ColorTransferFunction.h"
#include "ColorSubsampling.h"
#include "ColorRange.h"
#include "ColorFormat.h"
#include "Resolution.h"
#include "Math/Rational.h"
#include "Timing/Chrono.h"
#include "Graphics/Frame.h"
#include "Utils/Configuration.h"

#include <memory>
#include <variant>
#include <vector>

namespace Zuazo {

enum class VideoModeParameters {
	resolution,
	pixelAspectRatio,
	frameRate,
	colorPrimaries,
	colorModel,
	colorTransferFunction,
	colorSubsampling,
	colorRange,
	colorFormat
};

std::string_view toString(VideoModeParameters par);

using Video = std::shared_ptr<const Graphics::Frame>;
using VideoMode = Utils::Configuration<	VideoModeParameters,
										Resolution,
										AspectRatio,
										Timing::Rate,
										ColorPrimaries,
										ColorModel,
										ColorTransferFunction,
										ColorSubsampling,
										ColorRange,
										ColorFormat	>;


class VideoBase {
public:
	VideoBase(const VideoMode& videoMode);
	VideoBase(const VideoBase& other);
	virtual ~VideoBase();

	VideoBase&							operator=(const VideoBase& other);

	virtual VideoMode::Compatibilities 	getVideoModeCompatibility() const = 0;

	virtual void						setVideoMode(const VideoMode& videoMode) = 0;
	const VideoMode&					getVideoMode() const;

private:
	VideoMode							m_videoMode;

};

}


