#pragma once

#include "Instance.h"

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics::Vulkan {

class Surface {
public:
	Surface(Instance& instance, VkSurfaceKHR surface);
	Surface(const Surface& other) = delete;
	~Surface();

	VkSurfaceKHR			get();

private:
	Instance&				m_instance;

	VkSurfaceKHR 			m_surface;
};

}