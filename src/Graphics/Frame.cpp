#include <Graphics/Frame.h>

#include <Graphics/VulkanConversions.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				Image&& image,
				std::shared_ptr<Buffer>&& colorTransfer )
	: m_vulkan(vulkan)
	, m_image(std::move(image))
	, m_colorTransfer(std::move(colorTransfer))
	, m_descriptorPool(createDescriptorPool(m_vulkan))
	, m_descriptorSet(allocateDescriptorSet(m_vulkan, *m_descriptorPool))
{
}

Frame::~Frame(){
	m_vulkan.getDevice().waitIdle(m_vulkan.getDispatcher());
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




vk::UniqueDescriptorPool Frame::createDescriptorPool(const Vulkan& vulkan){
	//A descriptor pool is created, from which 2 descriptor sets, each one with 
	//one combined image sampler and one uniform buffer. Threrefore, 2 descriptors
	//of each type will be required.
	const std::array poolSizes = {
		vk::DescriptorPoolSize(
			vk::DescriptorType::eSampledImage,					//Descriptor type
			1													//Descriptor count //TODO maybe IMAGE_COUNT?
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,					//Descriptor type
			1													//Descriptor count
		)
	};

	const vk::DescriptorPoolCreateInfo createInfo(
		{},														//Flags
		1,														//Descriptor set count
		poolSizes.size(), poolSizes.data()						//Pool sizes
	);

	return vulkan.createDescriptorPool(createInfo);
}

vk::DescriptorSet Frame::allocateDescriptorSet(	const Vulkan& vulkan,
												vk::DescriptorPool pool )
{
	auto layout = vulkan.getColorTransferDescriptorSetLayout();

	const vk::DescriptorSetAllocateInfo allocInfo(
		pool,													//Pool
		1, &layout												//Layouts
	);

	vk::DescriptorSet descriptorSet;
	const auto result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, &descriptorSet, vulkan.getDispatcher());

	if(result != vk::Result::eSuccess){
		throw Exception("Error allocating descriptor sets");
	}

	return descriptorSet;
}

}