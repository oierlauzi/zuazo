#pragma once 

#include "vulkan/vulkan.h"

#include <vector>

namespace Zuazo::Graphics::Vulkan {

using ValidationLayer = VkLayerProperties;

extern std::vector<ValidationLayer> getUsedValidationLayers(const std::vector<ValidationLayer>& available, std::vector<ValidationLayer> required);
extern std::vector<const char*> getNames(const std::vector<ValidationLayer>& ext);


}