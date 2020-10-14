#pragma once

//Ensure they are included first of all in order to make use of
//the overloaded functions in templates defined in Utils/Limit.h
#include "Chrono.h"
#include "Resolution.h"
#include "ColorSubsampling.h"
#include "ColorFormat.h"
#include "ColorRange.h"
#include "ColorTransferFunction.h"
#include "ColorModel.h"
#include "ColorPrimaries.h"
#include "ScalingMode.h"
#include "ScalingFilter.h"

#include "Graphics/Frame.h"
#include "Utils/Pimpl.h"
#include "Utils/Limit.h"

#include "Signal/NamingConventions.h"

#include <tuple>
#include <vector>

namespace Zuazo {

using Video = std::shared_ptr<const Graphics::Frame>;

class VideoMode {
	friend std::ostream& operator<<(std::ostream& os, const VideoMode& videoMode);
public:
	VideoMode(	Utils::Limit<Rate> frameRate = {},
				Utils::Limit<Resolution> resolution = {},
				Utils::Limit<AspectRatio> pixelAspectRatio = {},
				Utils::Limit<ColorPrimaries> colorPrimaries = {},
				Utils::Limit<ColorModel> colorModel = {},
				Utils::Limit<ColorTransferFunction> colorTransferFunction = {},
				Utils::Limit<ColorSubsampling> colorSubsampling = {},
				Utils::Limit<ColorRange> colorRange = {},
				Utils::Limit<ColorFormat> colorFormat = {} );
	VideoMode(const VideoMode& other) = default;
	VideoMode(VideoMode&& other) = default;
	~VideoMode() = default;

	VideoMode& 									operator=(const VideoMode& other) = default;
	VideoMode& 									operator=(VideoMode&& other) = default;
	
	int 										operator==(const VideoMode& other) const;
	int 										operator!=(const VideoMode& other) const;
	int 										operator<(const VideoMode& other) const;
	int 										operator<=(const VideoMode& other) const;
	int 										operator>(const VideoMode& other) const;
	int 										operator>=(const VideoMode& other) const;

	operator bool() const;

	void 										setFrameRate(Utils::Limit<Rate> frameRate);
	const Utils::Limit<Rate>&					getFrameRate() const;
	Rate										getFrameRateValue() const;

	void 										setResolution(Utils::Limit<Resolution> frameRate);
	const Utils::Limit<Resolution>&				getResolution() const;
	Resolution									getResolutionValue() const;

	void 										setPixelAspectRatio(Utils::Limit<AspectRatio> frameRate);
	const Utils::Limit<AspectRatio>&			getPixelAspectRatio() const;
	AspectRatio									getPixelAspectRatioValue() const;

	void 										setColorPrimaries(Utils::Limit<ColorPrimaries> frameRate);
	const Utils::Limit<ColorPrimaries>&			getColorPrimaries() const;
	ColorPrimaries								getColorPrimariesValue() const;

	void 										setColorModel(Utils::Limit<ColorModel> frameRate);
	const Utils::Limit<ColorModel>&				getColorModel() const;
	ColorModel									getColorModelValue() const;

	void 										setColorTransferFunction(Utils::Limit<ColorTransferFunction> frameRate);
	const Utils::Limit<ColorTransferFunction>&	getColorTransferFunction() const;
	ColorTransferFunction						getColorTransferFunctionValue() const;

	void 										setColorSubsampling(Utils::Limit<ColorSubsampling> frameRate);
	const Utils::Limit<ColorSubsampling>&		getColorSubsampling() const;
	ColorSubsampling							getColorSubsamplingValue() const;

	void 										setColorRange(Utils::Limit<ColorRange> frameRate);
	const Utils::Limit<ColorRange>&				getColorRange() const;
	ColorRange									getColorRangeValue() const;

	void 										setColorFormat(Utils::Limit<ColorFormat> frameRate);
	const Utils::Limit<ColorFormat>&			getColorFormat() const;
	ColorFormat									getColorFormatValue() const;

	VideoMode									intersect(const VideoMode& other) const;

	VideoMode									lowest() const;
	VideoMode									highest() const;
	VideoMode									values() const;

	Graphics::Frame::Descriptor					getFrameDescriptor() const;

	static const VideoMode 						ANY;

private:
	using Data = std::tuple<Utils::Limit<Rate>,
							Utils::Limit<Resolution>,
							Utils::Limit<AspectRatio>,
							Utils::Limit<ColorPrimaries>,
							Utils::Limit<ColorModel>,
							Utils::Limit<ColorTransferFunction>,
							Utils::Limit<ColorSubsampling>,
							Utils::Limit<ColorRange>,
							Utils::Limit<ColorFormat> >;

	enum Indices {
		m_frameRate,
		m_resolution,
		m_pixelAspectRatio,
		m_colorPrimaries,
		m_colorModel,
		m_colorTransferFunction,
		m_colorSubsampling,
		m_colorRange,
		m_colorFormat
	};

	Data 										m_data;

};

class VideoBase {
public:
	using VideoModeCallback = std::function<void(VideoBase&, const VideoMode&)>;
	using VideoModeCompatibilityCallback = std::function<void(VideoBase&, const std::vector<VideoMode>&)>;

	explicit VideoBase(VideoMode videoModeLimits = VideoMode::ANY, VideoModeCallback cbk = {}) noexcept;
	VideoBase(const VideoBase& other) = delete;
	VideoBase(VideoBase&& other) noexcept;
	virtual ~VideoBase();

	VideoBase&									operator=(const VideoBase& other) = delete;
	VideoBase&									operator=(VideoBase&& other) noexcept;


	void										setVideoModeCompatibilityCallback(VideoModeCompatibilityCallback cbk) noexcept;
	const VideoModeCompatibilityCallback&		getVideoModeCompatibilityCallback() const noexcept;

	void										setVideoModeCallback(VideoModeCallback cbk) noexcept;
	const VideoModeCallback&					getVideoModeCallback() const noexcept;

	void										setVideoModeLimits(VideoMode videoMode) noexcept;
	const VideoMode&							getVideoModeLimits() const noexcept;
	const std::vector<VideoMode>& 				getVideoModeCompatibility() const noexcept;
	const VideoMode&							getVideoMode() const noexcept;

protected:
	void										setVideoModeLimitCallback(VideoModeCallback cbk) noexcept;
	const VideoModeCallback&					getVideoModeLimitCallback() const noexcept;

	void										setInternalVideoModeCallback(VideoModeCallback cbk) noexcept;
	const VideoModeCallback&					getInternalVideoModeCallback() const noexcept;

	void										setVideoModeCompatibility(std::vector<VideoMode> comp) noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>							m_impl;

};

class VideoScalerBase {
public:
	using ScalingModeCallback = std::function<void(VideoScalerBase&, ScalingMode)>;
	using ScalingFilterCallback = std::function<void(VideoScalerBase&, ScalingFilter)>;

	VideoScalerBase(	ScalingModeCallback modeCbk = {}, 
						ScalingFilterCallback filterCbk = {} ) noexcept;
	VideoScalerBase(const VideoScalerBase& other) = delete;
	VideoScalerBase(VideoScalerBase&& other) noexcept;
	virtual ~VideoScalerBase();

	VideoScalerBase& 							operator=(const VideoScalerBase& other) = delete;
	VideoScalerBase& 							operator=(VideoScalerBase&& other) noexcept;

	void 										setScalingMode(ScalingMode mode) noexcept;
	ScalingMode 								getScalingMode() const noexcept;

	void 										setScalingFilter(ScalingFilter filter) noexcept;
	ScalingFilter 								getScalingFilter() const noexcept;

protected:
	void										setScalingModeCallback(ScalingModeCallback cbk) noexcept;
	const ScalingModeCallback&					getScalingModeCallback() const noexcept;

	void										setScalingFilterCallback(ScalingFilterCallback cbk) noexcept;
	const ScalingFilterCallback&				getScalingFilterCallback() const noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>							m_impl;

};

std::ostream& operator<<(std::ostream& os, const VideoMode& videoMode);

namespace Signal {

template<>
constexpr std::string_view makeInputName<Video>() noexcept { return "videoIn"; }

template<>
constexpr std::string_view makeOutputName<Video>() noexcept { return "videoOut"; }

}

}


