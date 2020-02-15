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

static constexpr vk::FormatFeatureFlags			SRC_FORMAT_FLAGS = 	vk::FormatFeatureFlagBits::eTransferDst |
																	vk::FormatFeatureFlagBits::eSampledImage |
																	vk::FormatFeatureFlagBits::eSampledImageFilterLinear;

static constexpr vk::FormatFeatureFlags			DST_FORMAT_FLAGS = 	vk::FormatFeatureFlagBits::eTransferSrc |
																	vk::FormatFeatureFlagBits::eColorAttachmentBlend;

static constexpr vk::FormatFeatureFlags			YCBCR_FORMAT_FLAGS =vk::FormatFeatureFlagBits::eCositedChromaSamples |
																	vk::FormatFeatureFlagBits::eMidpointChromaSamples;


extern std::vector<vk::LayerProperties> 		getUsedLayers(const std::vector<vk::LayerProperties>& available, std::vector<vk::LayerProperties>& required);
extern std::vector<const char*> 				getNames(const std::vector<vk::LayerProperties>& layer);

extern std::vector<vk::ExtensionProperties> 	getUsedExtensions(const std::vector<vk::ExtensionProperties>& available, std::vector<vk::ExtensionProperties>& required);
extern std::vector<const char*> 				getNames(const std::vector<vk::ExtensionProperties>& ext);

extern std::vector<vk::QueueFamilyProperties>	getUsedQueueFamilies(const std::vector<vk::QueueFamilyProperties>& available, std::vector<vk::QueueFamilyProperties>& required);
extern size_t									getQueueFamilyIndex(const std::vector<vk::QueueFamilyProperties>& qf, vk::QueueFlags flags);
extern bool										getQueueFamilyCompatibility(vk::QueueFlags required, vk::QueueFlags available);

extern bool										hasSamplerSupport(vk::FormatProperties features);
extern bool										hasFramebufferSupport(vk::FormatProperties features);
extern bool										hasYCbCrSupport(vk::FormatProperties features);


extern vk::FormatProperties						getFormatFeatures(const Vulkan& vulkan, vk::Format format);

extern vk::UniqueShaderModule					createShader(const Vulkan& vulkan, const Utils::BufferView<uint32_t>& code);
extern vk::UniqueSemaphore						createSemaphore(const Vulkan& vulkan);
extern vk::UniqueFence							createFence(const Vulkan& vulkan);



}