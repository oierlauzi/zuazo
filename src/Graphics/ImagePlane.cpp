#include <zuazo/Graphics/Image.h>

namespace Zuazo::Graphics {

Image::Plane::Plane(vk::Extent3D extent,
					vk::Format format,
					const vk::ComponentMapping& swizzle,
					vk::Image image,
					vk::ImageView imageView ) noexcept
	: m_extent(extent)
	, m_format(format)
	, m_swizzle(swizzle)
	, m_image(image)
	, m_imageView(imageView)
{
}



void Image::Plane::setExtent(vk::Extent3D extent) noexcept {
	m_extent = extent;
}

vk::Extent3D Image::Plane::getExtent() const noexcept {
	return m_extent;
}


void Image::Plane::setFormat(vk::Format format) noexcept {
	m_format = format;
}

vk::Format Image::Plane::getFormat() const noexcept {
	return m_format;
}


void Image::Plane::setSwizzle(const vk::ComponentMapping& swizzle) noexcept {
	m_swizzle = swizzle;
}

const vk::ComponentMapping& Image::Plane::getSwizzle() const noexcept {
	return m_swizzle;
}


void Image::Plane::setImage(vk::Image image) noexcept {
	m_image = image;
}

vk::Image Image::Plane::getImage() const noexcept {
	return m_image;
}


void Image::Plane::setImageView(vk::ImageView imageView) noexcept {
	m_imageView = imageView;
}

vk::ImageView Image::Plane::getImageView() const noexcept {
	return m_imageView;
}

}