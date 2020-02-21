#include <Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				const Formats& formats,
				const ColorTransfer& colorTransfer,
				const Images& images )
	: m_vulkan(vulkan)
	, m_images(images)
	, m_imageViews(createImageViews(
		m_vulkan,
		formats,
		m_images ))
{
}

Frame::~Frame(){
	m_vulkan.getGraphicsQueue().waitIdle(m_vulkan.getDispatcher());
}


Frame::ImageViews Frame::createImageViews(	const Vulkan& vulkan,
											const Formats& formats,
											const Images& images )
{
	Frame::ImageViews result;

	for(size_t i = 0; i < result.size(); i++){
		if(images[i]) {
			const vk::ImageViewCreateInfo createInfo(
				{},												//Flags
				images[i],										//Image
				vk::ImageViewType::e2D,							//ImageView type
				std::get<vk::Format>(formats[i]),				//Image format
				std::get<vk::ComponentMapping>(formats[i]),		//Swizzle
				vk::ImageSubresourceRange(						//Image subresources
					vk::ImageAspectFlagBits::eColor,				//Aspect mask
					0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
				)
			);

			result[i] = vulkan.createImageView(createInfo);
		} else {
			break;
		}
	}


	return result;
}


}