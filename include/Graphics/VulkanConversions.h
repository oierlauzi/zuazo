#pragma once

#include "Vulkan.h"
#include "../Resolution.h"
#include "../PixelProperties.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(PixelFormat fmt, ColorEncoding enc);
constexpr std::tuple<PixelFormat, ColorEncoding> fromVulkan(vk::Format fmt);

}

#include "VulkanConversions.inl"