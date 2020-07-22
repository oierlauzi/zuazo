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

VideoMode::operator Graphics::Frame::Descriptor() const {
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

VideoMode::operator bool() const {
	return std::apply(
		[] (const auto&... x) -> bool {
			return ((x.index() != Utils::LIMIT_TYPE_NONE_INDEX) && ...);
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
		std::swap(vm, m_videoMode);

		for(const auto& cbk : m_videoModeCallbacks) {
			if(cbk) cbk(m_videoMode);
		}
	}
}

VideoMode VideoBase::selectVideoMode() const {
	for(const auto& compatibility : m_videoModeCompatibility) {
		const auto interscetion = compatibility.intersect(m_videoModeLimits);
		if(interscetion) return interscetion;
	}

	throw Exception("Unsupported video mode");
}

}