#include <zuazo/Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Descriptor::Descriptor(	Resolution resolution, 
								AspectRatio pixelAspectRatio, 
								ColorPrimaries colorPrimaries, 
								ColorModel colorModel, 
								ColorTransferFunction colorTransferFunction, 
								ColorSubsampling colorSubsampling, 
								ColorRange colorRange, 
								ColorFormat colorFormat ) noexcept
	: m_resolution(resolution)
	, m_pixelAspectRatio(pixelAspectRatio)
	, m_colorPrimaries(colorPrimaries)
	, m_colorModel(colorModel)
	, m_colorTransferFunction(colorTransferFunction)
	, m_colorSubsampling(colorSubsampling)
	, m_colorRange(colorRange)
	, m_colorFormat(colorFormat)
{
}


void Frame::Descriptor::setResolution(Resolution resolution) noexcept {
	m_resolution = resolution;
}

Resolution Frame::Descriptor::getResolution() const noexcept {
	return 	m_resolution;
}


void Frame::Descriptor::setPixelAspectRatio(AspectRatio pixelAspectRatio) noexcept {
	m_pixelAspectRatio = pixelAspectRatio;
}

AspectRatio Frame::Descriptor::getPixelAspectRatio() const noexcept {
	return m_pixelAspectRatio;
}


void Frame::Descriptor::setColorPrimaries(ColorPrimaries colorPrimaries) noexcept {
	m_colorPrimaries = colorPrimaries;
}

ColorPrimaries Frame::Descriptor::getColorPrimaries() const noexcept {
	return m_colorPrimaries;
}


void Frame::Descriptor::setColorModel(ColorModel colorModel) noexcept {
	m_colorModel = colorModel;
}

ColorModel Frame::Descriptor::getColorModel() const noexcept {
	return m_colorModel;
}


void Frame::Descriptor::setColorTransferFunction(ColorTransferFunction colorTransferFunction) noexcept {
	m_colorTransferFunction = colorTransferFunction;
}

ColorTransferFunction Frame::Descriptor::getColorTransferFunction() const noexcept {
	return m_colorTransferFunction;
}


void Frame::Descriptor::setColorSubsampling(ColorSubsampling colorSubsampling) noexcept {
	m_colorSubsampling = colorSubsampling;
}

ColorSubsampling Frame::Descriptor::getColorSubsampling() const noexcept {
	return m_colorSubsampling;
}


void Frame::Descriptor::setColorRange(ColorRange colorRange) noexcept {
	m_colorRange = colorRange;
}

ColorRange Frame::Descriptor::getColorRange() const noexcept {
	return m_colorRange;
}


void Frame::Descriptor::setColorFormat(ColorFormat colorFormat) noexcept {
	m_colorFormat = colorFormat;
}

ColorFormat Frame::Descriptor::getColorFormat() const noexcept {
	return m_colorFormat;
}


Math::Vec2f Frame::Descriptor::calculateSize() const noexcept {
	return Math::Vec2f(
		static_cast<float>(getPixelAspectRatio() * getResolution().x),
		static_cast<float>(getResolution().y)
	);
}

}