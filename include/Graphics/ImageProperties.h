#pragma once

#include "Vulkan.h"

namespace Zuazo::Graphics {

struct ImageProperties {
	vk::Format						format = vk::Format::eUndefined;
	vk::ComponentMapping 			swizzle = {};
	vk::SamplerYcbcrRange			nativeRange = vk::SamplerYcbcrRange::eItuFull;
	uint32_t						shaderRange = 0;
	vk::SamplerYcbcrModelConversion	nativeColorModel = vk::SamplerYcbcrModelConversion::eRgbIdentity;
	uint32_t						shaderColorModel = 0;
	uint32_t						shaderTransferFunction = 0;
	uint32_t						shaderColorPrimaries = 0;
};

}