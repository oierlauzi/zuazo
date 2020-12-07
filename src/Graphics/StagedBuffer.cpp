#include <zuazo/Graphics/StagedBuffer.h>

namespace Zuazo::Graphics {

StagedBuffer::StagedBuffer(	const Vulkan& vulkan, 
							vk::BufferUsageFlags usage,
							size_t size )
	: Buffer(createDeviceBuffer(vulkan, usage, size))
	, m_stagingBuffer(createStagingBuffer(vulkan, size))
	, m_data(mapStagingBuffer(vulkan, size, m_stagingBuffer))
	, m_commandPool(createCommandPool(vulkan))
	, m_uploadComplete(vulkan.createFence(false))
	, m_waitFence(nullptr)
{
}

StagedBuffer::StagedBuffer(StagedBuffer&& other) noexcept = default;

StagedBuffer::~StagedBuffer() = default;

StagedBuffer& StagedBuffer::operator=(StagedBuffer&& other) noexcept = default;



std::byte* StagedBuffer::data() noexcept {
	return m_data.data();
}

const std::byte* StagedBuffer::data() const noexcept {
	return m_data.data();
}

size_t StagedBuffer::size() const noexcept {
	return m_data.size();
}

const Utils::BufferView<std::byte>& StagedBuffer::getData() noexcept {
	return m_data;
}

Utils::BufferView<const std::byte> StagedBuffer::getData() const noexcept {
	return m_data;
}



void StagedBuffer::flushData(	const Vulkan& vulkan, 
								Utils::Area area,				
								uint32_t queue,
								vk::AccessFlags access,
								vk::PipelineStageFlags stage) 
{
	if(area.size() > 0) {
		assert(!m_waitFence);
		const auto offset = area.offset();
		const auto size = area.size();

		//The maximum size allowed for it
		const auto maxSize = m_data.size() - offset;

		//Flush the memory
		const vk::MappedMemoryRange range(
			m_stagingBuffer.getDeviceMemory(),
			offset, getFlushSize(vulkan, size, maxSize)
		);
		vulkan.flushMappedMemory(range);

		//vkCopyBuffer does not support VK_WHOLE_SIZE, so use the real value
		const auto copySize = (size == VK_WHOLE_SIZE) ? maxSize : size;

		//Ensure that the adecuate command buffer exists
		const auto key = Key(offset, copySize, queue, access, stage);
		auto ite = m_uploadCommands.find(key);
		if(ite == m_uploadCommands.end()) {
			std::tie(ite, std::ignore) = m_uploadCommands.emplace(
				key,
				createCommandBuffer(
					vulkan, 
					offset, copySize, 
					queue, access, stage
				)
			);
		}

		m_waitFence = *m_uploadComplete;
		vulkan.resetFences(m_waitFence);

		//Submit the command buffer
		const auto subInfo = vk::SubmitInfo(
			0, nullptr,							//Wait semaphores
			nullptr,							//Pipeline stages
			1, &(ite->second),					//Command buffers
			0, nullptr							//Signal semaphores
		);
		vulkan.submit(
			vulkan.getTransferQueue(),
			subInfo,
			m_waitFence
		);
	}
}

void StagedBuffer::flushData(	const Vulkan& vulkan, 					
								uint32_t queue,
								vk::AccessFlags access,
								vk::PipelineStageFlags stage )
{
	flushData(vulkan, Utils::Area(0, VK_WHOLE_SIZE), queue, access, stage);
}

bool StagedBuffer::waitCompletion(	const Vulkan& vulkan,
									uint64_t timeo ) const 
{
	if(m_waitFence) {
		if(vulkan.waitForFences(m_waitFence, true, timeo)) {
			m_waitFence = nullptr;
			return true;
		} else {
			return false;
		}
	} else {
		return true;
	}
}

Buffer StagedBuffer::finish(const Vulkan& vulkan) {
	waitCompletion(vulkan);
	return std::move(static_cast<Buffer&>(*this));
}

vk::CommandBuffer StagedBuffer::createCommandBuffer(const Vulkan& vulkan,
													size_t offset,
													size_t size,
													uint32_t queue,
													vk::AccessFlags access,
													vk::PipelineStageFlags stage )
{
	auto cmdBuffer = vulkan.allocateCommnadBuffer(*m_commandPool, vk::CommandBufferLevel::ePrimary);

	//Record the command buffer
	const vk::CommandBufferBeginInfo beginInfo(
		{},
		nullptr
	);

	vulkan.begin(*cmdBuffer, beginInfo);
	{
		{
			//Insert a memory barrier
			
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

			//We don't mind about the previous contents, so skip the ownewship transfer
			constexpr auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			constexpr auto dstFamily = VK_QUEUE_FAMILY_IGNORED;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				getBuffer(),					//Buffer
				offset, size					//Range
			);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTopOfPipe;

			constexpr vk::PipelineStageFlags dstStages = 
				vk::PipelineStageFlagBits::eTransfer;

			vulkan.pipelineBarrier(
				*cmdBuffer,						//Command buffer
				srcStages,						//Generating stages
				dstStages,						//Consuming stages
				{},								//dependency flags
				memoryBarrier					//Buffer memory barriers
			);
		}

		//Copy the data into the vertex buffer
		vulkan.copy(
			*cmdBuffer,
			m_stagingBuffer.getBuffer(),
			getBuffer(),
			vk::BufferCopy(offset, offset, size)
		);

		//Insert a memory barrier, so that changes are visible
		{
			const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != queue;

			constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
			const vk::AccessFlags dstAccess = access;

			const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? queue : VK_QUEUE_FAMILY_IGNORED;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				getBuffer(),					//Buffer
				offset, size					//Range
			);

			constexpr vk::PipelineStageFlags srcStages = 
				vk::PipelineStageFlagBits::eTransfer;

			const vk::PipelineStageFlags dstStages = stage;
			
			vulkan.pipelineBarrier(
				*cmdBuffer,						//Command buffer
				srcStages,						//Generating stages
				dstStages,						//Consuming stages
				{},								//dependency flags
				memoryBarrier					//Buffer memory barriers
			);
		}
	}
	vulkan.end(*cmdBuffer);

	return cmdBuffer.release();
}

Buffer StagedBuffer::createDeviceBuffer(const Vulkan& vulkan, 
										vk::BufferUsageFlags usage,
										size_t size )
{
	const vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferDst | usage;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eDeviceLocal;

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		size
	);
}

Buffer StagedBuffer::createStagingBuffer(	const Vulkan& vulkan, 
											size_t size) 
{
	constexpr vk::BufferUsageFlags usageFlags =
		vk::BufferUsageFlagBits::eTransferSrc;

	constexpr vk::MemoryPropertyFlags memoryFlags =
		vk::MemoryPropertyFlagBits::eHostVisible;

	return Buffer(
		vulkan,
		usageFlags,
		memoryFlags,
		size
	);
}

Utils::BufferView<std::byte> StagedBuffer::mapStagingBuffer(const Vulkan& vulkan, 
															size_t size,
															const Buffer& stagingBuffer )
{
	//Map its memory
	const vk::MappedMemoryRange range(
		stagingBuffer.getDeviceMemory(),
		0, VK_WHOLE_SIZE
	);
	auto* data = reinterpret_cast<std::byte*>(vulkan.mapMemory(range));

	return Utils::BufferView<std::byte>( 
		data, 
		size 
	);
}

vk::UniqueCommandPool StagedBuffer::createCommandPool(const Vulkan& vulkan) {
	const vk::CommandPoolCreateInfo createInfo(
		{},													//Flags
		vulkan.getTransferQueueIndex()						//Queue index
	);

	return vulkan.createCommandPool(createInfo);
}

size_t StagedBuffer::getFlushSize(const Vulkan& vulkan, size_t size, size_t maxSize) noexcept {
	size_t result;

	if(size == VK_WHOLE_SIZE) {
		result = VK_WHOLE_SIZE; //Do not modify
	} else {
		//Size must be a multiple of nonCoherentAtomSize or reach the end of the buffer
		const auto nonCoherentAtomSize = vulkan.getPhysicalDeviceProperties().limits.nonCoherentAtomSize;
		result = Utils::align(size, nonCoherentAtomSize);

		if(result >= maxSize) {
			result = VK_WHOLE_SIZE; //Size exceeds the maximum
		}
	}

	return result;
}

}