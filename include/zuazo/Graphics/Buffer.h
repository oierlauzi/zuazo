#pragma once

#include "Vulkan.h"

#include <utility>

namespace Zuazo::Graphics {

class Buffer {
public:
	Buffer() = default;
	Buffer(	const Vulkan& vulkan,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties,
			size_t size );
	Buffer(const Buffer& other) = delete;
	Buffer(Buffer&& other) noexcept = default;
	virtual ~Buffer() = default;

	Buffer& 						operator=(const Buffer& other) = delete;
	Buffer& 						operator=(Buffer&& other) = default;

	vk::Buffer						getBuffer() const noexcept;
	vk::DeviceMemory				getDeviceMemory() const noexcept;

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

}