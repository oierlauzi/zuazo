#include <zuazo/Video.h>

#include <zuazo/StringConversions.h>
#include <zuazo/Utils/Functions.h>

namespace Zuazo {

/*
 * VideoMode
 */

const VideoMode VideoMode::ANY = VideoMode(
	Utils::Any<Rate>(),
	Utils::Any<Resolution>(),
	Utils::Any<AspectRatio>(),
	Utils::Any<ColorPrimaries>(),
	Utils::Any<ColorModel>(),
	Utils::Any<ColorTransferFunction>(),
	Utils::Any<ColorSubsampling>(),
	Utils::Any<ColorRange>(),
	Utils::Any<ColorFormat>()
);

VideoMode::VideoMode(	Utils::Limit<Rate> frameRate,
						Utils::Limit<Resolution> resolution,
						Utils::Limit<AspectRatio> pixelAspectRatio,
						Utils::Limit<ColorPrimaries> colorPrimaries,
						Utils::Limit<ColorModel> colorModel,
						Utils::Limit<ColorTransferFunction> colorTransferFunction,
						Utils::Limit<ColorSubsampling> colorSubsampling,
						Utils::Limit<ColorRange> colorRange,
						Utils::Limit<ColorFormat> colorFormat )
	: m_data {
		std::move(frameRate),
		std::move(resolution),
		std::move(pixelAspectRatio),
		std::move(colorPrimaries),
		std::move(colorModel),
		std::move(colorTransferFunction),
		std::move(colorSubsampling),
		std::move(colorRange),
		std::move(colorFormat)
	}
{
}

int VideoMode::operator==(const VideoMode& other) const {
	return m_data == other.m_data;
}

int VideoMode::operator!=(const VideoMode& other) const {
	return m_data != other.m_data;
}

int VideoMode::operator<(const VideoMode& other) const {
	return m_data < other.m_data;
}

int VideoMode::operator<=(const VideoMode& other) const {
	return m_data <= other.m_data;
}

int VideoMode::operator>(const VideoMode& other) const {
	return m_data > other.m_data;
}

int VideoMode::operator>=(const VideoMode& other) const {
	return m_data >= other.m_data;
}


VideoMode::operator bool() const {
	return std::apply(
		[] (const auto&... x) -> bool {
			return (Utils::hasValue(x) && ...);
		},
		m_data
	);
}

void VideoMode::setFrameRate(Utils::Limit<Rate> frameRate) {
	std::get<m_frameRate>(m_data) = std::move(frameRate);
}

const Utils::Limit<Rate>& VideoMode::getFrameRate() const {
	return std::get<m_frameRate>(m_data);
}

Rate VideoMode::getFrameRateValue() const {
	return Utils::value(getFrameRate());
}


void VideoMode::setResolution(Utils::Limit<Resolution> resolution) {
	std::get<m_resolution>(m_data) = std::move(resolution);
}

const Utils::Limit<Resolution>& VideoMode::getResolution() const {
	return std::get<m_resolution>(m_data);
}

Resolution VideoMode::getResolutionValue() const {
	return Utils::value(getResolution());
}


void VideoMode::setPixelAspectRatio(Utils::Limit<AspectRatio> pixelAspectRatio) {
	std::get<m_pixelAspectRatio>(m_data) = std::move(pixelAspectRatio);
}

const Utils::Limit<AspectRatio>& VideoMode::getPixelAspectRatio() const {
	return std::get<m_pixelAspectRatio>(m_data);
}

AspectRatio VideoMode::getPixelAspectRatioValue() const {
	return Utils::value(getPixelAspectRatio());
}


void VideoMode::setColorPrimaries(Utils::Limit<ColorPrimaries> colorPrimaries) {
	std::get<m_colorPrimaries>(m_data) = std::move(colorPrimaries);
}

const Utils::Limit<ColorPrimaries>& VideoMode::getColorPrimaries() const {
	return std::get<m_colorPrimaries>(m_data);
}

ColorPrimaries VideoMode::getColorPrimariesValue() const {
	return Utils::value(getColorPrimaries());
}


void VideoMode::setColorModel(Utils::Limit<ColorModel> colorModel) {
	std::get<m_colorModel>(m_data) = std::move(colorModel);
}

const Utils::Limit<ColorModel>& VideoMode::getColorModel() const {
	return std::get<m_colorModel>(m_data);
}

ColorModel VideoMode::getColorModelValue() const {
	return Utils::value(getColorModel());
}


void VideoMode::setColorTransferFunction(Utils::Limit<ColorTransferFunction> colorTransferFunction) {
	std::get<m_colorTransferFunction>(m_data) = std::move(colorTransferFunction);
}

const Utils::Limit<ColorTransferFunction>& VideoMode::getColorTransferFunction() const {
	return std::get<m_colorTransferFunction>(m_data);
}

ColorTransferFunction VideoMode::getColorTransferFunctionValue() const {
	return Utils::value(getColorTransferFunction());
}


void VideoMode::setColorSubsampling(Utils::Limit<ColorSubsampling> colorSubsampling) {
	std::get<m_colorSubsampling>(m_data) = std::move(colorSubsampling);
}

const Utils::Limit<ColorSubsampling>& VideoMode::getColorSubsampling() const {
	return std::get<m_colorSubsampling>(m_data);
}

ColorSubsampling VideoMode::getColorSubsamplingValue() const {
	return Utils::value(getColorSubsampling());
}


void VideoMode::setColorRange(Utils::Limit<ColorRange> colorRange) {
	std::get<m_colorRange>(m_data) = std::move(colorRange);
}

const Utils::Limit<ColorRange>& VideoMode::getColorRange() const {
	return std::get<m_colorRange>(m_data);
}

ColorRange VideoMode::getColorRangeValue() const {
	return Utils::value(getColorRange());
}


void VideoMode::setColorFormat(Utils::Limit<ColorFormat> colorFormat) {
	std::get<m_colorFormat>(m_data) = std::move(colorFormat);
}

const Utils::Limit<ColorFormat>& VideoMode::getColorFormat() const {
	return std::get<m_colorFormat>(m_data);
}

ColorFormat VideoMode::getColorFormatValue() const {
	return Utils::value(getColorFormat());
}


VideoMode VideoMode::intersect(const VideoMode& other) const {
	return VideoMode(
		Utils::intersection(getFrameRate(), other.getFrameRate()),
		Utils::intersection(getResolution(), other.getResolution()),
		Utils::intersection(getPixelAspectRatio(), other.getPixelAspectRatio()),
		Utils::intersection(getColorPrimaries(), other.getColorPrimaries()),
		Utils::intersection(getColorModel(), other.getColorModel()),
		Utils::intersection(getColorTransferFunction(), other.getColorTransferFunction()),
		Utils::intersection(getColorSubsampling(), other.getColorSubsampling()),
		Utils::intersection(getColorRange(), other.getColorRange()),
		Utils::intersection(getColorFormat(), other.getColorFormat())
	);
}


VideoMode VideoMode::lowest() const {
	return VideoMode(
		Utils::MustBe(Utils::lowest(getFrameRate())),
		Utils::MustBe(Utils::lowest(getResolution())),
		Utils::MustBe(Utils::lowest(getPixelAspectRatio())),
		Utils::MustBe(Utils::lowest(getColorPrimaries())),
		Utils::MustBe(Utils::lowest(getColorModel())),
		Utils::MustBe(Utils::lowest(getColorTransferFunction())),
		Utils::MustBe(Utils::lowest(getColorSubsampling())),
		Utils::MustBe(Utils::lowest(getColorRange())),
		Utils::MustBe(Utils::lowest(getColorFormat()))
	);
}

VideoMode VideoMode::highest() const {
	return VideoMode(
		Utils::MustBe(Utils::highest(getFrameRate())),
		Utils::MustBe(Utils::highest(getResolution())),
		Utils::MustBe(Utils::highest(getPixelAspectRatio())),
		Utils::MustBe(Utils::highest(getColorPrimaries())),
		Utils::MustBe(Utils::highest(getColorModel())),
		Utils::MustBe(Utils::highest(getColorTransferFunction())),
		Utils::MustBe(Utils::highest(getColorSubsampling())),
		Utils::MustBe(Utils::highest(getColorRange())),
		Utils::MustBe(Utils::highest(getColorFormat()))
	);
}

VideoMode VideoMode::values() const {
	return VideoMode(
		Utils::MustBe(Utils::value(getFrameRate())),
		Utils::MustBe(Utils::value(getResolution())),
		Utils::MustBe(Utils::value(getPixelAspectRatio())),
		Utils::MustBe(Utils::value(getColorPrimaries())),
		Utils::MustBe(Utils::value(getColorModel())),
		Utils::MustBe(Utils::value(getColorTransferFunction())),
		Utils::MustBe(Utils::value(getColorSubsampling())),
		Utils::MustBe(Utils::value(getColorRange())),
		Utils::MustBe(Utils::value(getColorFormat()))
	);
}


Graphics::Frame::Descriptor VideoMode::getFrameDescriptor() const {
	return Graphics::Frame::Descriptor {
		getResolutionValue(),
		getPixelAspectRatioValue(),
		getColorPrimariesValue(),
		getColorModelValue(),
		getColorTransferFunctionValue(),
		getColorSubsamplingValue(),
		getColorRangeValue(),
		getColorFormatValue()
	};
}


/*
 * VideoBase::Impl
 */

struct VideoBase::Impl {
	enum VideoModeCallbacks {
		VMCBK_INTERNAL,
		VMCBK_EXTERNAL,
		VMCBK_COUNT
	};

	VideoModeCallback							videoModeLimitCallback;
	VideoModeCompatibilityCallback				videoModeCompatibilityCallback;
	std::array<VideoModeCallback, VMCBK_COUNT>	videoModeCallbacks;
	
	VideoMode									videoModeLimits;
	std::vector<VideoMode>						videoModeCompatibility;
	VideoMode									videoMode;


	Impl(VideoMode videoModeLimits, VideoModeCallback cbk) noexcept
		: videoModeLimitCallback()
		, videoModeCompatibilityCallback()
		, videoModeCallbacks{std::move(cbk), {}}
		, videoModeLimits(std::move(videoModeLimits))
		, videoModeCompatibility()
		, videoMode()
	{
	}

	~Impl() = default;


	void setVideoModeCompatibilityCallback(VideoModeCompatibilityCallback cbk) noexcept {
		videoModeCompatibilityCallback = std::move(cbk);
	}

	const VideoModeCompatibilityCallback& getVideoModeCompatibilityCallback() const noexcept {
		return videoModeCompatibilityCallback;
	}


	void setVideoModeCallback(VideoModeCallback cbk) noexcept {
		videoModeCallbacks[VMCBK_EXTERNAL] = std::move(cbk);
	}

	const VideoModeCallback& getVideoModeCallback() const noexcept {
		return videoModeCallbacks[VMCBK_EXTERNAL];
	}



	void setVideoModeLimits(VideoBase& base, VideoMode videoMode) noexcept {
		videoModeLimits = std::move(videoMode);
		Utils::invokeIf(videoModeLimitCallback, base, videoModeLimits);
		updateVideoMode(base);
	}

	const VideoMode& getVideoModeLimits() const noexcept {
		return videoModeLimits;
	}

	const std::vector<VideoMode>& getVideoModeCompatibility() const noexcept {
		return videoModeCompatibility;
	}

	const VideoMode& getVideoMode() const noexcept {
		return videoMode;
	}



	void setVideoModeLimitCallback(VideoModeCallback cbk) noexcept {
		videoModeLimitCallback = std::move(cbk);
	}

	const VideoModeCallback& getVideoModeLimitCallback() const noexcept {
		return videoModeLimitCallback;
	}


	void setInternalVideoModeCallback(VideoModeCallback cbk) noexcept {
		videoModeCallbacks[VMCBK_INTERNAL] = std::move(cbk);
	}

	const VideoModeCallback& getInternalVideoModeCallback() const noexcept {
		return videoModeCallbacks[VMCBK_INTERNAL];
	}

	void setVideoModeCompatibility(VideoBase& base, std::vector<VideoMode> comp) noexcept {
		videoModeCompatibility = std::move(comp);
		Utils::invokeIf(videoModeCompatibilityCallback, base, videoModeCompatibility);
		updateVideoMode(base);
	}

private:
	void updateVideoMode(VideoBase& base) noexcept {
		VideoMode vm = selectVideoMode();

		if(vm != videoMode) {
			//Videomode has changed
			videoMode = vm;

			//Call the callbacks
			for(const auto& cbk : videoModeCallbacks) {
				Utils::invokeIf(cbk, base, videoMode);
			}
		}
	}

	VideoMode selectVideoMode() const noexcept {
		for(const auto& compatibility : videoModeCompatibility) {
			const auto interscetion = compatibility.intersect(videoModeLimits);
			if(interscetion) return interscetion.values();
		}

		//If there is no compatibility, fail silently.
		return VideoMode();
	}

};



/*
 * VideoBase
 */

VideoBase::VideoBase(VideoMode videoModeLimits, VideoModeCallback cbk) noexcept
	: m_impl({}, std::move(videoModeLimits), std::move(cbk))
{
}

VideoBase::VideoBase(VideoBase&& other) noexcept = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(VideoBase&& other) noexcept = default;



void VideoBase::setVideoModeCompatibilityCallback(VideoModeCompatibilityCallback cbk) noexcept {
	m_impl->setVideoModeCompatibilityCallback(std::move(cbk));
}

const VideoBase::VideoModeCompatibilityCallback& VideoBase::getVideoModeCompatibilityCallback() const noexcept {
	return m_impl->getVideoModeCompatibilityCallback();
}


void VideoBase::setVideoModeCallback(VideoModeCallback cbk) noexcept {
	m_impl->setVideoModeCallback(std::move(cbk));
}

const VideoBase::VideoModeCallback& VideoBase::getVideoModeCallback() const noexcept {
	return m_impl->getVideoModeCallback();
}



void VideoBase::setVideoModeLimits(VideoMode videoMode) noexcept {
	m_impl->setVideoModeLimits(*this, std::move(videoMode));
}

const VideoMode& VideoBase::getVideoModeLimits() const noexcept {
	return m_impl->getVideoModeLimits();
}

const std::vector<VideoMode>& VideoBase::getVideoModeCompatibility() const noexcept {
	return m_impl->getVideoModeCompatibility();
}

const VideoMode& VideoBase::getVideoMode() const noexcept {
	return m_impl->getVideoMode();
}



void VideoBase::setVideoModeLimitCallback(VideoModeCallback cbk) noexcept {
	m_impl->setVideoModeLimitCallback(std::move(cbk));
}

const VideoBase::VideoModeCallback& VideoBase::getVideoModeLimitCallback() const noexcept {
	return m_impl->getVideoModeLimitCallback();
}


void VideoBase::setInternalVideoModeCallback(VideoModeCallback cbk) noexcept {
	m_impl->setInternalVideoModeCallback(std::move(cbk));
}

const VideoBase::VideoModeCallback& VideoBase::getInternalVideoModeCallback() const noexcept {
	return m_impl->getInternalVideoModeCallback();
}

void VideoBase::setVideoModeCompatibility(std::vector<VideoMode> comp) noexcept {
	m_impl->setVideoModeCompatibility(*this, std::move(comp));
}



/*
 * VideoScalerBase::Impl
 */
struct VideoScalerBase::Impl {	
	ScalingMode									scalingMode;
	ScalingFilter								scalingFilter;

	ScalingModeCallback							scalingModeCallback;
	ScalingFilterCallback						scalingFilterCallback;

	Impl(ScalingModeCallback modeCbk, ScalingFilterCallback filterCbk)
		: scalingMode(ScalingMode::STRETCH)
		, scalingFilter(ScalingFilter::NEAREST)
		, scalingModeCallback(std::move(modeCbk))
		, scalingFilterCallback(std::move(filterCbk))
	{
	}

	~Impl() = default;

	void setScalingMode(VideoScalerBase& base, ScalingMode mode) noexcept {
	if(scalingMode != mode) {
		scalingMode = mode;
		Utils::invokeIf(scalingModeCallback, base, scalingMode);
	}
}

	ScalingMode getScalingMode() const {
		return scalingMode;
	}


	void setScalingFilter(VideoScalerBase& base, ScalingFilter filter) noexcept {
		if(scalingFilter != filter) {
			scalingFilter = filter;
			Utils::invokeIf(scalingFilterCallback, base, scalingFilter);
		}
	}

	ScalingFilter getScalingFilter() const noexcept {
		return scalingFilter;
	}


	void setScalingModeCallback(ScalingModeCallback cbk) noexcept {
		scalingModeCallback = std::move(cbk);
	}

	const ScalingModeCallback& getScalingModeCallback() const noexcept {
		return scalingModeCallback;
	}


	void setScalingFilterCallback(ScalingFilterCallback cbk) noexcept {
		scalingFilterCallback = std::move(cbk);
	}

	const ScalingFilterCallback& getScalingFilterCallback() const noexcept {
		return scalingFilterCallback;
	}
};


/*
 * VideoScalerBase
 */

VideoScalerBase::VideoScalerBase(	ScalingModeCallback modeCbk, 
									ScalingFilterCallback filterCbk ) noexcept
	: m_impl({}, std::move(modeCbk), std::move(filterCbk))
{
}

VideoScalerBase::VideoScalerBase(VideoScalerBase&& other) noexcept = default;

VideoScalerBase::~VideoScalerBase() = default;


VideoScalerBase& VideoScalerBase::operator=(VideoScalerBase&& other) noexcept = default;


void VideoScalerBase::setScalingMode(ScalingMode mode) noexcept {
	m_impl->setScalingMode(*this, mode);
}

ScalingMode VideoScalerBase::getScalingMode() const noexcept {
	return m_impl->getScalingMode();
}


void VideoScalerBase::setScalingFilter(ScalingFilter filter) noexcept {
	m_impl->setScalingFilter(*this, filter);
}

ScalingFilter VideoScalerBase::getScalingFilter() const noexcept {
	return m_impl->getScalingFilter();
}


void VideoScalerBase::setScalingModeCallback(ScalingModeCallback cbk) noexcept {
	m_impl->setScalingModeCallback(std::move(cbk));
}

const VideoScalerBase::ScalingModeCallback& VideoScalerBase::getScalingModeCallback() const noexcept {
	return m_impl->getScalingModeCallback();
}


void VideoScalerBase::setScalingFilterCallback(ScalingFilterCallback cbk) noexcept {
	m_impl->setScalingFilterCallback(std::move(cbk));
}

const VideoScalerBase::ScalingFilterCallback& VideoScalerBase::getScalingFilterCallback() const noexcept {
	return m_impl->getScalingFilterCallback();
}


/*
 * operator<< std::ostream
 */

std::ostream& operator<<(std::ostream& os, const VideoMode& videoMode) {
	return Zuazo::operator<<(
		os, 
		std::forward_as_tuple(
			std::make_pair("frameRate", 			std::get<VideoMode::m_frameRate>(videoMode.m_data)),
			std::make_pair("resolution", 			std::get<VideoMode::m_resolution>(videoMode.m_data)),
			std::make_pair("pixelAspectRatio", 		std::get<VideoMode::m_pixelAspectRatio>(videoMode.m_data)),
			std::make_pair("colorPrimaries", 		std::get<VideoMode::m_colorPrimaries>(videoMode.m_data)),
			std::make_pair("colorModel", 			std::get<VideoMode::m_colorModel>(videoMode.m_data)),
			std::make_pair("colorTransferFunction", std::get<VideoMode::m_colorTransferFunction>(videoMode.m_data)),
			std::make_pair("colorSubsampling", 		std::get<VideoMode::m_colorSubsampling>(videoMode.m_data)),
			std::make_pair("colorRange", 			std::get<VideoMode::m_colorRange>(videoMode.m_data)),
			std::make_pair("colorFormat", 			std::get<VideoMode::m_colorFormat>(videoMode.m_data))
		)
	);
}


}