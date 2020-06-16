#include <Graphics/StagedFrame.h>

namespace Zuazo::Graphics {

StagedFrame::StagedFrame(	const Vulkan& vulkan,
							Math::Vec2f size,
							const std::shared_ptr<const Buffer>& colorTransfer,
							Utils::BufferView<const Frame::PlaneDescriptor> planes,
							const std::shared_ptr<const vk::UniqueCommandPool>& cmdPool )
	: Frame(
		vulkan,
		size,
		colorTransfer,
		planes,
		vk::ImageUsageFlagBits::eTransferDst )
	, m_stagingBuffer(createStagingBuffer(vulkan, getPlaneAreas()))
	, m_pixelData(getPixelData(vulkan, getPlaneAreas(), m_stagingBuffer))
	, m_commandPool(cmdPool)
	, m_commandBuffer(createCommandBuffer(getVulkan(), planes, **m_commandPool, getImages(), getPlaneAreas(), m_stagingBuffer))
	, m_commandBufferSubmit(createSubmitInfo(*m_commandBuffer))
{
}

StagedFrame::~StagedFrame() {
	waitDependecies();
}

const Frame::PixelData& StagedFrame::getPixelData() {
	return m_pixelData;
}

void StagedFrame::flush() {
	waitDependecies();

	//Flush the mapped memory
	const vk::MappedMemoryRange range(
		m_stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);
	getVulkan().flushMappedMemory(range);

	//Send it to the queue
	getVulkan().resetFences(*m_uploadComplete);
	getVulkan().submit(
		getVulkan().getTransferQueue(),
		m_commandBufferSubmit,
		*m_uploadComplete
	);

	addDependecy(*m_uploadComplete);
}




Buffer StagedFrame::createStagingBuffer(const Vulkan& vulkan,
										Utils::BufferView<const Utils::Area> areas )
{
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		areas.back().offset() + areas.back().size() 
	);
}

Frame::PixelData StagedFrame::getPixelData(	const Vulkan& vulkan,
											Utils::BufferView<const Utils::Area> areas,
											const Buffer& buffer )
{
	const vk::MappedMemoryRange range(
		buffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);

	auto* data = reinterpret_cast<std::byte*>(vulkan.mapMemory(range));
	return Utils::slice(data, areas);
}

vk::UniqueCommandBuffer StagedFrame::createCommandBuffer(	const Vulkan& vulkan,
															Utils::BufferView<const PlaneDescriptor> planes,
															vk::CommandPool cmdPool,
															Utils::BufferView<const vk::UniqueImage> images,
															Utils::BufferView<const Utils::Area> areas,
															const Buffer& stagingBuffer )
{
	const vk::CommandBufferAllocateInfo allocInfo(
		cmdPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffer = vulkan.allocateCommnadBuffer(allocInfo);

	//Record the command buffer
	const vk::CommandBufferBeginInfo cbBeginInfo(
		{},
		nullptr
	);

	vulkan.begin(*cmdBuffer, cbBeginInfo);
	{
		const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex();
		assert(images.size() == areas.size());
		assert(images.size() == planes.size());

		std::vector<vk::ImageMemoryBarrier> memoryBarriers(images.size());
		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		//Transition the layout of the images
		for(size_t i = 0; i < images.size(); i++){
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

			const auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

			memoryBarriers[i] = vk::ImageMemoryBarrier(
				srcAccess,								//Old access mask
				dstAccess,								//New access mask
				vk::ImageLayout::eUndefined,			//Old layout
				vk::ImageLayout::eTransferDstOptimal,	//New layout
				srcFamily,								//Old queue family
				dstFamily,								//New queue family
				*(images[i]),							//Image
				imageSubresourceRange					//Image subresource
			);
		}

		cmdBuffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,		//Generating stages
			vk::PipelineStageFlagBits::eTransfer,		//Consuming stages
			{},											//dependency flags
			{},											//Memory barriers
			{},											//Buffer memory barriers
			memoryBarriers,								//Image memory barriers
			vulkan.getDispatcher()
		);

		//Copy the buffer to the image
		for(size_t i = 0; i < images.size(); i++){
			const vk::BufferImageCopy region(
				areas[i].offset(), 						//Buffer offset
				0,										//Buffer line stride
				0,										//Buffer image lines
				vk::ImageSubresourceLayers(				//Image subresource
					imageSubresourceRange.aspectMask,		//Aspect mask
					imageSubresourceRange.baseMipLevel,		//Mip level
					imageSubresourceRange.baseArrayLayer,	//Array layer offset
					imageSubresourceRange.layerCount 		//Array layers
				),	
				vk::Offset3D(),							//Image offset
				vk::Extent3D(planes[i].extent, 1)		//Image extent
			);

			cmdBuffer->copyBufferToImage(
				stagingBuffer.getBuffer(),
				*(images[i]),
				vk::ImageLayout::eTransferDstOptimal,
				region,
				vulkan.getDispatcher()
			);
		}

		//Transition the layout of the images (again)
		for(size_t i = 0; i < images.size(); i++){
			constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

			const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

			memoryBarriers[i] = vk::ImageMemoryBarrier(
				srcAccess,								//Old access mask
				dstAccess,								//New access mask
				vk::ImageLayout::eTransferDstOptimal,	//Old layout
				vk::ImageLayout::eShaderReadOnlyOptimal,//New layout
				srcFamily,								//Old queue family
				dstFamily,								//New queue family
				*(images[i]),							//Image
				imageSubresourceRange					//Image subresource
			);
		}

		cmdBuffer->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,		//Generating stages
			vk::PipelineStageFlagBits::eAllGraphics,	//Consuming stages
			{},											//Dependency flags
			{},											//Memory barriers
			{},											//Buffer memory barriers
			memoryBarriers,								//Image memory barriers
			vulkan.getDispatcher()
		);
	}
	vulkan.end(*cmdBuffer);

	return cmdBuffer;
}

vk::SubmitInfo StagedFrame::createSubmitInfo(const vk::CommandBuffer& cmdBuffer)
{
	return vk::SubmitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);
}

}