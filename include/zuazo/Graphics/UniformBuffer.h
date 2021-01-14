#pragma once

#include "Vulkan.h"
#include "StagedBuffer.h"
#include "../Utils/Pimpl.h"
#include "../Utils/BufferView.h"
#include "../Utils/Area.h"

#include <map>


namespace Zuazo::Graphics {

class UniformBuffer {
public:
	UniformBuffer(	const Vulkan& vulkan,
					Utils::BufferView<const std::pair<uint32_t, size_t>> bindings );
	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer(UniformBuffer&& other) = default;
	~UniformBuffer() = default;

	UniformBuffer&							operator=(const UniformBuffer& other) = delete;
	UniformBuffer&							operator=(UniformBuffer&& other) = default;

	Utils::BufferView<const std::byte>		getBindingData(uint32_t binding) const;
	void									writeDescirptorSet(	const Vulkan& vulkan,
																vk::DescriptorSet descriptorSet ) const;
	void									writeDescirptorSet(	const Vulkan& vulkan,
																vk::DescriptorSet descriptorSet,
																Utils::BufferView<const std::pair<uint32_t, uint32_t>> bindingRemapping ) const;
	void									write(	const Vulkan& vulkan,
													uint32_t binding,
													const void* data,
													size_t size,
													size_t offset = 0 );
	void									flush(const Vulkan& vulkan);
	bool									waitCompletion(	const Vulkan& vulkan, 
															uint64_t timeout = Vulkan::NO_TIMEOUT ) const;

private:
	std::map<uint32_t, Utils::Area>			m_uniformBufferLayout;
	StagedBuffer							m_uniformBuffer;

	Utils::Area								m_uniformFlushArea;
	vk::ShaderStageFlags					m_uniformFlushStages;
	
	static std::map<uint32_t, Utils::Area> 	createUniformBufferLayout(	const Vulkan& vulkan,
																		Utils::BufferView<const std::pair<uint32_t, size_t>> bindings );

	static StagedBuffer						createUniformBuffer(const Vulkan& vulkan,
																const std::map<uint32_t, Utils::Area>& layout);
};

}