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

	virtual void						setVideoMode(const VideoMode& videoMode) = 0;
	const VideoMode&					getVideoMode() const;

	const VideoMode::Compatibilities&	getVideoModeCompatibility() const;

protected:
	void								setVideoModeCompatibility(const VideoMode::Compatibilities& comp);
	void								validate(const VideoMode& videoMode);

private:
	VideoMode							m_videoMode;

	VideoMode::Compatibilities			m_compatibilities;

};

}


