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
	RESOLUTION,
	ASPECT_RATIO,
	FRAME_RATE,
	COLOR_PRIMARIES,
	COLOR_MODEL,
	COLOR_TRANSFER_FUNCTION,
	COLOR_SUBSAMPLING,
	COLOR_RANGE,
	COLOR_FORMAT
};

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