#pragma once

#include "Vulkan.h"
#include "Buffer.h"
#include "../Utils/BufferView.h"

#include <map>
#include <tuple>

namespace Zuazo::Graphics {

class StagedBuffer 
	: public Buffer 
{
public:
	StagedBuffer(	const Vulkan& vulkan, 
					vk::BufferUsageFlags usage,
					size_t size );
	StagedBuffer(const StagedBuffer& other) = delete;
	StagedBuffer(StagedBuffer&& other) noexcept;
	virtual ~StagedBuffer();

	StagedBuffer& 						operator=(const StagedBuffer& other) = delete;
	StagedBuffer& 						operator=(StagedBuffer&& other) noexcept;

	std::byte*							data() noexcept;
	const std::byte*					data() const noexcept;
	size_t								size() const noexcept;
	const Utils::BufferView<std::byte>&	getData() noexcept;
	Utils::BufferView<const std::byte>	getData() const noexcept;

	void								flushData(	const Vulkan& vulkan, 
													size_t offset, 
													size_t size, 					
													uint32_t queue,
													vk::AccessFlags access,
													vk::PipelineStageFlags stage );
	void								flushData(	const Vulkan& vulkan, 					
													uint32_t queue,
													vk::AccessFlags access,
													vk::PipelineStageFlags stage );
	bool								waitCompletion(	const Vulkan& vulkan, 
														uint64_t timeo = Vulkan::NO_TIMEOUT ) const;

	Buffer								finish(const Vulkan& vulkan);
	
private:
	using Key = std::tuple<size_t, size_t, uint32_t, vk::AccessFlags, vk::PipelineStageFlags>;

	Buffer								m_stagingBuffer;
	Utils::BufferView<std::byte>		m_data;

	vk::UniqueCommandPool				m_commandPool;
	vk::UniqueFence						m_uploadComplete;

	std::map<Key, vk::CommandBuffer> 	m_uploadCommands;
	mutable vk::Fence					m_waitFence;

	vk::CommandBuffer					createCommandBuffer(const Vulkan& vulkan,
															size_t offset,
															size_t size,
															uint32_t queue,
															vk::AccessFlags access,
															vk::PipelineStageFlags stage );

	static Buffer						createDeviceBuffer(	const Vulkan& vulkan, 
															vk::BufferUsageFlags usage,
															size_t size );
	static Buffer						createStagingBuffer(const Vulkan& vulkan, 
															size_t size);
	static Utils::BufferView<std::byte> mapStagingBuffer(	const Vulkan& vulkan, 
															size_t size,
															const Buffer& stagingBuffer );
	static vk::UniqueCommandPool		createCommandPool(const Vulkan& vulkan);

	static size_t						getFlushSize(const Vulkan& vulkan, size_t size, size_t maxSize) noexcept;

};

}