#include <zuazo/Graphics/StagedFrame.h>

namespace Zuazo::Graphics {

StagedFrame::StagedFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const Buffer> colorTransfer,
							Utils::BufferView<const Image::PlaneDescriptor> planes,
							std::shared_ptr<const vk::UniqueCommandPool> cmdPool )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransfer),
		planes,
		vk::ImageUsageFlagBits::eTransferDst )
	, m_stagingBuffer(createStagingBuffer(vulkan, getImage().getMemory().areas))
	, m_pixelData(getPixelData(vulkan, getImage().getMemory().areas, m_stagingBuffer))
	, m_commandPool(std::move(cmdPool))
	, m_commandBuffer(createCommandBuffer(getVulkan(), planes, **m_commandPool, getImage(), m_stagingBuffer))
	, m_commandBufferSubmit(createSubmitInfo(*m_commandBuffer))
	, m_uploadComplete(vulkan.createFence(true))
{
}

StagedFrame::~StagedFrame() {
	waitCompletion(Vulkan::NO_TIMEOUT);
}

StagedFrame::PixelData StagedFrame::getPixelData() noexcept {
	return m_pixelData;
}

StagedFrame::ConstPixelData StagedFrame::getPixelData() const noexcept {
	return ConstPixelData(
		reinterpret_cast<const Utils::BufferView<const std::byte>*>(m_pixelData.data()),
		m_pixelData.size()
	);
}


void StagedFrame::flush() {
	//There should not be any pending upload
	assert(waitCompletion(0));

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
}

bool StagedFrame::waitCompletion(uint64_t timeo) const {
	return getVulkan().waitForFences(*m_uploadComplete, true, timeo);
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
		areas.back().end()
	);
}

std::vector<Utils::BufferView<std::byte>> StagedFrame::getPixelData(const Vulkan& vulkan,
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
															Utils::BufferView<const Image::PlaneDescriptor> planeDescriptors,
															vk::CommandPool cmdPool,
															const Image& image,
															const Buffer& stagingBuffer )
{
	auto cmdBuffer = vulkan.allocateCommnadBuffer(cmdPool, vk::CommandBufferLevel::ePrimary);

	const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != vulkan.getGraphicsQueueIndex();
	const size_t planeCount = planeDescriptors.size();
	assert(planeCount == image.getMemory().areas.size());
	assert(planeCount == image.getPlanes().size());
	
	//Record the command buffer
	const vk::CommandBufferBeginInfo beginInfo(
		{},
		nullptr
	);

	vulkan.begin(*cmdBuffer, beginInfo);
	{
		std::vector<vk::ImageMemoryBarrier> memoryBarriers;
		memoryBarriers.reserve(planeCount);

		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		//Transition the layout of the images
		{
			memoryBarriers.clear();
			for(const auto& plane : image.getPlanes()){
				constexpr vk::AccessFlags srcAccess = {};
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferDstOptimal;

				//We don't mind about the previous contents, so skip the ownewship transfer
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					*plane.image,							//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == planeCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTopOfPipe;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eTransfer;

			vulkan.pipelineBarrier(
				*cmdBuffer,									//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}

		//Copy the buffer to the image
		for(size_t i = 0; i < planeCount; i++){
			constexpr vk::ImageSubresourceLayers imageSubresourceLayers(
				imageSubresourceRange.aspectMask,					//Aspect mask
				imageSubresourceRange.baseMipLevel,					//Mip level
				imageSubresourceRange.baseArrayLayer,				//Array layer offset
				imageSubresourceRange.layerCount 					//Array layers
			);

			const vk::BufferImageCopy region(
				image.getMemory().areas[i].offset(), 				//Buffer offset
				0,													//Buffer line stride
				0,													//Buffer image lines
				imageSubresourceLayers,								//Image subresource
				vk::Offset3D(),										//Image offset
				vk::Extent3D(planeDescriptors[i].extent, 1)			//Image extent
			);

			vulkan.copy(
				*cmdBuffer,
				stagingBuffer.getBuffer(),
				*(image.getPlanes()[i].image),
				vk::ImageLayout::eTransferDstOptimal,
				region
			);
		}

		//Transition the layout of the images (again)
		{
			memoryBarriers.clear();
			for(const auto& plane : image.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferDstOptimal;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

				const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
				const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					*plane.image,							//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == planeCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTransfer;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eAllGraphics;

			vulkan.pipelineBarrier(
				*cmdBuffer,									//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}

		
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