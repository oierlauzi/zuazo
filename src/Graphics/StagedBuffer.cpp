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
								vk::AccessFlags access,
								vk::PipelineStageFlags stage) 
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
	const auto key = Key(offset, size, queue, access, stage);
	auto ite = m_uploadCommands.find(key);
	if(ite == m_uploadCommands.end()) {
		std::tie(ite, std::ignore) = m_uploadCommands.emplace(
			key,
			createCommandBuffer(
				vulkan, 
				offset, size, 
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

void StagedBuffer::flushData(	const Vulkan& vulkan, 					
								uint32_t queue,
								vk::AccessFlags access,
								vk::PipelineStageFlags stage )
{
	flushData(vulkan, 0, VK_WHOLE_SIZE, queue, access, stage);
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
	const vk::CommandBufferAllocateInfo allocInfo(
		*m_commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);

	auto cmdBuffer = vulkan.allocateCommnadBuffer(allocInfo);

	//Record the command buffer
	const vk::CommandBufferBeginInfo beginInfo(
		{},
		nullptr
	);

	cmdBuffer->begin(beginInfo, vulkan.getDispatcher()); 
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
			
			cmdBuffer->pipelineBarrier(
				srcStages,						//Generating stages
				dstStages,						//Consuming stages
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
			
			cmdBuffer->pipelineBarrier(
				srcStages,						//Generating stages
				dstStages,						//Consuming stages
				{},								//Dependency flags
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

}