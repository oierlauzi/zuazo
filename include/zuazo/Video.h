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
	
	bool 										operator==(const VideoMode& other) const;
	bool 										operator!=(const VideoMode& other) const;

	operator bool() const;

	void 										setFrameRate(Utils::Limit<Rate> frameRate);
	const Utils::Limit<Rate>&					getFrameRate() const;
	Rate										getFrameRateValue() const;

	void 										setResolution(Utils::Limit<Resolution> resolution);
	const Utils::Limit<Resolution>&				getResolution() const;
	Resolution									getResolutionValue() const;

	void 										setPixelAspectRatio(Utils::Limit<AspectRatio> par);
	const Utils::Limit<AspectRatio>&			getPixelAspectRatio() const;
	AspectRatio									getPixelAspectRatioValue() const;

	void 										setColorPrimaries(Utils::Limit<ColorPrimaries> primaries);
	const Utils::Limit<ColorPrimaries>&			getColorPrimaries() const;
	ColorPrimaries								getColorPrimariesValue() const;

	void 										setColorModel(Utils::Limit<ColorModel> model);
	const Utils::Limit<ColorModel>&				getColorModel() const;
	ColorModel									getColorModelValue() const;

	void 										setColorTransferFunction(Utils::Limit<ColorTransferFunction> transferFunc);
	const Utils::Limit<ColorTransferFunction>&	getColorTransferFunction() const;
	ColorTransferFunction						getColorTransferFunctionValue() const;

	void 										setColorSubsampling(Utils::Limit<ColorSubsampling> subsampling);
	const Utils::Limit<ColorSubsampling>&		getColorSubsampling() const;
	ColorSubsampling							getColorSubsamplingValue() const;

	void 										setColorRange(Utils::Limit<ColorRange> range);
	const Utils::Limit<ColorRange>&				getColorRange() const;
	ColorRange									getColorRangeValue() const;

	void 										setColorFormat(Utils::Limit<ColorFormat> format);
	const Utils::Limit<ColorFormat>&			getColorFormat() const;
	ColorFormat									getColorFormatValue() const;

	VideoMode									intersect(const VideoMode& other) const;

	VideoMode									lowest() const;
	VideoMode									highest() const;
	VideoMode									value() const;

	Graphics::Frame::Descriptor					getFrameDescriptor() const;

	static const VideoMode 						ANY;

private:
	Utils::Limit<Rate>							m_frameRate;
	Utils::Limit<Resolution>					m_resolution;
	Utils::Limit<AspectRatio>					m_pixelAspectRatio;
	Utils::Limit<ColorPrimaries>				m_colorPrimaries;
	Utils::Limit<ColorModel>					m_colorModel;
	Utils::Limit<ColorTransferFunction>			m_colorTransferFunction;
	Utils::Limit<ColorSubsampling>				m_colorSubsampling;
	Utils::Limit<ColorRange>					m_colorRange;
	Utils::Limit<ColorFormat>					m_colorFormat;

};



class VideoBase {
public:
	using VideoModeCallback = std::function<void(VideoBase&, const VideoMode&)>;
	using VideoModeNegotiationCallback = std::function<VideoMode(VideoBase&, const std::vector<VideoMode>&)>;

	explicit VideoBase(VideoModeCallback videoModeCbk = {}) noexcept;
	VideoBase(const VideoBase& other) = delete;
	VideoBase(VideoBase&& other) noexcept;
	virtual ~VideoBase();

	VideoBase&									operator=(const VideoBase& other) = delete;
	VideoBase&									operator=(VideoBase&& other) noexcept;


	void										setVideoModeNegotiationCallback(VideoModeNegotiationCallback cbk) noexcept;
	const VideoModeNegotiationCallback&			getVideoModeNegotiationCallback() const noexcept;
	const std::vector<VideoMode>& 				getVideoModeCompatibility() const noexcept;

	void										setVideoMode(VideoMode videoMode);
	const VideoMode&							getVideoMode() const noexcept;

protected:
	void										setVideoModeCallback(VideoModeCallback cbk) noexcept;
	const VideoModeCallback&					getVideoModeCallback() const noexcept;

	void										setVideoModeCompatibility(std::vector<VideoMode> comp) noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>							m_impl;

};



struct DefaultVideoModeNegotiator 
	: VideoMode
{
	explicit DefaultVideoModeNegotiator(VideoMode videoMode = VideoMode::ANY);
	using VideoMode::VideoMode;
	using VideoMode::operator=;

	VideoMode operator()(VideoBase& base, const std::vector<VideoMode>& videoModes) const;
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

VideoMode makeVideoMode(Rate rate);
VideoMode makeVideoMode(Resolution res, AspectRatio par);
VideoMode makeVideoMode(ColorPrimaries prim, ColorModel model, ColorTransferFunction trf);
VideoMode makeVideoMode(ColorSubsampling subs, ColorRange range, ColorFormat format);

namespace FrameRates {

extern const VideoMode P24;
extern const VideoMode P48;
extern const VideoMode P96;
extern const VideoMode P192;


extern const VideoMode P25;
extern const VideoMode P50;
extern const VideoMode P100;
extern const VideoMode P200;

extern const VideoMode P29_97;
extern const VideoMode P59_94;
extern const VideoMode P119_88;
extern const VideoMode P239_76;

extern const VideoMode P30;
extern const VideoMode P60;
extern const VideoMode P120;
extern const VideoMode P240;

}

namespace Resolutions {

//Broadcast-standard
extern const VideoMode PAL;
extern const VideoMode NTSC;

extern const VideoMode HD;
extern const VideoMode FHD;
extern const VideoMode QHD;
extern const VideoMode UHD;
extern const VideoMode UHD_2;

//DCI (Digital cinema Initiatives)
extern const VideoMode DCI_2K;
extern const VideoMode DCI_4K;
extern const VideoMode DCI_8K;

//VGA (Video graphics array)
extern const VideoMode QQVGA;
extern const VideoMode HQVGA;
extern const VideoMode QVGA;
extern const VideoMode WQVGA;
extern const VideoMode HVGA;
extern const VideoMode VGA;
extern const VideoMode WVGA;
extern const VideoMode FWVGA;
extern const VideoMode SVGA;
extern const VideoMode WSVGA;
extern const VideoMode DVGA;

//XGA (Extended graphics array)
extern const VideoMode XGA;
extern const VideoMode WXGA;
extern const VideoMode FWXGA;
extern const VideoMode SXGA;
extern const VideoMode WSXGA;
extern const VideoMode UXGA;
extern const VideoMode WUXGA; 

//QXGA (Quad Extended Graphics Array)
extern const VideoMode QWXGA;
extern const VideoMode QXGA;
extern const VideoMode WQXGA;
extern const VideoMode QSXGA;
extern const VideoMode WQSXGA;
extern const VideoMode QUXGA;
extern const VideoMode WQUXGA;

}

namespace ColorSpaces {

//ITU-R
extern const VideoMode BT601_625;
extern const VideoMode BT601_525;
extern const VideoMode BT709;
extern const VideoMode BT2020;

//Others
extern const VideoMode SRGB;
extern const VideoMode DISPLAY_P3;

}

namespace PixelFormats {

//Render optimal
extern const VideoMode RENDER_OPTIMAL_8;
extern const VideoMode RENDER_OPTIMAL_16;
extern const VideoMode RENDER_OPTIMAL_16F;

}

namespace Signal {

template<>
constexpr std::string_view makeInputName<Video>() noexcept { return "videoIn"; }

template<>
constexpr std::string_view makeOutputName<Video>() noexcept { return "videoOut"; }

}

}


