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

	const auto requirements = vulkan.getDevice().getBufferMemoryRequirements(buffer, vulkan.getDispatcher());

	auto deviceMemory = vulkan.allocateMemory(requirements, properties);
	vulkan.getDevice().bindBufferMemory(buffer, *deviceMemory, 0, vulkan.getDispatcher());
	return deviceMemory;
}






Buffer::MappedMemory::MappedMemory(	const Vulkan& vulkan,
									const vk::MappedMemoryRange& mapping )
	: m_vulkan(&vulkan)
	, m_mappedMemoryRange(mapping)
	, m_memory(map(*m_vulkan, m_mappedMemoryRange))
{
}

Buffer::MappedMemory::MappedMemory(MappedMemory&& other) 
	: m_vulkan(other.m_vulkan)
	, m_mappedMemoryRange(other.m_mappedMemoryRange)
	, m_memory(other.m_memory)
{
	other.m_memory = nullptr;
}

Buffer::MappedMemory::~MappedMemory() {
	unmap();
}


Buffer::MappedMemory& Buffer::MappedMemory::operator=(MappedMemory&& other) {
	unmap();
		
	m_vulkan = other.m_vulkan;
	m_mappedMemoryRange = other.m_mappedMemoryRange;
	m_memory = other.m_memory;

	other.m_memory = nullptr;

	return *this;
}

Buffer::MappedMemory::operator bool() const {
	return m_memory;
}

std::byte* Buffer::MappedMemory::data() {
	return m_memory;
}

const std::byte* Buffer::MappedMemory::data() const {
	return m_memory;
}


size_t Buffer::MappedMemory::size() const {
	return m_mappedMemoryRange.size;
}

size_t Buffer::MappedMemory::offset() const {
	return m_mappedMemoryRange.offset;
}

void Buffer::MappedMemory::flush() {
	if(*this){
		m_vulkan->getDevice().flushMappedMemoryRanges(
			m_mappedMemoryRange,
			m_vulkan->getDispatcher()
		);
	}
}

void Buffer::MappedMemory::unmap(){
	if(*this){
		m_vulkan->getDevice().unmapMemory(m_mappedMemoryRange.memory, m_vulkan->getDispatcher());
	}
}

std::byte* Buffer::MappedMemory::map(	const Vulkan& vulkan,
										const vk::MappedMemoryRange& mapping )
{
	return static_cast<std::byte*>(vulkan.getDevice().mapMemory(
		mapping.memory,												//Memory allocation
		mapping.offset,												//Offset
		mapping.size,												//Size
		{},															//Flags
		vulkan.getDispatcher()										//Dispatcher
	));
}

}