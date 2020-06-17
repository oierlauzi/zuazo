#include <Graphics/StagedBuffer.h>

namespace Zuazo::Graphics {

StagedBuffer::StagedBuffer(	const Vulkan& vulkan, 
							size_t size, 
							vk::BufferUsageFlags usage )
	: Buffer(createDeviceBuffer(vulkan, size, usage))
	, m_stagingBuffer(createStagingBuffer(vulkan, size))
	, m_data(mapStagingBuffer(vulkan, size, m_stagingBuffer))
	, m_commandPool(createCommandPool(vulkan))
	, m_uploadComplete(vulkan.createFence(true))
	, m_waitFence(nullptr)
{
}

StagedBuffer::StagedBuffer(StagedBuffer&& other) = default;

StagedBuffer::~StagedBuffer() = default;

StagedBuffer& StagedBuffer::operator=(StagedBuffer&& other) = default;



std::byte* StagedBuffer::data() {
	return m_data.data();
}

const std::byte* StagedBuffer::data() const {
	return m_data.data();
}

size_t StagedBuffer::size() const {
	return m_data.size();
}

const Utils::BufferView<std::byte>& StagedBuffer::getData() {
	return m_data;
}

Utils::BufferView<const std::byte> StagedBuffer::getData() const {
	return m_data;
}



void StagedBuffer::flushData(	const Vulkan& vulkan, 
								size_t offset, 
								size_t size, 					
								uint32_t queue,
								vk::PipelineStageFlags destination) 
{
	assert(!m_waitFence);

	//Flush the memory
	const vk::MappedMemoryRange range(
		m_stagingBuffer.getDeviceMemory(),
		offset, size
	);
	vulkan.flushMappedMemory(range);

	//vkCopyBuffer does not support VK_WHOLE_SIZE, so use the real value
	if(size == VK_WHOLE_SIZE) {
		size = m_data.size();
	}

	//Ensure that the adecuate command buffer exists
	const auto key = Key(offset, size, queue, destination);
	auto ite = m_uploadCommands.find(key);
	if(ite == m_uploadCommands.end()) {
		std::tie(ite, std::ignore) = m_uploadCommands.emplace(
			key,
			createCommandBuffer(
				vulkan, 
				offset, size, 
				queue, destination
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
	vulkan.getTransferQueue().submit(
		subInfo,
		m_waitFence,
		vulkan.getDispatcher()
	);
}

void StagedBuffer::flushData(	const Vulkan& vulkan, 					
								uint32_t queue,
								vk::PipelineStageFlags destination )
{
	flushData(vulkan, 0, VK_WHOLE_SIZE, queue, destination);
}

void StagedBuffer::waitCompletion(	const Vulkan& vulkan,
									uint64_t timeo ) const 
{
	if(m_waitFence) {
		vulkan.waitForFences(m_waitFence, true, timeo);
		m_waitFence = nullptr;
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
													vk::PipelineStageFlags destination )
{
	const vk::CommandBufferAllocateInfo cbAllocInfo(
		*m_commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffer = vulkan.allocateCommnadBuffer(cbAllocInfo);

	//Record the command buffer
	const vk::CommandBufferBeginInfo cbBeginInfo(
		{},
		nullptr
	);

	cmdBuffer->begin(cbBeginInfo, vulkan.getDispatcher()); 
	{
		const bool queueOwnershipTransfer = vulkan.getTransferQueueIndex() != queue;

		{
			//Insert a memory barrier to acquire buffer's ownership
			const auto srcFamily = VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? queue : VK_QUEUE_FAMILY_IGNORED;
			constexpr vk::AccessFlags srcAccess = {};
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eTransferWrite;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				getBuffer(),					//Buffer
				offset, size					//Range
			);

			constexpr vk::PipelineStageFlags sourceStages = 
				vk::PipelineStageFlagBits::eTopOfPipe;

			constexpr vk::PipelineStageFlags destinationStages = 
				vk::PipelineStageFlagBits::eTransfer;
			
			cmdBuffer->pipelineBarrier(
				sourceStages,					//Generating stages
				destinationStages,				//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}

		//Copy the data into the vertex buffer
		cmdBuffer->copyBuffer(
			m_stagingBuffer.getBuffer(),
			getBuffer(),
			vk::BufferCopy(offset, offset, size),
			vulkan.getDispatcher()
		);

		//Insert a memory barrier, so that changes are visible
		{
			const auto srcFamily = queueOwnershipTransfer ? vulkan.getTransferQueueIndex() : VK_QUEUE_FAMILY_IGNORED;
			const auto dstFamily = queueOwnershipTransfer ? queue : VK_QUEUE_FAMILY_IGNORED;
			constexpr vk::AccessFlags srcAccess = vk::AccessFlagBits::eTransferWrite;
			constexpr vk::AccessFlags dstAccess = vk::AccessFlagBits::eVertexAttributeRead;

			const vk::BufferMemoryBarrier memoryBarrier(
				srcAccess,						//Old access mask
				dstAccess,						//New access mask
				srcFamily,						//Old queue family
				dstFamily, 						//New queue family
				getBuffer(),					//Buffer
				offset, size					//Range
			);

			constexpr vk::PipelineStageFlags sourceStages = 
				vk::PipelineStageFlagBits::eTransfer;
			
			cmdBuffer->pipelineBarrier(
				sourceStages,					//Generating stages
				destination,					//Consuming stages
				{},								//dependency flags
				{},								//Memory barriers
				memoryBarrier,					//Buffer memory barriers
				{},								//Image memory barriers
				vulkan.getDispatcher()
			);
		}
	}
	cmdBuffer->end(vulkan.getDispatcher());

	return cmdBuffer.release();
}

Buffer StagedBuffer::createDeviceBuffer(const Vulkan& vulkan, 
										size_t size,
										vk::BufferUsageFlags usage )
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
	auto* data = vulkan.mapMemory(range);

	return Utils::BufferView<std::byte>( 
		reinterpret_cast<std::byte*>(data), 
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

}