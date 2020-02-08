#pragma once

#include "Vulkan.h"
#include "../Version.h"
#include "../Verbosity.h"
#include "../Resolution.h"

#include "../ColorFormat.h"
#include "../ColorEncoding.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version);

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

constexpr std::tuple<vk::Format, vk::ComponentMapping> toVulkan(ColorFormat fmt, ColorEncoding enc);
constexpr std::tuple<vk::Format, vk::ComponentMapping> removeSwizzle(const std::tuple<vk::Format, vk::ComponentMapping>& fmt);
constexpr std::tuple<ColorFormat, ColorEncoding> fromVulkan(vk::Format fmt);

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorEncoding enc);
constexpr std::tuple<ColorPrimaries, ColorEncoding> fromVulkan(vk::ColorSpaceKHR space);

inline vk::SurfaceFormatKHR toVulkan(ColorFormat format, ColorPrimaries prim, ColorEncoding enc);
constexpr std::tuple<ColorFormat, ColorPrimaries, ColorEncoding> fromVulkan(const vk::SurfaceFormatKHR& fmt);

}

#include "VulkanConversions.inl"