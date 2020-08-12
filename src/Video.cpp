#include <zuazo/Video.h>

#include <zuazo/Exception.h>
#include <zuazo/Macros.h>

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
 * VideoBase
 */ 

VideoBase::VideoBase() = default;

VideoBase::VideoBase(VideoMode videoMode, VideoModeCallback cbk)
	: m_videoModeCallbacks{std::move(cbk), {}}
	, m_videoModeLimits(std::move(videoMode))
{
}

VideoBase::VideoBase(const VideoBase& other) = default;

VideoBase::VideoBase(VideoBase&& other) = default;

VideoBase::~VideoBase() = default;

VideoBase& VideoBase::operator=(const VideoBase& other) = default;
VideoBase& VideoBase::operator=(VideoBase&& other) = default;



void VideoBase::setVideoModeCallback(VideoModeCallback cbk) {
	m_videoModeCallbacks[VMCBK_EXTERNAL] = std::move(cbk);
}

const VideoBase::VideoModeCallback& VideoBase::getVideoModeCallback() const {
	return m_videoModeCallbacks[VMCBK_EXTERNAL];
}



void VideoBase::setVideoModeLimits(VideoMode videoMode) {
	m_videoModeLimits = std::move(videoMode);
	updateVideoMode();
}

const VideoMode& VideoBase::getVideoModeLimits() const {
	return m_videoModeLimits;
}

const std::vector<VideoMode>& VideoBase::getVideoModeCompatibility() const {
	return m_videoModeCompatibility;
}

const VideoMode& VideoBase::getVideoMode() const {
	return m_videoMode;
}



void VideoBase::setInternalVideoModeCallback(VideoModeCallback cbk) {
	m_videoModeCallbacks[VMCBK_INTERNAL] = std::move(cbk);
}

const VideoBase::VideoModeCallback& VideoBase::getInternalVideoModeCallback() const {
	return m_videoModeCallbacks[VMCBK_INTERNAL];
}

void VideoBase::setVideoModeCompatibility(std::vector<VideoMode> comp) {
	m_videoModeCompatibility = std::move(comp);
	updateVideoMode();
}



void VideoBase::updateVideoMode() {
	VideoMode vm = selectVideoMode();

	if(vm != m_videoMode) {
		//Videomode has changed
		m_videoMode = vm;

		//Call the callbacks
		for(const auto& cbk : m_videoModeCallbacks) {
			if(cbk) cbk(m_videoMode);
		}
	}
}

VideoMode VideoBase::selectVideoMode() const {
	for(const auto& compatibility : m_videoModeCompatibility) {
		const auto interscetion = compatibility.intersect(m_videoModeLimits);
		if(interscetion) return interscetion.values();
	}

	throw Exception("Unsupported video mode");
}

}