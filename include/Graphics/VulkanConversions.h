#pragma once

#include "Vulkan.h"
#include "../Version.h"
#include "../Verbosity.h"
#include "../Resolution.h"
#include "../PixelProperties.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version);

constexpr vk::DebugUtilsMessageSeverityFlagsEXT toVulkan(Verbosity verbosity);
constexpr Verbosity fromVulkan(vk::DebugUtilsMessageSeverityFlagsEXT verbosity);

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(PixelFormat fmt, ColorEncoding enc);
constexpr std::tuple<PixelFormat, ColorEncoding> fromVulkan(vk::Format fmt);

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorEncoding enc);
constexpr std::tuple<ColorPrimaries, ColorEncoding> fromVulkan(vk::ColorSpaceKHR space);

inline vk::SurfaceFormatKHR toVulkan(PixelFormat format, ColorPrimaries prim, ColorEncoding enc);
constexpr std::tuple<PixelFormat, ColorPrimaries, ColorEncoding> fromVulkan(const vk::SurfaceFormatKHR& fmt);

}

#include "VulkanConversions.inl"