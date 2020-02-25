#include <Graphics/Buffer.h>

#include <Exception.h>

namespace Zuazo::Graphics {

Buffer::Buffer(	const Vulkan& vulkan,
				size_t size,
				vk::BufferUsageFlags usage,
				vk::MemoryPropertyFlags properties )
	: m_buffer(createBuffer(vulkan, size, usage))
	, m_memory(allocateMemory(vulkan, *m_buffer, properties))
{
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

	const auto requirements = vulkan.getDevice().getBufferMemoryRequirements(buffer, vulkan.getDispatcher());

	auto deviceMemory = vulkan.allocateMemory(requirements, properties);
	vulkan.getDevice().bindBufferMemory(buffer, *deviceMemory, 0, vulkan.getDispatcher());
	return deviceMemory;
}

}