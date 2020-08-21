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

#include "Graphics/Frame.h"
#include "Utils/Limit.h"

#include "Signal/NamingConventions.h"

#include <memory>
#include <tuple>
#include <vector>

namespace Zuazo {

using Video = std::shared_ptr<const Graphics::Frame>;

class VideoMode {
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
	using VideoModeCallback = std::function<void(const VideoMode&)>;

	VideoBase();
	VideoBase(VideoMode videoMode, VideoModeCallback cbk = {});
	VideoBase(const VideoBase& other);
	VideoBase(VideoBase&& other);
	virtual ~VideoBase();

	VideoBase&									operator=(const VideoBase& other);
	VideoBase&									operator=(VideoBase&& other);


	void										setVideoModeCallback(VideoModeCallback cbk);
	const VideoModeCallback&					getVideoModeCallback() const;

	void										setVideoModeLimits(VideoMode videoMode);
	const VideoMode&							getVideoModeLimits() const;
	const std::vector<VideoMode>& 				getVideoModeCompatibility() const;
	const VideoMode&							getVideoMode() const;

protected:
	void										setInternalVideoModeCallback(VideoModeCallback cbk);
	const VideoModeCallback&					getInternalVideoModeCallback() const;

	void										setVideoModeCompatibility(std::vector<VideoMode> comp);

private:
	enum VideoModeCallbacks {
		VMCBK_INTERNAL,
		VMCBK_EXTERNAL,
		VMCBK_COUNT
	};

	std::array<VideoModeCallback, VMCBK_COUNT>	m_videoModeCallbacks;
	
	VideoMode									m_videoModeLimits;
	std::vector<VideoMode>						m_videoModeCompatibility;
	VideoMode									m_videoMode;

	void										updateVideoMode();
	VideoMode									selectVideoMode() const;

};

namespace Signal {

template<>
constexpr std::string_view makeInputName<Video>() { return "videoIn"; }

template<>
constexpr std::string_view makeOutputName<Video>() { return "videoOut"; }

}

}


