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

uint32_t toVulkan(Version version);

vk::Extent2D toVulkan(const Resolution& res);
Resolution fromVulkan(const vk::Extent2D& res);

std::array<std::tuple<vk::Format, vk::ComponentMapping>, IMAGE_COUNT> toVulkan(ColorFormat fmt);
std::tuple<ColorFormat, ColorTransferFunction> fromVulkan(vk::Format fmt);

vk::ColorSpaceKHR toVulkan(ColorPrimaries prim, ColorTransferFunction enc);
std::tuple<ColorPrimaries, ColorTransferFunction> fromVulkan(vk::ColorSpaceKHR space);

int32_t toVulkan(ColorRange range, ColorModel model);

int32_t toVulkan(ColorTransferFunction transferFunction);

std::tuple<vk::Format, vk::ComponentMapping> optimizeFormat(const std::tuple<vk::Format, vk::ComponentMapping>& fmt);
std::tuple<vk::Format, int32_t> optimizeFormat(vk::Format fmt, int32_t trf, int32_t range);

}