#pragma once

#include "Vulkan.h"

#include <vector>


#define ZUAZO_CONSTRUCT_SWIZZLE(r, g, b, a) vk::ComponentMapping( 	\
	vk::ComponentSwizzle::e##r,										\
	vk::ComponentSwizzle::e##g,										\
	vk::ComponentSwizzle::e##b,										\
	vk::ComponentSwizzle::e##a 										\
)

namespace Zuazo::Graphics {

extern std::vector<vk::LayerProperties> 		getUsedLayers(const std::vector<vk::LayerProperties>& available, std::vector<vk::LayerProperties>& required);
extern std::vector<const char*> 				getNames(const std::vector<vk::LayerProperties>& layer);

extern std::vector<vk::ExtensionProperties> 	getUsedExtensions(const std::vector<vk::ExtensionProperties>& available, std::vector<vk::ExtensionProperties>& required);
extern std::vector<const char*> 				getNames(const std::vector<vk::ExtensionProperties>& ext);

extern std::vector<vk::QueueFamilyProperties>	getUsedQueueFamilies(const std::vector<vk::QueueFamilyProperties>& available, std::vector<vk::QueueFamilyProperties>& required);
extern std::map<uint32_t, uint32_t>				getQueueFamilyIndices(const std::vector<vk::QueueFamilyProperties>& available, const std::vector<vk::QueueFamilyProperties>& requested);
extern size_t									getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags);
extern bool										getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available);

}