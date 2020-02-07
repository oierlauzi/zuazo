#pragma once

#include "Vulkan.h"
#include "../Version.h"
#include "../Verbosity.h"
#include "../Resolution.h"

#include "../PixelFormat.h"
#include "../PixelEncoding.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version);

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(PixelFormat fmt, PixelEncoding enc);
constexpr std::tuple<vk::Format, vk::ComponentMapping> removeSwizzle(const std::tuple<vk::Format, vk::ComponentMapping>& fmt);
constexpr std::tuple<PixelFormat, PixelEncoding> fromVulkan(vk::Format fmt);

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, PixelEncoding enc);
constexpr std::tuple<ColorPrimaries, PixelEncoding> fromVulkan(vk::ColorSpaceKHR space);

inline vk::SurfaceFormatKHR toVulkan(PixelFormat format, ColorPrimaries prim, PixelEncoding enc);
constexpr std::tuple<PixelFormat, ColorPrimaries, PixelEncoding> fromVulkan(const vk::SurfaceFormatKHR& fmt);

}

#include "VulkanConversions.inl"