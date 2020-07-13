#include <zuazo/Graphics/Buffer.h>

#include <zuazo/Exception.h>

namespace Zuazo::Graphics {

Buffer::Buffer(	const Vulkan& vulkan,
				vk::BufferUsageFlags usage,
				vk::MemoryPropertyFlags properties,
				size_t size )
	: m_buffer(createBuffer(vulkan, size, usage))
	, m_memory(allocateMemory(vulkan, *m_buffer, properties))
{
}



vk::Buffer Buffer::getBuffer() const{
	return *m_buffer;
}

vk::DeviceMemory Buffer::getDeviceMemory() const {
	return *m_memory;
}



vk::UniqueBuffer Buffer::createBuffer(	const Vulkan& vulkan,
										size_t size,
										vk::BufferUsageFlags usage )
{
	const vk::BufferCreateInfo createInfo(
		{},										//Flags
		size,									//Size
		usage,									//Buffer usage
		vk::SharingMode::eExclusive,			//Sharing mode
		0, nullptr								//Queue family indices
	);

	return vulkan.createBuffer(createInfo);
}

vk::UniqueDeviceMemory Buffer::allocateMemory(	const Vulkan& vulkan,
												vk::Buffer buffer,
												vk::MemoryPropertyFlags properties )
{

	const auto requirements = vulkan.getMemoryRequirements(buffer);

	auto deviceMemory = vulkan.allocateMemory(requirements, properties);
	vulkan.bindMemory(buffer, *deviceMemory, 0);
	return deviceMemory;
}

}