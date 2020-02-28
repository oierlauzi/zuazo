#pragma once

#include "Vulkan.h"

namespace Zuazo::Graphics {

class MappedMemory {
public:
	MappedMemory() = default;
	MappedMemory(	const Vulkan& vulkan,
					const vk::MappedMemoryRange& mapping );
	MappedMemory(const MappedMemory& other) = delete;
	MappedMemory(MappedMemory&& other);
	~MappedMemory();

	MappedMemory&					operator=(const MappedMemory& other) = delete;
	MappedMemory&					operator=(MappedMemory&& other);

	operator bool() const;

	std::byte*						data();
	const std::byte*				data() const;

	size_t							size() const;
	size_t 							offset() const;

	void							flush();
	
private:
	const Vulkan*					m_vulkan = nullptr;

	vk::MappedMemoryRange			m_mappedMemoryRange;
	std::byte*						m_memory = nullptr;

	void							unmap();

	static std::byte*				map(const Vulkan& vulkan,
										const vk::MappedMemoryRange& mapping );
};

}