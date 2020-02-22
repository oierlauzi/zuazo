#include <Graphics/Frame.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				const Formats& formats,
				const ColorTransfer& colorTransfer,
				std::shared_ptr<Images>&& images )
	: m_vulkan(vulkan)
	, m_colorTransfer(colorTransfer)
	, m_images(std::move(images))
	, m_imageViews(createImageViews(
		m_vulkan,
		formats,
		*m_images ))
{
}

Frame::~Frame(){
	m_vulkan.getGraphicsQueue().waitIdle(m_vulkan.getDispatcher());
}

const ColorTransfer& Frame::getColorTransfer() const{
	return m_colorTransfer;
}

const Frame::Images& Frame::getImages() const{
	return *m_images;
}

const Frame::ImageViews& Frame::getImageViews() const{
	return m_imageViews;
}






Frame::ImageViews Frame::createImageViews(	const Vulkan& vulkan,
											const Formats& formats,
											const Images& images )
{
	Frame::ImageViews result;

	for(size_t i = 0; i < result.size(); i++){
		const auto& image = std::get<vk::UniqueImage>(images[i]);

		if(image) {
			const vk::ImageViewCreateInfo createInfo(
				{},												//Flags
				*image,											//Image
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