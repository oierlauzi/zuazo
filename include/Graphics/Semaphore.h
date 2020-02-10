#pragma once

#include "Vulkan.h"

namespace Zuazo::Graphics {

class Semaphore {
public:
	Semaphore() = default;
	Semaphore(const Vulkan& vulkan);
	Semaphore(const Semaphore& other) = delete;
	Semaphore(Semaphore&& opther) = default;
	~Semaphore() = default;

	Semaphore& operator=(const Semaphore& other) = delete;
	Semaphore& operator=(Semaphore&& other) = default;

	vk::Semaphore		getSemaphore() const;

private:
	vk::UniqueSemaphore m_semaphore;

	static vk::UniqueSemaphore createSemaphore(const Vulkan& vulkan);
};

}