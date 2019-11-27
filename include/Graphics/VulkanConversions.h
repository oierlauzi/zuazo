#pragma once

#include "Vulkan.h"
#include "../Resolution.h"
#include "../PixelFormat.h"

#include <tuple>
#include <array>

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

using VulkanPixelFmtRep = std::tuple<vk::Format, std::array<vk::ComponentSwizzle, 4>>;
constexpr VulkanPixelFmtRep toVulkan(const PixelFormat& fmt);
constexpr PixelFormat fromVulkan(const VulkanPixelFmtRep& fmt);

}

#include "VulkanConversions.inl"