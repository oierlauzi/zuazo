#pragma once

#include "Vulkan.h"

#include <utility>

namespace Zuazo::Graphics {

class Buffer {
public:
	class MappedMemory;

	Buffer() = default;
	Buffer(	const Vulkan& vulkan,
			size_t size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties );
	Buffer(const Buffer& other) = delete;
	Buffer(Buffer&& other) = default;
	~Buffer() = default;

	Buffer& 						operator=(const Buffer& other) = delete;
	Buffer& 						operator=(Buffer&& other) = default;

	vk::Buffer						getBuffer() const;
	vk::DeviceMemory				getDeviceMemory() const;

private:
	vk::UniqueBuffer				m_buffer;
	vk::UniqueDeviceMemory			m_memory;

	static vk::UniqueBuffer			createBuffer(	const Vulkan& vulkan,
													size_t size,
													vk::BufferUsageFlags usage );

	static vk::UniqueDeviceMemory 	allocateMemory(	const Vulkan& vulkan,
													vk::Buffer buffer,
													vk::MemoryPropertyFlags properties );
};

class Buffer::MappedMemory {
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

	static std::byte*				map(	const Vulkan& vulkan,
											const vk::MappedMemoryRange& mapping );
};

}