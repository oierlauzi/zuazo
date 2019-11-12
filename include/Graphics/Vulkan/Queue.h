#pragma once

#include <map>
#include <vector>

#include <vulkan/vulkan.h>

namespace Zuazo::Graphics::Vulkan {

using QueueFamily = VkQueueFamilyProperties;
using Queue = VkQueue;

extern std::vector<QueueFamily>	getUsedQueueFamilies(const std::vector<QueueFamily>& available, std::vector<QueueFamily> required);
extern std::map<uint32_t, uint32_t>	getQueueFamilyIndices(const std::vector<QueueFamily>& available, std::vector<QueueFamily> requested);
extern size_t getQueueFamilyIndex(const std::vector<QueueFamily>& qf, uint32_t flags);

}