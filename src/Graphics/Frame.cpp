#include <Graphics/Frame.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				const Descriptor& desc,
				std::shared_ptr<Images>&& images )
	: m_vulkan(vulkan)
	, m_colorTransfer(desc.colorTransfer)
	, m_images(std::move(images))
	, m_imageViews(createImageViews(
		m_vulkan,
		desc.colorFormat,
		*m_images ))
	, m_descriptorPool(createDescriptorPool(m_vulkan))
	, m_descriptorSets(allocateDescriptorSets(m_vulkan, *m_descriptorPool))
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


Frame::Descriptor Frame::getDescriptorForUpload(const Vulkan& vulkan,
												Resolution resolution,
												ColorSubsampling subsampling,
												ColorFormat format,
												ColorRange range,
												ColorTransferFunction transferFunction,
												ColorModel model,
												ColorPrimaries primaries )
{
	Frame::Descriptor result;

	//Get the plane count
	const auto planeCount = getPlaneCount(format);

	//Obtain the resolutions of each plane
	result.extents[0] = toVulkan(resolution);
	if(planeCount > 1 && planeCount <= 3) {
		const auto chromaSize = toVulkan(getChromaResolution(subsampling, resolution));

		for(size_t i = 1; i < planeCount; i++){
			result.extents[i] = chromaSize;
		}
	} //else 4 planar formats to be implemented

	//Obtain the format
	result.colorFormat = toVulkan(format);
	const auto& supportedFormats = vulkan.getFormatSupport();
	for(size_t i = 0; i < planeCount; i++){
		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(result.colorFormat[i]) )
			== supportedFormats.sampler.cend())
		{
			throw Exception("Unsupported format!");
		}
	}
	switch(planeCount){
	case 1:
		result.colorTransfer.planeFormat = PLANE_FORMAT_RGBA;	
		break;
	case 2:
		result.colorTransfer.planeFormat = PLANE_FORMAT_G_BR;
		break;
	case 3:
		result.colorTransfer.planeFormat = PLANE_FORMAT_G_B_R;
		break;
	default:
		throw Exception("Unsupported plane count!");
	}

	//Decide the range
	result.colorTransfer.colorRange = toVulkan(range, model);
	if(result.colorTransfer.colorRange < 0){
		throw Exception("Unsupported color range!");
	}

	//Decide the transfer function
	result.colorTransfer.colorTransferFunction = toVulkan(transferFunction);
	if(result.colorTransfer.colorTransferFunction < 0){
		throw Exception("Unsupported color transfer function!");
	}

	//Obtain the color model
	result.colorTransfer.colorModel = getConversionMatrix(model);
	if(result.colorTransfer.colorModel == Math::Mat4x4f()){
		throw Exception("Unsupported color model!");
	}

	//Obtain the color primaries
	result.colorTransfer.colorPrimaries = getConversionMatrix(primaries);
	if(result.colorTransfer.colorPrimaries == Math::Mat4x4f()){
		throw Exception("Unsupported color primaries!");
	}

	//Try to optimize the format
	for(size_t i = 0; i < planeCount; i++){
		const auto optimizedFormat = optimizeFormat(result.colorFormat[i]);
		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(optimizedFormat) )
			!= supportedFormats.sampler.cend())
		{
			result.colorFormat[i] = optimizedFormat;
		}
	}

	//Try to optimize the transfer function
	if(planeCount == 1){
		const auto optimizedFormat = optimizeFormat(
			std::get<vk::Format>(result.colorFormat[0]), 
			result.colorTransfer.colorTransferFunction,
			result.colorTransfer.colorRange
		);

		if(	std::find(
				supportedFormats.sampler.cbegin(), 
				supportedFormats.sampler.cend(), 
				std::get<vk::Format>(optimizedFormat) )
			!= supportedFormats.sampler.cend())
		{
			std::get<vk::Format>(result.colorFormat[0]) = std::get<vk::Format>(optimizedFormat);
			result.colorTransfer.colorTransferFunction = std::get<int32_t>(optimizedFormat);
		}
	}

	return result;
}





Frame::ImageViews Frame::createImageViews(	const Vulkan& vulkan,
											const Descriptor::Formats& formats,
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

vk::UniqueDescriptorPool Frame::createDescriptorPool(const Vulkan& vulkan){
	const std::array poolSizes = {
		vk::DescriptorPoolSize(
			vk::DescriptorType::eCombinedImageSampler,
			DESCRIPTOR_SET_COUNT
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,
			DESCRIPTOR_SET_COUNT
		)
	};

	const vk::DescriptorPoolCreateInfo createInfo(
		{},														//Flags
		DESCRIPTOR_SET_COUNT,									//Descriptor set count
		poolSizes.size(), poolSizes.data()						//Pool sizes
	);

	return vulkan.createDescriptorPool(createInfo);
}

Frame::DescriptorSets Frame::allocateDescriptorSets(const Vulkan& vulkan,
													vk::DescriptorPool pool )
{
	const std::array layouts = {
		vulkan.getColorTransferDescriptor(vk::Filter::eNearest),
		vulkan.getColorTransferDescriptor(vk::Filter::eLinear),
	};
	static_assert(layouts.size() == DESCRIPTOR_SET_COUNT);

	const vk::DescriptorSetAllocateInfo allocInfo(
		pool,													//Pool
		DESCRIPTOR_SET_COUNT,									//Descriptor set count
		layouts.data()											//Layouts
	);

	DescriptorSets result;
	vulkan.getDevice().allocateDescriptorSets(&allocInfo, result.data(), vulkan.getDispatcher());
	return result;
}

}