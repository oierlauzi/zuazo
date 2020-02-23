#pragma once

#include "Vulkan.h"
#include "ColorTransfer.h"

#include "../Version.h"
#include "../Verbosity.h"
#include "../Resolution.h"
#include "../ColorFormat.h"
#include "../ColorRange.h"
#include "../ColorSubsampling.h"
#include "../ColorTransferFunction.h"
#include "../ColorModel.h"
#include "../ColorPrimaries.h"

#include <tuple>

namespace Zuazo::Graphics {

constexpr uint32_t toVulkan(Version version);

constexpr vk::Extent2D toVulkan(const Resolution& res);
constexpr Resolution fromVulkan(const vk::Extent2D& res);

constexpr std::array<std::tuple<vk::Format, vk::ComponentMapping>, SAMPLER_COUNT> toVulkan(ColorFormat fmt);
constexpr std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt);

constexpr vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc);
constexpr std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space);

constexpr int32_t toVulkan(ColorRange range, ColorModel model);

constexpr int32_t toVulkan(ColorTransferFunction transferFunction);

constexpr std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt);
constexpr std::tuple<vk::Format, int32_t> optimizeFormat(vk::Format fmt, int32_t trf, int32_t range);

}

#include "VulkanConversions.inl"