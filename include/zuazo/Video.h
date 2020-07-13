#pragma once

#include "Math/Rational.h"
#include "Chrono.h"
#include "Graphics/Frame.h"
#include "Utils/Configuration.h"

#include <memory>
#include <variant>
#include <vector>

namespace Zuazo {

using Video = std::shared_ptr<const Graphics::Frame>;

struct VideoMode : public Graphics::Frame::Descriptor {
	static inline const std::string FRAME_RATE = "frameRate";	static constexpr auto DEFAULT_FRAME_RATE = Rate(30, 1);

	using Graphics::Frame::Descriptor::Descriptor;
	using Graphics::Frame::Descriptor::operator=;
	VideoMode();
	
	void 									setFrameRate(Rate res);
	Rate 									getFrameRate() const;
	void 									setFrameRateLimits(Limit<Rate> res);
	Limit<Rate>								getFrameRateLimits() const;
};

class VideoBase {
public:
	VideoBase();
	VideoBase(VideoMode videoMode);
	VideoBase(const VideoBase& other);
	VideoBase(VideoBase&& other);
	virtual ~VideoBase();

	VideoBase&							operator=(const VideoBase& other);

	virtual void						setVideoMode(const VideoMode& videoMode) = 0;
	const VideoMode&					getVideoMode() const;

	const std::vector<VideoMode>& 		getVideoModeCompatibility() const;

protected:
	void								setVideoModeCompatibility(std::vector<VideoMode> comp);
	void								validate(const VideoMode& videoMode);

private:
	VideoMode							m_videoMode;

	std::vector<VideoMode>				m_compatibilities;

};

}


