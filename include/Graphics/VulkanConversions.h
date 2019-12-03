#pragma once

#include "Vulkan.h"
#include "../Resolution.h"
#include "../PixelFormat.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);


#define ZUAZO_VK_SWIZZLE(r, g, b, a)	\
	vk::ComponentMapping(				\
		vk::ComponentSwizzle::e##r,		\
		vk::ComponentSwizzle::e##g,		\
		vk::ComponentSwizzle::e##b,		\
		vk::ComponentSwizzle::e##a		\
	)

#define ZUAZO_VK_SWIZZLE_IDENTITY 		\
	ZUAZO_VK_SWIZZLE(Identity, Identity, Identity, Identity)

using VulkanPixelFmtRep = std::tuple<vk::Format, vk::ComponentMapping>;

constexpr VulkanPixelFmtRep toVulkan(const PixelFormat& fmt);
constexpr PixelFormat fromVulkan(const VulkanPixelFmtRep& fmt);

}

#include "VulkanConversions.inl"