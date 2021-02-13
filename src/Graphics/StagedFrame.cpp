#include <zuazo/Graphics/StagedFrame.h>

namespace Zuazo::Graphics {

StagedFrame::StagedFrame(	const Vulkan& vulkan,
							std::shared_ptr<const Descriptor> desc,
							std::shared_ptr<const InputColorTransfer> colorTransfer,
							std::shared_ptr<const Buffer> colorTransferBuffer,
							Utils::BufferView<const Image::Plane> planes,
							std::shared_ptr<const vk::UniqueCommandPool> cmdPool )
	: Frame(
		vulkan,
		std::move(desc),
		std::move(colorTransfer),
		std::move(colorTransferBuffer),
		planes,
		vk::ImageUsageFlagBits::eTransferDst )
	, m_stagingImage(createStagingImage(vulkan, planes))
	, m_pixelData(getPixelData(vulkan, m_stagingImage))
	, m_commandPool(std::move(cmdPool))
	, m_commandBuffer(createCommandBuffer(getVulkan(), **m_commandPool))
	, m_commandBufferSubmit(createSubmitInfo(*m_commandBuffer))
	, m_uploadComplete(vulkan.createFence(false))
{
	transitionStagingimageLayout();
	recordCommandBuffer();
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
		*(m_stagingImage.getMemory().memory),
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


std::shared_ptr<vk::UniqueCommandPool> StagedFrame::createCommandPool(const Vulkan& vulkan) {
	const vk::CommandPoolCreateInfo createInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		vulkan.getTransferQueueIndex()
	);

	return Utils::makeShared<vk::UniqueCommandPool>(
		vulkan.createCommandPool(createInfo)
	);
}


void StagedFrame::transitionStagingimageLayout() {
	const auto& vulkan = getVulkan();
	const auto& image = m_stagingImage;
	const auto cmd = *m_commandBuffer;
	const size_t planeCount = image.getPlanes().size();

	constexpr vk::ImageSubresourceRange imageSubresourceRange(
		vk::ImageAspectFlagBits::eColor,				//Aspect mask
		0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
	);

	//Record the command buffer
	const vk::CommandBufferBeginInfo beginInfo(
		{},
		nullptr
	);

	vulkan.begin(cmd, beginInfo);

	std::vector<vk::ImageMemoryBarrier> memoryBarriers;
	memoryBarriers.reserve(planeCount);

	for(const auto& plane : image.getPlanes()){
		constexpr vk::AccessFlags srcAccess = {};
		constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eHostWrite;

		constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
		constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eGeneral;

		//Allways owned by the transfer queue
		constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
		constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

		memoryBarriers.emplace_back(
			srcAccess,								//Old access mask
			dstAccess,								//New access mask
			srcLayout,								//Old layout
			dstLayout,								//New layout
			srcFamily,								//Old queue family
			dstFamily,								//New queue family
			plane.getImage(),						//Image
			imageSubresourceRange					//Image subresource
		);
	}
	assert(memoryBarriers.size() == planeCount);

	constexpr vk::PipelineStageFlags srcStages =
		vk::PipelineStageFlagBits::eHost;

	constexpr vk::PipelineStageFlags dstStages = 
		vk::PipelineStageFlagBits::eHost;

	vulkan.pipelineBarrier(
		cmd,										//Command buffer
		srcStages,									//Generating stages
		dstStages,									//Consuming stages
		{},											//Dependency flags
		Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
	);

	vulkan.end(cmd);

	//Submit it
	vulkan.submit(vulkan.getTransferQueue(), m_commandBufferSubmit, *m_uploadComplete);
	waitCompletion(Vulkan::NO_TIMEOUT);
}

void StagedFrame::recordCommandBuffer() {
	const auto& vulkan = getVulkan();
	const auto& srcImage = m_stagingImage;
	const auto& dstImage = getImage();
	const auto cmd = *m_commandBuffer;

	const bool queueOwnershipTransfer = vulkan.getGraphicsQueueIndex() != vulkan.getTransferQueueIndex();
	const size_t barrierCount = srcImage.getPlanes().size() + dstImage.getPlanes().size();

	//Record the command buffer
	const vk::CommandBufferBeginInfo beginInfo(
		{},
		nullptr
	);

	vulkan.begin(cmd, beginInfo);
	{
		std::vector<vk::ImageMemoryBarrier> memoryBarriers;
		memoryBarriers.reserve(barrierCount);

		constexpr vk::ImageSubresourceRange imageSubresourceRange(
			vk::ImageAspectFlagBits::eColor,				//Aspect mask
			0, 1, 0, 1										//Base mipmap level, mipmap levels, base array layer, layers
		);

		//Transition the layout of the images
		{
			memoryBarriers.clear();
			for(const auto& plane : srcImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eHostWrite;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferRead;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eGeneral;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferSrcOptimal;

				//Allways owned by the transfer queue
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			for(const auto& plane : dstImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = {};
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eUndefined;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eTransferDstOptimal;

				//We don't mind about the previous contents, so skip the ownership transfer
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == barrierCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTopOfPipe |
				vk::PipelineStageFlagBits::eHost ;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eTransfer;

			vulkan.pipelineBarrier(
				cmd,										//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}

		//Copy the image to the image
		copy(vulkan, cmd, srcImage, const_cast<Image&>(dstImage)); //FIXME ugly const_cast

		//Transition the layout of the images (again)
		{
			memoryBarriers.clear();
			for(const auto& plane : srcImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferRead;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eHostWrite;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferSrcOptimal;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eGeneral;
				
				//Allways owned by the transfer queue
				constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
				constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			for(const auto& plane : dstImage.getPlanes()){
				constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
				constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eShaderRead;

				constexpr vk::ImageLayout srcLayout = vk::ImageLayout::eTransferDstOptimal;
				constexpr vk::ImageLayout dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

				//Handle ownership back to the graphics queue
				const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
				const auto dstFamily = queueOwnershipTransfer ? vulkan.getGraphicsQueueIndex() : VK_QUEUE_FAMILY_IGNORED;

				memoryBarriers.emplace_back(
					srcAccess,								//Old access mask
					dstAccess,								//New access mask
					srcLayout,								//Old layout
					dstLayout,								//New layout
					srcFamily,								//Old queue family
					dstFamily,								//New queue family
					plane.getImage(),						//Image
					imageSubresourceRange					//Image subresource
				);
			}
			assert(memoryBarriers.size() == barrierCount);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTransfer;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eAllGraphics |
				vk::PipelineStageFlagBits::eHost ;

			vulkan.pipelineBarrier(
				cmd,										//Command buffer
				srcStages,									//Generating stages
				dstStages,									//Consuming stages
				{},											//Dependency flags
				Utils::BufferView<const vk::ImageMemoryBarrier>(memoryBarriers) //Memory barriers
			);
		}

	}
	vulkan.end(cmd);
}



Image StagedFrame::createStagingImage(	const Vulkan& vulkan,
										Utils::BufferView<const Image::Plane> planes )
{
	constexpr vk::ImageUsageFlags usage =
		vk::ImageUsageFlagBits::eTransferSrc;

	constexpr vk::ImageTiling tiling =
		vk::ImageTiling::eLinear;

	constexpr vk::MemoryPropertyFlags memory =
		vk::MemoryPropertyFlagBits::eHostVisible;

	return Image(
		vulkan,
		planes,
		usage,
		tiling,
		memory
	);
}

std::vector<Utils::BufferView<std::byte>> StagedFrame::getPixelData(const Vulkan& vulkan,
																	const Image& stagingImage )
{
	const vk::MappedMemoryRange range(
		*(stagingImage.getMemory().memory),
		0, VK_WHOLE_SIZE
	);

	auto* data = reinterpret_cast<std::byte*>(vulkan.mapMemory(range));
	return Utils::slice(data, stagingImage.getMemory().areas);
}

vk::UniqueCommandBuffer StagedFrame::createCommandBuffer(	const Vulkan& vulkan,
															vk::CommandPool cmdPool )
{
	return vulkan.allocateCommnadBuffer(cmdPool, vk::CommandBufferLevel::ePrimary);
}

vk::SubmitInfo StagedFrame::createSubmitInfo(const vk::CommandBuffer& cmdBuffer) {
	return vk::SubmitInfo(
		0, nullptr,							//Wait semaphores
		nullptr,							//Pipeline stages
		1, &cmdBuffer,						//Command buffers
		0, nullptr							//Signal semaphores
	);
}

}