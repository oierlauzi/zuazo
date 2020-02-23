#include <Graphics/Buffer.h>

#include <Exception.h>

namespace Zuazo::Graphics {

Buffer::Buffer(	const Vulkan& vulkan,
				size_t size,
				vk::BufferUsageFlags usage,
				vk::MemoryPropertyFlags properties )
	: m_buffer(createBuffer(vulkan, size, usage))
	, m_allocation(allocateMemory(vulkan, *m_buffer, properties))
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
	const auto memoryProperties = vulkan.getPhysicalDevice().getMemoryProperties(vulkan.getDispatcher());

	//Find an apropiate index for the type
	uint32_t i;
	for (i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if(	(requirements.memoryTypeBits & (1 << i)) && 
			(memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) 
		{
			break; //Found one!
		}
	}
	if(i == memoryProperties.memoryTypeCount){
		throw Exception("Unable to find desired memory type");
	}

	const vk::MemoryAllocateInfo allocInfo(
		requirements.size,						//Size
		i										//Memory type index
	);

	auto deviceMemory = vulkan.allocateMemory(allocInfo);
	vulkan.getDevice().bindBufferMemory(buffer, *deviceMemory, 0, vulkan.getDispatcher());
	return deviceMemory;
}

}