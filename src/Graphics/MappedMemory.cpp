#include <Graphics/MappedMemory.h>

namespace Zuazo::Graphics {

MappedMemory::MappedMemory(	const Vulkan& vulkan,
							const vk::MappedMemoryRange& mapping )
	: m_vulkan(&vulkan)
	, m_mappedMemoryRange(mapping)
	, m_memory(map(*m_vulkan, m_mappedMemoryRange))
{
}

MappedMemory::MappedMemory(MappedMemory&& other) 
	: m_vulkan(other.m_vulkan)
	, m_mappedMemoryRange(other.m_mappedMemoryRange)
	, m_memory(other.m_memory)
{
	other.m_memory = nullptr;
}

MappedMemory::~MappedMemory() {
	unmap();
}


MappedMemory& MappedMemory::operator=(MappedMemory&& other) {
	unmap();
		
	m_vulkan = other.m_vulkan;
	m_mappedMemoryRange = other.m_mappedMemoryRange;
	m_memory = other.m_memory;

	other.m_memory = nullptr;

	return *this;
}

MappedMemory::operator bool() const {
	return m_memory;
}

std::byte* MappedMemory::data() {
	return m_memory;
}

const std::byte* MappedMemory::data() const {
	return m_memory;
}


size_t MappedMemory::size() const {
	return m_mappedMemoryRange.size;
}

size_t MappedMemory::offset() const {
	return m_mappedMemoryRange.offset;
}

void MappedMemory::flush() {
	if(*this){
		m_vulkan->getDevice().flushMappedMemoryRanges(
			m_mappedMemoryRange,
			m_vulkan->getDispatcher()
		);
	}
}

void MappedMemory::unmap(){
	if(*this){
		m_vulkan->getDevice().unmapMemory(m_mappedMemoryRange.memory, m_vulkan->getDispatcher());
	}
}

std::byte* MappedMemory::map(	const Vulkan& vulkan,
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