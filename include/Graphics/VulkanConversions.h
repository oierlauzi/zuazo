#pragma once

#include "Vulkan.h"
#include "../Resolution.h"
#include "../PixelProperties.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

using VulkanPixelFormatRep = std::tuple<vk::Format, vk::ComponentMapping>;

constexpr VulkanPixelFormatRep toVulkan(PixelFormat fmt, ColorTransferFunction eotf);
constexpr PixelFormat fromVulkan(const VulkanPixelFormatRep& fmt);

}

#include "VulkanConversions.inl"