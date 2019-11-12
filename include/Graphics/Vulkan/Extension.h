#pragma once 

#include "vulkan/vulkan.h"

#include <vector>

namespace Zuazo::Graphics::Vulkan {

using Extension = VkExtensionProperties;

extern std::vector<Extension> getUsedExtensions(const std::vector<Extension>& available, std::vector<Extension> required);
extern std::vector<const char*> getNames(const std::vector<Extension>& ext);


}