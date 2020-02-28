#include <Graphics/Frame.h>

#include <Exception.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				Image&& image,
				Buffer&& colorTransfer,
				std::unique_ptr<Data>&& data )
	: m_vulkan(vulkan)
	, m_readySemaphore(m_vulkan.createSemaphore())
	, m_readyFence(m_vulkan.createFence(true))
	, m_image(std::move(image))
	, m_colorTransfer(std::move(colorTransfer))
	, m_data(std::move(data))
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
				m_colorTransfer.getBuffer(),							//Buffer
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
	m_vulkan.getDevice().waitForFences(
		*m_readyFence,
		true,
		std::numeric_limits<uint64_t>::max(),
		m_vulkan.getDispatcher()
	);
}

vk::Semaphore& Frame::getReadySemaphore(){
	return *m_readySemaphore;
}

const vk::Semaphore& Frame::getReadySemaphore() const{
	return *m_readySemaphore;
}


vk::Fence& Frame::getReadyFence() {
	return *m_readyFence;
}

const vk::Fence& Frame::getReadyFence() const {
	return *m_readyFence;
}


Image& Frame::getImage() {
	return m_image;
}

const Image& Frame::getImage() const {
	return m_image;
}


Buffer& Frame::getColorTransfer() {
	return m_colorTransfer;
}

const Buffer& Frame::getColorTransfer() const {
	return m_colorTransfer;
}


Frame::Data& Frame::getData() {
	return *m_data;
}

const Frame::Data& Frame::getData() const {
	return *m_data;
}



void Frame::bind( 	vk::CommandBuffer cmd,
					vk::PipelineBindPoint bindPoint,
					vk::PipelineLayout layout,
					uint32_t index,
					vk::Filter filter ) 
{
	const std::array descriptorSets = {
		m_descriptorSets[static_cast<size_t>(filter)]
	};

	cmd.bindDescriptorSets(
		bindPoint,						//Pipeline bind point
		layout,							//Pipeline layout
		index,							//First index
		descriptorSets,					//Descriptor sets
		{},								//Dynamic offsets
		m_vulkan.getDispatcher()
	);
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