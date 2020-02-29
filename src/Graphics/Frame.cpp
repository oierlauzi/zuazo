#include <Graphics/Frame.h>

#include <Graphics/MappedMemory.h>
#include <Exception.h>

namespace Zuazo::Graphics {

Frame::Frame(	const Vulkan& vulkan,
				Image&& image,
				std::shared_ptr<const Buffer>&& colorTransfer )
	: m_vulkan(vulkan)
	, m_readyFence(m_vulkan.createFence(true))
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
	m_vulkan.getDevice().waitForFences(
		*m_readyFence,
		true,
		std::numeric_limits<uint64_t>::max(),
		m_vulkan.getDispatcher()
	);
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



const Buffer& Frame::getColorTransfer() const {
	return *m_colorTransfer;
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




std::shared_ptr<Buffer> Frame::createColorTransferBuffer( 	const Vulkan& vulkan,
															const ColorTransfer& colorTransfer )
{
	//Create the buffer
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eUniformBuffer |
		vk::BufferUsageFlagBits::eTransferDst;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	constexpr size_t size = sizeof(ColorTransfer);

	Buffer result(
		vulkan,
		usageFlags,
		memoryFlags,
		size
	);
 
	//Create a staging buffer. Using this technique due to preferably having
	//device memory allocated for the UBO as it will be only written once and
	//read frequently
	constexpr vk::BufferUsageFlags stagingUsageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags stagingMemoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	Buffer stagingBuffer(
		vulkan,
		stagingUsageFlags,
		stagingMemoryFlags,
		size
	);

	//Upload the contents to the staging buffer
	{
		MappedMemory mapping(
			vulkan,
			vk::MappedMemoryRange(
				stagingBuffer.getDeviceMemory(),
				0, VK_WHOLE_SIZE
			)
		);

		std::memcpy(
			mapping.data(),
			&colorTransfer,
			size
		);

		mapping.flush();
	}

	//Create a command pool and a command buffer for uploading it
	const vk::CommandPoolCreateInfo cpCreateInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	const auto commandPool = vulkan.createCommandPool(cpCreateInfo);

	const vk::CommandBufferAllocateInfo cbAllocInfo(
		*commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	vk::CommandBuffer cmdBuffer;
	vulkan.getDevice().allocateCommandBuffers(
		&cbAllocInfo, 
		&cmdBuffer, 
		vulkan.getDispatcher()
	);

	//Record the command buffer
	constexpr vk::CommandBufferUsageFlags cbUsage =
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	const vk::CommandBufferBeginInfo cbBeginInfo(
		cbUsage,
		nullptr
	);

	cmdBuffer.begin(cbBeginInfo, vulkan.getDispatcher()); 
	{
		cmdBuffer.copyBuffer(
			stagingBuffer.getBuffer(),
			result.getBuffer(),
			vk::BufferCopy(0, 0, size),
			vulkan.getDispatcher()
		);

		if(vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex()) {
			//Change the ownership to the graphics queue
			const vk::BufferMemoryBarrier memoryBarrier(
				{},								//Old access mask
				{},								//New access mask
				vulkan.getTransferQueueIndex(),	//Old queue family
				vulkan.getGraphicsQueueIndex(), //New queue family
				result.getBuffer(),				//Buffer
				0, VK_WHOLE_SIZE				//Range
			);

			constexpr vk::PipelineStageFlags sourceStages = 
				vk::PipelineStageFlagBits::eTransfer;

			constexpr vk::PipelineStageFlags destinationStages = 
				vk::PipelineStageFlagBits::eAllGraphics;
			
			cmdBuffer.pipelineBarrier(
				sourceStages,					//Generating stages
				destinationStages,				//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}
	}
	cmdBuffer.end(vulkan.getDispatcher());

	//Submit the command buffer
	const auto uploadFence = vulkan.createFence();
	const vk::SubmitInfo submitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);

	vulkan.getTransferQueue().submit(submitInfo, *uploadFence, vulkan.getDispatcher());

	//Wait for completion
	vulkan.getDevice().waitForFences(
		*uploadFence, 
		true, 
		std::numeric_limits<uint64_t>::max(),
		vulkan.getDispatcher()
	);

	return std::make_shared<Buffer>(std::move(result));
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