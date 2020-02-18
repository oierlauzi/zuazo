#include <Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan, 
				const ImageProperties& properties,
				vk::Image image )
	: m_properties(properties)
	, m_image(image)
	, m_imageView(createImageView(
		vulkan,
		m_properties.format,
		m_properties.swizzle,
		m_image ))
	, m_samplers(getSamplers(
		vulkan,
		m_properties ))
{
}





vk::UniqueImageView	Frame::createImageView(	const Vulkan& vulkan,
											vk::Format format,
											const vk::ComponentMapping& swizzle,
											vk::Image image )
{
	const vk::ImageViewCreateInfo createInfo(
		{},												//Flags
		image,											//Image
		vk::ImageViewType::e2D,							//ImageView type
		format,											//Image format
		swizzle,										//Swizzle
		vk::ImageSubresourceRange(						//Image subresources
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		)
	);

	return vulkan.getDevice().createImageViewUnique(createInfo, nullptr, vulkan.getDispatcher());
}

Frame::Samplers Frame::getSamplers(	const Vulkan& vulkan,
									const ImageProperties& properties )
{
	if(properties.nativeColorModel == vk::SamplerYcbcrModelConversion::eRgbIdentity){
		//None of the attributes of the YCbCr sampler would make a difference
		return {
			vulkan.getRgbSampler(vk::Filter::eNearest),
			vulkan.getRgbSampler(vk::Filter::eLinear),
		};
	} else {
		//YCbCr sampler is required
		return {
			vulkan.getYcbcrSampler(	vk::Filter::eNearest, 
									properties.nativeRange,
									properties.nativeColorModel,
									properties.format ),
			vulkan.getYcbcrSampler(	vk::Filter::eLinear, 
									properties.nativeRange,
									properties.nativeColorModel,
									properties.format )
		};
	}
}


}