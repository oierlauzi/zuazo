#include <zuazo/Video.h>

#include <zuazo/StringConversions.h>
#include <zuazo/Utils/Functions.h>

#include <tuple>

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
		: m_frameRate(std::move(frameRate))
		, m_resolution(std::move(resolution))
		, m_pixelAspectRatio(std::move(pixelAspectRatio))
		, m_colorPrimaries(std::move(colorPrimaries))
		, m_colorModel(std::move(colorModel))
		, m_colorTransferFunction(std::move(colorTransferFunction))
		, m_colorSubsampling(std::move(colorSubsampling))
		, m_colorRange(std::move(colorRange))
		, m_colorFormat(std::move(colorFormat))
{
}

bool VideoMode::operator==(const VideoMode& other) const {
	return
		std::tie(
			m_frameRate,
			m_resolution,
			m_pixelAspectRatio,
			m_colorPrimaries,
			m_colorModel,
			m_colorTransferFunction,
			m_colorSubsampling,
			m_colorRange,
			m_colorFormat
		)
		==
		std::tie(
			other.m_frameRate,
			other.m_resolution,
			other.m_pixelAspectRatio,
			other.m_colorPrimaries,
			other.m_colorModel,
			other.m_colorTransferFunction,
			other.m_colorSubsampling,
			other.m_colorRange,
			other.m_colorFormat
		) ;
}

bool VideoMode::operator!=(const VideoMode& other) const {
	return !operator==(other);
}


VideoMode::operator bool() const {
	return 
		static_cast<bool>(m_frameRate) &&
		static_cast<bool>(m_resolution) &&
		static_cast<bool>(m_pixelAspectRatio) &&
		static_cast<bool>(m_colorPrimaries) &&
		static_cast<bool>(m_colorModel) &&
		static_cast<bool>(m_colorTransferFunction) &&
		static_cast<bool>(m_colorSubsampling) &&
		static_cast<bool>(m_colorRange) &&
		static_cast<bool>(m_colorFormat) ;
}

void VideoMode::setFrameRate(Utils::Limit<Rate> frameRate) {
	m_frameRate = std::move(frameRate);
}

const Utils::Limit<Rate>& VideoMode::getFrameRate() const {
	return m_frameRate;
}

Rate VideoMode::getFrameRateValue() const {
	return getFrameRate().value();
}


void VideoMode::setResolution(Utils::Limit<Resolution> resolution) {
	m_resolution = std::move(resolution);
}

const Utils::Limit<Resolution>& VideoMode::getResolution() const {
	return m_resolution;
}

Resolution VideoMode::getResolutionValue() const {
	return getResolution().value();
}


void VideoMode::setPixelAspectRatio(Utils::Limit<AspectRatio> pixelAspectRatio) {
	m_pixelAspectRatio = std::move(pixelAspectRatio);
}

const Utils::Limit<AspectRatio>& VideoMode::getPixelAspectRatio() const {
	return m_pixelAspectRatio;
}

AspectRatio VideoMode::getPixelAspectRatioValue() const {
	return getPixelAspectRatio().value();
}


void VideoMode::setColorPrimaries(Utils::Limit<ColorPrimaries> colorPrimaries) {
	m_colorPrimaries = std::move(colorPrimaries);
}

const Utils::Limit<ColorPrimaries>& VideoMode::getColorPrimaries() const {
	return m_colorPrimaries;
}

ColorPrimaries VideoMode::getColorPrimariesValue() const {
	return getColorPrimaries().value();
}


void VideoMode::setColorModel(Utils::Limit<ColorModel> colorModel) {
	m_colorModel = std::move(colorModel);
}

const Utils::Limit<ColorModel>& VideoMode::getColorModel() const {
	return m_colorModel;
}

ColorModel VideoMode::getColorModelValue() const {
	return getColorModel().value();
}


void VideoMode::setColorTransferFunction(Utils::Limit<ColorTransferFunction> colorTransferFunction) {
	m_colorTransferFunction = std::move(colorTransferFunction);
}

const Utils::Limit<ColorTransferFunction>& VideoMode::getColorTransferFunction() const {
	return m_colorTransferFunction;
}

ColorTransferFunction VideoMode::getColorTransferFunctionValue() const {
	return getColorTransferFunction().value();
}


void VideoMode::setColorSubsampling(Utils::Limit<ColorSubsampling> colorSubsampling) {
	m_colorSubsampling = std::move(colorSubsampling);
}

const Utils::Limit<ColorSubsampling>& VideoMode::getColorSubsampling() const {
	return m_colorSubsampling;
}

ColorSubsampling VideoMode::getColorSubsamplingValue() const {
	return getColorSubsampling().value();
}


void VideoMode::setColorRange(Utils::Limit<ColorRange> colorRange) {
	m_colorRange = std::move(colorRange);
}

const Utils::Limit<ColorRange>& VideoMode::getColorRange() const {
	return m_colorRange;
}

ColorRange VideoMode::getColorRangeValue() const {
	return getColorRange().value();
}


void VideoMode::setColorFormat(Utils::Limit<ColorFormat> colorFormat) {
	m_colorFormat = std::move(colorFormat);
}

const Utils::Limit<ColorFormat>& VideoMode::getColorFormat() const {
	return m_colorFormat;
}

ColorFormat VideoMode::getColorFormatValue() const {
	return getColorFormat().value();
}


VideoMode VideoMode::intersect(const VideoMode& other) const {
	return VideoMode(
		getFrameRate().intersect(other.getFrameRate()),
		getResolution().intersect(other.getResolution()),
		getPixelAspectRatio().intersect(other.getPixelAspectRatio()),
		getColorPrimaries().intersect(other.getColorPrimaries()),
		getColorModel().intersect(other.getColorModel()),
		getColorTransferFunction().intersect(other.getColorTransferFunction()),
		getColorSubsampling().intersect(other.getColorSubsampling()),
		getColorRange().intersect(other.getColorRange()),
		getColorFormat().intersect(other.getColorFormat())
	);
}


VideoMode VideoMode::lowest() const {
	return VideoMode(
		Utils::MustBe(getFrameRate().lowest()),
		Utils::MustBe(getResolution().lowest()),
		Utils::MustBe(getPixelAspectRatio().lowest()),
		Utils::MustBe(getColorPrimaries().lowest()),
		Utils::MustBe(getColorModel().lowest()),
		Utils::MustBe(getColorTransferFunction().lowest()),
		Utils::MustBe(getColorSubsampling().lowest()),
		Utils::MustBe(getColorRange().lowest()),
		Utils::MustBe(getColorFormat().lowest())
	);
}

VideoMode VideoMode::highest() const {
	return VideoMode(
		Utils::MustBe(getFrameRate().highest()),
		Utils::MustBe(getResolution().highest()),
		Utils::MustBe(getPixelAspectRatio().highest()),
		Utils::MustBe(getColorPrimaries().highest()),
		Utils::MustBe(getColorModel().highest()),
		Utils::MustBe(getColorTransferFunction().highest()),
		Utils::MustBe(getColorSubsampling().highest()),
		Utils::MustBe(getColorRange().highest()),
		Utils::MustBe(getColorFormat().highest())
	);
}

VideoMode VideoMode::value() const {
	return VideoMode(
		Utils::MustBe(getFrameRate().value()),
		Utils::MustBe(getResolution().value()),
		Utils::MustBe(getPixelAspectRatio().value()),
		Utils::MustBe(getColorPrimaries().value()),
		Utils::MustBe(getColorModel().value()),
		Utils::MustBe(getColorTransferFunction().value()),
		Utils::MustBe(getColorSubsampling().value()),
		Utils::MustBe(getColorRange().value()),
		Utils::MustBe(getColorFormat().value())
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
		Math::Vec2<ColorChromaLocation>(ColorChromaLocation::cosited0, ColorChromaLocation::cosited0), //TODO
		getColorRangeValue(),
		getColorFormatValue()
	};
}


/*
 * VideoBase::Impl
 */

struct VideoBase::Impl {
	VideoModeNegotiationCallback				videoModeNegotiationCallback;
	VideoModeCallback							videoModeCallback;
	
	std::vector<VideoMode>						videoModeCompatibility;
	VideoMode									videoMode;


	Impl( VideoModeCallback videoModeCbk) noexcept
		: videoModeNegotiationCallback(DefaultVideoModeNegotiator())
		, videoModeCallback(std::move(videoModeCbk))
		, videoModeCompatibility()
		, videoMode()
	{
	}

	~Impl() = default;


	void setVideoModeNegotiationCallback(VideoBase& base, VideoModeNegotiationCallback cbk) noexcept {
		videoModeNegotiationCallback = std::move(cbk);
		setVideoModeCompatibility(base, std::move(videoModeCompatibility)); //Will invoke the callback
	}

	const VideoModeNegotiationCallback& getVideoModeNegotiationCallback() const noexcept {
		return videoModeNegotiationCallback;
	}

	const std::vector<VideoMode>& getVideoModeCompatibility() const noexcept {
		return videoModeCompatibility;
	}


	void setVideoMode(VideoBase& base, VideoMode vm) {
		//Sanitize the videomode
		if(static_cast<bool>(vm)) {
			bool valid = false;
			for(const auto& compatibility : videoModeCompatibility) {
				const auto sanitizedVideoMode = compatibility.intersect(vm);

				valid = static_cast<bool>(sanitizedVideoMode);
				if(valid) {
					vm = sanitizedVideoMode.value();
					break;
				}
			}
			if(!valid) {
				//Loop has finished without a valid video mode
				vm = VideoMode();
			}
		}

		//Write changes
		if(videoMode != vm) {
			videoMode = std::move(vm);
			Utils::invokeIf(videoModeCallback, base, videoMode);
		}
	}

	const VideoMode& getVideoMode() const noexcept {
		return videoMode;
	}



	void setVideoModeCallback(VideoModeCallback cbk) noexcept {
		videoModeCallback = std::move(cbk);
	}

	const VideoModeCallback& getVideoModeCallback() const noexcept {
		return videoModeCallback;
	}



	void setVideoModeCompatibility(VideoBase& base, std::vector<VideoMode> comp) noexcept {
		videoModeCompatibility = std::move(comp);

		if(!videoModeCompatibility.empty()) {
			if(videoModeNegotiationCallback) {
				//Videomode negotiation callback is defined, use it
				setVideoMode(base, videoModeNegotiationCallback(base, videoModeCompatibility));
			} else {
				//Use the last video mode compatibility. Sanitizer will kick in if not supported
				setVideoMode(base, videoMode);
			}

		} else {
			//No compatibility
			setVideoMode(base, VideoMode());
		}
	}

};



/*
 * VideoBase
 */

VideoBase::VideoBase(VideoModeCallback cbk) noexcept
	: m_impl({}, std::move(cbk))
{
}

VideoBase::VideoBase(VideoBase&& other) noexcept = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(VideoBase&& other) noexcept = default;



void VideoBase::setVideoModeNegotiationCallback(VideoModeNegotiationCallback cbk) noexcept {
	m_impl->setVideoModeNegotiationCallback(*this, std::move(cbk));
}

const VideoBase::VideoModeNegotiationCallback& VideoBase::getVideoModeNegotiationCallback() const noexcept {
	return m_impl->getVideoModeNegotiationCallback();
}

const std::vector<VideoMode>& VideoBase::getVideoModeCompatibility() const noexcept {
	return m_impl->getVideoModeCompatibility();
}


void VideoBase::setVideoMode(VideoMode videoMode) {
	m_impl->setVideoMode(*this, std::move(videoMode));
}

const VideoMode& VideoBase::getVideoMode() const noexcept {
	return m_impl->getVideoMode();
}



void VideoBase::setVideoModeCallback(VideoModeCallback cbk) noexcept {
	m_impl->setVideoModeCallback(std::move(cbk));
}

const VideoBase::VideoModeCallback& VideoBase::getVideoModeCallback() const noexcept {
	return m_impl->getVideoModeCallback();
}


void VideoBase::setVideoModeCompatibility(std::vector<VideoMode> comp) noexcept {
	m_impl->setVideoModeCompatibility(*this, std::move(comp));
}



/*
 * DefaultVideoModeNegotiator
 */

DefaultVideoModeNegotiator::DefaultVideoModeNegotiator(VideoMode videoMode)
	: VideoMode(std::move(videoMode))
{
}

VideoMode DefaultVideoModeNegotiator::operator()(VideoBase&, const std::vector<VideoMode>&) const {
	return static_cast<const VideoMode&>(*this);
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
		: scalingMode(ScalingMode::stretch)
		, scalingFilter(ScalingFilter::nearest)
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
			std::make_pair("frameRate", 			videoMode.getFrameRate()),
			std::make_pair("resolution", 			videoMode.getResolution()),
			std::make_pair("pixelAspectRatio", 		videoMode.getPixelAspectRatio()),
			std::make_pair("colorPrimaries", 		videoMode.getColorPrimaries()),
			std::make_pair("colorModel", 			videoMode.getColorModel()),
			std::make_pair("colorTransferFunction", videoMode.getColorTransferFunction()),
			std::make_pair("colorSubsampling", 		videoMode.getColorSubsampling()),
			std::make_pair("colorRange", 			videoMode.getColorRange()),
			std::make_pair("colorFormat", 			videoMode.getColorFormat())
		)
	);
}


}