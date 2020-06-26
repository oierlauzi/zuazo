#pragma once

#include "ColorPrimaries.h"
#include "ColorModel.h"
#include "ColorTransferFunction.h"
#include "ColorSubsampling.h"
#include "ColorRange.h"
#include "ColorFormat.h"
#include "Resolution.h"
#include "Math/Rational.h"
#include "Chrono.h"
#include "Graphics/Frame.h"
#include "Utils/Configuration.h"

#include <memory>
#include <variant>
#include <vector>

namespace Zuazo {

using Video = std::shared_ptr<const Graphics::Frame>;

struct VideoMode : Graphics::Frame::Descriptor {
	using Descriptor::Descriptor;

	static inline const std::string FRAME_RATE = "frameRate";	static constexpr auto DEFAULT_FRAME_RATE = Rate(30, 1);

	VideoMode(	Resolution res = DEFAULT_RESOLUTION,
				Rate frameRate = DEFAULT_FRAME_RATE );

	void					setFrameRate(Rate rate);
	Rate					getFrameRate() const;
};

class VideoBase {
public:
	VideoBase(const VideoMode& videoMode);
	VideoBase(const VideoBase& other);
	virtual ~VideoBase();

	VideoBase&							operator=(const VideoBase& other);

	virtual void						setVideoMode(const VideoMode& videoMode) = 0;
	const VideoMode&					getVideoMode() const;

	const std::vector<Utils::Compatibility>& getVideoModeCompatibility() const;

protected:
	void								setVideoModeCompatibility(const std::vector<Utils::Compatibility>& comp);
	void								validate(const VideoMode& videoMode);

private:
	VideoMode							m_videoMode;

	std::vector<Utils::Compatibility>	m_compatibilities;

};

}


