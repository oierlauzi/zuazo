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
	, m_descriptorSets(allocateDescriptorSets(m_vulkan, *m_descriptorPool))
{
	//Update the descriptors
	for(size_t i = 0; i < m_descriptorSets.size(); i++){
		//Update all images. As nullptr images can't be passed, repeat available images		
		const auto& imageViews = m_image.getImageViews();
		std::array<vk::DescriptorImageInfo, IMAGE_COUNT> images;
		for(size_t j = 0; j < images.size(); j++){
			images[j] = vk::DescriptorImageInfo(
				nullptr,												//Sampler
				*(imageViews[j % imageViews.size()]),					//Image views
				vk::ImageLayout::eShaderReadOnlyOptimal					//Layout
			);
		}

		const std::array buffers = {
			vk::DescriptorBufferInfo(
				m_colorTransfer->getBuffer(),							//Buffer 
				0,														//Offset
				sizeof(ColorTransfer)									//Size
			)
		};

		const std::array writeDescriptorSets ={
			vk::WriteDescriptorSet( //Image descriptor
				m_descriptorSets[i],									//Descriptor set
				IMAGE_BINDING,											//Binding
				0, 														//Index
				images.size(), 											//Descriptor count
				vk::DescriptorType::eCombinedImageSampler,				//Descriptor type
				images.data(), 											//Images
				nullptr, 												//Buffers
				nullptr													//Texel buffers
			),
			vk::WriteDescriptorSet( //Ubo descriptor set
				m_descriptorSets[i],									//Descriptor set
				COLOR_TRANSFER_BINDING,									//Binding
				0, 														//Index
				buffers.size(),											//Descriptor count		
				vk::DescriptorType::eUniformBuffer,						//Descriptor type
				nullptr, 												//Images 
				buffers.data(), 										//Buffers
				nullptr													//Texel buffers
			)
		};

		m_vulkan.getDevice().updateDescriptorSets(writeDescriptorSets, {}, m_vulkan.getDispatcher());
	}

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
			vk::DescriptorType::eCombinedImageSampler,			//Descriptor type
			DESCRIPTOR_COUNT * IMAGE_COUNT						//Descriptor count
		),
		vk::DescriptorPoolSize(
			vk::DescriptorType::eUniformBuffer,					//Descriptor type
			DESCRIPTOR_COUNT									//Descriptor count
		)
	};

	const vk::DescriptorPoolCreateInfo createInfo(
		{},														//Flags
		DESCRIPTOR_COUNT,										//Descriptor set count
		poolSizes.size(), poolSizes.data()						//Pool sizes
	);

	return vulkan.createDescriptorPool(createInfo);
}

Frame::DescriptorSets Frame::allocateDescriptorSets(const Vulkan& vulkan,
													vk::DescriptorPool pool )
{
	std::array<vk::DescriptorSetLayout, DESCRIPTOR_COUNT> layouts;
	for(size_t i = 0; i < layouts.size(); i++){
		const auto filter = static_cast<vk::Filter>(i);
		layouts[i] = vulkan.getColorTransferDescriptorSetLayout(filter);
	}

	const vk::DescriptorSetAllocateInfo allocInfo(
		pool,													//Pool
		layouts.size(), layouts.data()							//Layouts
	);

	DescriptorSets descriptorSets;
	static_assert(descriptorSets.size() == layouts.size());
	const auto result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, descriptorSets.data(), vulkan.getDispatcher());

	if(result != vk::Result::eSuccess){
		throw Exception("Error allocating descriptor sets");
	}

	return descriptorSets;
}

}